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

ladder_ins_err_t fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // detect rising edge to start pulse
    if (CELL_STATE_LEFT(ladder_ctx, column, row) && !ladder_get_previous_value(ladder_ctx, row, column, 0)
            && !(*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp = (*ladder_ctx).hw.time.millis();
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                / basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]);
    }

    // timer done --> deactivate
    if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                    >= ((*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32
                            * basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]))) {
        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;

        (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc =
                (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
    }

    CELL_STATE(ladder_ctx, column, row + 1) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    // Set the state to Tr every scan to persist across clears
    CELL_STATE(ladder_ctx, column, row) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    return LADDER_INS_ERR_OK;
}
