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

#include "ladder_fn_commons.h"
#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] =
                    (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
            (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            CELL_STATE(ladder_ctx, column, row) = false;
            CELL_STATE(ladder_ctx, column, row + 1) = false;
        }
    } else {
        if (CELL_STATE_LEFT(ladder_ctx, column, row + 1)) {
            (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] =
                    (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
            (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            CELL_STATE(ladder_ctx, column, row) = false;
            CELL_STATE(ladder_ctx, column, row + 1) = false;
        }
    }

    // counter is activated in this scan, change count
    if (CELL_STATE_LEFT(ladder_ctx, column, row) && !(*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && !(*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;
        CELL_STATE(ladder_ctx, column, row + 1) = true;
        (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]--;
    }

    // reset counter edge detection
    if (!CELL_STATE_LEFT(ladder_ctx, column, row)) {
        (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
        CELL_STATE(ladder_ctx, column, row + 1) = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] == 0) {
        (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;
        CELL_STATE(ladder_ctx, column, row) = true;
    }

    return LADDER_INS_ERR_OK;
}
