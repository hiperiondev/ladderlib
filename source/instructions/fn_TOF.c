#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_TOF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // input active --> reset
    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = 0;

        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
        (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
    } else {
        // timer is activated on falling edge, set timer running flag and snapshot the timestamp
        if (!(*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
                && !(*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
            (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

            (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp = (*ladder_ctx).hw.time.millis();
        }

        // timer is running, update acc value
        if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
            (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                    - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                    / basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]);
        }

        // timer done --> activate timer done flag and set acc value to his set point
        if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
                && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                        >= ((*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32
                                * basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]))) {
            (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;

            (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

            (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc =
                    (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
        }
    }

    CELL_STATE(ladder_ctx, column, row + 1) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    // Set the state to input || Tr every scan to persist across clears
    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row)
            || (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    return LADDER_INS_ERR_OK;
}
