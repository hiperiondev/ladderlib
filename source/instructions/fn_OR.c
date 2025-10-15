/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *
 *    please contact their authors for more information.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "fn_commons.h"
#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) | to_integer(ladder_get_data_value(ladder_ctx, row, column, 1), 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}
