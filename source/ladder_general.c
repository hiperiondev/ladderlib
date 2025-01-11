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
#include <stdlib.h>

#include "ladder.h"
#include "ladder_internals.h"

void ladder_scan_time(ladder_ctx_t *ladder_ctx) {
    uint32_t scanTimeMicros = (uint32_t)(*ladder_ctx).io.micros();
    (*ladder_ctx).internals.ladder_actual_scan_time = scanTimeMicros - (*ladder_ctx).internals.ladder_start_time;
    (*ladder_ctx).internals.ladder_start_time = scanTimeMicros;
}

void ladder_save_previous_values(ladder_ctx_t *ladder_ctx) {
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_M; i++) {
        (*ladder_ctx).prev_scan_vals.Mh[i] = (*ladder_ctx).memory.M[i];
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_I; i++) {
        (*ladder_ctx).prev_scan_vals.Ih[i] = (*ladder_ctx).memory.I[i];
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_Q; i++) {
        (*ladder_ctx).prev_scan_vals.Qh[i] = (*ladder_ctx).memory.Q[i];
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_C; i++) {
        (*ladder_ctx).prev_scan_vals.Crh[i] = (*ladder_ctx).memory.Cr[i];
        (*ladder_ctx).prev_scan_vals.Cdh[i] = (*ladder_ctx).memory.Cd[i];
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_T; i++) {
        (*ladder_ctx).prev_scan_vals.Tdh[i] = (*ladder_ctx).memory.Td[i];
        (*ladder_ctx).prev_scan_vals.Trh[i] = (*ladder_ctx).memory.Tr[i];
    }
}

void ladder_clear_memory(ladder_ctx_t *ladder_ctx) {
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_M; i++) {
        (*ladder_ctx).prev_scan_vals.Mh[i] = 0;
        (*ladder_ctx).memory.M[i] = 0;
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_I; i++) {
        (*ladder_ctx).prev_scan_vals.Ih[i] = 0;
        (*ladder_ctx).memory.I[i] = 0;
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_Q; i++) {
        (*ladder_ctx).prev_scan_vals.Qh[i] = 0;
        (*ladder_ctx).memory.Q[i] = 0;
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_C; i++) {
        (*ladder_ctx).prev_scan_vals.Cdh[i] = 0;
        (*ladder_ctx).memory.Cd[i] = 0;
        (*ladder_ctx).prev_scan_vals.Crh[i] = 0;
        (*ladder_ctx).memory.Cr[i] = 0;
    }
    for (uint32_t i = 0; i < (*ladder_ctx).ladder.QTY_T; i++) {
        (*ladder_ctx).prev_scan_vals.Tdh[i] = 0;
        (*ladder_ctx).memory.Td[i] = 0;
        (*ladder_ctx).prev_scan_vals.Trh[i] = 0;
        (*ladder_ctx).memory.Tr[i] = 0;
    }
}

void ladder_clear_program(ladder_ctx_t *ladder_ctx) {
    for (uint32_t n = 0; n < (*ladder_ctx).ladder.total_networks ; n++) {
        (*ladder_ctx).network[n].bars[0] = 0;
        (*ladder_ctx).network[n].bars[1] = 0;
        (*ladder_ctx).network[n].bars[2] = 0;
        (*ladder_ctx).network[n].bars[3] = 0;
        (*ladder_ctx).network[n].bars[4] = 0;
        for (uint32_t c = 0; c < NET_COLUMNS; c++) {
            for (uint32_t r = 0; r < NET_ROWS; r++) {
                (*ladder_ctx).network[n].cells[r][c].code = 0;
                (*ladder_ctx).network[n].cells[r][c].data = 0;
                (*ladder_ctx).network[n].cells[r][c].type = 0;
            }
        }
    }
}

int ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw,
        uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r) {
    (*ladder_ctx).internals.flags_mask[0] = 0x0001;           //
    (*ladder_ctx).internals.flags_mask[1] = 0x0002;           //
    (*ladder_ctx).internals.flags_mask[2] = 0x0004;           //
    (*ladder_ctx).internals.flags_mask[3] = 0x0008;           //
    (*ladder_ctx).internals.flags_mask[4] = 0x0010;           //
    (*ladder_ctx).internals.flags_mask_and[0] = 0xFFFE;       //
    (*ladder_ctx).internals.flags_mask_and[1] = 0xFFFD;       //
    (*ladder_ctx).internals.flags_mask_and[2] = 0xFFFB;       //
    (*ladder_ctx).internals.flags_mask_and[3] = 0x0007;       //
    (*ladder_ctx).internals.flags_mask_and[4] = 0xFFEF;       //

    (*ladder_ctx).io.read_inputs_local = NULL;
    (*ladder_ctx).io.write_outputs_local = NULL;
    (*ladder_ctx).io.read_inputs_remote = NULL;
    (*ladder_ctx).io.write_outputs_remote = NULL;
    (*ladder_ctx).io.external_on_scan = NULL;
    (*ladder_ctx).io.external_on_task = NULL;
    (*ladder_ctx).io.micros = NULL;
    (*ladder_ctx).io.delay = NULL;

    (*ladder_ctx).network = calloc(networks_qty, sizeof(ladder_network_t));

    (*ladder_ctx).memory.M = calloc(qty_m, sizeof(uint8_t));
    (*ladder_ctx).memory.I = calloc(qty_i, sizeof(uint8_t));
    (*ladder_ctx).memory.Q = calloc(qty_q, sizeof(uint8_t));
    (*ladder_ctx).memory.Cr = calloc(qty_c, sizeof(uint8_t));
    (*ladder_ctx).memory.Cd = calloc(qty_c, sizeof(uint8_t));
    (*ladder_ctx).memory.Tr = calloc(qty_t, sizeof(uint8_t));
    (*ladder_ctx).memory.Td = calloc(qty_t, sizeof(uint8_t));

    (*ladder_ctx).prev_scan_vals.Mh = calloc(qty_m, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Ih = calloc(qty_i, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Qh = calloc(qty_q, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Crh = calloc(qty_c, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Cdh = calloc(qty_c, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Trh = calloc(qty_t, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Tdh = calloc(qty_t, sizeof(uint8_t));

    (*ladder_ctx).registers.IW = calloc(qty_iw, sizeof(uint16_t));
    (*ladder_ctx).registers.QW = calloc(qty_qw, sizeof(uint16_t));
    (*ladder_ctx).registers.C = calloc(qty_c, sizeof(uint16_t));
    (*ladder_ctx).registers.D = calloc(qty_d, sizeof(int16_t));
    (*ladder_ctx).registers.R = calloc(qty_r, sizeof(float));

    (*ladder_ctx).timers = calloc(qty_t, sizeof(ladder_timer_t));

    (*ladder_ctx).ladder.QTY_M = qty_m;
    (*ladder_ctx).ladder.QTY_I = qty_i;
    (*ladder_ctx).ladder.QTY_Q = qty_q;
    (*ladder_ctx).ladder.QTY_IW = qty_iw;
    (*ladder_ctx).ladder.QTY_QW = qty_qw;
    (*ladder_ctx).ladder.QTY_C = qty_c;
    (*ladder_ctx).ladder.QTY_T = qty_t;
    (*ladder_ctx).ladder.QTY_D = qty_d;
    (*ladder_ctx).ladder.QTY_R = qty_r;
    (*ladder_ctx).ladder.total_networks = networks_qty;

    return 0;
}

int ladder_ctx_deinit(ladder_ctx_t *ladder_ctx) {
    free((*ladder_ctx).network);

    free((*ladder_ctx).memory.M);
    free((*ladder_ctx).memory.I);
    free((*ladder_ctx).memory.Q);
    free((*ladder_ctx).memory.Cr);
    free((*ladder_ctx).memory.Cd);
    free((*ladder_ctx).memory.Tr);
    free((*ladder_ctx).memory.Td);

    free((*ladder_ctx).prev_scan_vals.Mh);
    free((*ladder_ctx).prev_scan_vals.Ih);
    free((*ladder_ctx).prev_scan_vals.Qh);
    free((*ladder_ctx).prev_scan_vals.Crh);
    free((*ladder_ctx).prev_scan_vals.Cdh);
    free((*ladder_ctx).prev_scan_vals.Trh);
    free((*ladder_ctx).prev_scan_vals.Tdh);

    free((*ladder_ctx).registers.IW);
    free((*ladder_ctx).registers.QW);
    free((*ladder_ctx).registers.C);
    free((*ladder_ctx).registers.D);
    free((*ladder_ctx).registers.R);

    free((*ladder_ctx).timers);

    return 0;
}
