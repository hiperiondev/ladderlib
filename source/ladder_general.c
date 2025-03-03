/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *
 *    please contact their authors for more information.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ladder.h"
#include "ladder_internals.h"

void ladder_scan_time(ladder_ctx_t *ladder_ctx) {
    uint64_t scanTimeMillis = (*ladder_ctx).hw.time.millis();
    (*ladder_ctx).scan_internals.actual_scan_time = scanTimeMillis - (*ladder_ctx).scan_internals.start_time;
    (*ladder_ctx).scan_internals.start_time = scanTimeMillis;
}

void ladder_save_previous_values(ladder_ctx_t *ladder_ctx) {
    memcpy((*ladder_ctx).prev_scan_vals.Mh, (*ladder_ctx).memory.M, (*ladder_ctx).ladder.quantity.m * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Ih, (*ladder_ctx).memory.I, (*ladder_ctx).ladder.quantity.i * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Qh, (*ladder_ctx).memory.Q, (*ladder_ctx).ladder.quantity.q * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Crh, (*ladder_ctx).memory.Cr, (*ladder_ctx).ladder.quantity.c * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Cdh, (*ladder_ctx).memory.Cd, (*ladder_ctx).ladder.quantity.c * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Tdh, (*ladder_ctx).memory.Td, (*ladder_ctx).ladder.quantity.t * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Trh, (*ladder_ctx).memory.Tr, (*ladder_ctx).ladder.quantity.t * sizeof(uint8_t));
}

void ladder_clear_memory(ladder_ctx_t *ladder_ctx) {
    memset((*ladder_ctx).prev_scan_vals.Mh, 0, (*ladder_ctx).ladder.quantity.m * sizeof(uint8_t));
    memset((*ladder_ctx).memory.M, 0, (*ladder_ctx).ladder.quantity.m * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Ih, 0, (*ladder_ctx).ladder.quantity.i * sizeof(uint8_t));
    memset((*ladder_ctx).memory.I, 0, (*ladder_ctx).ladder.quantity.i * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Qh, 0, (*ladder_ctx).ladder.quantity.q * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Q, 0, (*ladder_ctx).ladder.quantity.q * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Cdh, 0, (*ladder_ctx).ladder.quantity.c * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Cd, 0, (*ladder_ctx).ladder.quantity.c * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Crh, 0, (*ladder_ctx).ladder.quantity.c * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Cr, 0, (*ladder_ctx).ladder.quantity.c * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Tdh, 0, (*ladder_ctx).ladder.quantity.t * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Td, 0, (*ladder_ctx).ladder.quantity.t * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Trh, 0, (*ladder_ctx).ladder.quantity.t * sizeof(uint8_t));
    memset((*ladder_ctx).memory.Tr, 0, (*ladder_ctx).ladder.quantity.t * sizeof(uint8_t));
}

void ladder_clear_program(ladder_ctx_t *ladder_ctx) {
    for (uint32_t nt = 0; nt < (*ladder_ctx).ladder.quantity.networks; nt++) {
        for (uint32_t c = 0; c < (*ladder_ctx).ladder.quantity.net_columns; c++) {
            (*ladder_ctx).network[nt].bars[c] = 0;
            for (uint32_t r = 0; r < (*ladder_ctx).ladder.quantity.net_rows - 1; r++) {
                (*ladder_ctx).network[nt].cells[r][c].code = 0;
                free((*ladder_ctx).network[nt].cells[r][c].data);
                (*ladder_ctx).network[nt].cells[r][c].data = NULL;
            }
        }
    }
}

bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i,
        uint32_t qty_q, uint32_t qty_iw, uint32_t qty_qw, uint32_t qty_c, uint32_t qty_t, uint32_t qty_d, uint32_t qty_r) {

    if (net_rows_qty > 32)
        return false;

    (*ladder_ctx).foreign.qty = 0;
    (*ladder_ctx).foreign.fn = NULL;

    (*ladder_ctx).scan_internals.actual_scan_time = 0;
    (*ladder_ctx).scan_internals.start_time = 0;

    (*ladder_ctx).hw.io.read_inputs_local = NULL;
    (*ladder_ctx).hw.io.write_outputs_local = NULL;
    (*ladder_ctx).hw.io.read_inputs_remote = NULL;
    (*ladder_ctx).hw.io.write_outputs_remote = NULL;
    (*ladder_ctx).on.scan_end = NULL;
    (*ladder_ctx).on.instruction = NULL;
    (*ladder_ctx).on.task_before = NULL;
    (*ladder_ctx).on.task_after = NULL;
    (*ladder_ctx).hw.time.millis = NULL;
    (*ladder_ctx).hw.time.delay = NULL;
    (*ladder_ctx).on.panic = NULL;
    (*ladder_ctx).on.end_task = NULL;

    (*ladder_ctx).network = calloc(networks_qty, sizeof(ladder_network_t));

    for (uint32_t nt = 0; nt < networks_qty; nt++) {
        (*ladder_ctx).network[nt].bars = calloc(net_columns_qty, sizeof(uint32_t));
        (*ladder_ctx).network[nt].cells = malloc(net_rows_qty * sizeof(ladder_cell_t*));
        for (uint32_t cl = 0; cl < net_rows_qty; cl++)
            (*ladder_ctx).network[nt].cells[cl] = calloc(net_columns_qty, sizeof(ladder_cell_t));

        for (uint32_t column = 0; column < net_columns_qty; column++) {
            for (uint32_t row = 0; row < net_rows_qty; row++) {
                (*ladder_ctx).network[nt].cells[row][column].code = LADDER_INS_NOP;
                (*ladder_ctx).network[nt].cells[row][column].data_qty = 0;
                (*ladder_ctx).network[nt].cells[row][column].data = NULL;
            }
        }
    }

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

    (*ladder_ctx).registers.IW = calloc(qty_iw, sizeof(int32_t));
    (*ladder_ctx).registers.QW = calloc(qty_qw, sizeof(int32_t));
    (*ladder_ctx).registers.C = calloc(qty_c, sizeof(uint32_t));
    (*ladder_ctx).registers.D = calloc(qty_d, sizeof(int32_t));
    (*ladder_ctx).registers.R = calloc(qty_r, sizeof(float));

    (*ladder_ctx).timers = calloc(qty_t, sizeof(ladder_timer_t));

    (*ladder_ctx).scan_internals.network_flags = calloc(net_columns_qty, sizeof(uint32_t));

    (*ladder_ctx).ladder.quantity.net_columns = net_columns_qty;
    (*ladder_ctx).ladder.quantity.net_rows = net_rows_qty;
    (*ladder_ctx).ladder.quantity.m = qty_m;
    (*ladder_ctx).ladder.quantity.i = qty_i;
    (*ladder_ctx).ladder.quantity.q = qty_q;
    (*ladder_ctx).ladder.quantity.iw = qty_iw;
    (*ladder_ctx).ladder.quantity.qw = qty_qw;
    (*ladder_ctx).ladder.quantity.c = qty_c;
    (*ladder_ctx).ladder.quantity.t = qty_t;
    (*ladder_ctx).ladder.quantity.d = qty_d;
    (*ladder_ctx).ladder.quantity.r = qty_r;
    (*ladder_ctx).ladder.quantity.networks = networks_qty;

    return true;
}

bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx) {
    for (uint32_t nt = 0; nt < (*ladder_ctx).ladder.quantity.networks; nt++) {
        free((*ladder_ctx).network[nt].bars);

        for (uint32_t column = 0; column < (*ladder_ctx).ladder.quantity.net_columns; column++)
            for (uint32_t row = 0; row < (*ladder_ctx).ladder.quantity.net_rows; row++)
                free((*ladder_ctx).network[nt].cells[row][column].data);

        for (uint32_t cl = 0; cl < (*ladder_ctx).ladder.quantity.net_rows; cl++)
            free((*ladder_ctx).network[nt].cells[cl]);

        free((*ladder_ctx).network[nt].cells);
    }
    free((*ladder_ctx).network);

    free((*ladder_ctx).scan_internals.network_flags);

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

    for (uint32_t f = 0; f < (*ladder_ctx).foreign.qty; f++) {
        (*ladder_ctx).foreign.fn[f].deinit(&((*ladder_ctx).foreign.fn[f]));
    }
    free((*ladder_ctx).foreign.fn);

    return true;
}

bool ladder_add_foreign(ladder_ctx_t *ladder_ctx, _foreign_fn_init fn_init, void *init_data, uint32_t qty) {
    if (fn_init == NULL)
        return false;

    ladder_foreign_function_t fn_new;

    memset(&fn_new, 0, sizeof(ladder_foreign_function_t));
    fn_new.id = (*ladder_ctx).foreign.qty;

    if (!fn_init(&fn_new, init_data, qty))
        return false;

    if ((*ladder_ctx).foreign.qty == 0)
        (*ladder_ctx).foreign.fn = malloc(sizeof(ladder_foreign_function_t));
    else (*ladder_ctx).foreign.fn = realloc((*ladder_ctx).foreign.fn, ((*ladder_ctx).foreign.qty + 1) * sizeof(ladder_foreign_function_t));

    memcpy(&((*ladder_ctx).foreign.fn[(*ladder_ctx).foreign.qty]), &fn_new, sizeof(ladder_foreign_function_t));
    ++(*ladder_ctx).foreign.qty;

    return true;
}

bool ladder_fn_cell(ladder_ctx_t *ladder_ctx, uint32_t network, uint32_t row, uint32_t column, ladder_instruction_t function, uint32_t foreign_id) {
    ladder_instructions_iocd_t actual_ioc;

    if (function == LADDER_INS_FOREIGN) {
        if ((*ladder_ctx).foreign.qty < foreign_id + 1)
            return false;
        else memcpy(&actual_ioc, &((*ladder_ctx).foreign.fn[foreign_id]).description, sizeof(ladder_instructions_iocd_t));
    } else memcpy(&actual_ioc, &(ladder_fn_iocd[function]), sizeof(ladder_instructions_iocd_t));

    // not available rows for function
    if ((*ladder_ctx).ladder.quantity.net_rows < actual_ioc.cells + row)
        return false;

    if ((*ladder_ctx).network[network].cells[row][column].data != NULL)
        free((*ladder_ctx).network[network].cells[row][column].data);

    (*ladder_ctx).network[network].cells[row][column].code = function;
    (*ladder_ctx).network[network].cells[row][column].data_qty = actual_ioc.data_qty;

    if (actual_ioc.data_qty == 0) {
        (*ladder_ctx).network[network].cells[row][column].data = NULL;
        return true;
    }

    (*ladder_ctx).network[network].cells[row][column].data = calloc(actual_ioc.data_qty, sizeof(ladder_value_t));

    for (uint8_t r = 1; r < ladder_fn_iocd[function].cells; r++) {
        (*ladder_ctx).network[network].cells[row + r][column].code = LADDER_INS_MULTI;
        if ((*ladder_ctx).network[network].cells[row + r][column].data != NULL) {
            free((*ladder_ctx).network[network].cells[row + r][column].data);
            (*ladder_ctx).network[network].cells[row + r][column].data = NULL;
        }
    }

    return true;
}
