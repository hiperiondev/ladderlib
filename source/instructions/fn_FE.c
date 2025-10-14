#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    MAKE_BOOL(
            !ladder_get_data_value(ladder_ctx, row, column, 0)
            && ladder_get_previous_value(ladder_ctx, row, column, 0)) && CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}
