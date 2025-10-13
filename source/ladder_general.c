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
    if (ladder_ctx == NULL || ladder_ctx->hw.time.millis == NULL) {
        if (ladder_ctx != NULL) {
            ladder_ctx->ladder.state = LADDER_ST_ERROR;
            if (ladder_ctx->on.panic != NULL) {
                ladder_ctx->on.panic(ladder_ctx);
            }
        }
        return;  // Fallback: no update, scan time remains 0
    }

    uint64_t scanTimeMillis = ladder_ctx->hw.time.millis();
    uint64_t diff;

    // Explicitly handle potential timer wrap-around using modulo arithmetic
    // If scanTimeMillis < start_time, assume wrap occurred and compute correct diff
    if (scanTimeMillis >= ladder_ctx->scan_internals.start_time) {
        diff = scanTimeMillis - ladder_ctx->scan_internals.start_time;
    } else {
        diff = (UINT64_MAX - ladder_ctx->scan_internals.start_time) + scanTimeMillis + 1;
    }

    ladder_ctx->scan_internals.actual_scan_time = diff;
    ladder_ctx->scan_internals.start_time = scanTimeMillis;

    if (ladder_ctx->ladder.quantity.watchdog_ms > 0 && diff > ladder_ctx->ladder.quantity.watchdog_ms) {
        // Set error and invoke panic immediately for synchronous fault handling.
        ladder_ctx->ladder.state = LADDER_ST_ERROR;
        if (ladder_ctx->on.panic != NULL) {
            ladder_ctx->on.panic(ladder_ctx);
        }
    }
}

