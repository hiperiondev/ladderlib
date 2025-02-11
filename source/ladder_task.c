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
    if ((*ladder_ctx).hw.time.millis == NULL || (*ladder_ctx).hw.time.delay == NULL || (*ladder_ctx).hw.io.read_inputs_local == NULL
            || (*ladder_ctx).hw.io.write_outputs_local == NULL) {
        (*ladder_ctx).ladder.state = LADDER_ST_NULLFN;
        goto exit;
    }

    // task main loop
    while ((*ladder_ctx).ladder.state != LADDER_ST_EXIT_TSK) {

        if ((*ladder_ctx).ladder.state != LADDER_ST_RUNNING) {
            if ((*ladder_ctx).hw.manage.on_panic != NULL)
                (*ladder_ctx).hw.manage.on_panic(ladder_ctx);

            goto exit;
        }

        while ((*ladder_ctx).ladder.state != LADDER_ST_RUNNING) {
            if ((*ladder_ctx).ladder.state == LADDER_ST_EXIT_TSK)
                return;
        }

        // external function before scan
        if ((*ladder_ctx).hw.manage.on_task_before != NULL)
            (*ladder_ctx).hw.manage.on_task_before(ladder_ctx);

        // ladder program scan
        ladder_scan_time(ladder_ctx);

        if ((*ladder_ctx).hw.io.read_inputs_local != NULL)
            (*ladder_ctx).hw.io.read_inputs_local(ladder_ctx);

        if ((*ladder_ctx).hw.io.read_inputs_remote != NULL)
            (*ladder_ctx).hw.io.read_inputs_remote(ladder_ctx);

        ladder_scan(ladder_ctx);
        ladder_save_previous_values(ladder_ctx);

        if ((*ladder_ctx).hw.io.write_outputs_local != NULL)
            (*ladder_ctx).hw.io.write_outputs_local(ladder_ctx);

        if ((*ladder_ctx).hw.io.write_outputs_remote != NULL)
            (*ladder_ctx).hw.io.write_outputs_remote(ladder_ctx);

        // external function after scan
        if ((*ladder_ctx).hw.manage.on_task_after != NULL)
            (*ladder_ctx).hw.manage.on_task_after(ladder_ctx);
    }

    exit:
    if ((*ladder_ctx).hw.manage.on_end_task != NULL)
        (*ladder_ctx).hw.manage.on_end_task();
}
