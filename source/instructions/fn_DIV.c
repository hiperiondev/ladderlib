#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    uint8_t error = LADDER_INS_ERR_OK;
    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        int32_t divisor = ladder_get_data_value(ladder_ctx, row, column, 1);
        if (divisor == 0) {
            int32_t zero = 0;
            ladder_set_data_value(ladder_ctx, row, column, 2, &zero, &error);
        } else {
            int32_t val = ladder_get_data_value(ladder_ctx, row, column, 0) / divisor;
            ladder_set_data_value(ladder_ctx, row, column, 2, &val, &error);
        }
    }
    return LADDER_INS_ERR_OK;
}
