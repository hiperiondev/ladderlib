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

#ifndef LADDER_INSTRUCTIONS_H
#define LADDER_INSTRUCTIONS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ladder.h"

extern uint32_t basetime_factor[];

static inline int32_t to_integer(int32_t val, ladder_data_type_t type) {
    (void)type;  // Unused; suppress warning (original behavior preserved)
    if (val == 0) {
        return (int32_t)(uint32_t)val;  // Explicit cast as in original
    } else if (val == 1) {
        return (int32_t)val;
    }
    return 0;
}

#define exnet(lctx)  (*(*lctx).exec_network)

static inline int32_t safe_get_register_index(ladder_ctx_t *lctx, ladder_register_t type, int32_t raw_idx, ladder_ins_err_t *err) {
    if (raw_idx < 0) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return -1;
    }
    uint32_t idx = (uint32_t) raw_idx;
    uint32_t qty = 0;
    switch (type) {
        case LADDER_REGISTER_M:
            qty = lctx->ladder.quantity.m;
            break;
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_C:
            qty = lctx->ladder.quantity.c;
            break;
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
        case LADDER_REGISTER_T:
            qty = lctx->ladder.quantity.t;
            break;
        case LADDER_REGISTER_D:
            qty = lctx->ladder.quantity.d;
            break;
        case LADDER_REGISTER_R:
            qty = lctx->ladder.quantity.r;
            break;
        default:
            *err = LADDER_INS_ERR_TYPEMISMATCH;
            return -1;
    }
    if (idx >= qty) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return -1;
    }
    *err = LADDER_INS_ERR_OK;
    return (int32_t) idx;
}

static inline bool safe_check_module_port(ladder_ctx_t *lctx, ladder_register_t type, uint32_t mod, uint8_t port, uint32_t *qty_out, ladder_ins_err_t *err) {
    uint32_t fn_qty = 0;
    switch (type) {
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_QW:
            fn_qty = lctx->hw.io.fn_write_qty;
            break;
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_IW:
            fn_qty = lctx->hw.io.fn_read_qty;
            break;
        default:
            *err = LADDER_INS_ERR_TYPEMISMATCH;
            return false;
    }
    if (mod >= fn_qty) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }
    switch (type) {
        case LADDER_REGISTER_Q:
            *qty_out = lctx->output[mod].q_qty;
            break;
        case LADDER_REGISTER_QW:
            *qty_out = lctx->output[mod].qw_qty;
            break;
        case LADDER_REGISTER_I:
            *qty_out = lctx->input[mod].i_qty;
            break;
        case LADDER_REGISTER_IW:
            *qty_out = lctx->input[mod].iw_qty;
            break;
        default:
            break; // Unreachable
    }
    if (port >= *qty_out) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }
    *err = LADDER_INS_ERR_OK;
    return true;
}

static inline int32_t ladder_get_data_value(ladder_ctx_t *lctx, uint32_t r, uint32_t c, uint32_t i) {
    if (lctx == NULL || lctx->exec_network == NULL) {
        return 0;  // Safe default on invalid context
    }
    ladder_network_t *net = lctx->exec_network;
    if (r >= net->rows || c >= net->cols || i >= net->cells[r][c].data_qty) {
        return 0;  // Safe default on OOB
    }
    ladder_register_t type = net->cells[r][c].data[i].type;
    ladder_value_t *val = &net->cells[r][c].data[i];
    ladder_ins_err_t err = LADDER_INS_ERR_OK;

    switch (type) {
        case LADDER_REGISTER_NONE:
            return (int32_t) (uint32_t) val->value.i32;
        case LADDER_REGISTER_M:
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
        case LADDER_REGISTER_C:
        case LADDER_REGISTER_T:
        case LADDER_REGISTER_D:
        case LADDER_REGISTER_R: {
            int32_t idx = safe_get_register_index(lctx, type, val->value.i32, &err);
            if (err != LADDER_INS_ERR_OK) {
                lctx->ladder.last.err = err;
                return 0;  // Safe default on error
            }
            switch (type) {
                case LADDER_REGISTER_M: return (int32_t) lctx->memory.M[idx];
                case LADDER_REGISTER_Cd: return (int32_t) lctx->memory.Cd[idx];
                case LADDER_REGISTER_Cr: return (int32_t) lctx->memory.Cr[idx];
                case LADDER_REGISTER_Td: return (int32_t) lctx->memory.Td[idx];
                case LADDER_REGISTER_Tr: return (int32_t) lctx->memory.Tr[idx];
                case LADDER_REGISTER_C: return (int32_t) lctx->registers.C[idx];
                case LADDER_REGISTER_T: {
                    uint64_t acc = lctx->timers[idx].acc;
                    if (acc > (uint64_t) INT32_MAX) {
                        lctx->ladder.last.err = LADDER_INS_ERR_OVERFLOW;
                        return 0;
                    }
                    return (int32_t) acc;
                }
                case LADDER_REGISTER_D: return lctx->registers.D[idx];
                case LADDER_REGISTER_R: return (int32_t) lctx->registers.R[idx];
                default: return 0;
            }
        }
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_IW:
        case LADDER_REGISTER_QW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            uint32_t qty = 0;
            if (!safe_check_module_port(lctx, type, mod, port, &qty, &err)) {
                lctx->ladder.last.err = err;
                return 0;
            }
            switch (type) {
                case LADDER_REGISTER_Q: return (int32_t) lctx->output[mod].Q[port];
                case LADDER_REGISTER_I: return (int32_t) lctx->input[mod].I[port];
                case LADDER_REGISTER_IW: return lctx->input[mod].IW[port];
                case LADDER_REGISTER_QW: return lctx->output[mod].QW[port];
                default: return 0;
            }
        }
        case LADDER_REGISTER_S:
            return 0;  // Strings not numeric; default 0
        case LADDER_REGISTER_INV:
        default:
            return 0;
    }
}

