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
#include <stdio.h>

#include "ladder.h"
#include "ladder_internals.h"

//--------------------------------------------------------------------------------
// Ladder logic execution Task
//--------------------------------------------------------------------------------
void ladder_task(void *pvParameters) {
    plc_ctx_t *plc_ctx = (plc_ctx_t*) pvParameters;

    // task lock / unlock
    ladder_wait_settings(plc_ctx);

    // ladder logic initializations
    ladder_clear_program(plc_ctx, (*plc_ctx).networks);
    (*plc_ctx).io.fn.configure_local_io(plc_ctx);

    // task main loop
    while (1) {
        // load saved program indicated in settings.ladder.user_program
        //    if file doesn't exist creates the empty file
        //    if user program number is 0, load demo to this slot
        //    assuming that it is the first boot.
        //
        //    Issue #19 : to validate the size of program file before to load it to ram
        //                if size is invalid, re-generate file and send plc to error
        if ((*plc_ctx).internals.load_selected_program) {
            (*plc_ctx).io.fn.load_program(DEFAULT_PROGRAM);
        }

        // for online edit
        (*plc_ctx).io.fn.external_on_task(plc_ctx);

        // ladder program scan
        //     read inputs
        //     evaluate logic
        //     write outputs
        ladder_scan_time(plc_ctx);

        (*plc_ctx).io.fn.read_inputs_local(plc_ctx);
        (*plc_ctx).io.fn.read_inputs_remote(plc_ctx);

        ladder_scan_plc(plc_ctx, (*plc_ctx).networks);
        ladder_save_previous_values(plc_ctx);

        (*plc_ctx).io.fn.write_outputs_local(plc_ctx);
        (*plc_ctx).io.fn.write_outputs_remote(plc_ctx);
    }
}
