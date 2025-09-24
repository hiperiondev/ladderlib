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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "ladder.h"
#include "ladder_internals.h"

// Mock hardware time functions
static uint64_t mock_millis(void) {
    static uint64_t time = 0;
    return time++;
}
static void mock_delay(long ms) { /* No-op */
}

// Mock I/O read/write (single module with 1 bit each for simplicity)
static void mock_read(ladder_ctx_t *ctx, uint32_t fn_id) {
    // Simulate input: Set I[0] to 1 for tests needing input high
    ctx->input[0].I[0] = 1;
}
static void mock_write(ladder_ctx_t *ctx, uint32_t fn_id) {
    // No-op, outputs checked in memory
}
static bool mock_io_init(ladder_ctx_t *ctx, uint32_t fn_id, bool init) {
    return true;
}

// Helper to reset context for each test
static void reset_context(ladder_ctx_t *ctx) {
    ladder_clear_memory(ctx);
    ladder_clear_program(ctx);
    memset(&ctx->scan_internals, 0, sizeof(ctx->scan_internals));
    ctx->ladder.state = LADDER_ST_RUNNING;
}

// Assertion macro (use assert or printf for portability)
#define TEST_ASSERT(cond, msg) if (!(cond)) { printf("FAIL: %s\n", msg); passed = false;}

// Helper to set left power controller at col=0
static void set_left_power(ladder_ctx_t *ctx, uint32_t net, uint32_t row, bool power_on) {
    ladder_fn_cell(ctx, net, row, 0, power_on ? LADDER_INS_CONN : LADDER_INS_NOP, 0);
}

