#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = ~to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1);
        ladder_set_data_value(ladder_ctx, row, column, 1, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}
