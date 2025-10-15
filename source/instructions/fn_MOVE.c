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

#include "fn_commons.h"
#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    ladder_ctx_t *lctx = ladder_ctx;
    ladder_ins_err_t err = LADDER_INS_ERR_OK;

    if (!exnet(lctx).cells[row][column].data || exnet(lctx).cells[row][column].data_qty < 2) {
        return LADDER_INS_ERR_OUTOFRANGE;
    }

    ladder_register_t src_type = exnet(lctx).cells[row][column].data[0].type;
    ladder_register_t dest_type = exnet(lctx).cells[row][column].data[1].type;

    // Define union for type-safe 32-bit copy (int32/float both 4 bytes)
    typedef union {
        int32_t i;
        float f;
    } move_val_t;

    move_val_t src_val = {0};
    bool is_float_src = false;
    bool is_int_src = false;

    // Fetch source value based on type (direct access like set_data to avoid macro cast)
    switch (src_type) {
        case LADDER_REGISTER_D:
        case LADDER_REGISTER_IW:
        case LADDER_REGISTER_QW:
        case LADDER_REGISTER_C:
        case LADDER_REGISTER_T:  // uint64_t but treat as i32 for move subset
            src_val.i = ladder_get_data_value(lctx, row, column, 0);  // Safe for ints
            is_int_src = true;
            break;
        case LADDER_REGISTER_R:
            // Direct fetch for float (macro casts wrong)
            {
                uint32_t idx = exnet(lctx).cells[row][column].data[0].value.i32;
                if (idx >= lctx->ladder.quantity.r) {
                    return LADDER_INS_ERR_OUTOFRANGE;
                }
                src_val.f = lctx->registers.R[idx];
                is_float_src = true;
            }
            break;
        default:
            return LADDER_INS_ERR_TYPEMISMATCH;
    }

    // Determine dest category
    bool is_float_dest = (dest_type == LADDER_REGISTER_R);
    bool is_int_dest = (dest_type == LADDER_REGISTER_D || dest_type == LADDER_REGISTER_IW ||
                        dest_type == LADDER_REGISTER_QW || dest_type == LADDER_REGISTER_C ||
                        dest_type == LADDER_REGISTER_T);

    // Check compatibility: both int or both float
    if ((is_int_src && !is_int_dest) || (is_float_src && !is_float_dest)) {
        return LADDER_INS_ERR_TYPEMISMATCH;
    }

    // Copy via union (preserves bits)
    move_val_t dest_val = src_val;

    // Set destination (use set_data for consistency, pass correct pointer/size)
    uint8_t set_err;
    void *val_ptr = is_float_dest ? (void *)&dest_val.f : (void *)&dest_val.i;  // MODIFIED: Casts to void* for ternary compatibility.
    ladder_set_data_value(lctx, row, column, 1, val_ptr, &set_err);
    if (set_err != LADDER_INS_ERR_OK) {
        err = LADDER_INS_ERR_SETDATAVAL;
    }

    // Propagate power flow
    CELL_STATE(lctx, column, row) = CELL_STATE_LEFT(lctx, column, row);

    return err;
}
