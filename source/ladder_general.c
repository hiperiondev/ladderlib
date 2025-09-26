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
    uint64_t scanTimeMillis = ladder_ctx->hw.time.millis();
    ladder_ctx->scan_internals.actual_scan_time = scanTimeMillis - ladder_ctx->scan_internals.start_time;
    ladder_ctx->scan_internals.start_time = scanTimeMillis;

    if (ladder_ctx->ladder.quantity.watchdog_ms > 0 && ladder_ctx->scan_internals.actual_scan_time > ladder_ctx->ladder.quantity.watchdog_ms) {
        ladder_ctx->ladder.state = LADDER_ST_ERROR;
    }
}

void ladder_save_previous_values(ladder_ctx_t *ladder_ctx) {
    memcpy(ladder_ctx->prev_scan_vals.Mh, ladder_ctx->memory.M, ladder_ctx->ladder.quantity.m * sizeof(uint8_t));
    memcpy(ladder_ctx->prev_scan_vals.Crh, ladder_ctx->memory.Cr, ladder_ctx->ladder.quantity.c * sizeof(bool));
    memcpy(ladder_ctx->prev_scan_vals.Cdh, ladder_ctx->memory.Cd, ladder_ctx->ladder.quantity.c * sizeof(bool));
    memcpy(ladder_ctx->prev_scan_vals.Tdh, ladder_ctx->memory.Td, ladder_ctx->ladder.quantity.t * sizeof(bool));
    memcpy(ladder_ctx->prev_scan_vals.Trh, ladder_ctx->memory.Tr, ladder_ctx->ladder.quantity.t * sizeof(bool));
}

void ladder_clear_memory(ladder_ctx_t *ladder_ctx) {
    if (ladder_ctx->ladder.quantity.m > 0) {
        memset(ladder_ctx->prev_scan_vals.Mh, 0, ladder_ctx->ladder.quantity.m * sizeof(uint8_t));
        memset(ladder_ctx->memory.M, 0, ladder_ctx->ladder.quantity.m * sizeof(uint8_t));
    }

    if (ladder_ctx->ladder.quantity.c > 0) {
        memset(ladder_ctx->prev_scan_vals.Cdh, 0, ladder_ctx->ladder.quantity.c * sizeof(bool));
        memset(ladder_ctx->memory.Cd, 0, ladder_ctx->ladder.quantity.c * sizeof(bool));
        memset(ladder_ctx->prev_scan_vals.Crh, 0, ladder_ctx->ladder.quantity.c * sizeof(bool));
        memset(ladder_ctx->memory.Cr, 0, ladder_ctx->ladder.quantity.c * sizeof(bool));
    }

    if (ladder_ctx->ladder.quantity.t > 0) {
        memset(ladder_ctx->prev_scan_vals.Tdh, 0, ladder_ctx->ladder.quantity.t * sizeof(bool));
        memset(ladder_ctx->memory.Td, 0, ladder_ctx->ladder.quantity.t * sizeof(bool));
        memset(ladder_ctx->prev_scan_vals.Trh, 0, ladder_ctx->ladder.quantity.t * sizeof(bool));
        memset(ladder_ctx->memory.Tr, 0, ladder_ctx->ladder.quantity.t * sizeof(bool));
    }
}

void ladder_clear_program(ladder_ctx_t *ladder_ctx) {
    for (uint32_t nt = 0; nt < ladder_ctx->ladder.quantity.networks; nt++) {
        for (uint32_t r = 0; r < ladder_ctx->network[nt].rows; r++) {
            for (uint32_t c = 0; c < ladder_ctx->network[nt].cols; c++) {
                ladder_ctx->network[nt].cells[r][c].code = LADDER_INS_NOP;
                ladder_ctx->network[nt].cells[r][c].vertical_bar = false;
                ladder_ctx->network[nt].cells[r][c].state = false;
                if (ladder_ctx->network[nt].cells[r][c].data != NULL) {
                    for (uint32_t d = 0; d < ladder_ctx->network[nt].cells[r][c].data_qty; d++) {
                        if (ladder_ctx->network[nt].cells[r][c].data[d].type == LADDER_REGISTER_S&&
                        ladder_ctx->network[nt].cells[r][c].data[d].value.cstr != NULL) {
                            free((void*) ladder_ctx->network[nt].cells[r][c].data[d].value.cstr);
                            ladder_ctx->network[nt].cells[r][c].data[d].value.cstr = NULL;
                        }
                    }
                    free(ladder_ctx->network[nt].cells[r][c].data);
                    ladder_ctx->network[nt].cells[r][c].data = NULL;
                    ladder_ctx->network[nt].cells[r][c].data_qty = 0;
                }
            }
        }
    }
}

