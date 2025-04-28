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

#ifndef LADDER_INTERNALS_H
#define LADDER_INTERNALS_H

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

/**
 * @fn void ladder_clear_memory(ladder_ctx_t *ladder_ctx)
 * @brief Delete memory areas
 *
 * @param ladder_ctx Ladder context
 */
void ladder_clear_memory(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_scan_time(ladder_ctx_t *ladder_ctx)
 * @brief Calculate scan time in us
 *
 * @param ladder_ctx Ladder context
 */
void ladder_scan_time(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_scan_plc(ladder_ctx_t *ladder_ctx)
 * @brief Execute ladder logic
 *
 * @param ladder_ctx Ladder context
 */
void ladder_scan(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_save_previous_values(ladder_ctx_t *ladder_ctx)
 * @brief Copy values to history
 *
 * @param ladder_ctx Ladder context
 */
void ladder_save_previous_values(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, void *value, uint8_t *error)
 * @brief Set values
 *
 * @param ladder_ctx Ladder context
 * @param row Ladder row
 * @param column Ladder column
 * @param pos Position in data values
 * @param value Value
 * @param error Error status
 * @return Status
 */
void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, uint32_t pos, void *value, uint8_t *error);

#endif /* LADDER_INTERNALS_H */
