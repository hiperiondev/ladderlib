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

#define NET_COLUMNS    6
#define NET_ROWS       8
#define TOTAL_NETWORKS 2

#define QTY_M  100
#define QTY_I  200
#define QTY_Q  200
#define QTY_IW 50
#define QTY_QW 50
#define QTY_C  200
#define QTY_T  300
#define QTY_D  10000
#define QTY_R  2000

// 0: id, 1:value
typedef struct {
    uint32_t M[2][10];
    uint32_t I[2][10];
    uint32_t Q[2][10];
    uint32_t Cr[2][10];
    uint32_t Cd[2][10];
    uint32_t Tr[2][10];
    uint32_t Td[2][10];
    uint32_t IW[2][10];
    uint32_t QW[2][10];
    uint32_t C[2][10];
    uint32_t D[2][10];
} network_result_t;

uint32_t cycles_counter;
uint32_t cycles_end;;

bool test_on_task_after(ladder_ctx_t *ladder_ctx) {
    if (cycles_counter++ >= cycles_end)
        (*ladder_ctx).ladder.state = LADDER_ST_EXIT_TSK;

    return true;
}

static void load_network(ladder_network_t network, uint32_t network_id, ladder_ctx_t *ladder_ctx) {
    for (uint32_t column = 0; column < (*ladder_ctx).ladder.quantity.net_columns; column++) {
        for (uint32_t row = 0; row < (*ladder_ctx).ladder.quantity.net_rows; row++) {
            (*ladder_ctx).network[network_id].cells[row][column].code = network.cells[row][column].code;
            (*ladder_ctx).network[network_id].cells[row][column].data = network.cells[row][column].data;
            (*ladder_ctx).network[network_id].cells[row][column].type = network.cells[row][column].type;
        }
    }

    for (uint32_t i = 0; i < (*ladder_ctx).ladder.quantity.net_rows - 1; i++) {
        (*ladder_ctx).network[network_id].bars[i] = network.bars[i];
    }
}

static bool test_results(ladder_ctx_t ladder_ctx, network_result_t result) {
    bool res = true;

    for (uint8_t i = 0; i < 9; i++) {
        res &= ladder_ctx.memory.M[result.M[0][i]] == result.M[1][i];
        res &= ladder_ctx.memory.I[result.I[0][i]] == result.I[1][i];
        res &= ladder_ctx.memory.Q[result.Q[0][i]] == result.Q[1][i];
        res &= ladder_ctx.memory.Cr[result.Cr[0][i]] == result.Cr[1][i];
        res &= ladder_ctx.memory.Cd[result.Cd[0][i]] == result.Cd[1][i];
        res &= ladder_ctx.memory.Tr[result.Tr[0][i]] == result.Tr[1][i];
        res &= ladder_ctx.memory.Td[result.Td[0][i]] == result.Td[1][i];
        res &= ladder_ctx.registers.IW[result.IW[0][i]] == result.IW[1][i];
        res &= ladder_ctx.registers.QW[result.QW[0][i]] == result.QW[1][i];
        res &= ladder_ctx.registers.C[result.C[0][i]] == result.C[1][i];
        res &= ladder_ctx.registers.D[result.D[0][i]] == result.C[1][i];
    }

    return res;
}

static bool network_test(ladder_network_t network, uint32_t rows, uint32_t columns, uint32_t cycles, network_result_t result) {
    ladder_ctx_t ladder_ctx;
    bool res;

    cycles_counter = 1;
    cycles_end = cycles;

    if (!ladder_ctx_init(&ladder_ctx, columns, rows, 1, QTY_M, QTY_I, QTY_Q, QTY_IW, QTY_QW, QTY_C, QTY_T, QTY_D, QTY_R)) {
        printf("ERROR Initializing\n");
        return 1;
    }

    ladder_ctx.hw.io.read_inputs_local = dummy_read_inputs_local;
    ladder_ctx.hw.io.write_outputs_local = dummy_write_outputs_local;
    ladder_ctx.hw.manage.on_task_after = test_on_task_after;
    ladder_ctx.hw.time.millis = dummy_millis;
    ladder_ctx.hw.time.delay = dummy_delay;

    ladder_clear_program(&ladder_ctx);
    load_network(network, 0, &ladder_ctx);

    ladder_task((void*) &ladder_ctx);

    res = test_results(ladder_ctx, result);

    if (!ladder_ctx_deinit(&ladder_ctx)) {
        printf("ERROR Deinitializing\n");
        return 1;
    }

    return res;
}

