#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    if (to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 0) >= (*ladder_ctx).foreign.qty)
        return LADDER_INS_ERR_NOFOREIGN;

    return (*ladder_ctx).foreign.fn[to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 0)].exec(ladder_ctx, column, row);
}
