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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ladder.h"
#include "port_dummy.h"
#include "ladder_print.h"
#include "ladder_instructions.h"
#include "fn_dummy.h"
#include "test_functions.h"

// quantity of results for test
#define TEST_MAX_RESULTS 8

/**
 * @typedef network_result_t
 * @brief Expected results on test
 * 0: register id, 1: expected value
 *
 */
typedef struct {
    uint32_t  M[2][TEST_MAX_RESULTS];
    uint32_t  I[2][TEST_MAX_RESULTS];
    uint32_t  Q[2][TEST_MAX_RESULTS];
    uint32_t Cr[2][TEST_MAX_RESULTS];
    uint32_t Cd[2][TEST_MAX_RESULTS];
    uint32_t Tr[2][TEST_MAX_RESULTS];
    uint32_t Td[2][TEST_MAX_RESULTS];
    uint32_t IW[2][TEST_MAX_RESULTS];
    uint32_t QW[2][TEST_MAX_RESULTS];
    uint32_t  C[2][TEST_MAX_RESULTS];
    uint32_t  D[2][TEST_MAX_RESULTS];
} network_result_t;

uint32_t cycles_counter;
uint32_t cycles_end;

/**
 * @fn bool test_on_task_after(ladder_ctx_t*)
 * @brief Manage on_task_after function for tests
 *
 * @param ladder_ctx Context
 * @return Result
 */
bool test_on_task_after(ladder_ctx_t *ladder_ctx) {
    if(cycles_end == 0)
        return true;

    if (cycles_counter++ >= cycles_end)
        (*ladder_ctx).ladder.state = LADDER_ST_EXIT_TSK;

    return true;
}

/**
 * @fn static void init_network(ladder_network_t *network, uint32_t rows, uint32_t columns)
 * @brief Initialize network container
 *
 * @param network Network
 * @param rows Rows quantity
 * @param columns Columns quantity
 */
static void init_network(ladder_network_t *network, uint32_t rows, uint32_t columns) {
    (*network).bars = calloc(columns, sizeof(uint32_t));
    (*network).cells = malloc(rows * sizeof(ladder_cell_t*));
    for (uint32_t cl = 0; cl < rows; cl++)
        (*network).cells[cl] = calloc(columns, sizeof(ladder_cell_t));
}

/**
 * @fn static void deinit_network(ladder_network_t *network, uint32_t rows)
 * @brief Free network container
 *
 * @param network Network
 * @param rows Rows quantity
 */
static void deinit_network(ladder_network_t *network, uint32_t rows) {
    free((*network).bars);
    for (uint32_t cl = 0; cl < rows; cl++)
        free((*network).cells[cl]);
    free((*network).cells);
}

/**
 * @fn static void load_network(ladder_network_t network, uint32_t network_id, ladder_ctx_t *ladder_ctx)
 * @brief Load a network to context
 *
 * @param network Network
 * @param network_id Network number
 * @param ladder_ctx Context
 */
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

/**
 * @fn static bool test_results(ladder_ctx_t ladder_ctx, network_result_t result)
 * @brief Check expected results
 *
 * @param ladder_ctx Context
 * @param result Expected results
 * @return Result
 */
static bool test_results(ladder_ctx_t ladder_ctx, network_result_t result) {
    bool res = true;

    for (uint8_t i = 0; i < TEST_MAX_RESULTS; i++) {
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

/**
 * @fn static bool network_test(ladder_network_t network, uint32_t rows, uint32_t columns, uint32_t cycles, network_result_t result)
 * @brief Run an execution test for a network
 *
 * @param network Network to load test
 * @param rows Rows quantity
 * @param columns Columns quantity
 * @param cycles Cycles to run. 0 to wait for exit (usually Quit on dummy)
 * @param result Expected results
 * @return Test result
 */
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
    ladder_ctx.on.task_after = test_on_task_after;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// function tests

// TODO: Implement
static bool fn_nop(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_conn(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_neg(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_no(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_nc(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_re(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_fe(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_coil(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_coill(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_coilu(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_ton(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_toff(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_tp(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_ctu(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_ctd(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_move(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_sub(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_add(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_mul(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_div(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_mod(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_shl(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_shr(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_rol(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_ror(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_and(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_or(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_xor(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_not(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_eq(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_gt(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_ge(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_lt(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_le(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_ne(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

// TODO: Implement
static bool fn_foreign(void) {
    bool res;
    ladder_network_t network;
    const network_result_t result = { 0 };

    init_network(&network, 1, 1);
    res = network_test(network, 1, 1, 1, result);
    deinit_network(&network, 1);

    return res;
}

bool function_tests(void) {
    bool res = true;

    res &= fn_nop();
    res &= fn_conn();
    res &= fn_neg();
    res &= fn_no();
    res &= fn_nc();
    res &= fn_re();
    res &= fn_fe();
    res &= fn_coil();
    res &= fn_coill();
    res &= fn_coilu();
    res &= fn_ton();
    res &= fn_toff();
    res &= fn_tp();
    res &= fn_ctu();
    res &= fn_ctd();
    res &= fn_move();
    res &= fn_sub();
    res &= fn_add();
    res &= fn_mul();
    res &= fn_div();
    res &= fn_mod();
    res &= fn_shl();
    res &= fn_shr();
    res &= fn_rol();
    res &= fn_ror();
    res &= fn_and();
    res &= fn_or();
    res &= fn_xor();
    res &= fn_not();
    res &= fn_eq();
    res &= fn_gt();
    res &= fn_ge();
    res &= fn_lt();
    res &= fn_le();
    res &= fn_ne();
    res &= fn_foreign();

    return res;
}