static inline int32_t ladder_get_previous_value(ladder_ctx_t *lctx, uint32_t r, uint32_t c, uint32_t i) {
    if (lctx == NULL || lctx->exec_network == NULL) {
        return 0;  // Safe default on invalid context
    }
    ladder_network_t *net = lctx->exec_network;
    if (r >= net->rows || c >= net->cols || i >= net->cells[r][c].data_qty) {
        return 0;  // Safe default on OOB
    }
    ladder_register_t type = net->cells[r][c].data[i].type;
    ladder_value_t *val = &net->cells[r][c].data[i];

    switch (type) {
        case LADDER_REGISTER_M:
            return (int32_t) lctx->prev_scan_vals.Mh[val->value.i32];
        case LADDER_REGISTER_Q: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_write_qty) {
                return 0;
            }
            // Null check for Qh to prevent dereference of unallocated array
            if (lctx->output[mod].Qh == NULL) {
                return 0;  // Safe default if history not allocated
            }
            // Bounds check for port to prevent OOB access
            if (port >= lctx->output[mod].q_qty) {
                return 0;  // Safe default on invalid port
            }
            return (int32_t) lctx->output[mod].Qh[port];
        }
        case LADDER_REGISTER_I: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_read_qty) {
                return 0;
            }
            // Null check for Ih to prevent dereference of unallocated array
            if (lctx->input[mod].Ih == NULL) {
                return 0;  // Safe default if history not allocated
            }
            // Bounds check for port to prevent OOB access
            if (port >= lctx->input[mod].i_qty) {
                return 0;  // Safe default on invalid port
            }
            return (int32_t) lctx->input[mod].Ih[port];
        }
        case LADDER_REGISTER_Cd:
            return (int32_t) lctx->prev_scan_vals.Cdh[val->value.i32];
        case LADDER_REGISTER_Cr:
            return (int32_t) lctx->prev_scan_vals.Crh[val->value.i32];
        case LADDER_REGISTER_Td:
            return (int32_t) lctx->prev_scan_vals.Tdh[val->value.i32];
        case LADDER_REGISTER_Tr:
            return (int32_t) lctx->prev_scan_vals.Trh[val->value.i32];
            // For other types like IW/QW/C/T/D/R/S, previous values may not be stored (e.g., no prev for accumulators or data regs).
            // Default to current value or 0; adjust based on requirements (e.g., add prev for D if needed).
        case LADDER_REGISTER_IW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_read_qty)
                return 0;
            return lctx->input[mod].IW[port];  // No prev for words; use current
        }
        case LADDER_REGISTER_QW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_write_qty)
                return 0;
            return lctx->output[mod].QW[port];  // No prev for words; use current
        }
        case LADDER_REGISTER_C:
            return (int32_t) lctx->registers.C[val->value.i32];  // No prev; use current
        case LADDER_REGISTER_T:
            return (int32_t) lctx->timers[val->value.i32].acc;  // No prev acc; use current
        case LADDER_REGISTER_D:
            return lctx->registers.D[val->value.i32];  // No prev; use current
        case LADDER_REGISTER_R:
            return (int32_t) lctx->registers.R[val->value.i32];  // No prev; use current
        case LADDER_REGISTER_S:
            return 0;  // Strings not applicable for previous
        case LADDER_REGISTER_NONE:
        case LADDER_REGISTER_INV:
        default:
            return 0;
    }
}

