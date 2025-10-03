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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ladder.h"
#include "ladder_internals.h"

#define MAX_WAIT_CYCLES 1000

void ladder_task(void *ladderctx) {
    if (ladderctx == NULL)
        return;

    ladder_ctx_t *ladder_ctx = (ladder_ctx_t*) ladderctx;

    if (ladder_ctx == NULL || ladder_ctx->hw.time.millis == NULL || ladder_ctx->hw.time.delay == NULL
            || (ladder_ctx->hw.io.fn_read_qty > 0 && (ladder_ctx->hw.io.read == NULL || ladder_ctx->input == NULL))
            || (ladder_ctx->hw.io.fn_write_qty > 0 && (ladder_ctx->hw.io.write == NULL || ladder_ctx->output == NULL))
            || (ladder_ctx->ladder.quantity.m > 0 && (ladder_ctx->memory.M == NULL || ladder_ctx->prev_scan_vals.Mh == NULL))
            || (ladder_ctx->ladder.quantity.c > 0 && (ladder_ctx->memory.Cd == NULL || ladder_ctx->memory.Cr == NULL || ladder_ctx->prev_scan_vals.Cdh == NULL || ladder_ctx->prev_scan_vals.Crh == NULL || ladder_ctx->registers.C == NULL))
            || (ladder_ctx->ladder.quantity.t > 0 && (ladder_ctx->memory.Td == NULL || ladder_ctx->memory.Tr == NULL || ladder_ctx->prev_scan_vals.Tdh == NULL || ladder_ctx->prev_scan_vals.Trh == NULL || ladder_ctx->timers == NULL))
            || (ladder_ctx->ladder.quantity.d > 0 && ladder_ctx->registers.D == NULL)
            || (ladder_ctx->ladder.quantity.r > 0 && ladder_ctx->registers.R == NULL)
            || (ladder_ctx->ladder.quantity.networks > 0 && ladder_ctx->network == NULL)) {
        if (ladder_ctx != NULL)
            ladder_ctx->ladder.state = LADDER_ST_NULLFN;
        return;
    }

    // task main loop
    while (ladder_ctx->ladder.state != LADDER_ST_EXIT_TSK) {
        // Inner while with bounded loop using wait_count to prevent infinite wait.
        // Initializes counter, adds timeout condition, retains EXIT_TSK check, and sets ERROR on timeout.
        uint32_t wait_count = 0;
        while (ladder_ctx->ladder.state != LADDER_ST_RUNNING && wait_count < MAX_WAIT_CYCLES) {
            if (ladder_ctx->ladder.state == LADDER_ST_EXIT_TSK)
                return;
            ladder_ctx->hw.time.delay(ladder_ctx->ladder.quantity.delay_not_run);
            wait_count++;
        }
        if (wait_count >= MAX_WAIT_CYCLES) {
            ladder_ctx->ladder.state = LADDER_ST_ERROR;
            // Invoke panic here after timeout, integrating it into the error path for consistency.
            if (ladder_ctx->on.panic != NULL)
                ladder_ctx->on.panic(ladder_ctx);
            // After panic (user recovery opportunity), force EXIT if still not RUNNING to prevent infinite loop on persistent ERROR/STOPPED.
            if (ladder_ctx->ladder.state != LADDER_ST_RUNNING) {
                ladder_ctx->ladder.state = LADDER_ST_EXIT_TSK;
            }
        }

        // Proceed only if now RUNNING (after wait or direct)
        if (ladder_ctx->ladder.state != LADDER_ST_RUNNING) {
            continue;  // Skip to next iteration if still not running after wait/timeout.
        }

        // Set start_time here to capture full cycle time (before pre-hook, reads, scan, writes)
        ladder_ctx->scan_internals.start_time = ladder_ctx->hw.time.millis();

        // external function before scan
        if (ladder_ctx->on.task_before != NULL)
            ladder_ctx->on.task_before(ladder_ctx);

        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_read_qty; n++)
            ladder_ctx->hw.io.read[n](ladder_ctx, n);

        // Revised comment to reflect correct timing for input and output history.
        // Copy output history before scan to ensure RE/FE on outputs use pre-scan values (last cycle's final outputs).
        // Input history is now updated at the end of the cycle for the next scan's edge detection.
        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++) {
            memcpy(ladder_ctx->output[n].Qh, ladder_ctx->output[n].Q, ladder_ctx->output[n].q_qty * sizeof(uint8_t));
        }

        // ladder program scan
        ladder_scan(ladder_ctx);
        if (ladder_ctx->ladder.state == LADDER_ST_INV) {
            ladder_ctx->ladder.state = LADDER_ST_EXIT_TSK;
            // Reversion to last good state to avoid partial updates from failed scan
            for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_read_qty; n++) {
                memcpy(ladder_ctx->input[n].Ih, ladder_ctx->input[n].I, ladder_ctx->input[n].i_qty * sizeof(uint8_t));  // Always copy current inputs to history
            }
            for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++) {
                memcpy(ladder_ctx->output[n].Q, ladder_ctx->output[n].Qh, ladder_ctx->output[n].q_qty * sizeof(uint8_t));  // Revert Q to last good
            }
            memcpy(ladder_ctx->memory.M, ladder_ctx->prev_scan_vals.Mh, ladder_ctx->ladder.quantity.m * sizeof(uint8_t));  // Revert M to last good
            memcpy(ladder_ctx->memory.Cd, ladder_ctx->prev_scan_vals.Cdh, ladder_ctx->ladder.quantity.c * sizeof(bool));  // Revert Cd
            memcpy(ladder_ctx->memory.Cr, ladder_ctx->prev_scan_vals.Crh, ladder_ctx->ladder.quantity.c * sizeof(bool));  // Revert Cr
            memcpy(ladder_ctx->memory.Td, ladder_ctx->prev_scan_vals.Tdh, ladder_ctx->ladder.quantity.t * sizeof(bool));  // Revert Td
            memcpy(ladder_ctx->memory.Tr, ladder_ctx->prev_scan_vals.Trh, ladder_ctx->ladder.quantity.t * sizeof(bool));  // Revert Tr

            // Reversion for counters (C), timers (T.acc), D, R which were missing
            if (ladder_ctx->ladder.quantity.c > 0 && ladder_ctx->registers.C != NULL) {
                memset(ladder_ctx->registers.C, 0, ladder_ctx->ladder.quantity.c * sizeof(uint32_t));  // Reset counters to 0 on fault
            }
            if (ladder_ctx->ladder.quantity.t > 0 && ladder_ctx->timers != NULL) {
                for (uint32_t i = 0; i < ladder_ctx->ladder.quantity.t; ++i) {
                    ladder_ctx->timers[i].acc = 0;  // Reset timer accumulators
                }
            }
            if (ladder_ctx->ladder.quantity.d > 0 && ladder_ctx->registers.D != NULL) {
                memset(ladder_ctx->registers.D, 0, ladder_ctx->ladder.quantity.d * sizeof(int32_t));  // Reset D to 0
            }
            if (ladder_ctx->ladder.quantity.r > 0 && ladder_ctx->registers.R != NULL) {
                memset(ladder_ctx->registers.R, 0, ladder_ctx->ladder.quantity.r * sizeof(float));  // Reset R to 0
            }

            ladder_save_previous_values(ladder_ctx);  // Save the reverted state to history for consistency

            // If !write_on_fault, clear outputs to safe state (0)
            if (!ladder_ctx->ladder.write_on_fault) {
                for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++) {
                    memset(ladder_ctx->output[n].Q, 0, ladder_ctx->output[n].q_qty * sizeof(uint8_t));
                    memset(ladder_ctx->output[n].QW, 0, ladder_ctx->output[n].qw_qty * sizeof(int32_t));
                    memcpy(ladder_ctx->output[n].Qh, ladder_ctx->output[n].Q, ladder_ctx->output[n].q_qty * sizeof(uint8_t));  // Update history to cleared state
                }
            }
            // Always perform writes on fault to enforce hold last good (if write_on_fault) or cleared state
            for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++)
                ladder_ctx->hw.io.write[n](ladder_ctx, n);

            ladder_scan_time(ladder_ctx);

            // external function after scan
            if (ladder_ctx->on.task_after != NULL)
                ladder_ctx->on.task_after(ladder_ctx);

            goto exit;
        }

        ladder_save_previous_values(ladder_ctx);

        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++)
            ladder_ctx->hw.io.write[n](ladder_ctx, n);

        // ensuring the next cycle's edge detection compares new fresh inputs to this cycle's.
        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_read_qty; n++) {
            memcpy(ladder_ctx->input[n].Ih, ladder_ctx->input[n].I, ladder_ctx->input[n].i_qty * sizeof(uint8_t));
        }

        ladder_scan_time(ladder_ctx);

        // external function after scan
        if (ladder_ctx->on.task_after != NULL)
            ladder_ctx->on.task_after(ladder_ctx);
    }

    exit:
    if (ladder_ctx != NULL && ladder_ctx->on.end_task != NULL)
        ladder_ctx->on.end_task(ladder_ctx);
}
