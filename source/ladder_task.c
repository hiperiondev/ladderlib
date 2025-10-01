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
            || (ladder_ctx->hw.io.fn_read_qty > 0 && ladder_ctx->hw.io.read == NULL)
            || (ladder_ctx->hw.io.fn_write_qty > 0 && ladder_ctx->hw.io.write == NULL)) {
        if (ladder_ctx != NULL)
            ladder_ctx->ladder.state = LADDER_ST_NULLFN;
        return;
    }

    // task main loop
    while (ladder_ctx->ladder.state != LADDER_ST_EXIT_TSK) {
        if (ladder_ctx->ladder.state != LADDER_ST_RUNNING) {
            if (ladder_ctx->on.panic != NULL)
                ladder_ctx->on.panic(ladder_ctx);

            break;
        }

        /* Inner while with bounded loop using wait_count to prevent infinite wait.
         * Initializes counter, adds timeout condition, retains EXIT_TSK check, and sets ERROR on timeout.
         */
        uint32_t wait_count = 0;
        while (ladder_ctx->ladder.state != LADDER_ST_RUNNING && wait_count < MAX_WAIT_CYCLES) {
            if (ladder_ctx->ladder.state == LADDER_ST_EXIT_TSK)
                return;
            ladder_ctx->hw.time.delay(ladder_ctx->ladder.quantity.delay_not_run);
            wait_count++;
        }
        if (wait_count >= MAX_WAIT_CYCLES) {
            ladder_ctx->ladder.state = LADDER_ST_ERROR;
        }

        // Set start_time here to capture full cycle time (before pre-hook, reads, scan, writes)
        ladder_ctx->scan_internals.start_time = ladder_ctx->hw.time.millis();

        // external function before scan
        if (ladder_ctx->on.task_before != NULL)
            ladder_ctx->on.task_before(ladder_ctx);

        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_read_qty; n++)
            ladder_ctx->hw.io.read[n](ladder_ctx, n);

        // ladder program scan
        ladder_scan(ladder_ctx);
        if (ladder_ctx->ladder.state == LADDER_ST_INV) {
            ladder_ctx->ladder.state = LADDER_ST_EXIT_TSK;
            if (ladder_ctx->ladder.write_on_fault) {
                // Perform writes even on INV for continuity.
                for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_read_qty; n++) {
                    memcpy(ladder_ctx->input[n].Ih, ladder_ctx->input[n].I, ladder_ctx->input[n].i_qty * sizeof(uint8_t));
                }
                for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++) {
                    memcpy(ladder_ctx->output[n].Qh, ladder_ctx->output[n].Q, ladder_ctx->output[n].q_qty * sizeof(uint8_t));
                }

                ladder_save_previous_values(ladder_ctx);

                for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++)
                    ladder_ctx->hw.io.write[n](ladder_ctx, n);

                ladder_scan_time(ladder_ctx);

                // external function after scan
                if (ladder_ctx->on.task_after != NULL)
                    ladder_ctx->on.task_after(ladder_ctx);
            }
            goto exit;
        }

        // Add copies for I/O previous values post-scan (before internals save)
        // This captures updated Q from scan and stable I from read, enabling correct RE/FE on I/Q
        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_read_qty; n++) {
            memcpy(ladder_ctx->input[n].Ih, ladder_ctx->input[n].I, ladder_ctx->input[n].i_qty * sizeof(uint8_t));
        }
        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++) {
            memcpy(ladder_ctx->output[n].Qh, ladder_ctx->output[n].Q, ladder_ctx->output[n].q_qty * sizeof(uint8_t));
        }

        ladder_save_previous_values(ladder_ctx);

        for (uint32_t n = 0; n < ladder_ctx->hw.io.fn_write_qty; n++)
            ladder_ctx->hw.io.write[n](ladder_ctx, n);

        ladder_scan_time(ladder_ctx);

        // external function after scan
        if (ladder_ctx->on.task_after != NULL)
            ladder_ctx->on.task_after(ladder_ctx);
    }

    exit:
    if (ladder_ctx != NULL && ladder_ctx->on.end_task != NULL)
        ladder_ctx->on.end_task(ladder_ctx);
}
