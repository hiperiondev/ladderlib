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
#include <stdlib.h>
#include <string.h>

#include "ladder.h"
#include "ladder_internals.h"

#ifdef DEBUG
const char *ladder_state_str[] = {
        "STOPPED",   //
        "RUNNING",   //
        "ERROR",     //
        "EXIT_TASK", //
        "INVALID",   //
};

const char *ladder_type_str[] = {
        "M",       //
        "Q",       //
        "I",       //
        "Cd",      //
        "Td",      //
        "Tr",      //
        "IW",      //
        "QW",      //
        "C",       //
        "T",       //
        "D",       //
        "K",       //
        "R",       //
        "KR",      //
        "INVALID", //
};

#endif

void ladder_scan_time(ladder_ctx_t *ladder_ctx) {
    uint32_t scanTimeMicros = (uint32_t) (*ladder_ctx).io.micros();
    (*ladder_ctx).internals.ladder_actual_scan_time = scanTimeMicros - (*ladder_ctx).internals.ladder_start_time;
    (*ladder_ctx).internals.ladder_start_time = scanTimeMicros;
}

void ladder_save_previous_values(ladder_ctx_t *ladder_ctx) {
    memcpy((*ladder_ctx).prev_scan_vals.Mh, (*ladder_ctx).memory.M, (*ladder_ctx).ladder.qty_m * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Ih, (*ladder_ctx).memory.I, (*ladder_ctx).ladder.qty_i * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Qh, (*ladder_ctx).memory.Q, (*ladder_ctx).ladder.qty_q * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Crh, (*ladder_ctx).memory.Cr, (*ladder_ctx).ladder.qty_c * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Cdh, (*ladder_ctx).memory.Cd, (*ladder_ctx).ladder.qty_c * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Tdh, (*ladder_ctx).memory.Td, (*ladder_ctx).ladder.qty_t * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Trh, (*ladder_ctx).memory.Tr, (*ladder_ctx).ladder.qty_t * sizeof(uint8_t));
}

void ladder_clear_memory(ladder_ctx_t *ladder_ctx) {
    memset((*ladder_ctx).prev_scan_vals.Mh, 0, (*ladder_ctx).ladder.qty_m * sizeof(uint8_t));
    memset((*ladder_ctx).memory.M, 0, (*ladder_ctx).ladder.qty_m * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Ih, 0, (*ladder_ctx).ladder.qty_i * sizeof(uint8_t));
    memset((*ladder_ctx).memory.I, 0, (*ladder_ctx).ladder.qty_i * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Qh, 0, (*ladder_ctx).ladder.qty_q * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Q, 0, (*ladder_ctx).ladder.qty_q * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Cdh, 0, (*ladder_ctx).ladder.qty_c * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Cd, 0, (*ladder_ctx).ladder.qty_c * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Crh, 0, (*ladder_ctx).ladder.qty_c * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Cr, 0, (*ladder_ctx).ladder.qty_c * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Tdh, 0, (*ladder_ctx).ladder.qty_t * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Td, 0, (*ladder_ctx).ladder.qty_t * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Trh, 0, (*ladder_ctx).ladder.qty_t * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Tr, 0, (*ladder_ctx).ladder.qty_t * sizeof(uint8_t));
}

void ladder_clear_program(ladder_ctx_t *ladder_ctx) {
    memset((*ladder_ctx).network, 0, (*ladder_ctx).ladder.total_networks * sizeof(ladder_network_t));
}

bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw,
        uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r) {
    memset((*ladder_ctx).internals.flags_mask, 0, NET_ROWS * sizeof(uint16_t));
    (*ladder_ctx).internals.flags_mask[0] = 0x0001;     //
    (*ladder_ctx).internals.flags_mask[1] = 0x0002;     //
    (*ladder_ctx).internals.flags_mask[2] = 0x0004;     //
    (*ladder_ctx).internals.flags_mask[3] = 0x0008;     //
    (*ladder_ctx).internals.flags_mask[4] = 0x0010;     //

    (*ladder_ctx).io.read_inputs_local = NULL;
    (*ladder_ctx).io.write_outputs_local = NULL;
    (*ladder_ctx).io.read_inputs_remote = NULL;
    (*ladder_ctx).io.write_outputs_remote = NULL;
    (*ladder_ctx).io.external_on_scan = NULL;
    (*ladder_ctx).io.external_on_task = NULL;
    (*ladder_ctx).io.micros = NULL;
    (*ladder_ctx).io.delay = NULL;
    (*ladder_ctx).io.panic = NULL;
    (*ladder_ctx).io.end_task = NULL;

    (*ladder_ctx).network = calloc(networks_qty, sizeof(ladder_network_t));

    (*ladder_ctx).memory.M = calloc(qty_m, sizeof(uint8_t));
    (*ladder_ctx).memory.I = calloc(qty_i, sizeof(uint8_t));
    (*ladder_ctx).memory.Q = calloc(qty_q, sizeof(uint8_t));
    (*ladder_ctx).memory.Cr = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).memory.Cd = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).memory.Tr = calloc(qty_t, sizeof(bool));
    (*ladder_ctx).memory.Td = calloc(qty_t, sizeof(bool));

    (*ladder_ctx).prev_scan_vals.Mh = calloc(qty_m, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Ih = calloc(qty_i, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Qh = calloc(qty_q, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Crh = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).prev_scan_vals.Cdh = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).prev_scan_vals.Trh = calloc(qty_t, sizeof(bool));
    (*ladder_ctx).prev_scan_vals.Tdh = calloc(qty_t, sizeof(bool));

    (*ladder_ctx).registers.IW = calloc(qty_iw, sizeof(uint16_t));
    (*ladder_ctx).registers.QW = calloc(qty_qw, sizeof(uint16_t));
    (*ladder_ctx).registers.C = calloc(qty_c, sizeof(uint16_t));
    (*ladder_ctx).registers.D = calloc(qty_d, sizeof(int16_t));
    (*ladder_ctx).registers.R = calloc(qty_r, sizeof(float));

    (*ladder_ctx).timers = calloc(qty_t, sizeof(ladder_timer_t));

    (*ladder_ctx).ladder.qty_m = qty_m;
    (*ladder_ctx).ladder.qty_i = qty_i;
    (*ladder_ctx).ladder.qty_q = qty_q;
    (*ladder_ctx).ladder.qty_iw = qty_iw;
    (*ladder_ctx).ladder.qty_qw = qty_qw;
    (*ladder_ctx).ladder.qty_c = qty_c;
    (*ladder_ctx).ladder.qty_t = qty_t;
    (*ladder_ctx).ladder.qty_d = qty_d;
    (*ladder_ctx).ladder.qty_r = qty_r;
    (*ladder_ctx).ladder.total_networks = networks_qty;

    return true;
}

bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx) {
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

    return true;
}
