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

/**
 * @def TOTAL_NETWORKS
 * @brief Total Networks.
 *
 */
#define TOTAL_NETWORKS 400

/**
 * @def QTY_M
 * @brief Memory Areas quantities. Marks. Regular flags
 *
 */
#define QTY_M 2000

/**
 * @def QTY_I
 * @brief Memory Areas quantities. Digital Inputs
 *
 */
#define QTY_I 200

/**
 * @def QTY_Q
 * @brief Memory Areas quantities. Digital Outputs
 *
 */
#define QTY_Q 200

/**
 * @def QTY_IW
 * @brief Memory Areas quantities. Analog Inputs
 *
 */
#define QTY_IW 50

/**
 * @def QTY_QW
 * @brief Memory Areas quantities. Analog Outputs
 *
 */
#define QTY_QW 50

/**
 * @def QTY_C
 * @brief Memory Areas quantities. Counter registers (16 bits)
 *
 */
#define QTY_C 200

/**
 * @def QTY_T
 * @brief Memory Areas quantities. Timers
 *
 */
#define QTY_T 300

/**
 * @def QTY_D
 * @brief Memory Areas quantities. Regular registers (16 bit signed)
 *
 */
#define QTY_D 10000

/**
 * @def QTY_R
 * @brief Memory Areas quantities. Float or Real registers
 *
 */
#define QTY_R 2000

static void load_demo_1(ladder_ctx_t *ladder_ctx) {
    // NETWORK 0
    (*ladder_ctx).network[0].cells[0][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[0][0].data = 500;
    (*ladder_ctx).network[0].cells[0][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[0][1].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[0][1].data = 0;
    (*ladder_ctx).network[0].cells[0][1].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[0].cells[1][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[1][0].data = 501;
    (*ladder_ctx).network[0].cells[1][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[1][1].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[1][1].data = 1;
    (*ladder_ctx).network[0].cells[1][1].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[0].cells[2][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[0].cells[2][0].data = 502;
    (*ladder_ctx).network[0].cells[2][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[0].cells[2][1].code = LADDER_INS_COIL;
    (*ladder_ctx).network[0].cells[2][1].data = 2;
    (*ladder_ctx).network[0].cells[2][1].type = LADDER_TYPE_Q;

    // NETWORK 1
    (*ladder_ctx).network[1].cells[0][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[0][0].data = 503;
    (*ladder_ctx).network[1].cells[0][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[0][1].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[0][1].data = 3;
    (*ladder_ctx).network[1].cells[0][1].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[1].cells[1][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[1][0].data = 504;
    (*ladder_ctx).network[1].cells[1][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[1][1].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[1][1].data = 4;
    (*ladder_ctx).network[1].cells[1][1].type = LADDER_TYPE_Q;

    (*ladder_ctx).network[1].cells[2][0].code = LADDER_INS_NO;
    (*ladder_ctx).network[1].cells[2][0].data = 505;
    (*ladder_ctx).network[1].cells[2][0].type = LADDER_TYPE_M;

    (*ladder_ctx).network[1].cells[2][1].code = LADDER_INS_COIL;
    (*ladder_ctx).network[1].cells[2][1].data = 5;
    (*ladder_ctx).network[1].cells[2][1].type = LADDER_TYPE_Q;

    // NETWORK 2
    (*ladder_ctx).network[2].cells[0][0].code = LADDER_INS_MOVE;
    (*ladder_ctx).network[2].cells[0][0].data = 500;
    (*ladder_ctx).network[2].cells[0][0].type = LADDER_TYPE_D;

    (*ladder_ctx).network[2].cells[1][0].code = LADDER_INS_MOVE | LADDER_INS_CELL_USED_MASK;
    (*ladder_ctx).network[2].cells[1][0].data = 4;
    (*ladder_ctx).network[2].cells[1][0].type = LADDER_TYPE_QW;

    (*ladder_ctx).network[2].cells[2][0].code = LADDER_INS_MOVE;
    (*ladder_ctx).network[2].cells[2][0].data = 501;
    (*ladder_ctx).network[2].cells[2][0].type = LADDER_TYPE_D;

    (*ladder_ctx).network[2].cells[3][0].code = LADDER_INS_MOVE | LADDER_INS_CELL_USED_MASK;
    (*ladder_ctx).network[2].cells[3][0].data = 5;
    (*ladder_ctx).network[2].cells[3][0].type = LADDER_TYPE_QW;

    (*ladder_ctx).ladder.total_networks = 3;
}

int main(void) {
    ladder_ctx_t ladder_ctx;

    if (!ladder_ctx_init(&ladder_ctx, TOTAL_NETWORKS, QTY_M, QTY_I, QTY_Q, QTY_IW, QTY_QW, QTY_C, QTY_T, QTY_D, QTY_R)) {
        printf("ERROR Initializing\n");
        return 1;
    }

    ladder_clear_program(&ladder_ctx);

    ///////////////////////////////////////////////////////

    ladder_ctx.io.read_inputs_local = dummy_read_inputs_local;
    ladder_ctx.io.write_outputs_local = dummy_write_outputs_local;
    ladder_ctx.io.read_inputs_remote = dummy_read_inputs_remote;
    ladder_ctx.io.write_outputs_remote = dummy_write_outputs_remote;
    ladder_ctx.io.external_on_scan = dummy_external_on_scan;
    ladder_ctx.io.external_on_task = dummy_external_on_task;
    ladder_ctx.io.panic = dummy_panic;
    ladder_ctx.io.micros = dummy_micros;
    ladder_ctx.io.delay = dummy_delay;

    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    printf("Load demo program\n\n");
    load_demo_1(&ladder_ctx);

    ladder_print(&ladder_ctx);

    printf("Start Task Ladder\n");
    ladder_task((void*) &ladder_ctx);

    ///////////////////////////////////////////////////////

    if (!ladder_ctx_deinit(&ladder_ctx)) {
        printf("ERROR Deinitializing\n");
        return 1;
    }

    return 0;
}
