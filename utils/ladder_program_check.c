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
    if (ladder_ctx == NULL)
        return LADDER_ERR_PRG_CHECK_FAIL;

    ladder_prg_check_t status;

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
        for (uint32_t column = 0; column < (*ladder_ctx).network[nt].cols; column++)
            for (uint32_t row = 0; row < (*ladder_ctx).network[nt].rows; row++)
                for (uint32_t d = 0; d < (*ladder_ctx).network[nt].cells[row][column].data_qty; d++) {
                    status.network = nt;
                    status.row = row;
                    status.column = column;
                    status.error = LADDER_ERR_PRG_CHECK_OK;
                    status.code = (*ladder_ctx).network[nt].cells[row][column].code;

                    ladder_register_t reg_type = (*ladder_ctx).network[nt].cells[row][column].data[d].type;

                    // Checks timer index (data[0]: must be REGISTER_T with valid index) and basetime/preset (data[1]: basetime enum 0-4, preset u32 >=0).
                    // This prevents enum overlaps from triggering false I/O errors and ensures timers are properly bounds-checked.
                    bool is_timer_data = false;
                    if (status.code == LADDER_INS_TON || status.code == LADDER_INS_TOF || status.code == LADDER_INS_TP) {
                        is_timer_data = true;
                        if (d == 0) { // Validate timer index
                            if (reg_type != LADDER_REGISTER_T) {
                                status.error = LADDER_ERR_PRG_CHECK_T_INV_TYPE;
                                goto end;
                            }
                            if ((*ladder_ctx).network[nt].cells[row][column].data[d].value.i32 >= (*ladder_ctx).ladder.quantity.t) {
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
                        for (size_t i = 0; i < num_checks; i++) {
                            if (reg_type == checks[i].type) {
                                uint32_t fn_qty = checks[i].is_input ? (*ladder_ctx).hw.io.fn_read_qty : (*ladder_ctx).hw.io.fn_write_qty;
                                uint32_t module = (*ladder_ctx).network[nt].cells[row][column].data[d].value.mp.module;
                                uint32_t port = (*ladder_ctx).network[nt].cells[row][column].data[d].value.mp.port;

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
                }

    end:
    return status;
}
