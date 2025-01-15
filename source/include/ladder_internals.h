/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
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

/**
 * @fn void ladder_clear_memory(ladder_ctx_t *ladder_ctx)
 * @brief Deletes PLC Memory Areas
 *
 * @param ladder_ctx
 */
void ladder_clear_memory(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_scan_time(ladder_ctx_t *ladder_ctx)
 * @brief Calculate Scan Time in us
 *
 * @param ladder_ctx
 */
void ladder_scan_time(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_scan_plc(ladder_ctx_t *ladder_ctx, ladder_network_t networks[])
 * @brief ladder logic execution (scan)
 *
 * @param ladder_ctx
 * @param networks
 */
void ladder_scan(ladder_ctx_t *ladder_ctx);

/**
 * @fn void ladder_save_previous_values(ladder_ctx_t *ladder_ctx)
 * @brief Copy values to history for RE and FE instructions
 *
 * @param ladder_ctx
 */
void ladder_save_previous_values(ladder_ctx_t *ladder_ctx);

/**
 * @fn ladder_ins_err_t ladder_get_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int *value)
 * @brief
 *
 * @param ladder_ctx
 * @param row
 * @param column
 * @param value
 * @return
 */
ladder_ins_err_t ladder_get_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int *value);

/**
 * @fn ladder_ins_err_t ladder_get_previous_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int *value)
 * @brief
 *
 * @param ladder_ctx
 * @param row
 * @param column
 * @param value
 * @return
 */
ladder_ins_err_t ladder_get_previous_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int *value);

/**
 * @fn void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int value)
 * @brief
 *
 * @param ladder_ctx
 * @param row
 * @param column
 * @param value
 */
void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int value);

#endif /* LADDER_INTERNALS_H */
