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
#include <stdbool.h>

#include "ladder.h"
#include "ladder_internals.h"
#ifdef OPTIONAL_CRON
#include "ladderlib_cron.h"
#endif

void ladder_scan_time(ladder_ctx_t *ladder_ctx) {
    uint64_t scanTimeMillis = (*ladder_ctx).hw.time.millis();
    (*ladder_ctx).scan_internals.actual_scan_time = scanTimeMillis - (*ladder_ctx).scan_internals.start_time;
    (*ladder_ctx).scan_internals.start_time = scanTimeMillis;
}

void ladder_save_previous_values(ladder_ctx_t *ladder_ctx) {
    memcpy((*ladder_ctx).prev_scan_vals.Mh, (*ladder_ctx).memory.M, (*ladder_ctx).ladder.quantity.m * sizeof(uint8_t));
    memcpy((*ladder_ctx).prev_scan_vals.Crh, (*ladder_ctx).memory.Cr, (*ladder_ctx).ladder.quantity.c * sizeof(bool));
    memcpy((*ladder_ctx).prev_scan_vals.Cdh, (*ladder_ctx).memory.Cd, (*ladder_ctx).ladder.quantity.c * sizeof(bool));
    memcpy((*ladder_ctx).prev_scan_vals.Tdh, (*ladder_ctx).memory.Td, (*ladder_ctx).ladder.quantity.t * sizeof(bool));
    memcpy((*ladder_ctx).prev_scan_vals.Trh, (*ladder_ctx).memory.Tr, (*ladder_ctx).ladder.quantity.t * sizeof(bool));
}

void ladder_clear_memory(ladder_ctx_t *ladder_ctx) {
    memset((*ladder_ctx).prev_scan_vals.Mh, 0, (*ladder_ctx).ladder.quantity.m * sizeof(uint8_t));
    memset((*ladder_ctx).memory.M, 0, (*ladder_ctx).ladder.quantity.m * sizeof(uint8_t));
    memset((*ladder_ctx).prev_scan_vals.Cdh, 0, (*ladder_ctx).ladder.quantity.c * sizeof(bool));
    memset((*ladder_ctx).memory.Cd, 0, (*ladder_ctx).ladder.quantity.c * sizeof(bool));
    memset((*ladder_ctx).prev_scan_vals.Crh, 0, (*ladder_ctx).ladder.quantity.c * sizeof(bool));
    memset((*ladder_ctx).memory.Cr, 0, (*ladder_ctx).ladder.quantity.c * sizeof(bool));
    memset((*ladder_ctx).prev_scan_vals.Tdh, 0, (*ladder_ctx).ladder.quantity.t * sizeof(bool));
    memset((*ladder_ctx).memory.Td, 0, (*ladder_ctx).ladder.quantity.t * sizeof(bool));
    memset((*ladder_ctx).prev_scan_vals.Trh, 0, (*ladder_ctx).ladder.quantity.t * sizeof(bool));
    memset((*ladder_ctx).memory.Tr, 0, (*ladder_ctx).ladder.quantity.t * sizeof(bool));
}

void ladder_clear_program(ladder_ctx_t *ladder_ctx) {
    for (uint32_t nt = 0; nt < (*ladder_ctx).ladder.quantity.networks; nt++) {
        for (uint32_t r = 0; r < (*ladder_ctx).network[nt].rows; r++) {
            for (uint32_t c = 0; c < (*ladder_ctx).network[nt].cols; c++) {
                (*ladder_ctx).network[nt].cells[r][c].code = 0;
                free((*ladder_ctx).network[nt].cells[r][c].data);
                (*ladder_ctx).network[nt].cells[r][c].data = NULL;
            }
        }
    }
}

bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_c,
        uint32_t qty_t, uint32_t qty_d, uint32_t qty_r, bool init_netwok) {
    if (net_rows_qty > 32)
        return false;

    (*ladder_ctx).hw.io.fn_read_qty = 0;
    (*ladder_ctx).hw.io.fn_write_qty = 0;
    (*ladder_ctx).foreign.qty = 0;
    (*ladder_ctx).foreign.fn = NULL;

    (*ladder_ctx).scan_internals.actual_scan_time = 0;
    (*ladder_ctx).scan_internals.start_time = 0;

    (*ladder_ctx).hw.io.read = NULL;
    (*ladder_ctx).hw.io.write = NULL;
    (*ladder_ctx).on.scan_end = NULL;
    (*ladder_ctx).on.instruction = NULL;
    (*ladder_ctx).on.task_before = NULL;
    (*ladder_ctx).on.task_after = NULL;
    (*ladder_ctx).hw.time.millis = NULL;
    (*ladder_ctx).hw.time.delay = NULL;
    (*ladder_ctx).on.panic = NULL;
    (*ladder_ctx).on.end_task = NULL;

    if (init_netwok) {
        (*ladder_ctx).network = calloc(networks_qty, sizeof(ladder_network_t));

        for (uint32_t nt = 0; nt < networks_qty; nt++) {
            (*ladder_ctx).network[nt].rows = net_rows_qty;
            (*ladder_ctx).network[nt].cols = net_columns_qty;
            (*ladder_ctx).network[nt].cells = malloc(net_rows_qty * sizeof(ladder_cell_t*));
            for (uint32_t cl = 0; cl < net_rows_qty; cl++)
                (*ladder_ctx).network[nt].cells[cl] = calloc(net_columns_qty, sizeof(ladder_cell_t));

            for (uint32_t column = 0; column < net_columns_qty; column++) {
                for (uint32_t row = 0; row < net_rows_qty; row++) {
                    (*ladder_ctx).network[nt].cells[row][column].code = LADDER_INS_NOP;
                    (*ladder_ctx).network[nt].cells[row][column].data_qty = 0;
                    (*ladder_ctx).network[nt].cells[row][column].data = NULL;
                    (*ladder_ctx).network[nt].cells[row][column].state = false;
                    (*ladder_ctx).network[nt].cells[row][column].vertical_bar = false;
                }
            }
        }
    }
    (*ladder_ctx).memory.M = calloc(qty_m, sizeof(uint8_t));
    (*ladder_ctx).memory.Cr = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).memory.Cd = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).memory.Tr = calloc(qty_t, sizeof(bool));
    (*ladder_ctx).memory.Td = calloc(qty_t, sizeof(bool));

    (*ladder_ctx).prev_scan_vals.Mh = calloc(qty_m, sizeof(uint8_t));
    (*ladder_ctx).prev_scan_vals.Crh = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).prev_scan_vals.Cdh = calloc(qty_c, sizeof(bool));
    (*ladder_ctx).prev_scan_vals.Trh = calloc(qty_t, sizeof(bool));
    (*ladder_ctx).prev_scan_vals.Tdh = calloc(qty_t, sizeof(bool));

    (*ladder_ctx).registers.C = calloc(qty_c, sizeof(uint32_t));
    (*ladder_ctx).registers.D = calloc(qty_d, sizeof(int32_t));
    (*ladder_ctx).registers.R = calloc(qty_r, sizeof(float));

    (*ladder_ctx).timers = calloc(qty_t, sizeof(ladder_timer_t));

    (*ladder_ctx).ladder.quantity.m = 0;
    (*ladder_ctx).ladder.quantity.c = qty_c;
    (*ladder_ctx).ladder.quantity.t = qty_t;
    (*ladder_ctx).ladder.quantity.d = qty_d;
    (*ladder_ctx).ladder.quantity.r = qty_r;
    (*ladder_ctx).ladder.quantity.networks = networks_qty;

#ifdef OPTIONAL_CRON
    (*ladder_ctx).cron = (ladderlib_cron_t*) calloc(1, sizeof(ladderlib_cron_t));
#endif

    return true;
}

bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx) {
    ladder_clear_memory(ladder_ctx);
    ladder_clear_program(ladder_ctx);

    if (ladder_ctx->network != NULL) {
        for (uint32_t nt = 0; nt < ladder_ctx->ladder.quantity.networks; nt++) {
            if (ladder_ctx->network[nt].cells != NULL) {
                for (uint32_t r = 0; r < ladder_ctx->network[nt].rows; r++) {
                    // Add inner data free (though clear_program did; redundant safety).
                    for (uint32_t c = 0; c < ladder_ctx->network[nt].cols; c++) {
                        free(ladder_ctx->network[nt].cells[r][c].data);
                        ladder_ctx->network[nt].cells[r][c].data = NULL;
                    }
                    free(ladder_ctx->network[nt].cells[r]);
                }
                free(ladder_ctx->network[nt].cells);
                ladder_ctx->network[nt].cells = NULL;
            }
        }
        free(ladder_ctx->network);
        ladder_ctx->network = NULL;
    }

    free(ladder_ctx->memory.M); ladder_ctx->memory.M = NULL;
    free(ladder_ctx->memory.Cr); ladder_ctx->memory.Cr = NULL;
    free(ladder_ctx->memory.Cd); ladder_ctx->memory.Cd = NULL;
    free(ladder_ctx->memory.Tr); ladder_ctx->memory.Tr = NULL;
    free(ladder_ctx->memory.Td); ladder_ctx->memory.Td = NULL;

    free(ladder_ctx->prev_scan_vals.Mh); ladder_ctx->prev_scan_vals.Mh = NULL;
    free(ladder_ctx->prev_scan_vals.Crh); ladder_ctx->prev_scan_vals.Crh = NULL;
    free(ladder_ctx->prev_scan_vals.Cdh); ladder_ctx->prev_scan_vals.Cdh = NULL;
    free(ladder_ctx->prev_scan_vals.Trh); ladder_ctx->prev_scan_vals.Trh = NULL;
    free(ladder_ctx->prev_scan_vals.Tdh); ladder_ctx->prev_scan_vals.Tdh = NULL;

    free(ladder_ctx->registers.C); ladder_ctx->registers.C = NULL;
    free(ladder_ctx->registers.D); ladder_ctx->registers.D = NULL;
    free(ladder_ctx->registers.R); ladder_ctx->registers.R = NULL;

    free(ladder_ctx->timers); ladder_ctx->timers = NULL;

    for (uint32_t f = 0; f < ladder_ctx->foreign.qty; f++) {
        if (ladder_ctx->foreign.fn[f].deinit) {
            ladder_ctx->foreign.fn[f].deinit(&(ladder_ctx->foreign.fn[f]));
        }
    }
    free(ladder_ctx->foreign.fn); ladder_ctx->foreign.fn = NULL;

    for (uint32_t f = 0; f < ladder_ctx->hw.io.fn_read_qty; f++) {
        if (ladder_ctx->hw.io.init_read[f]) {
            ladder_ctx->hw.io.init_read[f](ladder_ctx, f, false);
        }
    }
    for (uint32_t f = 0; f < ladder_ctx->hw.io.fn_write_qty; f++) {
        if (ladder_ctx->hw.io.init_write[f]) {
            ladder_ctx->hw.io.init_write[f](ladder_ctx, f, false);
        }
    }

    free(ladder_ctx->hw.io.read); ladder_ctx->hw.io.read = NULL;
    free(ladder_ctx->hw.io.write); ladder_ctx->hw.io.write = NULL;
    free(ladder_ctx->hw.io.init_read); ladder_ctx->hw.io.init_read = NULL;
    free(ladder_ctx->hw.io.init_write); ladder_ctx->hw.io.init_write = NULL;
    free(ladder_ctx->input); ladder_ctx->input = NULL;
    free(ladder_ctx->output); ladder_ctx->output = NULL;

#ifdef OPTIONAL_CRON
    if (((ladderlib_cron_t *) ladder_ctx->cron)->ctx != NULL)
        ladderlib_cron_deinit(ladder_ctx);
    free(ladder_ctx->cron); ladder_ctx->cron = NULL;
#endif

    return true;
}

