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

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"

//--------------------------------------------------------------------------------
// Ladder logic execution task
//--------------------------------------------------------------------------------
void ladder_task(void *pvParameters) {
    ladder_ctx_t *ladder_ctx = (ladder_ctx_t*) pvParameters;

    // task main loop
    while ((*ladder_ctx).ladder.state != LADDER_ST_EXIT_TSK) {

        // load saved program indicated in settings.ladder.user_program
        //    if file doesn't exist creates the empty file
        //    if user program number is 0, load demo to this slot
        //    assuming that it is the first boot.
        //
        //    Issue #19 : to validate the size of program file before to load it to ram
        //                if size is invalid, re-generate file and send plc to error
        //if ((*ladder_ctx).internals.load_selected_program) {
        //    (*ladder_ctx).io.fn.load_program(DEFAULT_PROGRAM);
        //}

        // external function (ex. for online edit)
        if ((*ladder_ctx).io.external_on_task != NULL)
            (*ladder_ctx).io.external_on_task(ladder_ctx);

        // ladder program scan
        //     read inputs
        //     evaluate logic
        //     write outputs
        ladder_scan_time(ladder_ctx);

        (*ladder_ctx).io.read_inputs_local(ladder_ctx);
        (*ladder_ctx).io.read_inputs_remote(ladder_ctx);

        ladder_scan(ladder_ctx);
        ladder_save_previous_values(ladder_ctx);

        (*ladder_ctx).io.write_outputs_local(ladder_ctx);
        (*ladder_ctx).io.write_outputs_remote(ladder_ctx);
    }
}