bool test_ladder_instructions(void) {
    printf("------- START INSTRUCTIONS TEST -------\n\n");
    ladder_ctx_t ctx;
    bool passed = true;
    if (!ladder_ctx_init(&ctx, 5, 5, 1, 10, 10, 10, 10, 10, 100, true)) {
        printf("Init failed\n");
        return false;
    }
    ctx.hw.time.millis = mock_millis;
    ctx.hw.time.delay = mock_delay;
    ladder_add_read_fn(&ctx, mock_read, mock_io_init);
    ladder_add_write_fn(&ctx, mock_write, mock_io_init);

    // Network 0 for all tests
    uint32_t net = 0, row = 0, col = 1; // Test instructions at col=1
    ctx.network[net].enable = true;

    // NOP: No operation, state does not propagate
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true); // Explicit left true
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_NOP, 0);
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == false, "NOP should not set state");

    // CONN: Connection, passes state
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_CONN, 0);
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "CONN should pass state");

    // NEG: Negate, inverts left state
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_NEG, 0);
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == false, "NEG should invert state");

    // NO: Normally open, passes if data true and left true
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_NO, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ctx.memory.M[0] = 1; // Data true
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "NO should pass if data and left true");

    // NC: Normally closed, passes if data false and left true
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_NC, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ctx.memory.M[0] = 0; // Data false
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "NC should pass if data false and left true");

    // RE: Rising edge, detects true transition
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_RE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ctx.memory.M[0] = 1; // Current true
    ctx.prev_scan_vals.Mh[0] = 0; // Prev false
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "RE should detect rising edge");

    // FE: Falling edge, detects false transition
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_FE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ctx.memory.M[0] = 0; // Current false
    ctx.prev_scan_vals.Mh[0] = 1; // Prev true
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "FE should detect falling edge");

    // COIL: Set coil
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_COIL, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.memory.M[0] == 1, "COIL should set memory if left true");

    // COILL: Latch coil
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_COILL, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.memory.M[0] == 1, "COILL should latch memory");

    // COILU: Unlatch coil
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_COILU, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_M;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    ctx.memory.M[0] = 1; // Pre-set to test unlatch
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.memory.M[0] == 0, "COILU should unlatch memory if left true");

    // TON: Timer on delay
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_TON, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_T;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    // Fixed: Moved PT to top cell data[1]; set PT=55 (value=11, base_index=0) for 11-unit timeout
    ctx.network[net].cells[row][col].data[1].value.i32 = 55; // PT = 11*5 + 0
    for (int i = 0; i < 11; i++) {
        ladder_scan(&ctx);
    }
    TEST_ASSERT(ctx.memory.Td[0] == true, "TON should time out");

    // TOF: Timer off delay
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_TOF, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_T;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    // Fixed: Moved PT to top cell data[1]; PT=55; toggle power for off-delay
    ctx.network[net].cells[row][col].data[1].value.i32 = 55; // PT = 11*5 + 0
    ladder_scan(&ctx); // Activate
    set_left_power(&ctx, net, row, false); // Deactivate
    for (int i = 0; i < 11; i++) {
        ladder_scan(&ctx);
    }
    TEST_ASSERT(ctx.memory.Td[0] == false, "TOF should delay off");

    // TP: Timer pulse
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_TP, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_T;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    // Fixed: Moved PT to top cell data[1]; PT=55
    ctx.network[net].cells[row][col].data[1].value.i32 = 55; // PT = 11*5 + 0
    ladder_scan(&ctx); // Trigger
    TEST_ASSERT(ctx.memory.Tr[0] == true, "TP should pulse on");
    for (int i = 0; i < 11; i++) {
        ladder_scan(&ctx);
    }
    TEST_ASSERT(ctx.memory.Tr[0] == false, "TP should end pulse");

    // CTU: Counter up
    reset_context(&ctx);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_CTU, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_C;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    // Fixed: Moved PV to top cell data[1]; toggle power for rising edges
    ctx.network[net].cells[row][col].data[1].value.i32 = 5; // PV = 5
    for (int i = 0; i < 10; i++) {
        set_left_power(&ctx, net, row, (i % 2 == 0)); // Toggle for edges
        ladder_scan(&ctx);
    }
    TEST_ASSERT(ctx.registers.C[0] == 5, "CTU should count up");
    TEST_ASSERT(ctx.memory.Cd[0] == true, "CTU done after PV");

    // CTD: Counter down
    reset_context(&ctx);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_CTD, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_C;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0;
    // Fixed: Moved PV to top cell data[1]; toggle power for edges
    ctx.network[net].cells[row][col].data[1].value.i32 = 5; // PV = 5
    ctx.registers.C[0] = 5; // Pre-set CV
    for (int i = 0; i < 10; i++) {
        set_left_power(&ctx, net, row, (i % 2 == 0)); // Toggle for edges
        ladder_scan(&ctx);
    }
    TEST_ASSERT(ctx.registers.C[0] == 0, "CTD should count down");
    TEST_ASSERT(ctx.memory.Cd[0] == true, "CTD done at 0");

    // MOVE: Move value
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_MOVE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 42;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 42, "MOVE should copy value");

    // SUB: Subtract
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_SUB, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 10;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 3;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 7, "SUB should subtract");

    // ADD: Add
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_ADD, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 5;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 3;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 8, "ADD should add");

    // MUL: Multiply
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_MUL, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 4;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 3;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 12, "MUL should multiply");

    // DIV: Divide
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_DIV, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 10;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 2;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 5, "DIV should divide");

    // MOD: Modulo
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_MOD, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 10;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 3;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 1, "MOD should give remainder");

    // SHL: Shift left
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_SHL, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 1;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0;
    ctx.registers.D[0] = 2; // Pre-set shift amount
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 4, "SHL should shift left");

    // SHR: Shift right
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_SHR, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 8;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0;
    ctx.registers.D[0] = 2; // Pre-set shift amount
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 2, "SHR should shift right");

    // ROL: Rotate left
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_ROL, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 1;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0;
    ctx.registers.D[0] = 1; // Pre-set rotate amount
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 2, "ROL should rotate left");

    // ROR: Rotate right
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_ROR, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 2;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0;
    ctx.registers.D[0] = 1; // Pre-set rotate amount
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 1, "ROR should rotate right");

    // AND: Bitwise AND
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_AND, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 1;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 3;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 1, "AND should bitwise AND");

    // OR: Bitwise OR
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_OR, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 1;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 2;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 3, "OR should bitwise OR");

    // XOR: Bitwise XOR
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_XOR, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 3;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 1;
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 2, "XOR should bitwise XOR");

    // NOT: Bitwise NOT
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_NOT, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0xFFFFFFFF; // Fixed: Input to test inversion
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 0, "NOT should invert bits");

    // EQ: Equal comparison
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_EQ, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 5;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 5;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "EQ should be true if equal");

    // GT: Greater than
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_GT, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 6;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 5;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "GT should be true if greater");

    // GE: Greater or equal
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_GE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 5;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 5;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "GE should be true if >=");

    // LT: Less than
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_LT, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 4;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 5;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "LT should be true if less");

    // LE: Less or equal
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_LE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 5;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 5;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "LE should be true if <=");

    // NE: Not equal
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_NE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 4;
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 5;
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.network[net].cells[row][col].state == true, "NE should be true if not equal");

    // FOREIGN: Invalid foreign
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_FOREIGN, 0);
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.ladder.last.err == LADDER_INS_ERR_NOFOREIGN, "FOREIGN should error on invalid ID");

    // TMOVE: Table move
    reset_context(&ctx);
    set_left_power(&ctx, net, row, true);
    ladder_fn_cell(&ctx, net, row, col, LADDER_INS_TMOVE, 0);
    ctx.network[net].cells[row][col].data[0].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[0].value.i32 = 0; // Table 0
    ctx.network[net].cells[row][col].data[1].type = LADDER_REGISTER_NONE;
    ctx.network[net].cells[row][col].data[1].value.i32 = 0; // Pos 0
    ctx.network[net].cells[row][col].data[2].type = LADDER_REGISTER_D;
    ctx.network[net].cells[row][col].data[2].value.i32 = 0;
    ctx.network[net].cells[0][0].data[0].value.i32 = 42; // Table value
    ctx.network[net].enable = false; // Disable for TMOVE
    ladder_scan(&ctx);
    TEST_ASSERT(ctx.registers.D[0] == 42, "TMOVE should move from table");

    ladder_ctx_deinit(&ctx);
    if (passed)
        printf("All tests passed\n\n");
    printf("------- END INSTRUCTIONS TEST -------\n\n");
    return passed;
}
