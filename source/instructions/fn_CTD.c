#include <stdbool.h>
#include <stdint.h>

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
