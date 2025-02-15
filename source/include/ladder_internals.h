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

#ifndef LADDER_INTERNALS_H
#define LADDER_INTERNALS_H

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_instructions.h"

#define LADDER_FLAG_MASK(n) (1L << (n))

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
 * @fn void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int value, uint8_t *error)
 * @brief Set values
 *
 * @param ladder_ctx Ladder context
 * @param row Ladder row
 * @param column Ladder column
 * @param value Value
 * @param error Error status
 * @return Status
 */
void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int value, uint8_t *error);

#endif /* LADDER_INTERNALS_H */
