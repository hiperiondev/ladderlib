#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    int32_t val = ladder_get_data_value(ladder_ctx, row, column, 0);
    int32_t amount = ladder_get_data_value(ladder_ctx, row, column, 1);
    uint32_t uval = (uint32_t) val;
    amount %= 32;
    if (amount < 0)
        amount += 32;
    uval = (uval >> amount) | (uval << (32 - amount));
    val = (int32_t) uval;

    uint8_t err = LADDER_INS_ERR_OK;
    ladder_set_data_value(ladder_ctx, row, column, 0, &val, &err);

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    return err;
}
