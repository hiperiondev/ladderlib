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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ccronexpr.h"
#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"
#include "fn_crontab.h"

ladder_ins_err_t cron_init(ladder_ctx_t *ladder_ctx, void *data) {
    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t cron_exec(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t cron_deinit(ladder_ctx_t *ladder_ctx) {
    return LADDER_INS_ERR_OK;
}

