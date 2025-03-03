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
#include <string.h>
#include <stdlib.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"
#include "fn_dummy.h"

ladder_instructions_iocd_t dummy_description = { 3, 3, 3, 3 };

bool dummy_init(ladder_foreign_function_t *function, void *data, uint32_t qty) {
    if (qty == 0)
        qty = 1;

    strcpy((*function).name, "DMY");

    memcpy(&((*function).description), &dummy_description, sizeof(ladder_instructions_iocd_t));

    (*function).deinit = dummy_deinit;
    (*function).exec = dummy_exec;
    (*function).data = calloc(qty, sizeof(float));

    return true;
}

ladder_ins_err_t dummy_exec(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

bool dummy_deinit(ladder_foreign_function_t *function) {
    free((*function).data);

    return true;
}

