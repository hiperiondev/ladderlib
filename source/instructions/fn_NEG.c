#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_NEG(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) = !CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}
