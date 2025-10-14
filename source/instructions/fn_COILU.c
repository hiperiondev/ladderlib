#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_COILU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    uint8_t error = 0;

    bool state = CELL_STATE_LEFT(ladder_ctx, column, row);
    bool prev = ladder_get_previous_value(ladder_ctx, row, column, 0); // From history (Mh/Qh)
    uint32_t val = (prev && !state) ? 1 : 0;

    CELL_STATE(ladder_ctx, column, row) = (bool) val; // Power flow = latched output

    ladder_set_data_value(ladder_ctx, row, column, 0, (void*) &val, &error);

    return LADDER_INS_ERR_OK;
}