static void load_demo(ladder_ctx_t *ladder_ctx) {
    printf("DEMO: blink (2 network, 7 rows, 6 columns) \n\n");

    // Network 0
    (*ladder_ctx).network[0].cells[0][0].code = LADDER_INS_NC;
    (*ladder_ctx).network[0].cells[0][0].data = 3;
    (*ladder_ctx).network[0].cells[0][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[0][1].code = LADDER_INS_NC;
    (*ladder_ctx).network[0].cells[0][1].data = 2;
    (*ladder_ctx).network[0].cells[0][1].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[0][2].code = LADDER_INS_TON;
    (*ladder_ctx).network[0].cells[0][2].data = 0;
    (*ladder_ctx).network[0].cells[0][2].type = LADDER_TYPE_T;
    (*ladder_ctx).network[0].cells[1][2].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[0].cells[1][2].data = 5;
    (*ladder_ctx).network[0].cells[1][2].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[0].cells[0][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[0][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[0][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[0][5].data = 1;
    (*ladder_ctx).network[0].cells[0][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[0].cells[2][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[2][0].data = 1;
    (*ladder_ctx).network[0].cells[2][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[2][1].code = LADDER_INS_TON;
    (*ladder_ctx).network[0].cells[2][1].data = 1;
    (*ladder_ctx).network[0].cells[2][1].type = LADDER_TYPE_T;
    (*ladder_ctx).network[0].cells[3][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[0].cells[3][1].data = 5;
    (*ladder_ctx).network[0].cells[3][1].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[0].cells[2][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[2][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[2][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[2][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[2][5].data = 2;
    (*ladder_ctx).network[0].cells[2][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[0].cells[4][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[4][0].data = 1;
    (*ladder_ctx).network[0].cells[4][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[4][1].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[4][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[4][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[4][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[4][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[4][5].data = 0;
    (*ladder_ctx).network[0].cells[4][5].type = LADDER_TYPE_Q;

    // ----------------------------- //

    (*ladder_ctx).network[0].cells[5][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[5][0].data = 0;
    (*ladder_ctx).network[0].cells[5][0].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[0].cells[5][1].code = LADDER_INS_CTU;
    (*ladder_ctx).network[0].cells[5][1].data = 0;
    (*ladder_ctx).network[0].cells[5][1].type = LADDER_TYPE_C;
    (*ladder_ctx).network[0].cells[6][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[0].cells[6][1].data = 5;
    (*ladder_ctx).network[0].cells[6][1].type = LADDER_TYPE_NONE;

    (*ladder_ctx).network[0].cells[5][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[5][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[0].cells[5][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[0].cells[5][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[5][5].data = 3;
    (*ladder_ctx).network[0].cells[5][5].type = LADDER_TYPE_M;

    //------------------------------------------------------------//

    // Network 1
    (*ladder_ctx).network[1].cells[0][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[0][0].data = 3;
    (*ladder_ctx).network[1].cells[0][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][1].code = LADDER_INS_NC;
    (*ladder_ctx).network[1].cells[0][1].data = 4;
    (*ladder_ctx).network[1].cells[0][1].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][2].code = LADDER_INS_NC;
    (*ladder_ctx).network[1].cells[0][2].data = 5;
    (*ladder_ctx).network[1].cells[0][2].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][3].code = LADDER_INS_TON;
    (*ladder_ctx).network[1].cells[0][3].data = 2;
    (*ladder_ctx).network[1].cells[0][3].type = LADDER_TYPE_T;
    (*ladder_ctx).network[1].cells[1][3].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[1].cells[1][3].data = 5;
    (*ladder_ctx).network[1].cells[1][3].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[1].cells[0][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[0][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[0][5].data = 6;
    (*ladder_ctx).network[1].cells[0][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[1].cells[2][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[2][0].data = 6;
    (*ladder_ctx).network[1].cells[2][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[2][1].code = LADDER_INS_TON;
    (*ladder_ctx).network[1].cells[2][1].data = 3;
    (*ladder_ctx).network[1].cells[2][1].type = LADDER_TYPE_T;
    (*ladder_ctx).network[1].cells[3][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[1].cells[3][1].data = 5;
    (*ladder_ctx).network[1].cells[3][1].type = LADDER_BASETIME_SEC;

    (*ladder_ctx).network[1].cells[2][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[2][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[2][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[2][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[2][5].data = 5;
    (*ladder_ctx).network[1].cells[2][5].type = LADDER_TYPE_M;

    // ----------------------------- //

    (*ladder_ctx).network[1].cells[4][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[4][0].data = 6;
    (*ladder_ctx).network[1].cells[4][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[4][1].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[4][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[4][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[4][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[4][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[4][5].data = 1;
    (*ladder_ctx).network[1].cells[4][5].type = LADDER_TYPE_Q;

    // ----------------------------- //

    (*ladder_ctx).network[1].cells[5][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[5][0].data = 1;
    (*ladder_ctx).network[1].cells[5][0].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[1].cells[5][1].code = LADDER_INS_CTU;
    (*ladder_ctx).network[1].cells[5][1].data = 1;
    (*ladder_ctx).network[1].cells[5][1].type = LADDER_TYPE_C;
    (*ladder_ctx).network[1].cells[6][1].code = LADDER_INS_MULTI;
    (*ladder_ctx).network[1].cells[6][1].data = 5;
    (*ladder_ctx).network[1].cells[6][1].type = LADDER_TYPE_NONE;

    (*ladder_ctx).network[1].cells[5][2].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[5][3].code = LADDER_INS_CONN;
    (*ladder_ctx).network[1].cells[5][4].code = LADDER_INS_CONN;

    (*ladder_ctx).network[1].cells[5][5].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[5][5].data = 4;
    (*ladder_ctx).network[1].cells[5][5].type = LADDER_TYPE_M;
}

int main(void) {
    // main context
    ladder_ctx_t ladder_ctx;

    // clear screen
    printf("\e[1;1H\e[2J");

    // initialize context
    if (!ladder_ctx_init(&ladder_ctx, NET_COLUMNS, NET_ROWS, TOTAL_NETWORKS, QTY_M, QTY_I, QTY_Q, QTY_IW, QTY_QW, QTY_C, QTY_T, QTY_D, QTY_R)) {
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
    ladder_ctx.hw.manage.on_scan_end = dummy_on_scan_end;
    ladder_ctx.hw.manage.on_instruction = dummy_on_instruction;
    ladder_ctx.hw.manage.on_task_before = dummy_on_task_before;
    ladder_ctx.hw.manage.on_task_after = dummy_on_task_after;
    ladder_ctx.hw.manage.on_panic = dummy_on_panic;
    ladder_ctx.hw.manage.on_end_task = dummy_on_end_task;
    ladder_ctx.hw.time.millis = dummy_millis;
    ladder_ctx.hw.time.delay = dummy_delay;

    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    printf("Load demo program\n\n");
    load_demo(&ladder_ctx);

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
