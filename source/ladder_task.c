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

#include "ladder.h"
#include "ladder_internals.h"

// ladder logic execution task
void ladder_task(void *ladderctx) {
    ladder_ctx_t *ladder_ctx = NULL;
    if (ladderctx == NULL)
        goto exit;

    ladder_ctx = (ladder_ctx_t*) ladderctx;
    if ((*ladder_ctx).hw.time.millis == NULL || (*ladder_ctx).hw.time.delay == NULL || (*ladder_ctx).hw.io.read == NULL || (*ladder_ctx).hw.io.write == NULL) {
        (*ladder_ctx).ladder.state = LADDER_ST_NULLFN;
        goto exit;
    }

    // task main loop
    while ((*ladder_ctx).ladder.state != LADDER_ST_EXIT_TSK) {
        if ((*ladder_ctx).ladder.state != LADDER_ST_RUNNING) {
            if ((*ladder_ctx).on.panic != NULL)
                (*ladder_ctx).on.panic(ladder_ctx);

            break;
        }

        while ((*ladder_ctx).ladder.state != LADDER_ST_RUNNING) {
            if ((*ladder_ctx).ladder.state == LADDER_ST_EXIT_TSK)
                return;
            (*ladder_ctx).hw.time.delay(10);
        }

        // external function before scan
        if ((*ladder_ctx).on.task_before != NULL)
            (*ladder_ctx).on.task_before(ladder_ctx);

        for (uint32_t n = 0; n < (*ladder_ctx).hw.io.fn_read_qty; n++)
            (*ladder_ctx).hw.io.read[n](ladder_ctx, n);

        // ladder program scan
        ladder_scan(ladder_ctx);
        if ((*ladder_ctx).ladder.state == LADDER_ST_INV) {
            (*ladder_ctx).ladder.state = LADDER_ST_EXIT_TSK;
            goto exit;
        }

        ladder_save_previous_values(ladder_ctx);

        for (uint32_t n = 0; n < (*ladder_ctx).hw.io.fn_write_qty; n++)
            (*ladder_ctx).hw.io.write[n](ladder_ctx, n);

        ladder_scan_time(ladder_ctx);

        // external function after scan
        if ((*ladder_ctx).on.task_after != NULL)
            (*ladder_ctx).on.task_after(ladder_ctx);
    }

    exit:
    if ((*ladder_ctx).on.end_task != NULL)
        (*ladder_ctx).on.end_task(ladder_ctx);
}
