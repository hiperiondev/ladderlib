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

#include <stdio.h>
#include <stdlib.h>

#include "ladder.h"
#include "port_dummy.h"
#include "ladder_print.h"
#include "ladder_instructions.h"
#include "fn_dummy.h"
#include "test_functions.h"

static void load_demo(ladder_ctx_t *ladder_ctx) {
    printf("blink (2 network, 7 rows, 6 columns)");

    // Network 0
    (*ladder_ctx).network[0].enable = true;

    (*ladder_ctx).network[0].cells[0][0].code = LADDER_INS_NC;
    (*ladder_ctx).network[0].cells[0][0].data.i32 = 3;
    (*ladder_ctx).network[0].cells[0][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[0][1].code = LADDER_INS_NC;
    (*ladder_ctx).network[0].cells[0][1].data.i32 = 2;
    (*ladder_ctx).network[0].cells[0][1].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[0][2].code = LADDER_INS_TON;
    (*ladder_ctx).network[0].cells[0][2].data.i32 = 0;
    (*ladder_ctx).network[0].cells[0][2].type = LADDER_TYPE_T;
    (*ladder_ctx).network[0].cells[1][2].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[0].cells[1][2].data.i32 = 5;
    (*ladder_ctx).network[0].cells[1][2].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[0].cells[0][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[0][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[0][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[0][5].data.i32 = 1;
    (*ladder_ctx).network[0].cells[0][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[0].cells[2][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[2][0].data.i32 = 1;
    (*ladder_ctx).network[0].cells[2][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[2][1].code = LADDER_INS_TON;
    (*ladder_ctx).network[0].cells[2][1].data.i32 = 1;
    (*ladder_ctx).network[0].cells[2][1].type = LADDER_TYPE_T;
    (*ladder_ctx).network[0].cells[3][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[0].cells[3][1].data.i32 = 5;
    (*ladder_ctx).network[0].cells[3][1].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[0].cells[2][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[2][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[2][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[2][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[2][5].data.i32 = 2;
    (*ladder_ctx).network[0].cells[2][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[0].cells[4][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[4][0].data.i32 = 1;
    (*ladder_ctx).network[0].cells[4][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[4][1].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[4][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[4][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[4][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[4][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[4][5].data.i32 = 0;
    (*ladder_ctx).network[0].cells[4][5].type = LADDER_TYPE_Q;

    // ----------------------------- //

    (*ladder_ctx).network[0].cells[5][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[5][0].data.i32 = 0;
    (*ladder_ctx).network[0].cells[5][0].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[0].cells[5][1].code = LADDER_INS_CTU;
    (*ladder_ctx).network[0].cells[5][1].data.i32 = 0;
    (*ladder_ctx).network[0].cells[5][1].type = LADDER_TYPE_C;
    (*ladder_ctx).network[0].cells[6][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[0].cells[6][1].data.i32 = 5;
    (*ladder_ctx).network[0].cells[6][1].type = LADDER_TYPE_NONE;

    (*ladder_ctx).network[0].cells[5][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[5][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[5][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[5][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[5][5].data.i32 = 3;
    (*ladder_ctx).network[0].cells[5][5].type = LADDER_TYPE_M;

    //------------------------------------------------------------//

    // Network 1
    (*ladder_ctx).network[1].enable = true;

    (*ladder_ctx).network[1].cells[0][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[0][0].data.i32 = 3;
    (*ladder_ctx).network[1].cells[0][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][1].code = LADDER_INS_NC;
    (*ladder_ctx).network[1].cells[0][1].data.i32 = 4;
    (*ladder_ctx).network[1].cells[0][1].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][2].code = LADDER_INS_NC;
    (*ladder_ctx).network[1].cells[0][2].data.i32 = 5;
    (*ladder_ctx).network[1].cells[0][2].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][3].code = LADDER_INS_TON;
    (*ladder_ctx).network[1].cells[0][3].data.i32 = 2;
    (*ladder_ctx).network[1].cells[0][3].type = LADDER_TYPE_T;
    (*ladder_ctx).network[1].cells[1][3].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[1].cells[1][3].data.i32 = 5;
    (*ladder_ctx).network[1].cells[1][3].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[1].cells[0][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[0][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[0][5].data.i32 = 6;
    (*ladder_ctx).network[1].cells[0][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[1].cells[2][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[2][0].data.i32 = 6;
    (*ladder_ctx).network[1].cells[2][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[2][1].code = LADDER_INS_TON;
    (*ladder_ctx).network[1].cells[2][1].data.i32 = 3;
    (*ladder_ctx).network[1].cells[2][1].type = LADDER_TYPE_T;
    (*ladder_ctx).network[1].cells[3][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[1].cells[3][1].data.i32 = 5;
    (*ladder_ctx).network[1].cells[3][1].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[1].cells[2][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[2][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[2][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[2][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[2][5].data.i32 = 5;
    (*ladder_ctx).network[1].cells[2][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[1].cells[4][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[4][0].data.i32 = 6;
    (*ladder_ctx).network[1].cells[4][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[4][1].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[4][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[4][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[4][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[4][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[4][5].data.i32 = 1;
    (*ladder_ctx).network[1].cells[4][5].type = LADDER_TYPE_Q;

    // ----------------------------- //

    (*ladder_ctx).network[1].cells[5][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[5][0].data.i32 = 1;
    (*ladder_ctx).network[1].cells[5][0].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[1].cells[5][1].code = LADDER_INS_CTU;
    (*ladder_ctx).network[1].cells[5][1].data.i32 = 1;
    (*ladder_ctx).network[1].cells[5][1].type = LADDER_TYPE_C;
    (*ladder_ctx).network[1].cells[6][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[1].cells[6][1].data.i32 = 5;
    (*ladder_ctx).network[1].cells[6][1].type = LADDER_TYPE_NONE;

    (*ladder_ctx).network[1].cells[5][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[5][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[5][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[5][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[5][5].data.i32 = 4;
    (*ladder_ctx).network[1].cells[5][5].type = LADDER_TYPE_M;

    //------------------------------------------------------------//

    // Network 2
    (*ladder_ctx).network[2].enable = true;

    (*ladder_ctx).network[2].cells[0][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[2].cells[0][0].data.i32 = 7;
    (*ladder_ctx).network[2].cells[0][0].type = LADDER_TYPE_I;

    (*ladder_ctx).network[2].cells[0][1].code = LADDER_INS_CONN;
    (*ladder_ctx).network[2].cells[0][2].code = LADDER_INS_CONN;

    (*ladder_ctx).network[2].cells[0][3].code = LADDER_INS_FOREIGN;
    (*ladder_ctx).network[2].cells[0][3].data.i32 = 0;
    (*ladder_ctx).network[2].cells[0][3].type = LADDER_TYPE_NONE;
    (*ladder_ctx).network[2].cells[1][3].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[2].cells[1][3].data.cstr = "0 0 0 31 6 *";
    (*ladder_ctx).network[2].cells[1][3].type = LADDER_TYPE_CSTR;
    (*ladder_ctx).network[2].cells[2][3].type = LADDER_INS_MULTI;
    (*ladder_ctx).network[2].cells[2][3].data.vector.a = 45;
    (*ladder_ctx).network[2].cells[2][3].data.vector.b = 73;
    (*ladder_ctx).network[2].cells[2][3].type = LADDER_TYPE_VECT;

    (*ladder_ctx).network[2].cells[0][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[2].cells[0][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[2].cells[0][5].data.i32 = 7;
    (*ladder_ctx).network[2].cells[0][5].type = LADDER_TYPE_Q;

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
    if(!ladder_add_foreign(&ladder_ctx, dummy_init, NULL, 5)) {
        printf("ERROR Load foreign\n");
        exit(1);
    }

    printf("Load demo program: ");
    load_demo(&ladder_ctx);
    printf("\n");

    ladder_print(ladder_ctx);

    printf("Start Task Ladder\n\n");
    ladder_task((void*) &ladder_ctx);

    ///////////////////////////////////////////////////////

    // release context
    if (!ladder_ctx_deinit(&ladder_ctx)) {
        printf("ERROR Deinitializing\n");
        return 1;
    }

    return 0;
}