bool ladder_add_read_fn(ladder_ctx_t *ladder_ctx, _io_read read, _io_init read_init) {
    if ((*ladder_ctx).hw.io.fn_read_qty == 0) {
        (*ladder_ctx).hw.io.read = calloc(1, sizeof(_io_read*));
        (*ladder_ctx).hw.io.init_read = calloc(1, sizeof(_io_init*));
        (*ladder_ctx).input = calloc(1, sizeof(ladder_hw_input_vals_t));
    } else {
        (*ladder_ctx).hw.io.read = realloc((*ladder_ctx).hw.io.read, ((*ladder_ctx).hw.io.fn_read_qty + 1) * sizeof(_io_read*));
        (*ladder_ctx).hw.io.init_read = realloc((*ladder_ctx).hw.io.init_read, ((*ladder_ctx).hw.io.fn_read_qty + 1) * sizeof(_io_init*));
        (*ladder_ctx).input = realloc((*ladder_ctx).input, ((*ladder_ctx).hw.io.fn_read_qty + 1) * sizeof(ladder_hw_input_vals_t));
    }

    if ((*ladder_ctx).hw.io.read == NULL || (*ladder_ctx).hw.io.init_read == NULL)
        return false;

    (*ladder_ctx).input[(*ladder_ctx).hw.io.fn_read_qty].fn_id = (*ladder_ctx).hw.io.fn_read_qty;
    (*ladder_ctx).input[(*ladder_ctx).hw.io.fn_read_qty].i_qty = 0;
    (*ladder_ctx).input[(*ladder_ctx).hw.io.fn_read_qty].iw_qty = 0;
    (*ladder_ctx).hw.io.read[(*ladder_ctx).hw.io.fn_read_qty] = read;
    (*ladder_ctx).hw.io.init_read[(*ladder_ctx).hw.io.fn_read_qty] = read_init;

    read_init(ladder_ctx, (*ladder_ctx).hw.io.fn_read_qty, true);

    ++(*ladder_ctx).hw.io.fn_read_qty;

    return true;
}

bool ladder_add_write_fn(ladder_ctx_t *ladder_ctx, _io_write write, _io_init write_init) {
    if ((*ladder_ctx).hw.io.fn_write_qty == 0) {
        (*ladder_ctx).hw.io.write = calloc(1, sizeof(_io_write*));
        (*ladder_ctx).hw.io.init_write = calloc(1, sizeof(_io_init*));
        (*ladder_ctx).output = calloc(1, sizeof(ladder_hw_output_vals_t));
    } else {
        (*ladder_ctx).hw.io.write = realloc((*ladder_ctx).hw.io.write, ((*ladder_ctx).hw.io.fn_write_qty + 1) * sizeof(_io_write*));
        (*ladder_ctx).hw.io.init_write = realloc((*ladder_ctx).hw.io.init_write, ((*ladder_ctx).hw.io.fn_write_qty + 1) * sizeof(_io_init*));
        (*ladder_ctx).output = realloc((*ladder_ctx).output, ((*ladder_ctx).hw.io.fn_write_qty + 1) * sizeof(ladder_hw_output_vals_t));
    }

    if ((*ladder_ctx).hw.io.write == NULL || (*ladder_ctx).hw.io.init_write == NULL)
        return false;

    (*ladder_ctx).output[(*ladder_ctx).hw.io.fn_write_qty].fn_id = (*ladder_ctx).hw.io.fn_write_qty;
    (*ladder_ctx).output[(*ladder_ctx).hw.io.fn_write_qty].q_qty = 0;
    (*ladder_ctx).output[(*ladder_ctx).hw.io.fn_write_qty].qw_qty = 0;
    (*ladder_ctx).hw.io.write[(*ladder_ctx).hw.io.fn_write_qty] = write;
    (*ladder_ctx).hw.io.init_write[(*ladder_ctx).hw.io.fn_write_qty] = write_init;

    write_init(ladder_ctx, (*ladder_ctx).hw.io.fn_write_qty, true);

    ++(*ladder_ctx).hw.io.fn_write_qty;

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
    if ((*ladder_ctx).network[network].rows < actual_ioc.cells + row)
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
