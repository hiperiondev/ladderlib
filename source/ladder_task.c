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

#include "ladder.h"
#include "ladder_internals.h"

// ladder logic execution task
void ladder_task(void *ladderctx) {
    ladder_ctx_t *ladder_ctx = (ladder_ctx_t*) ladderctx;
    if ((*ladder_ctx).io.micros == NULL || (*ladder_ctx).io.delay == NULL) {
        (*ladder_ctx).ladder.status = LADDER_ST_NULLFN;
        goto exit;
    }

    // task main loop
    while ((*ladder_ctx).ladder.status != LADDER_ST_EXIT_TSK) {

        if ((*ladder_ctx).ladder.status != LADDER_ST_RUNNING) {
#ifdef DEBUG
            printf("------------ PANIC ------------\n");
            printf("       STATUS: %s (%d)\n", ladder_status_str[(*ladder_ctx).ladder.status], (*ladder_ctx).ladder.status);
            printf("  INSTRUCTION: %s (%d)\n", fn_str[(*ladder_ctx).ladder.last_instr], (*ladder_ctx).ladder.last_instr);
            printf("      NETWORK: %d\n", (*ladder_ctx).ladder.last_instr_network);
            printf("         CELL: %d, %d\n", (*ladder_ctx).ladder.last_instr_cell_row, (*ladder_ctx).ladder.last_instr_cell_column);
            printf("        ERROR: %s (%d)\n", fn_err_str[(*ladder_ctx).ladder.last_instr_err], (*ladder_ctx).ladder.last_instr_err);
            printf("-------------------------------\n\n");
#endif
            if ((*ladder_ctx).io.panic != NULL)
                (*ladder_ctx).io.panic(ladder_ctx);

            goto exit;
        }

        while ((*ladder_ctx).ladder.status != LADDER_ST_RUNNING) {
            if ((*ladder_ctx).ladder.status == LADDER_ST_EXIT_TSK)
                return;
        }

        // external function (ex. for online edit or events)
        if ((*ladder_ctx).io.external_on_task != NULL)
            (*ladder_ctx).io.external_on_task(ladder_ctx);

        // ladder program scan
        ladder_scan_time(ladder_ctx);

        if ((*ladder_ctx).io.read_inputs_local != NULL)
            (*ladder_ctx).io.read_inputs_local(ladder_ctx);

        if ((*ladder_ctx).io.read_inputs_remote != NULL)
            (*ladder_ctx).io.read_inputs_remote(ladder_ctx);

        ladder_scan(ladder_ctx);
        ladder_save_previous_values(ladder_ctx);

        if ((*ladder_ctx).io.write_outputs_local != NULL)
            (*ladder_ctx).io.write_outputs_local(ladder_ctx);

        if ((*ladder_ctx).io.write_outputs_remote != NULL)
            (*ladder_ctx).io.write_outputs_remote(ladder_ctx);
    }

    exit:
    if ((*ladder_ctx).io.end_task != NULL)
        (*ladder_ctx).io.end_task();
}
