#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) > ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}
