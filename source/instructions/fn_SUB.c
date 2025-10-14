#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);
    int32_t auxValue1 = ladder_get_data_value(ladder_ctx, row, column, 0);
    int32_t auxValue2 = ladder_get_data_value(ladder_ctx, row, column, 1);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        if (auxValue1 > auxValue2) {
            CELL_STATE(ladder_ctx, column, row) = true;
        } else if (auxValue1 == auxValue2) {
            CELL_STATE(ladder_ctx, column, row + 1) = true;
        } else {
            CELL_STATE(ladder_ctx, column, row + 2) = true;
        }

        uint32_t res = auxValue1 - auxValue2;
        ladder_set_data_value(ladder_ctx, row, column, 2, &res, &error);
    }

    return LADDER_INS_ERR_OK;
}
