#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    return LADDER_INS_ERR_OK;
}