static inline int32_t ladder_get_data_int32(ladder_ctx_t *lctx, uint32_t r, uint32_t c, uint32_t i) {
    if (lctx == NULL || lctx->exec_network == NULL) {
        return 0;  // Safe default on invalid context
    }
    ladder_network_t *net = lctx->exec_network;
    if (r >= net->rows || c >= net->cols || i >= net->cells[r][c].data_qty) {
        return 0;  // Safe default on OOB
    }
    ladder_register_t type = net->cells[r][c].data[i].type;
    ladder_value_t *val = &net->cells[r][c].data[i];

    switch (type) {
        case LADDER_REGISTER_NONE:
            return val->value.i32;
        case LADDER_REGISTER_M:
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
            // Bool types: Treat false=0, true=1
            return (int32_t) ladder_get_data_value(lctx, r, c, i);
        case LADDER_REGISTER_IW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_read_qty)
                return 0;
            return lctx->input[mod].IW[port];
        }
        case LADDER_REGISTER_QW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_write_qty)
                return 0;
            return lctx->output[mod].QW[port];
        }
        case LADDER_REGISTER_C:
            return (int32_t) lctx->registers.C[val->value.i32];
        case LADDER_REGISTER_T:
            return (int32_t) lctx->timers[val->value.i32].acc;  // Truncate u64 to i32 if needed
        case LADDER_REGISTER_D:
            return lctx->registers.D[val->value.i32];
        case LADDER_REGISTER_R:
            return (int32_t) lctx->registers.R[val->value.i32];
        case LADDER_REGISTER_S:
            return 0;  // Strings not numeric
        case LADDER_REGISTER_INV:
        default:
            return 0;
    }
}

/**
 * @def ladder_cell_data_exec
 * @brief
 *
 */
#define ladder_cell_data_exec(lctx, r, c, i)  (exnet(lctx).cells[r][c].data[i])
/**
 * @def ladder_get_type
 * @brief
 *
 */
#define ladder_get_type(lctx, r, c, i)  (exnet(lctx).cells[r][c].data[i].type)

/**
 * @def ladder_table_pos_row
 * @brief
 *
 */
#define ladder_table_pos_row(lctx, pos)  ((pos) / (*lctx).network[(*lctx).ladder.last.network].cols)

/**
 * @def ladder_table_pos_column
 * @brief
 *
 */
#define ladder_table_pos_column(lctx, pos)  ((pos) - (ladder_table_pos_row(lctx, pos) * (*lctx).network[(*lctx).ladder.last.network].cols))

/**
 * @def CELL_STATE
 * @brief Cell state
 *
 */
#define CELL_STATE(lctx, c, r)  (exnet(lctx).cells[r][c].state)

/**
 * @def CELL_STATE_LEFT
 * @brief Left cell state
 *
 */
#define CELL_STATE_LEFT(lctx, c, r)  \
    (c == 0 ? true : CELL_STATE(lctx, c - 1, r))

/**
 * @def MAKE_BOOL
 * @brief Ensure bool value
 *
 */
#define MAKE_BOOL(v)  ((v) == 0 ? false : true)

/**
 * @fn ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_Conn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Connector
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_CONN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_Neg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Negate
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NEG(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Normally open contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Normally closed contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief  Rise Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Fall Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_Coil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Coil
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 * @return
 */
ladder_ins_err_t fn_COIL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Latch coil
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 * @return
 */
ladder_ins_err_t fn_COILL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Unlatch coil
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 * @return
 */
ladder_ins_err_t fn_COILU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Timer on
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Timer off
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TOF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Timer pulse
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Counter up
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Counter down
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Register move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic subtraction
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic addition
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic multiplication
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic division
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic division module
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit shifting left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit shifting right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit rotate left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit rotate right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise AND
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise OR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise XOR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise NOT
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison equal to
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison greater than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison lesser than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison lesser or equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison not equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Execute external functions
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Execute table data move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

#endif /* LADDER_INSTRUCTIONS_H */
