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

#include <stdio.h>
#include <stdlib.h>

#include "ladder.h"
#include "port_dummy.h"
#include "ladder_print.h"
#include "ladder_instructions.h"
#include "fn_dummy.h"

// registers quantity
#define QTY_M  8
#define QTY_I  8
#define QTY_Q  8
#define QTY_IW 8
#define QTY_QW 8
#define QTY_C  8
#define QTY_T  8
#define QTY_D  8
#define QTY_R  8

static bool load_demo1(ladder_ctx_t *ladder_ctx) {
    bool ret = true;

    printf("blink (2 network, 7 rows, 6 columns)");

    // Network 0
    (*ladder_ctx).network[0].enable = true;
    //                           N  R  C  D
    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 0, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 0, 0, LADDER_TYPE_M, 3)

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 1, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 1, 0, LADDER_TYPE_M, 2);

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 2, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 2, 0, LADDER_TYPE_T, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 2, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 5, 0, LADDER_TYPE_M, 1);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 0, 2, 0, 0, LADDER_TYPE_M, 1);

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 1, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 0, 2, 1, 0, LADDER_TYPE_T, 1);
    ladder_cell_data(ladder_ctx, 0, 2, 1, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 2, 5, 0, LADDER_TYPE_M, 2);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 0, 4, 0, 0, LADDER_TYPE_M, 1);

    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 1, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 4, 5, 0, LADDER_TYPE_Q, 0);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 0, 0, LADDER_TYPE_Q, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 1, LADDER_INS_CTU, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 1, 0, LADDER_TYPE_C, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 1, 1, LADDER_TYPE_NONE, 5);

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 5, 0, LADDER_TYPE_M, 3);
    //------------------------------------------------------------//

    // Network 1
    (*ladder_ctx).network[1].enable = true;

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 0, 0, LADDER_TYPE_M, 3)

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 1, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 1, 0, LADDER_TYPE_M, 4);

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 2, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 2, 0, LADDER_TYPE_M, 5)

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 3, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 3, 0, LADDER_TYPE_T, 2);
    ladder_cell_data(ladder_ctx, 1, 0, 3, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 5, 0, LADDER_TYPE_M, 6);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 2, 0, 0, LADDER_TYPE_M, 6);

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 1, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 1, 2, 1, 0, LADDER_TYPE_T, 3);
    ladder_cell_data(ladder_ctx, 1, 2, 1, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 2, 5, 0, LADDER_TYPE_M, 5);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 4, 0, 0, LADDER_TYPE_M, 6);

    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 1, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 4, 5, 0, LADDER_TYPE_Q, 1);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 5, 0, 0, LADDER_TYPE_Q, 1);

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 1, LADDER_INS_CTU, 0);
    ladder_cell_data(ladder_ctx, 1, 5, 1, 0, LADDER_TYPE_C, 1);
    ladder_cell_data(ladder_ctx, 1, 5, 1, 1, LADDER_TYPE_NONE, 5);

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 5, 5, 0, LADDER_TYPE_M, 4);

    //------------------------------------------------------------//

    // Network 2
    (*ladder_ctx).network[2].enable = true;

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 2, 0, 0, 0, LADDER_TYPE_I, 7);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 1, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 2, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 3, LADDER_INS_CONN, 0);
    /*
     ret &=   ladder_fn_cell(ladder_ctx, 2, 0, 3,    LADDER_INS_FOREIGN, 0);
     ladder_cell_data(ladder_ctx, 2, 0, 3, 0, LADDER_TYPE_NONE, 0);
     ladder_cell_data_cstr(ladder_ctx, 2, 0, 3, 1, "0 0 0 31 6 *");
     ladder_cell_data(ladder_ctx, 2, 0, 3, 2, LADDER_TYPE_REAL, -45.3);
     */
    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 5, LADDER_INS_COIL, 7);
    ladder_cell_data(ladder_ctx, 2, 0, 5, 0, LADDER_TYPE_Q, 4);

    return ret;
}

int main(void) {
    // main context
    ladder_ctx_t ladder_ctx;

    // clear screen
    printf("\e[1;1H\e[2J");

    // initialize context
    if (!ladder_ctx_init(&ladder_ctx, 6, 7, 3, QTY_M, QTY_I, QTY_Q, QTY_IW, QTY_QW, QTY_C, QTY_T, QTY_D, QTY_R)) {
        printf("ERROR Initializing\n");
        return 1;
    }

    // clear program
    ladder_clear_program(&ladder_ctx);

    ///////////////////////////////////////////////////////

    // assign port functions
    ladder_ctx.hw.io.read_inputs_local = dummy_read_inputs_local;
    ladder_ctx.hw.io.write_outputs_local = dummy_write_outputs_local;
    ladder_ctx.hw.io.read_inputs_remote = dummy_read_inputs_remote;
    ladder_ctx.hw.io.write_outputs_remote = dummy_write_outputs_remote;
    ladder_ctx.on.scan_end = dummy_on_scan_end;
    ladder_ctx.on.instruction = dummy_on_instruction;
    ladder_ctx.on.task_before = dummy_on_task_before;
    ladder_ctx.on.task_after = dummy_on_task_after;
    ladder_ctx.on.panic = dummy_on_panic;
    ladder_ctx.on.end_task = dummy_on_end_task;
    ladder_ctx.hw.time.millis = dummy_millis;
    ladder_ctx.hw.time.delay = dummy_delay;

    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    // add foreign
    if (!ladder_add_foreign(&ladder_ctx, dummy_init, NULL, 5)) {
        printf("ERROR Load foreign\n");
        exit(1);
    }

    printf("Load demo program: ");
    if (!load_demo1(&ladder_ctx)) {
        printf("ERROR: Load demo program: ");
        goto end;
    }
    printf("\n");

    ladder_print(ladder_ctx);

    printf("Start Task Ladder\n\n");
    ladder_task((void*) &ladder_ctx);

    ///////////////////////////////////////////////////////

    end:
    // release context
    if (!ladder_ctx_deinit(&ladder_ctx)) {
        printf("ERROR Deinitializing\n");
        return 1;
    }

    return 0;
}
