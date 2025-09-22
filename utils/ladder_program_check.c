/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *
 *    please contact their authors for more information.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_program_check.h"

typedef struct {
    ladder_register_t type;
    uint32_t fn_qty;
    bool is_input;
    ladder_err_prg_check_t no_mod_err;
    ladder_err_prg_check_t inv_mod_err;
    ladder_err_prg_check_t inv_port_err;
} check_info_t;

ladder_prg_check_t ladder_program_check(ladder_ctx_t *ladder_ctx) {
    ladder_prg_check_t status;
    if (ladder_ctx == NULL) {
        status.error = LADDER_ERR_PRG_CHECK_FAIL;
        return status;
    }

    // Static table for all I/O register types, mapping to correct quantities and errors.
    // This eliminates duplication and makes it easy to add/maintain entries.
    static const check_info_t checks[] = { //
                    { LADDER_REGISTER_I, 0, true, LADDER_ERR_PRG_CHECK_NO_INPUT_MODULES, LADDER_ERR_PRG_CHECK_I_INV_MODULE, LADDER_ERR_PRG_CHECK_I_INV_PORT }, //
                    { LADDER_REGISTER_IW, 0, true, LADDER_ERR_PRG_CHECK_NO_INPUT_MODULES, LADDER_ERR_PRG_CHECK_IW_INV_MODULE, LADDER_ERR_PRG_CHECK_IW_INV_PORT }, //
                    { LADDER_REGISTER_Q, 0, false, LADDER_ERR_PRG_CHECK_NO_OUTPUT_MODULES, LADDER_ERR_PRG_CHECK_Q_INV_MODULE, LADDER_ERR_PRG_CHECK_Q_INV_PORT }, //
                    { LADDER_REGISTER_QW, 0, false, LADDER_ERR_PRG_CHECK_NO_OUTPUT_MODULES, LADDER_ERR_PRG_CHECK_QW_INV_MODULE, LADDER_ERR_PRG_CHECK_QW_INV_PORT }, //
            };
    static const size_t num_checks = sizeof(checks) / sizeof(checks[0]);

    for (uint32_t nt = 0; nt < (*ladder_ctx).ladder.quantity.networks; nt++)
        for (uint32_t column = 0; column < (*ladder_ctx).network[nt].cols; column++) {
            // Tracking for expected MULTI cells to validate multi-cell instruction integrity.
            // This prevents dangling or missing MULTI cells.
            uint32_t expected_multi = 0;

            for (uint32_t row = 0; row < (*ladder_ctx).network[nt].rows; row++) {
                status.network = nt;
                status.row = row;
                status.column = column;
                status.error = LADDER_ERR_PRG_CHECK_OK;
                status.code = (*ladder_ctx).network[nt].cells[row][column].code;

                // Check for MULTI integrity before processing data.
                // If expected_multi > 0, this must be a MULTI cell with no data.
                if (expected_multi > 0) {
                    if (status.code != LADDER_INS_MULTI) {
                        status.error = LADDER_ERR_PRG_CHECK_MISSING_MULTI; // Missing expected MULTI cell.
                        goto end;
                    }
                    if ((*ladder_ctx).network[nt].cells[row][column].data_qty != 0 || (*ladder_ctx).network[nt].cells[row][column].data != NULL) {
                        status.error = LADDER_ERR_PRG_CHECK_MULTI_HAS_DATA; // MULTI cell has unexpected data.
                        goto end;
                    }
                    expected_multi--;
                    continue; // Skip further checks for this MULTI cell.
                }

                // If this is an unexpected MULTI (no parent), it's dangling.
                if (status.code == LADDER_INS_MULTI) {
                    status.error = LADDER_ERR_PRG_CHECK_DANGLING_MULTI; // Dangling MULTI without parent.
                    goto end;
                }

                // Original: Proceed with data checks only for non-MULTI cells.
                for (uint32_t d = 0; d < (*ladder_ctx).network[nt].cells[row][column].data_qty; d++) {
                    ladder_register_t reg_type = (*ladder_ctx).network[nt].cells[row][column].data[d].type;

                    // Checks timer index (data[0]: must be REGISTER_T with valid index) and basetime/preset (data[1]: basetime enum 0-4, preset u32 >=0).
                    // This prevents enum overlaps from triggering false I/O errors and ensures timers are properly bounds-checked.
                    bool is_timer_data = false;
                    if (status.code == LADDER_INS_TON || status.code == LADDER_INS_TOF || status.code == LADDER_INS_TP) {
                        is_timer_data = true;
                        if ((*ladder_ctx).network[nt].cells[row][column].data_qty < 2) {
                            status.error = LADDER_ERR_PRG_CHECK_FAIL; // Insufficient data for timer instruction
                            goto end;
                        }
                        if (d == 0) { // Validate timer index
                            if (reg_type != LADDER_REGISTER_T) {
                                status.error = LADDER_ERR_PRG_CHECK_T_INV_TYPE;
                                goto end;
                            }
                            if ((*ladder_ctx).ladder.quantity.t > 0
                                    && (*ladder_ctx).network[nt].cells[row][column].data[d].value.i32 >= (*ladder_ctx).ladder.quantity.t) {
                                status.error = LADDER_ERR_PRG_CHECK_T_INV_INDEX;
                                goto end;
                            }
                        } else if (d == 1) { // Validate basetime and preset
                            if ((uint8_t) reg_type < (uint8_t) LADDER_BASETIME_MS || (uint8_t) reg_type > (uint8_t) LADDER_BASETIME_MIN) {
                                status.error = LADDER_ERR_PRG_CHECK_INV_BASE_TIME;
                                goto end;
                            }
                            // Preset is u32; zero is permitted (e.g., for no-delay timers), but applications may add custom checks here if zero is invalid.
                            if ((*ladder_ctx).network[nt].cells[row][column].data[d].value.u32 == 0) {
                                // No error; context-dependent validity.
                            }
                        }
                    }

                    // Ensures only relevant data (actual I/O registers) undergoes module/port checks, avoiding misapplication to timer fields.
                    if (!is_timer_data) {
                        // Loop through the table to find matching register type and perform checks.
                        // This replaces the error-prone switch with a data-driven approach.
                        size_t i;
                        for (i = 0; i < num_checks; i++) {
                            if (reg_type == checks[i].type) {
                                uint32_t fn_qty = checks[i].is_input ? (*ladder_ctx).hw.io.fn_read_qty : (*ladder_ctx).hw.io.fn_write_qty;
                                // Directly access mp fields for I/O types to avoid reading uninitialized union bytes via i32
                                // This prevents Valgrind errors from accessing larger union members when only smaller ones are initialized
                                uint32_t module = (uint32_t) (*ladder_ctx).network[nt].cells[row][column].data[d].value.mp.module;
                                uint32_t port = (uint32_t) (*ladder_ctx).network[nt].cells[row][column].data[d].value.mp.port;

                                if (fn_qty == 0) {
                                    status.error = checks[i].no_mod_err;
                                    goto end;
                                }
                                if (module > fn_qty) {
                                    status.error = checks[i].inv_mod_err;
                                    goto end;
                                }

                                uint32_t port_qty;
                                if (checks[i].is_input) {
                                    port_qty = (reg_type == LADDER_REGISTER_I ? (*ladder_ctx).input[module].i_qty : (*ladder_ctx).input[module].iw_qty) - 1;
                                } else {
                                    port_qty = (reg_type == LADDER_REGISTER_Q ? (*ladder_ctx).output[module].q_qty : (*ladder_ctx).output[module].qw_qty) - 1;
                                }

                                if (port > port_qty) {
                                    status.error = checks[i].inv_port_err;
                                    goto end;
                                }
                                break;
                            }
                        }
                    }

                    // Loop through non-I/O registers to validate indices.
                    // This bounds-checks memory/register accesses to prevent out-of-bounds errors.
                    if (reg_type != LADDER_REGISTER_NONE && reg_type != LADDER_REGISTER_I && reg_type != LADDER_REGISTER_IW && reg_type != LADDER_REGISTER_Q
                            && reg_type != LADDER_REGISTER_QW && reg_type != LADDER_REGISTER_S && reg_type != LADDER_REGISTER_R) {
                        int32_t index = (*ladder_ctx).network[nt].cells[row][column].data[d].value.i32;
                        if (index < 0) {
                            status.error = LADDER_ERR_PRG_CHECK_FAIL; // Negative indices are invalid
                            goto end;
                        }
                        uint32_t uindex = (uint32_t) index;
                        uint32_t qty = 0; // Will be set based on type
                        switch (reg_type) {
                            case LADDER_REGISTER_NONE:
                                // Constant value; no index check needed.
                                break;
                            case LADDER_REGISTER_M:
                                qty = (*ladder_ctx).ladder.quantity.m;
                                if (uindex >= qty) {
                                    status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid index for M register
                                    goto end;
                                }
                                break;
                            case LADDER_REGISTER_Cd:
                            case LADDER_REGISTER_Cr:
                            case LADDER_REGISTER_C:
                                qty = (*ladder_ctx).ladder.quantity.c;
                                if (uindex >= qty) {
                                    status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid index for C register
                                    goto end;
                                }
                                break;
                            case LADDER_REGISTER_Td:
                            case LADDER_REGISTER_Tr:
                            case LADDER_REGISTER_T:
                                qty = (*ladder_ctx).ladder.quantity.t;
                                if (uindex >= qty) {
                                    status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid index for T register
                                    goto end;
                                }
                                break;
                            case LADDER_REGISTER_D:
                                qty = (*ladder_ctx).ladder.quantity.d;
                                if (uindex >= qty) {
                                    status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid index for D register
                                    goto end;
                                }
                                break;
                            case LADDER_REGISTER_R:
                                qty = (*ladder_ctx).ladder.quantity.r;
                                if (uindex >= qty) {
                                    status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid index for R register
                                    goto end;
                                }
                                break;
                            case LADDER_REGISTER_S:
                                // String constant; no index check needed.
                                break;
                            default:
                                // Unknown or invalid register type.
                                status.error = LADDER_ERR_PRG_CHECK_FAIL;
                                goto end;
                        }
                    }

                    // Instruction-specific register type compatibility validation.
                    bool valid_type = false;
                    switch (status.code) {
                        case LADDER_INS_NO:
                        case LADDER_INS_NC:
                        case LADDER_INS_RE:
                        case LADDER_INS_FE:
                            if (d == 0) { // Operand for contact instructions
                                if (reg_type == LADDER_REGISTER_NONE || reg_type == LADDER_REGISTER_I || reg_type == LADDER_REGISTER_Q
                                        || reg_type == LADDER_REGISTER_M || reg_type == LADDER_REGISTER_Cd || reg_type == LADDER_REGISTER_Cr
                                        || reg_type == LADDER_REGISTER_Td || reg_type == LADDER_REGISTER_Tr) {
                                    valid_type = true;
                                }
                            }
                            break;
                        case LADDER_INS_COIL:
                        case LADDER_INS_COILL:
                        case LADDER_INS_COILU:
                            if (d == 0) {
                                if (reg_type == LADDER_REGISTER_Q || reg_type == LADDER_REGISTER_M) {
                                    valid_type = true;
                                }
                            }
                            break;
                        case LADDER_INS_TON:
                        case LADDER_INS_TOF:
                        case LADDER_INS_TP:
                            valid_type = true;
                            break;
                        case LADDER_INS_ADD:
                        case LADDER_INS_SUB:
                        case LADDER_INS_MUL:
                        case LADDER_INS_DIV:
                        case LADDER_INS_MOD:
                        case LADDER_INS_SHL:
                        case LADDER_INS_SHR:
                        case LADDER_INS_ROL:
                        case LADDER_INS_ROR:
                        case LADDER_INS_AND:
                        case LADDER_INS_OR:
                        case LADDER_INS_XOR:
                        case LADDER_INS_NOT:
                        case LADDER_INS_EQ:
                        case LADDER_INS_GT:
                        case LADDER_INS_GE:
                        case LADDER_INS_LT:
                        case LADDER_INS_LE:
                        case LADDER_INS_NE:
                            // Operands must be numeric types (e.g., D, R, constants, etc.)
                            if (reg_type == LADDER_REGISTER_D || reg_type == LADDER_REGISTER_R || reg_type == LADDER_REGISTER_NONE
                                    || reg_type == LADDER_REGISTER_C || reg_type == LADDER_REGISTER_IW || reg_type == LADDER_REGISTER_QW) {
                                valid_type = true;
                            }
                            break;
                        case LADDER_INS_MOVE:
                        case LADDER_INS_TMOVE:
                            // Similar to arithmetic, but allow string for TMOV if applicable
                            if (reg_type == LADDER_REGISTER_D || reg_type == LADDER_REGISTER_R || reg_type == LADDER_REGISTER_NONE
                                    || reg_type == LADDER_REGISTER_C || reg_type == LADDER_REGISTER_IW || reg_type == LADDER_REGISTER_QW
                                    || reg_type == LADDER_REGISTER_S) {
                                valid_type = true;
                            }
                            break;
                        case LADDER_INS_CTU:
                        case LADDER_INS_CTD:
                            if (d == 0) {
                                if (reg_type == LADDER_REGISTER_C) {
                                    valid_type = true;
                                }
                            } else if (d == 1) {
                                if (reg_type == LADDER_REGISTER_NONE || reg_type == LADDER_REGISTER_D) {
                                    valid_type = true;
                                }
                            }
                            break;
                        default:
                            if (status.code >= LADDER_INS_INV) {
                                status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid instruction code
                                goto end;
                            }
                            valid_type = true; // Assume valid for unhandled instructions
                            break;
                    }
                    if (!valid_type) {
                        status.error = LADDER_ERR_PRG_CHECK_FAIL;
                        goto end;
                    }
                }

                // After validating the primary cell, compute and set expected_multi for multi-cell instructions.
                // This includes handling FOREIGN by fetching its description.
                if (status.code != LADDER_INS_NOP && status.code != LADDER_INS_CONN && status.code < LADDER_INS_INV) { // Skip trivial single-cells.
                    ladder_instructions_iocd_t iocd;
                    if (status.code == LADDER_INS_FOREIGN) {
                        if ((*ladder_ctx).network[nt].cells[row][column].data_qty < 1) {
                            status.error = LADDER_ERR_PRG_CHECK_FAIL; // FOREIGN missing ID data.
                            goto end;
                        }
                        uint32_t fid = (*ladder_ctx).network[nt].cells[row][column].data[0].value.u32;
                        if (fid >= (*ladder_ctx).foreign.qty) {
                            status.error = LADDER_ERR_PRG_CHECK_FAIL; // Invalid FOREIGN ID.
                            goto end;
                        }
                        iocd = (*ladder_ctx).foreign.fn[fid].description;
                    } else {
                        iocd = ladder_fn_iocd[status.code];
                    }

                    // Check if multi-cell would exceed rows.
                    if (row + iocd.cells > (*ladder_ctx).network[nt].rows) {
                        status.error = LADDER_ERR_PRG_CHECK_FAIL; // Multi-cell overflows network rows.
                        goto end;
                    }

                    expected_multi = iocd.cells - 1;
                }
            }
        }

    end:
    return status;
}