bool ladder_fault_clear(ladder_ctx_t *ladder_ctx) {
    if (ladder_ctx == NULL || ladder_ctx->ladder.state != LADDER_ST_ERROR) {
        return false;  // Only clear if actually in error state.
    }

    // Reset state to running.
    ladder_ctx->ladder.state = LADDER_ST_RUNNING;

    // Clear scan accumulators to prevent carryover effects.
    ladder_ctx->scan_internals.actual_scan_time = 0;
    if (ladder_ctx->hw.time.millis == NULL) {
        ladder_ctx->scan_internals.start_time = 0;  // Fallback
        if (ladder_ctx->on.panic != NULL) {
            ladder_ctx->on.panic(ladder_ctx);
        }
    } else {
        ladder_ctx->scan_internals.start_time = ladder_ctx->hw.time.millis();
    }

    // Resets for timers and outputs to ensure safe recovery.
    // Reset timers: Clear accumulators and flags.
    if (ladder_ctx->ladder.quantity.t > 0 && ladder_ctx->timers != NULL) {
        for (uint32_t i = 0; i < ladder_ctx->ladder.quantity.t; ++i) {
            ladder_ctx->timers[i].acc = 0;
            ladder_ctx->memory.Td[i] = false;
            ladder_ctx->memory.Tr[i] = false;
        }
    }

    if (ladder_ctx->ladder.quantity.c > 0) {
        if (ladder_ctx->registers.C != NULL) {
            memset(ladder_ctx->registers.C, 0, ladder_ctx->ladder.quantity.c * sizeof(uint32_t));
        }
        memset(ladder_ctx->memory.Cd, 0, ladder_ctx->ladder.quantity.c * sizeof(bool));
        memset(ladder_ctx->memory.Cr, 0, ladder_ctx->ladder.quantity.c * sizeof(bool));
    }

    if (ladder_ctx->ladder.quantity.d > 0 && ladder_ctx->registers.D != NULL) {
        memset(ladder_ctx->registers.D, 0, ladder_ctx->ladder.quantity.d * sizeof(int32_t));
    }
    if (ladder_ctx->ladder.quantity.r > 0 && ladder_ctx->registers.R != NULL) {
        memset(ladder_ctx->registers.R, 0, ladder_ctx->ladder.quantity.r * sizeof(float));
    }

    // Reset outputs: Set to safe state (e.g., all false/0) for each module.
    if (ladder_ctx->hw.io.fn_write_qty > 0 && ladder_ctx->output != NULL) {
        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; ++n) {
            if (ladder_ctx->output[n].q_qty > 0 && ladder_ctx->output[n].Q != NULL) {
                memset(ladder_ctx->output[n].Q, 0, ladder_ctx->output[n].q_qty * sizeof(uint8_t));
            }
            if (ladder_ctx->output[n].qw_qty > 0 && ladder_ctx->output[n].QW != NULL) {
                memset(ladder_ctx->output[n].QW, 0, ladder_ctx->output[n].qw_qty * sizeof(int32_t));
            }
            // Also update history to match.
            if (ladder_ctx->output[n].Qh != NULL) {
                memcpy(ladder_ctx->output[n].Qh, ladder_ctx->output[n].Q, ladder_ctx->output[n].q_qty * sizeof(uint8_t));
            }
        }
    }

    // Optional: Invoke a clear hook if registered (for logging recovery).
    if (ladder_ctx->on.end_task != NULL) {  // Reuse end_task as clear notifier, or add dedicated if needed.
        ladder_ctx->on.end_task(ladder_ctx);
    }

    return true;
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
        uint32_t qty_t, uint32_t qty_d, uint32_t qty_r, uint32_t delay_not_run, uint32_t watchdog_ms, bool init_network, bool write_on_fault) {
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
    ladder_ctx->ladder.write_on_fault = write_on_fault ? true : false;

    if (init_network) {
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
    if (ladder_ctx == NULL || read == NULL || read_init == NULL) {
        return false;
    }

    if (ladder_ctx->hw.io.fn_read_qty >= 255) {  // Arbitrary safe max; define const if needed
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }

    uint32_t new_qty = ladder_ctx->hw.io.fn_read_qty + 1;
    size_t fn_size = sizeof(_io_read);
    size_t init_size = sizeof(_io_init);
    size_t input_size = sizeof(ladder_hw_input_vals_t);
    uint32_t old_qty = ladder_ctx->hw.io.fn_read_qty;

    if (old_qty == 0) {
        // Initial allocation: calloc all three, check each, free partial on any failure.
        _io_read *tmp_read = calloc(1, fn_size);
        if (!tmp_read)
            return false;

        _io_init *tmp_init_read = calloc(1, init_size);
        if (!tmp_init_read) {
            free(tmp_read);
            return false;
        }

        ladder_hw_input_vals_t *tmp_input = calloc(1, input_size);
        if (!tmp_input) {
            free(tmp_read);
            free(tmp_init_read);
            return false;
        }

        // All succeeded: assign.
        ladder_ctx->hw.io.read = tmp_read;
        ladder_ctx->hw.io.init_read = tmp_init_read;
        ladder_ctx->input = tmp_input;

        // Initialize the new slot.
        ladder_ctx->hw.io.read[0] = read;
        ladder_ctx->hw.io.init_read[0] = read_init;
        ladder_ctx->input[0].fn_id = 0;
        ladder_ctx->input[0].i_qty = 0;
        ladder_ctx->input[0].iw_qty = 0;

        if (!read_init(ladder_ctx, 0, true)) {
            read_init(ladder_ctx, 0, false);
            free(ladder_ctx->hw.io.read);
            free(ladder_ctx->hw.io.init_read);
            free(ladder_ctx->input);
            ladder_ctx->hw.io.read = NULL;
            ladder_ctx->hw.io.init_read = NULL;
            ladder_ctx->input = NULL;
            ladder_ctx->hw.io.fn_read_qty = 0;
            return false;
        }

        ladder_ctx->hw.io.fn_read_qty = 1;
        return true;
    } else {
        // Expansion: realloc one at a time, assign immediately on success, return false on any failure.
        _io_read *tmp_read = realloc(ladder_ctx->hw.io.read, new_qty * fn_size);
        if (!tmp_read)
            return false;  // Original unchanged.
        ladder_ctx->hw.io.read = tmp_read;

        _io_init *tmp_init_read = realloc(ladder_ctx->hw.io.init_read, new_qty * init_size);
        if (!tmp_init_read) {
            // Undo previous success: realloc back to old size.
            ladder_ctx->hw.io.read = realloc(ladder_ctx->hw.io.read, old_qty * fn_size);
            return false;
        }
        ladder_ctx->hw.io.init_read = tmp_init_read;

        ladder_hw_input_vals_t *tmp_input = realloc(ladder_ctx->input, new_qty * input_size);
        if (!tmp_input) {
            // Undo previous successes.
            ladder_ctx->hw.io.read = realloc(ladder_ctx->hw.io.read, old_qty * fn_size);
            ladder_ctx->hw.io.init_read = realloc(ladder_ctx->hw.io.init_read, old_qty * init_size);
            return false;
        }
        ladder_ctx->input = tmp_input;

        // All succeeded: initialize new slot.
        uint32_t idx = old_qty;
        ladder_ctx->hw.io.read[idx] = read;
        ladder_ctx->hw.io.init_read[idx] = read_init;
        ladder_ctx->input[idx].fn_id = idx;
        ladder_ctx->input[idx].i_qty = 0;
        ladder_ctx->input[idx].iw_qty = 0;

        if (!read_init(ladder_ctx, idx, true)) {
            read_init(ladder_ctx, idx, false);
            // Rollback allocations to previous size.
            ladder_ctx->hw.io.read = realloc(ladder_ctx->hw.io.read, old_qty * fn_size);
            ladder_ctx->hw.io.init_read = realloc(ladder_ctx->hw.io.init_read, old_qty * init_size);
            ladder_ctx->input = realloc(ladder_ctx->input, old_qty * input_size);
            return false;
        }

        ladder_ctx->hw.io.fn_read_qty = new_qty;
        return true;
    }
}

bool ladder_add_write_fn(ladder_ctx_t *ladder_ctx, _io_write write, _io_init write_init) {
    if (ladder_ctx == NULL || write == NULL || write_init == NULL) {
        return false;
    }

    if (ladder_ctx->hw.io.fn_write_qty >= 255) {
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }

    uint32_t new_qty = ladder_ctx->hw.io.fn_write_qty + 1;
    size_t fn_size = sizeof(_io_write);
    size_t init_size = sizeof(_io_init);
    size_t output_size = sizeof(ladder_hw_output_vals_t);
    uint32_t old_qty = ladder_ctx->hw.io.fn_write_qty;

    if (old_qty == 0) {
        // Initial allocation: calloc all three, check each, free partial on any failure.
        _io_write *tmp_write = calloc(1, fn_size);
        if (!tmp_write)
            return false;

        _io_init *tmp_init_write = calloc(1, init_size);
        if (!tmp_init_write) {
            free(tmp_write);
            return false;
        }

        ladder_hw_output_vals_t *tmp_output = calloc(1, output_size);
        if (!tmp_output) {
            free(tmp_write);
            free(tmp_init_write);
            return false;
        }

        // All succeeded: assign.
        ladder_ctx->hw.io.write = tmp_write;
        ladder_ctx->hw.io.init_write = tmp_init_write;
        ladder_ctx->output = tmp_output;

        // Initialize the new slot.
        ladder_ctx->hw.io.write[0] = write;
        ladder_ctx->hw.io.init_write[0] = write_init;
        ladder_ctx->output[0].fn_id = 0;
        ladder_ctx->output[0].q_qty = 0;
        ladder_ctx->output[0].qw_qty = 0;

        if (!write_init(ladder_ctx, 0, true)) {
            write_init(ladder_ctx, 0, false);
            free(ladder_ctx->hw.io.write);
            free(ladder_ctx->hw.io.init_write);
            free(ladder_ctx->output);
            ladder_ctx->hw.io.write = NULL;
            ladder_ctx->hw.io.init_write = NULL;
            ladder_ctx->output = NULL;
            ladder_ctx->hw.io.fn_write_qty = 0;
            return false;
        }

        ladder_ctx->hw.io.fn_write_qty = 1;
        return true;
    } else {
        // Expansion: realloc one at a time, assign immediately on success, return false on any failure.
        _io_write *tmp_write = realloc(ladder_ctx->hw.io.write, new_qty * fn_size);
        if (!tmp_write)
            return false;  // Original unchanged.
        ladder_ctx->hw.io.write = tmp_write;

        _io_init *tmp_init_write = realloc(ladder_ctx->hw.io.init_write, new_qty * init_size);
        if (!tmp_init_write) {
            // Undo previous success.
            ladder_ctx->hw.io.write = realloc(ladder_ctx->hw.io.write, old_qty * fn_size);
            return false;
        }
        ladder_ctx->hw.io.init_write = tmp_init_write;

        ladder_hw_output_vals_t *tmp_output = realloc(ladder_ctx->output, new_qty * output_size);
        if (!tmp_output) {
            // Undo previous successes.
            ladder_ctx->hw.io.write = realloc(ladder_ctx->hw.io.write, old_qty * fn_size);
            ladder_ctx->hw.io.init_write = realloc(ladder_ctx->hw.io.init_write, old_qty * init_size);
            return false;
        }
        ladder_ctx->output = tmp_output;

        // All succeeded: initialize new slot.
        uint32_t idx = old_qty;
        ladder_ctx->hw.io.write[idx] = write;
        ladder_ctx->hw.io.init_write[idx] = write_init;
        ladder_ctx->output[idx].fn_id = idx;
        ladder_ctx->output[idx].q_qty = 0;
        ladder_ctx->output[idx].qw_qty = 0;

        if (!write_init(ladder_ctx, idx, true)) {
            write_init(ladder_ctx, idx, false);
            // Rollback allocations to previous size.
            ladder_ctx->hw.io.write = realloc(ladder_ctx->hw.io.write, old_qty * fn_size);
            ladder_ctx->hw.io.init_write = realloc(ladder_ctx->hw.io.init_write, old_qty * init_size);
            ladder_ctx->output = realloc(ladder_ctx->output, old_qty * output_size);
            return false;
        }

        ladder_ctx->hw.io.fn_write_qty = new_qty;
        return true;
    }
}

bool ladder_add_foreign(ladder_ctx_t *ladder_ctx, _foreign_fn_init fn_init, void *init_data, uint32_t qty) {
    if (ladder_ctx == NULL || fn_init == NULL) {
        return false;
    }

    if (qty == 0 || ladder_ctx->foreign.qty + qty > 100) {  // Safe max; define const LADDER_MAX_FOREIGN
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }

    ladder_foreign_function_t fn_new;
    memset(&fn_new, 0, sizeof(ladder_foreign_function_t));
    fn_new.id = ladder_ctx->foreign.qty;

    if (!fn_init(&fn_new, init_data, qty))
        return false;

    void *tmp_fn = NULL;
    if (ladder_ctx->foreign.qty == 0)
        tmp_fn = malloc(sizeof(ladder_foreign_function_t));
    else tmp_fn = realloc(ladder_ctx->foreign.fn, (ladder_ctx->foreign.qty + 1) * sizeof(ladder_foreign_function_t));

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
    if (ladder_ctx == NULL) {
        return false;
    }

    if (network >= ladder_ctx->ladder.quantity.networks) {
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }
    if (row >= ladder_ctx->network[network].rows) {
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }
    if (column >= ladder_ctx->network[network].cols) {
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }

    if (function == LADDER_INS_FOREIGN && foreign_id >= ladder_ctx->foreign.qty) {
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_NOFOREIGN;
        return false;
    }

    ladder_instructions_iocd_t actual_ioc;

    if (function == LADDER_INS_FOREIGN) {
        if (ladder_ctx->foreign.qty < foreign_id + 1) {
            ladder_ctx->ladder.last.err = LADDER_INS_ERR_NOFOREIGN;
            return false;
        } else memcpy(&actual_ioc, &(ladder_ctx->foreign.fn[foreign_id]).description, sizeof(ladder_instructions_iocd_t));
    } else memcpy(&actual_ioc, &(ladder_fn_iocd[function]), sizeof(ladder_instructions_iocd_t));

    // Safe check for multi-cell span to avoid overflow: cells > available rows from row.
    if (actual_ioc.cells > ladder_ctx->network[network].rows - row) {
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_OUTOFRANGE;
        return false;
    }

    // Full span check for all cells (including starting) to be INV before any modifications
    for (uint8_t r = 0; r < actual_ioc.cells; r++) {
        if (ladder_ctx->network[network].cells[row + r][column].code != LADDER_INS_INV) {
            ladder_ctx->ladder.last.err = LADDER_INS_ERR_FAIL;  // Occupied cell in span
            return false;
        }
    }

    // After validation, free any existing data on all spanned cells (defensive)
    for (uint8_t r = 0; r < actual_ioc.cells; r++) {
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

    ladder_ctx->network[network].cells[row][column].code = function;
    ladder_ctx->network[network].cells[row][column].data_qty = actual_ioc.data_qty;

    if (actual_ioc.data_qty == 0) {
        ladder_ctx->network[network].cells[row][column].data = NULL;
        // For single-cell, no vertical_bar needed, but ensure it's false if not part of a larger group
        ladder_ctx->network[network].cells[row][column].vertical_bar = false;
        return true;
    }

    ladder_ctx->network[network].cells[row][column].data = calloc(actual_ioc.data_qty, sizeof(ladder_value_t));
    if (ladder_ctx->network[network].cells[row][column].data == NULL) {
        ladder_ctx->network[network].cells[row][column].data_qty = 0;
        ladder_ctx->network[network].cells[row][column].code = LADDER_INS_INV;
        ladder_ctx->network[network].cells[row][column].data = NULL;
        ladder_ctx->ladder.last.err = LADDER_INS_ERR_FAIL;  // Alloc fail
        return false;
    }

    for (uint8_t r = 1; r < actual_ioc.cells; r++) {
        ladder_ctx->network[network].cells[row + r][column].code = LADDER_INS_MULTI;
    }

    // Set vertical_bar = true for upper cells in multi-cell stack to ensure they form a single group
    for (uint8_t r = 0; r < actual_ioc.cells - 1; r++) {
        ladder_ctx->network[network].cells[row + r][column].vertical_bar = true;
    }
    // Set vertical_bar = false for the bottom cell to terminate the group
    ladder_ctx->network[network].cells[row + actual_ioc.cells - 1][column].vertical_bar = false;

    return true;
}
