/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *    ccronexpr (https://github.com/staticlibs/ccronexpr)
 *
 *    please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
