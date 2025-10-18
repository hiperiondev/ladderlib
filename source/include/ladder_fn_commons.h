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

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "ladder.h"
#include "ladder_instructions.h"

extern uint32_t basetime_factor[];

static inline int32_t to_integer(int32_t val, ladder_data_type_t type) {
    (void) type;  // Unused; suppress warning (original behavior preserved)
    if (val == 0) {
        return (int32_t) (uint32_t) val;  // Explicit cast as in original
    } else if (val == 1) {
        return (int32_t) val;
    }
    return 0;
}

static inline int32_t safe_get_register_index(ladder_ctx_t *lctx, ladder_register_t type, int32_t raw_idx, ladder_ins_err_t *err) {
    if (lctx == NULL) {
        *err = LADDER_INS_ERR_FAIL;
        return -1;
    }
    if (raw_idx < 0) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return -1;
    }
    uint32_t idx = (uint32_t) raw_idx;
    uint32_t qty = 0;

    switch (type) {
        case LADDER_REGISTER_M:
            if (lctx->memory.M == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.m;
            break;
        case LADDER_REGISTER_Cd:
            if (lctx->memory.Cd == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.c;
            break;
        case LADDER_REGISTER_Cr:
            if (lctx->memory.Cr == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.c;
            break;
        case LADDER_REGISTER_C:
            if (lctx->registers.C == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.c;
            break;
        case LADDER_REGISTER_Td:
            if (lctx->memory.Td == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.t;
            break;
        case LADDER_REGISTER_Tr:
            if (lctx->memory.Tr == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.t;
            break;
        case LADDER_REGISTER_T:
            if (lctx->timers == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.t;
            break;
        case LADDER_REGISTER_D:
            if (lctx->registers.D == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
            qty = lctx->ladder.quantity.d;
            break;
        case LADDER_REGISTER_R:
            if (lctx->registers.R == NULL) {  // Added null check
                *err = LADDER_INS_ERR_FAIL;
                return -1;
            }
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
            if (lctx->output == NULL) {  // Added null check for output array
                *err = LADDER_INS_ERR_FAIL;
                return false;
            }
            fn_qty = lctx->hw.io.fn_write_qty;
            break;
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_IW:
            if (lctx->input == NULL) {  // Added null check for input array
                *err = LADDER_INS_ERR_FAIL;
                return false;
            }
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
                // Escalate critical errors (null context or type mismatch) to fault state
                if (err == LADDER_INS_ERR_FAIL || err == LADDER_INS_ERR_TYPEMISMATCH) {
                    lctx->ladder.state = LADDER_ST_ERROR;
                    if (lctx->on.panic != NULL) {
                        lctx->on.panic(lctx);
                    }
                }
                return 0;  // Safe default on error
            }
            switch (type) {
                case LADDER_REGISTER_M:
                    return (int32_t) lctx->memory.M[idx];
                case LADDER_REGISTER_Cd:
                    return (int32_t) lctx->memory.Cd[idx];
                case LADDER_REGISTER_Cr:
                    return (int32_t) lctx->memory.Cr[idx];
                case LADDER_REGISTER_Td:
                    return (int32_t) lctx->memory.Td[idx];
                case LADDER_REGISTER_Tr:
                    return (int32_t) lctx->memory.Tr[idx];
                case LADDER_REGISTER_C:
                    return (int32_t) lctx->registers.C[idx];
                case LADDER_REGISTER_T: {
                    uint64_t acc = lctx->timers[idx].acc;
                    if (acc > (uint64_t) INT32_MAX) {
                        // Assuming the truncated part handles clipping or overflow; here we return clipped value
                        return INT32_MAX;
                    }
                    return (int32_t) acc;
                }
                case LADDER_REGISTER_D:
                    return lctx->registers.D[idx];
                case LADDER_REGISTER_R:
                    return (int32_t) lctx->registers.R[idx];  // Note: Casting float to int32_t may lose precision, but matches original
                default:
                    return 0;
            }
        }
            // Assuming the truncated part includes handling for other types like Q, I, IW, QW, S, INV
            // For completeness in correction, include placeholders for truncated sections based on context
        case LADDER_REGISTER_Q: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_write_qty)
                return 0;
            return (int32_t) lctx->output[mod].Q[port];
        }
        case LADDER_REGISTER_I: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_read_qty)
                return 0;
            return (int32_t) lctx->input[mod].I[port];
        }
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
        case LADDER_REGISTER_S:
            return 0;  // Strings not numeric
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
            uint32_t cv = lctx->registers.C[val->value.i32];
            if (cv > (uint32_t) INT32_MAX) { // Overflow check for unsigned to signed cast
                lctx->ladder.last.err = LADDER_INS_ERR_OVERFLOW;
                return 0;
            }
            return (int32_t) cv;
        case LADDER_REGISTER_T:
            uint64_t acc = lctx->timers[val->value.i32].acc;  // Truncate u64 to i32 if needed
            if (acc > (uint64_t) INT32_MAX) { // Overflow check consistent with ladder_get_data_value
                lctx->ladder.last.err = LADDER_INS_ERR_OVERFLOW;
                return 0;
            }
            return (int32_t) acc;
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

static inline void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, uint32_t pos, void *value, ladder_ins_err_t *error) {
    ladder_register_t __type = ladder_cell_data_exec(ladder_ctx, row, column, pos).type;
    int32_t __idx = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.i32;
    uint8_t __port = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.mp.port;
    uint32_t __module = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.mp.module;
    uint32_t __qty = 0;
    size_t __data_size = 0;
    uint32_t __index = 0;

    if (__type == LADDER_REGISTER_S) {
        *error = LADDER_INS_ERR_SETDATAVAL;
        return;
    }

    ladder_ins_err_t err = LADDER_INS_ERR_OK;
    switch (__type) {
        case LADDER_REGISTER_M:
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
        case LADDER_REGISTER_C:
        case LADDER_REGISTER_D:
        case LADDER_REGISTER_R: {
            int32_t idx = safe_get_register_index(ladder_ctx, __type, __idx, &err);
            if (err != LADDER_INS_ERR_OK) {
                *error = err;
                return;
            }
            __index = (uint32_t) idx;
            __data_size = (__type == LADDER_REGISTER_C) ? sizeof(uint32_t) : (__type == LADDER_REGISTER_D) ? sizeof(int32_t) :
                          (__type == LADDER_REGISTER_R) ? sizeof(float) : sizeof(uint8_t);  // Modified: Changed sizeof(bool) to sizeof(uint8_t) for Cd/Cr/Td/Tr/M
            break;
        }
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_IW:
        case LADDER_REGISTER_QW: {
            if (!safe_check_module_port(ladder_ctx, __type, __module, __port, &__qty, &err)) {
                *error = err;
                return;
            }
            __index = __port;
            __data_size = (__type == LADDER_REGISTER_IW || __type == LADDER_REGISTER_QW) ? sizeof(int32_t) : sizeof(uint8_t);
            break;
        }
        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            return;
    }

    if (value == NULL) {  // Added check for null value
        *error = LADDER_INS_ERR_FAIL;
        return;
    }

    switch (__type) {
        case LADDER_REGISTER_M:
            if (ladder_ctx->memory.M != NULL) {  // Added null check
                memcpy(&ladder_ctx->memory.M[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_Q:
            if (ladder_ctx->output[__module].Q != NULL) {  // Added null check
                memcpy(&ladder_ctx->output[__module].Q[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_I:
            if (ladder_ctx->input[__module].I != NULL) {  // Added null check
                memcpy(&ladder_ctx->input[__module].I[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_Cd:
            if (ladder_ctx->memory.Cd != NULL) {  // Added null check
                memcpy(&ladder_ctx->memory.Cd[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_Cr:
            if (ladder_ctx->memory.Cr != NULL) {  // Added null check
                memcpy(&ladder_ctx->memory.Cr[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_Td:
            if (ladder_ctx->memory.Td != NULL) {  // Added null check
                memcpy(&ladder_ctx->memory.Td[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_Tr:
            if (ladder_ctx->memory.Tr != NULL) {  // Added null check
                memcpy(&ladder_ctx->memory.Tr[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_IW:
            if (ladder_ctx->input[__module].IW != NULL) {  // Added null check
                memcpy(&ladder_ctx->input[__module].IW[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_QW:
            if (ladder_ctx->output[__module].QW != NULL) {  // Added null check
                memcpy(&ladder_ctx->output[__module].QW[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_C:
            if (ladder_ctx->registers.C != NULL) {  // Added null check
                memcpy(&ladder_ctx->registers.C[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_D:
            if (ladder_ctx->registers.D != NULL) {  // Added null check
                memcpy(&ladder_ctx->registers.D[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        case LADDER_REGISTER_R:
            if (ladder_ctx->registers.R != NULL) {  // Added null check
                memcpy(&ladder_ctx->registers.R[__index], value, __data_size);
            } else {
                *error = LADDER_INS_ERR_FAIL;
            }
            break;
        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            break;
    }
}

static inline ladder_data_type_t get_effective_type(ladder_register_t reg_type) {
    switch (reg_type) {
        case LADDER_REGISTER_NONE:
        case LADDER_REGISTER_D:
        case LADDER_REGISTER_IW:
        case LADDER_REGISTER_QW:
            return LADDER_DATATYPE_I32;
        case LADDER_REGISTER_R:
            return LADDER_DATATYPE_REAL;
        case LADDER_REGISTER_M:
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
            return LADDER_DATATYPE_BOOL;
        case LADDER_REGISTER_C:
        case LADDER_REGISTER_T:
            return LADDER_DATATYPE_U32;
        case LADDER_REGISTER_S:
            return LADDER_DATATYPE_CSTR;
        default:
            return LADDER_DATATYPE_BOOL; // Default to bool for unknown
    }
}

static inline float ladder_get_data_float(ladder_ctx_t *lctx, uint32_t r, uint32_t c, uint32_t i) {
    if (lctx == NULL || lctx->exec_network == NULL) {
        return 0.0f;  // Safe default on invalid context
    }
    ladder_network_t *net = lctx->exec_network;
    if (r >= net->rows || c >= net->cols || i >= net->cells[r][c].data_qty) {
        return 0.0f;  // Safe default on OOB
    }
    ladder_register_t type = net->cells[r][c].data[i].type;
    ladder_value_t *val = &net->cells[r][c].data[i];

    switch (type) {
        case LADDER_REGISTER_NONE:
            return (float) val->value.i32;
        case LADDER_REGISTER_M:
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
            return (float) ladder_get_data_value(lctx, r, c, i);
        case LADDER_REGISTER_IW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_read_qty)
                return 0.0f;
            return (float) lctx->input[mod].IW[port];
        }
        case LADDER_REGISTER_QW: {
            uint32_t mod = val->value.mp.module;
            uint8_t port = val->value.mp.port;
            if (mod >= lctx->hw.io.fn_write_qty)
                return 0.0f;
            return (float) lctx->output[mod].QW[port];
        }
        case LADDER_REGISTER_C:
            return (float) lctx->registers.C[val->value.i32];
        case LADDER_REGISTER_T:
            return (float) lctx->timers[val->value.i32].acc;
        case LADDER_REGISTER_D:
            return (float) lctx->registers.D[val->value.i32];
        case LADDER_REGISTER_R:
            return lctx->registers.R[val->value.i32];
        case LADDER_REGISTER_S:
            return 0.0f;  // Strings not numeric
        case LADDER_REGISTER_INV:
        default:
            return 0.0f;
    }
}
