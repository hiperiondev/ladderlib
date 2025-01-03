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

/**
 * @def PLC_INS_CELL_USED_MASK
 * @brief
 *
 */
#define PLC_INS_CELL_USED_MASK 0

/**
 * @def PLC_INS_CELL_CODE_MASK
 * @brief
 *
 */
#define PLC_INS_CELL_CODE_MASK 0

/**
 * @fn void ladder_wait_settings(plc_ctx_t *plc_ctx)
 * @brief Wait for tskDisk to load the User Settings
 *
 * @param plc_ctx
 */
void ladder_wait_settings(plc_ctx_t *plc_ctx);

/**
 * @fn void ladder_clear_memory(plc_ctx_t *plc_ctx)
 * @brief Deletes PLC Memory Areas
 *
 * @param plc_ctx
 */
void ladder_clear_memory(plc_ctx_t *plc_ctx);

/**
 * @fn void ladder_scan_time(plc_ctx_t *plc_ctx)
 * @brief Calculate Scan Time in us
 *
 * @param plc_ctx
 */
void ladder_scan_time(plc_ctx_t *plc_ctx);

/**
 * @fn void ladder_scan_plc(plc_ctx_t *plc_ctx, ladder_network_t networks[])
 * @brief ladder logic execution (scan)
 *
 * @param plc_ctx
 * @param networks
 */
void ladder_scan_plc(plc_ctx_t *plc_ctx, ladder_network_t networks[]);

/**
 * @fn void ladder_save_previous_values(plc_ctx_t *plc_ctx)
 * @brief Copy values to history for RE and FE instructions
 *
 * @param plc_ctx
 */
void ladder_save_previous_values(plc_ctx_t *plc_ctx);

/**
 * @fn int ladder_get_data_value(plc_ctx_t *plc_ctx, int r, int c)
 * @brief
 *
 * @param plc_ctx
 * @param r
 * @param c
 * @return
 */
int ladder_get_data_value(plc_ctx_t *plc_ctx, int r, int c);

/**
 * @fn int ladder_get_previous_value(plc_ctx_t *plc_ctx, int r, int c)
 * @brief
 *
 * @param plc_ctx
 * @param r
 * @param c
 * @return
 */
int ladder_get_previous_value(plc_ctx_t *plc_ctx, int r, int c);

/**
 * @fn void ladder_set_data_value(plc_ctx_t *plc_ctx, int r, int c, int Value)
 * @brief
 *
 * @param plc_ctx
 * @param r
 * @param c
 * @param Value
 */
void ladder_set_data_value(plc_ctx_t *plc_ctx, int r, int c, int Value);

/**
 * @fn void ladder_clear_program(plc_ctx_t *plc_ctx, ladder_network_t networks[])
 * @brief Deletes all Networks (all values to 0)
 *
 * @param plc_ctx
 * @param networks
 */
void ladder_clear_program(plc_ctx_t *plc_ctx, ladder_network_t networks[]);

#endif /* LADDER_INTERNALS_H */
