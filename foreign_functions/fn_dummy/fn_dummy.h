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

#ifndef _FN_DUMMY_H_
#define _FN_DUMMY_H_

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

/**
 * @struct fn_dummy_value_s
 * @brief
 *
 */
typedef struct fn_dummy_value_s {
    uint32_t qty;   /**< Dummy values quantity */
      float *value; /**< Dummy values */
} fn_dummy_value_t;

/**
 * @fn ladder_ins_err_t dummy_exec(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Function implementation
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t dummy_exec(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn bool dummy_init(ladder_foreign_function_t *function, uint32_t id, void *data, uint32_t qty)
 * @brief Initialize function
 *
 * @param function Function container
 * @param data Initialization data
 * @param qty Quantity of internal data
 * @return Status
 */
bool dummy_init(ladder_foreign_function_t *function, void *data, uint32_t qty);

/**
 * @fn bool dummy_deinit(ladder_foreign_function_t *function)
 * @brief Deinitialize function
 *
 * @param function Function container
 * @return Status
 */
bool dummy_deinit(ladder_foreign_function_t *function);

#endif /* FN_DUMMY_H_ */
