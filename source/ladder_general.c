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
#include <stdlib.h>

#include "ladder.h"
#include "ladder_internals.h"

void ladder_scan_time(plc_ctx_t *plc_ctx) {
    unsigned long scanTimeMicros = micros();
    (*plc_ctx).internals.ladder_actual_scan_time = scanTimeMicros - (*plc_ctx).internals.ladder_start_time;
    (*plc_ctx).internals.ladder_start_time = scanTimeMicros;
}

void ladder_save_previous_values(plc_ctx_t *plc_ctx) {
    for (int i = 0; i < QTY_M; i++) {
        (*plc_ctx).prev_scan_vals.Mh[i] = (*plc_ctx).memory.M[i];
    }
    for (int i = 0; i < QTY_I; i++) {
        (*plc_ctx).prev_scan_vals.Ih[i] = (*plc_ctx).memory.I[i];
    }
    for (int i = 0; i < QTY_Q; i++) {
        (*plc_ctx).prev_scan_vals.Qh[i] = (*plc_ctx).memory.Q[i];
    }
    for (int i = 0; i < QTY_C; i++) {
        (*plc_ctx).prev_scan_vals.Crh[i] = (*plc_ctx).memory.Cr[i];
        (*plc_ctx).prev_scan_vals.Cdh[i] = (*plc_ctx).memory.Cd[i];
    }
    for (int i = 0; i < QTY_T; i++) {
        (*plc_ctx).prev_scan_vals.Tdh[i] = (*plc_ctx).memory.Td[i];
        (*plc_ctx).prev_scan_vals.Trh[i] = (*plc_ctx).memory.Tr[i];
    }
}

void ladder_clear_memory(plc_ctx_t *plc_ctx) {
    for (int i = 0; i < QTY_M; i++) {
        (*plc_ctx).prev_scan_vals.Mh[i] = 0;
        (*plc_ctx).memory.M[i] = 0;
    }
    for (int i = 0; i < QTY_I; i++) {
        (*plc_ctx).prev_scan_vals.Ih[i] = 0;
        (*plc_ctx).memory.I[i] = 0;
    }
    for (int i = 0; i < QTY_Q; i++) {
        (*plc_ctx).prev_scan_vals.Qh[i] = 0;
        (*plc_ctx).memory.Q[i] = 0;
    }
    for (int i = 0; i < QTY_C; i++) {
        (*plc_ctx).prev_scan_vals.Cdh[i] = 0;
        (*plc_ctx).memory.Cd[i] = 0;
        (*plc_ctx).prev_scan_vals.Crh[i] = 0;
        (*plc_ctx).memory.Cr[i] = 0;
    }
    for (int i = 0; i < QTY_T; i++) {
        (*plc_ctx).prev_scan_vals.Tdh[i] = 0;
        (*plc_ctx).memory.Td[i] = 0;
        (*plc_ctx).prev_scan_vals.Trh[i] = 0;
        (*plc_ctx).memory.Tr[i] = 0;
    }
}

void ladder_wait_settings(plc_ctx_t *plc_ctx) {
    printf("TaskLadder - Waiting for Unlock\n");
    while ((*plc_ctx).internals.boot_sequence != PLC_BOOT_TASK_UNLOCKED) {
        delay(10); // forces to update boot_sequence global variable
    }
    printf("TaskLadder - Task Unlocked\n");
}

void ladder_clear_program(plc_ctx_t *plc_ctx, ladder_network_t networks[]) {
    for (int n = 0; n < (*plc_ctx).ladder.networks_quantity; n++) {
        networks[n].bars[0] = 0;
        networks[n].bars[1] = 0;
        networks[n].bars[2] = 0;
        networks[n].bars[3] = 0;
        networks[n].bars[4] = 0;
        for (int c = 0; c < NET_COLUMNS; c++) {
            for (int r = 0; r < NET_ROWS; r++) {
                networks[n].cells[r][c].code = 0;
                networks[n].cells[r][c].data = 0;
                networks[n].cells[r][c].type = 0;
            }
        }
    }
    printf("TaskLadder - PROGRAM RAM CLEARED\n");
}

int ladder_ctx_init(plc_ctx_t *plc_ctx) {
    (*plc_ctx).internals.boot_sequence = PLC_BOOT_INIT;    //
    (*plc_ctx).internals.load_selected_program = 1;        // Force to load user program on boot
    (*plc_ctx).internals.update_selected_program_ram = 0;  //
    (*plc_ctx).internals.update_selected_program_disk = 0; //
    (*plc_ctx).internals.flags_mask[0] = 0x0001;           //
    (*plc_ctx).internals.flags_mask[1] = 0x0002;           //
    (*plc_ctx).internals.flags_mask[2] = 0x0004;           //
    (*plc_ctx).internals.flags_mask[3] = 0x0008;           //
    (*plc_ctx).internals.flags_mask[4] = 0x0010;           //
    (*plc_ctx).internals.flags_mask_and[0] = 0xFFFE;       //
    (*plc_ctx).internals.flags_mask_and[1] = 0xFFFD;       //
    (*plc_ctx).internals.flags_mask_and[2] = 0xFFFB;       //
    (*plc_ctx).internals.flags_mask_and[3] = 0x0007;       //
    (*plc_ctx).internals.flags_mask_and[4] = 0xFFEF;       //

    (*plc_ctx).io.fn.read_inputs_local = NULL;
    (*plc_ctx).io.fn.write_outputs_local = NULL;
    (*plc_ctx).io.fn.analog_in_filter = NULL;
    (*plc_ctx).io.fn.read_inputs_remote = NULL;
    (*plc_ctx).io.fn.write_outputs_remote = NULL;
    (*plc_ctx).io.fn.configure_local_io = NULL;
    (*plc_ctx).io.fn.external_on_scan = NULL;
    (*plc_ctx).io.fn.external_on_task = NULL;
    (*plc_ctx).io.fn.load_program = NULL;
    (*plc_ctx).io.fn.save_program = NULL;

    return 0;
}

int ladder_ctx_deinit(plc_ctx_t *plc_ctx) {

    return 0;
}