bool ladder_ctx_init(ladder_ctx_t *ladder_ctx, uint8_t net_columns_qty, uint8_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_c,
        uint32_t qty_t, uint32_t qty_d, uint32_t qty_r, uint32_t delay_not_run, uint32_t watchdog_ms, bool init_netwok) {
    if (ladder_ctx == NULL)
        return false;
    if (net_rows_qty > LADDER_MAX_ROWS)
        return false;
    if (net_columns_qty < 1 || net_rows_qty < 1 || networks_qty < 1 || qty_m < 1 || qty_c < 1 || qty_t < 1 || qty_d < 1 || qty_r < 1)
        return false;

    memset(ladder_ctx, 0, sizeof(ladder_ctx_t));

    ladder_ctx->hw.io.fn_read_qty = 0;
    ladder_ctx->hw.io.fn_write_qty = 0;
    ladder_ctx->foreign.qty = 0;
    ladder_ctx->foreign.fn = NULL;

    ladder_ctx->scan_internals.actual_scan_time = 0;
    ladder_ctx->scan_internals.start_time = 0;

    ladder_ctx->hw.io.read = NULL;
    ladder_ctx->hw.io.write = NULL;
    ladder_ctx->on.scan_end = NULL;
    ladder_ctx->on.instruction = NULL;
    ladder_ctx->on.task_before = NULL;
    ladder_ctx->on.task_after = NULL;
    ladder_ctx->hw.time.millis = NULL;
    ladder_ctx->hw.time.delay = NULL;
    ladder_ctx->on.panic = NULL;
    ladder_ctx->on.end_task = NULL;

    if (init_netwok) {
        if (networks_qty > SIZE_MAX / sizeof(ladder_network_t)) {
            return false;
        }
        ladder_ctx->network = calloc(networks_qty, sizeof(ladder_network_t));
        if (ladder_ctx->network == NULL)
            goto cleanup;

        for (uint32_t nt = 0; nt < networks_qty; nt++) {
            ladder_ctx->network[nt].rows = net_rows_qty;
            ladder_ctx->network[nt].cols = net_columns_qty;

            if (net_rows_qty > SIZE_MAX / sizeof(ladder_cell_t*)) {
                goto cleanup;
            }
            ladder_ctx->network[nt].cells = calloc(net_rows_qty, sizeof(ladder_cell_t*));
            if (ladder_ctx->network[nt].cells == NULL) {
                goto cleanup;
            }
            for (uint32_t cl = 0; cl < net_rows_qty; cl++) {
                if (net_columns_qty > SIZE_MAX / sizeof(ladder_cell_t)) {
                    goto cleanup;
                }
                ladder_ctx->network[nt].cells[cl] = calloc(net_columns_qty, sizeof(ladder_cell_t));
                if (ladder_ctx->network[nt].cells[cl] == NULL) {
                    goto cleanup;
                }
            }

            for (uint32_t column = 0; column < net_columns_qty; column++) {
                for (uint32_t row = 0; row < net_rows_qty; row++) {
                    ladder_ctx->network[nt].cells[row][column].vertical_bar = false;
                    ladder_ctx->network[nt].cells[row][column].state = false;
                    ladder_ctx->network[nt].cells[row][column].data_qty = 0;
                    ladder_ctx->network[nt].cells[row][column].data = NULL;
                }
            }
        }
    }

    if (qty_m > SIZE_MAX / sizeof(uint8_t))
        goto cleanup;
    ladder_ctx->prev_scan_vals.Mh = calloc(qty_m, sizeof(uint8_t));
    if (ladder_ctx->prev_scan_vals.Mh == NULL)
        goto cleanup;

    if (qty_m > SIZE_MAX / sizeof(uint8_t))
        goto cleanup;
    ladder_ctx->memory.M = calloc(qty_m, sizeof(uint8_t));
    if (ladder_ctx->memory.M == NULL)
        goto cleanup;

    if (qty_c > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->prev_scan_vals.Crh = calloc(qty_c, sizeof(bool));
    if (ladder_ctx->prev_scan_vals.Crh == NULL)
        goto cleanup;

    if (qty_c > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->memory.Cr = calloc(qty_c, sizeof(bool));
    if (ladder_ctx->memory.Cr == NULL)
        goto cleanup;

    if (qty_c > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->prev_scan_vals.Cdh = calloc(qty_c, sizeof(bool));
    if (ladder_ctx->prev_scan_vals.Cdh == NULL)
        goto cleanup;

    if (qty_c > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->memory.Cd = calloc(qty_c, sizeof(bool));
    if (ladder_ctx->memory.Cd == NULL)
        goto cleanup;

    if (qty_t > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->prev_scan_vals.Trh = calloc(qty_t, sizeof(bool));
    if (ladder_ctx->prev_scan_vals.Trh == NULL)
        goto cleanup;

    if (qty_t > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->memory.Tr = calloc(qty_t, sizeof(bool));
    if (ladder_ctx->memory.Tr == NULL)
        goto cleanup;

    if (qty_t > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->prev_scan_vals.Tdh = calloc(qty_t, sizeof(bool));
    if (ladder_ctx->prev_scan_vals.Tdh == NULL)
        goto cleanup;

    if (qty_t > SIZE_MAX / sizeof(bool))
        goto cleanup;
    ladder_ctx->memory.Td = calloc(qty_t, sizeof(bool));
    if (ladder_ctx->memory.Td == NULL)
        goto cleanup;

    if (qty_c > SIZE_MAX / sizeof(uint32_t))
        goto cleanup;
    ladder_ctx->registers.C = calloc(qty_c, sizeof(uint32_t));
    if (ladder_ctx->registers.C == NULL)
        goto cleanup;

    if (qty_t > SIZE_MAX / sizeof(ladder_timer_t))
        goto cleanup;
    ladder_ctx->timers = calloc(qty_t, sizeof(ladder_timer_t));
    if (ladder_ctx->timers == NULL)
        goto cleanup;

    if (qty_d > SIZE_MAX / sizeof(int32_t))
        goto cleanup;
    ladder_ctx->registers.D = calloc(qty_d, sizeof(int32_t));
    if (ladder_ctx->registers.D == NULL)
        goto cleanup;

    if (qty_r > SIZE_MAX / sizeof(float))
        goto cleanup;
    ladder_ctx->registers.R = calloc(qty_r, sizeof(float));
    if (ladder_ctx->registers.R == NULL)
        goto cleanup;

#ifdef OPTIONAL_CRON
    ladder_ctx->cron = calloc(1, sizeof(ladderlib_cron_t));
    if (ladder_ctx->cron == NULL)
        goto cleanup;
#endif

    ladder_ctx->ladder.quantity.networks = networks_qty;
    ladder_ctx->ladder.quantity.m = qty_m;
    ladder_ctx->ladder.quantity.c = qty_c;
    ladder_ctx->ladder.quantity.t = qty_t;
    ladder_ctx->ladder.quantity.d = qty_d;
    ladder_ctx->ladder.quantity.r = qty_r;
    ladder_ctx->ladder.quantity.delay_not_run = delay_not_run == 0 ? 1 : delay_not_run;
    ladder_ctx->ladder.quantity.watchdog_ms = watchdog_ms;
    ladder_ctx->ladder.state = LADDER_ST_STOPPED;

    return true;

    cleanup:
    ladder_ctx_deinit(ladder_ctx);
    return false;
}

bool ladder_ctx_deinit(ladder_ctx_t *ladder_ctx) {
    ladder_clear_memory(ladder_ctx);
    ladder_clear_program(ladder_ctx);

    // Free networks, including cells and their data
    if (ladder_ctx->network != NULL) {
        for (uint32_t nt = 0; nt < ladder_ctx->ladder.quantity.networks; nt++) {
            for (uint32_t r = 0; r < ladder_ctx->network[nt].rows; r++) {
                for (uint32_t c = 0; c < ladder_ctx->network[nt].cols; c++) {
                    if (ladder_ctx->network[nt].cells[r][c].data != NULL) {
                        // Free any allocated strings (cstr) before freeing the data array
                        // This prevents memory leaks from strdup calls in other parts of the code (e.g., JSON parsing)
                        for (uint32_t d = 0; d < ladder_ctx->network[nt].cells[r][c].data_qty; d++) {
                            if (ladder_ctx->network[nt].cells[r][c].data[d].type == LADDER_REGISTER_S&&
                            ladder_ctx->network[nt].cells[r][c].data[d].value.cstr != NULL) {
                                free((void*) ladder_ctx->network[nt].cells[r][c].data[d].value.cstr);
                                ladder_ctx->network[nt].cells[r][c].data[d].value.cstr = NULL; // Prevent dangling pointer
                            }
                        }
                        free(ladder_ctx->network[nt].cells[r][c].data);
                        ladder_ctx->network[nt].cells[r][c].data = NULL;
                    }
                }
                free(ladder_ctx->network[nt].cells[r]);
            }
            free(ladder_ctx->network[nt].cells);
        }
        free(ladder_ctx->network);
    }

    free(ladder_ctx->memory.M);
    ladder_ctx->memory.M = NULL;
    free(ladder_ctx->memory.Cr);
    ladder_ctx->memory.Cr = NULL;
    free(ladder_ctx->memory.Cd);
    ladder_ctx->memory.Cd = NULL;
    free(ladder_ctx->memory.Tr);
    ladder_ctx->memory.Tr = NULL;
    free(ladder_ctx->memory.Td);
    ladder_ctx->memory.Td = NULL;

    free(ladder_ctx->prev_scan_vals.Mh);
    ladder_ctx->prev_scan_vals.Mh = NULL;
    free(ladder_ctx->prev_scan_vals.Crh);
    ladder_ctx->prev_scan_vals.Crh = NULL;
    free(ladder_ctx->prev_scan_vals.Cdh);
    ladder_ctx->prev_scan_vals.Cdh = NULL;
    free(ladder_ctx->prev_scan_vals.Trh);
    ladder_ctx->prev_scan_vals.Trh = NULL;
    free(ladder_ctx->prev_scan_vals.Tdh);
    ladder_ctx->prev_scan_vals.Tdh = NULL;

    free(ladder_ctx->registers.C);
    ladder_ctx->registers.C = NULL;
    free(ladder_ctx->registers.D);
    ladder_ctx->registers.D = NULL;
    free(ladder_ctx->registers.R);
    ladder_ctx->registers.R = NULL;

    free(ladder_ctx->timers);
    ladder_ctx->timers = NULL;

    for (uint32_t f = 0; f < ladder_ctx->foreign.qty; f++) {
        if (ladder_ctx->foreign.fn[f].deinit) {
            ladder_ctx->foreign.fn[f].deinit(&(ladder_ctx->foreign.fn[f]));
        }
    }
    free(ladder_ctx->foreign.fn);
    ladder_ctx->foreign.fn = NULL;

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

    free(ladder_ctx->hw.io.read);
    ladder_ctx->hw.io.read = NULL;
    free(ladder_ctx->hw.io.write);
    ladder_ctx->hw.io.write = NULL;
    free(ladder_ctx->hw.io.init_read);
    ladder_ctx->hw.io.init_read = NULL;
    free(ladder_ctx->hw.io.init_write);
    ladder_ctx->hw.io.init_write = NULL;
    free(ladder_ctx->input);
    ladder_ctx->input = NULL;
    free(ladder_ctx->output);
    ladder_ctx->output = NULL;

#ifdef OPTIONAL_CRON
    if (((ladderlib_cron_t*) ladder_ctx->cron)->ctx != NULL)
        ladderlib_cron_deinit(ladder_ctx);
    free(ladder_ctx->cron);
    ladder_ctx->cron = NULL;
#endif

    return true;
}

bool ladder_add_read_fn(ladder_ctx_t *ladder_ctx, _io_read read, _io_init read_init) {
    void *tmp_read = NULL;
    void *tmp_init_read = NULL;
    void *tmp_input = NULL;

    if (ladder_ctx->hw.io.fn_read_qty == 0) {
        tmp_read = calloc(1, sizeof(_io_read*));
        tmp_init_read = calloc(1, sizeof(_io_init*));
        tmp_input = calloc(1, sizeof(ladder_hw_input_vals_t));
    } else {
        tmp_read = realloc(ladder_ctx->hw.io.read, (ladder_ctx->hw.io.fn_read_qty + 1) * sizeof(_io_read*));
        tmp_init_read = realloc(ladder_ctx->hw.io.init_read, (ladder_ctx->hw.io.fn_read_qty + 1) * sizeof(_io_init*));
        tmp_input = realloc(ladder_ctx->input, (ladder_ctx->hw.io.fn_read_qty + 1) * sizeof(ladder_hw_input_vals_t));
    }

    // Check all allocations succeeded before assigning to ensure atomicity.
    // If any fail, free any newly allocated temps to prevent leaks from partial successes.
    if (tmp_read == NULL || tmp_init_read == NULL || tmp_input == NULL) {
        free(tmp_read);
        free(tmp_init_read);
        free(tmp_input);
        return false;
    }

    // Assign only after all reallocs/callocs succeed, preventing inconsistent array sizes.
    ladder_ctx->hw.io.read = tmp_read;
    ladder_ctx->hw.io.init_read = tmp_init_read;
    ladder_ctx->input = tmp_input;

    // Zero-initialize the new input struct to prevent undefined behavior from uninitialized fields.
    memset(&ladder_ctx->input[ladder_ctx->hw.io.fn_read_qty], 0, sizeof(ladder_hw_input_vals_t));

    ladder_ctx->input[ladder_ctx->hw.io.fn_read_qty].fn_id = ladder_ctx->hw.io.fn_read_qty;
    ladder_ctx->input[ladder_ctx->hw.io.fn_read_qty].i_qty = 0;
    ladder_ctx->input[ladder_ctx->hw.io.fn_read_qty].iw_qty = 0;
    ladder_ctx->hw.io.read[ladder_ctx->hw.io.fn_read_qty] = read;
    ladder_ctx->hw.io.init_read[ladder_ctx->hw.io.fn_read_qty] = read_init;

    if (!read_init(ladder_ctx, ladder_ctx->hw.io.fn_read_qty, true)) {
        read_init(ladder_ctx, ladder_ctx->hw.io.fn_read_qty, false);

        if (ladder_ctx->hw.io.fn_read_qty == 0) {
            free(ladder_ctx->hw.io.read);
            free(ladder_ctx->hw.io.init_read);
            free(ladder_ctx->input);
            ladder_ctx->hw.io.read = NULL;
            ladder_ctx->hw.io.init_read = NULL;
            ladder_ctx->input = NULL;
        } else {
            ladder_ctx->hw.io.read = realloc(ladder_ctx->hw.io.read, ladder_ctx->hw.io.fn_read_qty * sizeof(_io_read*));
            ladder_ctx->hw.io.init_read = realloc(ladder_ctx->hw.io.init_read, ladder_ctx->hw.io.fn_read_qty * sizeof(_io_init*));
            ladder_ctx->input = realloc(ladder_ctx->input, ladder_ctx->hw.io.fn_read_qty * sizeof(ladder_hw_input_vals_t));
        }
        return false;
    }

    ++ladder_ctx->hw.io.fn_read_qty;

    return true;
}

bool ladder_add_write_fn(ladder_ctx_t *ladder_ctx, _io_write write, _io_init write_init) {
    if (ladder_ctx == NULL || write == NULL || write_init == NULL)
        return false;

    void *tmp_write = NULL;
    void *tmp_init_write = NULL;
    void *tmp_output = NULL;

    // Atomic allocation check preserved, but failure path fixed below
    if (ladder_ctx->hw.io.fn_write_qty == 0) {
        tmp_write = calloc(1, sizeof(_io_write*));
        tmp_init_write = calloc(1, sizeof(_io_init*));
        tmp_output = calloc(1, sizeof(ladder_hw_output_vals_t));
    } else {
        tmp_write = realloc(ladder_ctx->hw.io.write, (ladder_ctx->hw.io.fn_write_qty + 1) * sizeof(_io_write*));
        tmp_init_write = realloc(ladder_ctx->hw.io.init_write, (ladder_ctx->hw.io.fn_write_qty + 1) * sizeof(_io_init*));
        tmp_output = realloc(ladder_ctx->output, (ladder_ctx->hw.io.fn_write_qty + 1) * sizeof(ladder_hw_output_vals_t));
    }

    // Check all allocations succeeded before assigning to ensure atomicity.
    // If any fail, free any newly allocated temps to prevent leaks from partial successes.
    if (tmp_write == NULL || tmp_init_write == NULL || tmp_output == NULL) {
        free(tmp_write);
        free(tmp_init_write);
        free(tmp_output);
        return false;
    }

    // Assign only after all reallocs/callocs succeed, preventing inconsistent array sizes.
    ladder_ctx->hw.io.write = tmp_write;
    ladder_ctx->hw.io.init_write = tmp_init_write;
    ladder_ctx->output = tmp_output;

    // Zero-initialize the new output struct to prevent undefined behavior from uninitialized fields.
    memset(&ladder_ctx->output[ladder_ctx->hw.io.fn_write_qty], 0, sizeof(ladder_hw_output_vals_t));

    ladder_ctx->output[ladder_ctx->hw.io.fn_write_qty].fn_id = ladder_ctx->hw.io.fn_write_qty;
    ladder_ctx->output[ladder_ctx->hw.io.fn_write_qty].q_qty = 0;
    ladder_ctx->output[ladder_ctx->hw.io.fn_write_qty].qw_qty = 0;
    ladder_ctx->hw.io.write[ladder_ctx->hw.io.fn_write_qty] = write;
    ladder_ctx->hw.io.init_write[ladder_ctx->hw.io.fn_write_qty] = write_init;

    if (!write_init(ladder_ctx, ladder_ctx->hw.io.fn_write_qty, true)) {
        write_init(ladder_ctx, ladder_ctx->hw.io.fn_write_qty, false);

        if (ladder_ctx->hw.io.fn_write_qty == 0) {
            free(ladder_ctx->hw.io.write);
            free(ladder_ctx->hw.io.init_write);
            free(ladder_ctx->output);
            ladder_ctx->hw.io.write = NULL;
            ladder_ctx->hw.io.init_write = NULL;
            ladder_ctx->output = NULL;
        } else {
            ladder_ctx->hw.io.write = realloc(ladder_ctx->hw.io.write, ladder_ctx->hw.io.fn_write_qty * sizeof(_io_write*));
            ladder_ctx->hw.io.init_write = realloc(ladder_ctx->hw.io.init_write, ladder_ctx->hw.io.fn_write_qty * sizeof(_io_init*));
            ladder_ctx->output = realloc(ladder_ctx->output, ladder_ctx->hw.io.fn_write_qty * sizeof(ladder_hw_output_vals_t));
        }
        return false;
    }

    ++ladder_ctx->hw.io.fn_write_qty;

    return true;
}

bool ladder_add_foreign(ladder_ctx_t *ladder_ctx, _foreign_fn_init fn_init, void *init_data, uint32_t qty) {
    if (fn_init == NULL)
        return false;

    void *tmp_fn = NULL;
    ladder_foreign_function_t fn_new;
    memset(&fn_new, 0, sizeof(ladder_foreign_function_t));
    fn_new.id = ladder_ctx->foreign.qty;

    if (!fn_init(&fn_new, init_data, qty))
        return false;

    if (ladder_ctx->foreign.qty == 0)
        tmp_fn = malloc(sizeof(ladder_foreign_function_t));
    else
        tmp_fn = realloc(ladder_ctx->foreign.fn, (ladder_ctx->foreign.qty + 1) * sizeof(ladder_foreign_function_t));

    if (!tmp_fn) {
        if (fn_new.deinit)
            fn_new.deinit(&fn_new);
        return false;
    }

    ladder_ctx->foreign.fn = tmp_fn;
    memcpy(&(ladder_ctx->foreign.fn[ladder_ctx->foreign.qty]), &fn_new, sizeof(ladder_foreign_function_t));
    ++ladder_ctx->foreign.qty;

    return true;
}

bool ladder_fn_cell(ladder_ctx_t *ladder_ctx, uint32_t network, uint32_t row, uint32_t column, ladder_instruction_t function, uint32_t foreign_id) {
    ladder_instructions_iocd_t actual_ioc;

    if (function == LADDER_INS_FOREIGN) {
        if (ladder_ctx->foreign.qty < foreign_id + 1)
            return false;
        else memcpy(&actual_ioc, &(ladder_ctx->foreign.fn[foreign_id]).description, sizeof(ladder_instructions_iocd_t));
    } else memcpy(&actual_ioc, &(ladder_fn_iocd[function]), sizeof(ladder_instructions_iocd_t));

    // not available rows for function
    if (ladder_ctx->network[network].rows < actual_ioc.cells + row)
        return false;

    if (ladder_ctx->network[network].cells[row][column].data != NULL) {
        for (uint32_t d = 0; d < ladder_ctx->network[network].cells[row][column].data_qty; d++) {
            if (ladder_ctx->network[network].cells[row][column].data[d].type == LADDER_REGISTER_S&&
            ladder_ctx->network[network].cells[row][column].data[d].value.cstr != NULL) {
                free((void*) ladder_ctx->network[network].cells[row][column].data[d].value.cstr);
                ladder_ctx->network[network].cells[row][column].data[d].value.cstr = NULL;
            }
        }
        free(ladder_ctx->network[network].cells[row][column].data);
    }

    ladder_ctx->network[network].cells[row][column].code = function;
    ladder_ctx->network[network].cells[row][column].data_qty = actual_ioc.data_qty;

    if (actual_ioc.data_qty == 0) {
        ladder_ctx->network[network].cells[row][column].data = NULL;
        return true;
    }

    ladder_ctx->network[network].cells[row][column].data = calloc(actual_ioc.data_qty, sizeof(ladder_value_t));
    if (ladder_ctx->network[network].cells[row][column].data == NULL) {
        ladder_ctx->network[network].cells[row][column].data_qty = 0;
        return false;
    }

    for (uint8_t r = 1; r < ladder_fn_iocd[function].cells; r++) {
        ladder_ctx->network[network].cells[row + r][column].code = LADDER_INS_MULTI;

        if (ladder_ctx->network[network].cells[row + r][column].data != NULL) {
            for (uint32_t d = 0; d < ladder_ctx->network[network].cells[row + r][column].data_qty; d++) {
                if (ladder_ctx->network[network].cells[row + r][column].data[d].type == LADDER_REGISTER_S&&
                ladder_ctx->network[network].cells[row + r][column].data[d].value.cstr != NULL) {
                    free((void*) ladder_ctx->network[network].cells[row + r][column].data[d].value.cstr);
                    ladder_ctx->network[network].cells[row + r][column].data[d].value.cstr = NULL;
                }
            }
            free(ladder_ctx->network[network].cells[row + r][column].data);
            ladder_ctx->network[network].cells[row + r][column].data = NULL;
            ladder_ctx->network[network].cells[row + r][column].data_qty = 0;
        }
    }

    return true;
}
