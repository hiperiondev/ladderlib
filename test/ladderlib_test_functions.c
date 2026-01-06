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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

#include "ladder_instructions.h"
#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_print.h"

#define TEST_QTY_M  18
#define TEST_QTY_C  8
#define TEST_QTY_T  8
#define TEST_QTY_D  8
#define TEST_QTY_R  8

#define CHECK(condition, desc, print_pass) \
    do { \
        tests_qty++; \
        if (condition) { \
            if (print_pass) { \
                printf("\033[92m[PASS] %s\033[0m\n", desc); \
            } \
        } else { \
            printf("\033[91m[FAIL] %s\033[0m\n", desc); \
            tests_failed++; \
        } \
    } while (0)

#define CHECK_EQ(val, exp, desc, print_pass) \
    do { \
        tests_qty++; \
        if ((val) == (exp)) { \
            if (print_pass) { \
                printf("\033[92m[PASS] %s\033[0m\n", desc); \
            } \
        } else { \
            printf("\033[91m[FAIL] %s: got=%" PRIi32 " expected=%" PRIi32 "\033[0m\n", desc, (int32_t)(val), (int32_t)(exp)); \
            tests_failed++; \
        } \
    } while (0)

#define CHECK_CLOSE(val, exp, tol, desc, print_pass) \
    do { \
        tests_qty++; \
        int32_t diff = abs((val) - (exp)); \
        if (diff <= (tol)) { \
            if (print_pass) { \
                printf("\033[92m[PASS] %s\033[0m\n", desc); \
            } \
        } else { \
            printf("\033[91m[FAIL] %s: got=%" PRIi32 " expected=%" PRIi32 " tol=%" PRIi32 " diff=%" PRIi32 "\033[0m\n", desc, (int32_t)(val), (int32_t)(exp), \
                   (int32_t)(tol), (int32_t)diff); \
            tests_failed++; \
        } \
    } while (0)

#define CHECK_CLOSE_U32(val, exp, tol, desc, print_pass) \
    do { \
        tests_qty++; \
        uint32_t v = (val); \
        uint32_t e = (exp); \
        uint32_t diff = (v > e) ? (v - e) : (e - v); \
        if (diff <= (tol)) { \
            if (print_pass) \
                printf("\033[92m[PASS] %s\033[0m\n", desc); \
        } else { \
            printf("\033[91m[FAIL] %s: got=%" PRIu32 " expected=%" PRIu32 " diff=%" PRIu32 "\033[0m\n", desc, v, e, diff); \
            tests_failed++; \
        } \
    } while (0)

#define CHECK_LADDER_FN_CELL(result, instr)                                     \
    if (!(result)) {                                                            \
        printf("\033[91m[FAIL] ladder_fn_cell failed for " #instr ": %s \033[0m\n", functions_err_str[ladder_ctx.ladder.last.err]); \
        tests_failed++;                                                         \
        test_deinit();                                                          \
        return;                                                                 \
    }

static int32_t tests_qty = 0, tests_failed = 0;
ladder_ctx_t ladder_ctx;

#define EP(x) [x] = #x

const char *functions_err_str[] = { //
        EP(LADDER_INS_ERR_OK), //
                EP(LADDER_INS_ERR_GETPREVVAL), //
                EP(LADDER_INS_ERR_GETDATAVAL), //
                EP(LADDER_INS_ERR_SETDATAVAL), //
                EP(LADDER_INS_ERR_NOFOREIGN), //
                EP(LADDER_INS_ERR_NOTABLE), //
                EP(LADDER_INS_ERR_OUTOFRANGE), //
                EP(LADDER_INS_ERR_DIVBYZERO), //
                EP(LADDER_INS_ERR_TYPEMISMATCH), //
                EP(LADDER_INS_ERR_OVERFLOW), //
                EP(LADDER_INS_ERR_NULL), //
                EP(LADDER_INS_ERR_FAIL), //
        };

#define TEST_INIT(msg)                         \
            printf("-- TEST: ");               \
            printf(msg);                       \
            printf("\n");                      \
            tests_qty++;                       \
            if (test_init()) {                 \
                tests_failed++;                \
                return;                        \
            }

#define SET_REG_D(idx, value) ladder_ctx.registers.D[idx] = value
#define SET_REG_R(idx, value) ladder_ctx.registers.R[idx] = value
#define SET_REG_M(idx, value) ladder_ctx.memory.M[idx] = value
#define CHECK_REG_D(idx, exp, desc) CHECK_EQ(ladder_ctx.registers.D[idx], exp, desc, true)
#define CHECK_REG_R(idx, exp, tol, desc) CHECK_CLOSE(ladder_ctx.registers.R[idx], exp, tol, desc, true)
#define CHECK_CELL_STATE(net, row, col, exp, desc) CHECK(ladder_ctx.network[net].cells[row][col].state == exp, desc, true)

/////////////////////////////////////////////////////////////////

static inline int32_t to_integer(int32_t val, ladder_data_type_t type) {
    (void) type;  // Unused parameter
    // Original implementation forced values to 0 or 1, breaking arithmetic/bitwise ops.
    // Modified to return the actual integer value for proper register operations.
    return val;
}

static ladder_ins_err_t dummy_foreign_exec(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // Simple foreign function that just sets the cell state to true
    if (ladder_ctx && ladder_ctx->exec_network) {
        ladder_ctx->exec_network->cells[row][column].state = true;
    }
    return LADDER_INS_ERR_OK;
}

static bool dummy_foreign_fn_init(ladder_foreign_function_t *fn, void *data, uint32_t qty) {
    (void) data;
    (void) qty;
    fn->id = 0;
    strncpy(fn->name, "DUMMY", sizeof(fn->name) - 1);
    fn->name[sizeof(fn->name) - 1] = '\0';
    fn->description.inputs = 1;
    fn->description.outputs = 1;
    fn->description.cells = 1;
    fn->description.data_qty = 0;
    fn->exec = dummy_foreign_exec;
    fn->deinit = NULL;
    fn->data = NULL;
    return true;
}

void test_read(ladder_ctx_t *ladder_ctx, uint32_t id) {
}

bool test_init_read(ladder_ctx_t *ladder_ctx, uint32_t id, bool init) {
    return true;
}

void test_write(ladder_ctx_t *ladder_ctx, uint32_t id) {
}

bool test_init_write(ladder_ctx_t *ladder_ctx, uint32_t id, bool init) {
    return true;
}

bool test_on_scan_end(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool test_on_instruction(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool test_on_task_before(ladder_ctx_t *ladder_ctx) {

    return false;
}

bool test_on_task_after(ladder_ctx_t *ladder_ctx) {
    ladder_ctx->ladder.state = LADDER_ST_EXIT_TSK;

    return false;
}

void test_on_panic(ladder_ctx_t *ladder_ctx) {
}

void test_on_end_task(ladder_ctx_t *ladder_ctx) {

}

void test_delay(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return;
}

uint64_t test_millis(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (uint64_t) ((time.tv_sec * 1000000L + time.tv_usec) / 1000);
}

bool test_init(void) {
    // Initialize with 3 networks to support TMOVE test (networks 0, 1, 2)
    if (!ladder_ctx_init(&ladder_ctx, 5, 5, 3, TEST_QTY_M, TEST_QTY_C, TEST_QTY_T, TEST_QTY_D, TEST_QTY_R, 10, 0, true, true, 1000000UL, 100)) {
        printf("ERROR Initializing\n");
        return true;
    }

    ladder_clear_program(&ladder_ctx);

    if (!ladder_add_read_fn(&ladder_ctx, test_read, test_init_read)) {
        printf("ERROR Adding io read function\n");
        return true;
    }

    if (!ladder_add_write_fn(&ladder_ctx, test_write, test_init_write)) {
        printf("ERROR Adding io write function\n");
        return true;
    }

    ladder_ctx.on.scan_end = test_on_scan_end;
    ladder_ctx.on.instruction = test_on_instruction;
    ladder_ctx.on.task_before = test_on_task_before;
    ladder_ctx.on.task_after = test_on_task_after;
    ladder_ctx.on.panic = test_on_panic;
    ladder_ctx.on.end_task = test_on_end_task;
    ladder_ctx.hw.time.millis = test_millis;
    ladder_ctx.hw.time.delay = test_delay;
    ladder_ctx.cron = NULL;
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    return false;
}

bool test_deinit(void) {
    ladder_ctx_deinit(&ladder_ctx);
    return false;
}

/////////////////////////////////////////////////////////////////

void test_fn_ADD(void) {
    TEST_INIT("ADD");

    // Initialize data values in registers
    SET_REG_D(0, 10);
    SET_REG_D(1, 20);
    SET_REG_D(2, 0);

    // Create ADD instruction cell
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_ADD, 0), ADD);

    // Configure data pointers for the instruction
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    // Enable network
    ladder_ctx.network[0].enable = true;

    // Execute the ladder task
    ladder_task((void*) &ladder_ctx);

    // Verify result
    CHECK_REG_D(2, 30, "ADD should sum D[0] and D[1] into D[2]");

    test_deinit();
}

void test_fn_AND(void) {
    TEST_INIT("AND");

    SET_REG_D(0, 1);
    SET_REG_D(1, 1);
    SET_REG_D(2, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_AND, 0), AND);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 1, "AND should bitwise AND values");

    test_deinit();
}

void test_fn_COIL(void) {
    TEST_INIT("COIL");

    // Clear all memory first
    memset(ladder_ctx.memory.M, 0, TEST_QTY_M * sizeof(uint8_t));

    // Use a NO contact in column 0 to control power flow
    SET_REG_M(1, 0); // Input variable for the contact

    // Add NO contact at (0,0)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NO, 0), NO);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 1; // M[1] is the contact input

    // Add COIL at (0,1)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_COIL, 0), COIL);
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0; // M[0] is the coil output

    ladder_ctx.network[0].enable = true;

    // Turn on contact -> coil should set M[0] to 1
    SET_REG_M(1, 1);
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.M[0] == 1, "COIL should set M[0] to 1 when powered", true);

    // Turn off contact -> coil should set M[0] to 0
    SET_REG_M(1, 0);
    // Reset state to allow another scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.M[0] == 0, "COIL should set M[0] to 0 when not powered", true);

    test_deinit();
}

void test_fn_COILL(void) {
    TEST_INIT("COILL");

    SET_REG_M(0, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_COILL, 0), COILL);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;

    ladder_ctx.network[0].enable = true;

    // First scan: power on, should latch to 1
    ladder_ctx.prev_scan_vals.Mh[0] = 0; // Previous state
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.M[0] == 1, "COILL should latch to 1 when powered", true);

    // Second scan: power off, should remain 1
    ladder_ctx.prev_scan_vals.Mh[0] = 1; // Previous state
    ladder_ctx.network[0].cells[0][0].state = false;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.M[0] == 1, "COILL should remain 1 after power off", true);

    test_deinit();
}

void test_fn_COILU(void) {
    TEST_INIT("COILU");
    // Use a NO contact in column 0 to control power flow
    SET_REG_M(1, 0); // Input variable for the contact
    // Add NO contact at (0,0)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NO, 0), NO);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 1; // M[1] is the contact input

    // Add COILU at (0,1)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_COILU, 0), COILU);
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0; // M[0] is the coil output

    ladder_ctx.network[0].enable = true;

    // Initialize M[0] to 1 (latched)
    SET_REG_M(0, 1);

    // First scan: contact open, coil should remain latched
    SET_REG_M(1, 0);
    ladder_ctx.prev_scan_vals.Mh[0] = 1; // previous state of M[0] (latched)
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.M[0] == 1, "COILU should remain 1 when not powered", true);

    // Second scan: contact closed, coil should unlatch
    SET_REG_M(1, 1);
    ladder_ctx.prev_scan_vals.Mh[0] = 1; // previous state still latched
    // Reset state to allow another scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.M[0] == 0, "COILU should unlatch to 0 when powered", true);

    test_deinit();
}

void test_fn_CONN(void) {
    TEST_INIT("CONN");

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_CONN, 0), CONN);

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "CONN should pass power through", true);

    test_deinit();
}

void test_fn_CTD(void) {
    TEST_INIT("CTD");
    // Reset counter state
    ladder_ctx.registers.C[0] = 5;
    ladder_ctx.memory.Cd[0] = false;
    ladder_ctx.memory.Cr[0] = false;

    // Use a NO contact in column 0 to drive the count input
    SET_REG_M(2, 0); // Contact input variable
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NO, 0), NO);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 2; // M[2] is the contact input

    // Create CTD at column 1 (spans rows 0‑1)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_CTD, 0), CTD);
    // Configure CTD parameters
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_C;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0; // Counter register
    ladder_ctx.network[0].cells[0][1].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][1].data[1].value.i32 = 0; // Preset from D[0]
    SET_REG_D(0, 5); // Preset value

    // Ensure reset input (row 1, column 1) is off
    ladder_ctx.network[0].cells[1][1].state = false;

    ladder_ctx.network[0].enable = true;

    // Generate three count pulses (contact closed → open)
    for (int i = 0; i < 3; i++) {
        // Reset state for this scan iteration
        ladder_ctx.ladder.state = LADDER_ST_RUNNING;
        SET_REG_M(2, 1); // contact closed
        ladder_task((void*) &ladder_ctx);
        // Reset state for this scan iteration
        ladder_ctx.ladder.state = LADDER_ST_RUNNING;
        SET_REG_M(2, 0); // contact open
        ladder_task((void*) &ladder_ctx);
    }

    // start modified part
    CHECK_EQ(ladder_ctx.registers.C[0], 2, "CTD should count down to 2", true);
    // end modified part

    test_deinit();
}

void test_fn_CTU(void) {
    TEST_INIT("CTU");

    ladder_ctx.registers.C[0] = 0;
    ladder_ctx.memory.Cd[0] = false;
    ladder_ctx.memory.Cr[0] = false;

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_CTU, 0), CTU);
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_C;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][1].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][1].data[1].value.i32 = 0;

    SET_REG_D(0, 5); // Preset

    ladder_ctx.network[0].enable = true;

    // Count up 5 times to reach preset
    for (int i = 0; i < 5; i++) {
        // Reset state for this scan iteration
        ladder_ctx.ladder.state = LADDER_ST_RUNNING;
        ladder_ctx.network[0].cells[0][1].state = true;
        ladder_task((void*) &ladder_ctx);
        // Reset state for this scan iteration
        ladder_ctx.ladder.state = LADDER_ST_RUNNING;
        ladder_ctx.network[0].cells[0][1].state = false;
        ladder_task((void*) &ladder_ctx);
    }

    CHECK(ladder_ctx.memory.Cd[0] == true, "CTU should set done flag at preset", true);

    test_deinit();
}

void test_fn_DIV(void) {
    TEST_INIT("DIV");

    SET_REG_D(0, 20);
    SET_REG_D(1, 4);
    SET_REG_D(2, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_DIV, 0), DIV);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 5, "DIV should divide values");

    // Test division by zero
    SET_REG_D(1, 0);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK_REG_D(2, 0, "DIV should return 0 on division by zero");

    test_deinit();
}

void test_fn_EQ(void) {
    TEST_INIT("EQ");

    SET_REG_D(0, 5);
    SET_REG_D(1, 5);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_EQ, 0), EQ);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "EQ should be true for equal values", true);

    SET_REG_D(1, 6);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "EQ should be false for unequal values", true);

    test_deinit();
}

void test_fn_FE(void) {
    TEST_INIT("FE");

    SET_REG_M(0, 0);
    ladder_ctx.prev_scan_vals.Mh[0] = 1; // Previous was true

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_FE, 0), FE);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true; // Power flow

    // Falling edge: previous=1, current=0 -> true
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "FE should detect falling edge", true);

    // No edge: previous=0, current=0 -> false
    ladder_ctx.prev_scan_vals.Mh[0] = 0;
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "FE should not trigger without edge", true);

    test_deinit();
}

void test_fn_FOREIGN(void) {
    TEST_INIT("FOREIGN");
    // Add a dummy foreign function before creating the instruction
    if (!ladder_add_foreign(&ladder_ctx, dummy_foreign_fn_init, NULL, 1)) {
        printf("\033[91m[FAIL] ladder_add_foreign failed\033[0m\n");
        tests_failed++;
        test_deinit();
        return;
    }
    // Create a FOREIGN instruction cell
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_FOREIGN, 0), FOREIGN);
    ladder_ctx.network[0].enable = true;
    // Execute the program; the dummy foreign function sets the cell state to true
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "FOREIGN should execute", true);
    test_deinit();
}

void test_fn_GE(void) {
    TEST_INIT("GE");

    SET_REG_D(0, 5);
    SET_REG_D(1, 3);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_GE, 0), GE);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "GE should be true for greater values", true);

    SET_REG_D(0, 2);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "GE should be false for lesser values", true);

    test_deinit();
}

void test_fn_GT(void) {
    TEST_INIT("GT");

    SET_REG_D(0, 5);
    SET_REG_D(1, 3);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_GT, 0), GT);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "GT should be true for greater values", true);

    SET_REG_D(0, 3);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "GT should be false for equal values", true);

    test_deinit();
}

void test_fn_LE(void) {
    TEST_INIT("LE");

    SET_REG_D(0, 3);
    SET_REG_D(1, 5);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_LE, 0), LE);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "LE should be true for lesser values", true);

    SET_REG_D(0, 6);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "LE should be false for greater values", true);

    test_deinit();
}

void test_fn_LT(void) {
    TEST_INIT("LT");

    SET_REG_D(0, 3);
    SET_REG_D(1, 5);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_LT, 0), LT);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "LT should be true for lesser values", true);

    SET_REG_D(0, 5);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "LT should be false for equal values", true);

    test_deinit();
}

void test_fn_MOD(void) {
    TEST_INIT("MOD");

    SET_REG_D(0, 10);
    SET_REG_D(1, 3);
    SET_REG_D(2, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_MOD, 0), MOD);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;

    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 1, "MOD should return remainder");

    test_deinit();
}

void test_fn_MOVE(void) {
    TEST_INIT("MOVE");

    SET_REG_D(0, 42);
    SET_REG_D(1, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_MOVE, 0), MOVE);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(1, 42, "MOVE should copy value from D[0] to D[1]");

    test_deinit();
}

void test_fn_MUL(void) {
    TEST_INIT("MUL");

    SET_REG_D(0, 5);
    SET_REG_D(1, 6);
    SET_REG_D(2, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_MUL, 0), MUL);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 30, "MUL should multiply values");

    test_deinit();
}

void test_fn_NC(void) {
    TEST_INIT("NC");

    SET_REG_M(0, 0); // Input false

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NC, 0), NC);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "NC should pass power when input is false", true);

    SET_REG_M(0, 1); // Input true
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "NC should block power when input is true", true);

    test_deinit();
}

void test_fn_NE(void) {
    TEST_INIT("NE");

    SET_REG_D(0, 5);
    SET_REG_D(1, 3);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NE, 0), NE);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "NE should be true for unequal values", true);

    SET_REG_D(1, 5);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "NE should be false for equal values", true);

    test_deinit();
}

void test_fn_NEG(void) {
    TEST_INIT("NEG");
    // Use a NO contact in column 0 to control left‑side power
    SET_REG_M(1, 0); // Input variable for the contact
    // Add NO contact at (0,0)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NO, 0), NO);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 1; // M[1] drives the contact

    // Add NEG at (0,1)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_NEG, 0), NEG);
    ladder_ctx.network[0].enable = true;

    // Test case 1: left side true (contact closed) -> NEG output false
    SET_REG_M(1, 1);
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][1].state == false, "NEG should invert true to false", true);

    // Test case 2: left side false (contact open) -> NEG output true
    SET_REG_M(1, 0);
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][1].state == true, "NEG should invert false to true", true);

    test_deinit();
}

void test_fn_NO(void) {
    TEST_INIT("NO");

    SET_REG_M(0, 1); // Input true

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NO, 0), NO);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "NO should pass power when input is true", true);

    SET_REG_M(0, 0); // Input false
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "NO should block power when input is false", true);

    test_deinit();
}

void test_fn_NOP(void) {
    TEST_INIT("NOP");

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NOP, 0), NOP);

    ladder_ctx.network[0].enable = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.ladder.last.err == LADDER_INS_ERR_OK, "NOP should execute without error", true);

    test_deinit();
}

void test_fn_NOT(void) {
    TEST_INIT("NOT");

    SET_REG_D(0, 0x0F);
    SET_REG_D(1, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NOT, 0), NOT);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(1, ~0x0F, "NOT should bitwise invert value");

    test_deinit();
}

void test_fn_OR(void) {
    TEST_INIT("OR");

    SET_REG_D(0, 0x0F);
    SET_REG_D(1, 0x30);
    SET_REG_D(2, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_OR, 0), OR);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 0x3F, "OR should bitwise OR values");

    test_deinit();
}

void test_fn_RE(void) {
    TEST_INIT("RE");

    SET_REG_M(0, 1); // Current true
    ladder_ctx.prev_scan_vals.Mh[0] = 0; // Previous false

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_RE, 0), RE);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "RE should detect rising edge", true);

    // No edge: previous=1, current=1 -> false
    ladder_ctx.prev_scan_vals.Mh[0] = 1;
    // Reset state for second scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.network[0].cells[0][0].state == false, "RE should not trigger without edge", true);

    test_deinit();
}

void test_fn_ROL(void) {
    TEST_INIT("ROL");

    SET_REG_D(0, 0x80000001);
    SET_REG_D(1, 1);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_ROL, 0), ROL);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(0, 0x00000003, "ROL should rotate left");

    test_deinit();
}

// File: ladderlib_test_functions.c

void test_fn_ROR(void) {
    TEST_INIT("ROR");

    SET_REG_D(0, 0x80000001);
    SET_REG_D(1, 1);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_ROR, 0), ROR);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(0, -1073741824, "ROR should rotate right");

    test_deinit();
}

void test_fn_SHL(void) {
    TEST_INIT("SHL");

    SET_REG_D(0, 0x0000000F);
    SET_REG_D(1, 2);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_SHL, 0), SHL);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(0, 0x0000003C, "SHL should shift left");

    test_deinit();
}

void test_fn_SHR(void) {
    TEST_INIT("SHR");

    SET_REG_D(0, 0x0000000F);
    SET_REG_D(1, 2);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_SHR, 0), SHR);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(0, 0x00000003, "SHR should shift right");

    test_deinit();
}

void test_fn_SUB(void) {
    TEST_INIT("SUB");

    SET_REG_D(0, 10);
    SET_REG_D(1, 3);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_SUB, 0), SUB);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 7, "SUB should subtract values");
    CHECK(ladder_ctx.network[0].cells[0][0].state == true, "SUB top output should be true for positive result", true);

    test_deinit();
}

void test_fn_TMOVE(void) {
    TEST_INIT("TMOVE");

    // Set up source table (network 1, disabled)
    ladder_ctx.network[1].enable = false;
    ladder_ctx.network[1].cells[0][0].data = calloc(1, sizeof(ladder_value_t));
    if (ladder_ctx.network[1].cells[0][0].data == NULL) {
        printf("\033[91m[FAIL] calloc failed in test_fn_TMOVE\033[0m\n");
        tests_failed++;
        test_deinit();
        return;
    }
    ladder_ctx.network[1].cells[0][0].data_qty = 1;
    ladder_ctx.network[1].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[1].cells[0][0].data[0].value.i32 = 42;

    // Set up destination table (network 2, disabled)
    ladder_ctx.network[2].enable = false;
    ladder_ctx.network[2].cells[0][0].data = calloc(1, sizeof(ladder_value_t));
    if (ladder_ctx.network[2].cells[0][0].data == NULL) {
        printf("\033[91m[FAIL] calloc failed for dest in test_fn_TMOVE\033[0m\n");
        free(ladder_ctx.network[1].cells[0][0].data);
        ladder_ctx.network[1].cells[0][0].data = NULL;
        ladder_ctx.network[1].cells[0][0].data_qty = 0;
        tests_failed++;
        test_deinit();
        return;
    }
    ladder_ctx.network[2].cells[0][0].data_qty = 1;
    ladder_ctx.network[2].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[2].cells[0][0].data[0].value.i32 = 0;

    // Create TMOVE instruction
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_TMOVE, 0), TMOVE);

    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_NONE;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 2;  // dest_net = 2
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_NONE;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 0;  // dest_pos = 0
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_NONE;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 1;  // src_net = 1
    ladder_ctx.network[0].cells[0][0].data[3].type = LADDER_REGISTER_NONE;
    ladder_ctx.network[0].cells[0][0].data[3].value.i32 = 0;  // src_pos = 0
    ladder_ctx.network[0].cells[0][0].data[4].type = LADDER_REGISTER_NONE;
    ladder_ctx.network[0].cells[0][0].data[4].value.i32 = 1;  // qty = 1

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    // Reset state for this scan
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;
    ladder_task((void*) &ladder_ctx);

    CHECK(ladder_ctx.network[2].cells[0][0].data[0].value.i32 == 42, "TMOVE should copy table data", true);

    free(ladder_ctx.network[1].cells[0][0].data);
    ladder_ctx.network[1].cells[0][0].data = NULL;
    ladder_ctx.network[1].cells[0][0].data_qty = 0;

    free(ladder_ctx.network[2].cells[0][0].data);
    ladder_ctx.network[2].cells[0][0].data = NULL;
    ladder_ctx.network[2].cells[0][0].data_qty = 0;

    test_deinit();
}

void test_fn_TOF(void) {
    TEST_INIT("TOF");

    // Set up timer
    ladder_ctx.timers[0].acc = 0;
    ladder_ctx.memory.Tr[0] = false;
    ladder_ctx.memory.Td[0] = false;

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_TOF, 0), TOF);
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_T;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][1].data[1].type = LADDER_BASETIME_MS;
    ladder_ctx.network[0].cells[0][1].data[1].value.i32 = 10; // 10ms preset

    ladder_ctx.network[0].enable = true;

    // Timer starts timing on falling edge
    ladder_ctx.network[0].cells[0][1].state = true; // Input on
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.Td[0] == false, "TOF should not be done immediately", true);

    ladder_ctx.network[0].cells[0][1].state = false; // Input off (falling edge)
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.Tr[0] == true, "TOF should start timing", true);

    test_deinit();
}

void test_fn_TON(void) {
    TEST_INIT("TON");

    ladder_ctx.timers[0].acc = 0;
    ladder_ctx.memory.Tr[0] = false;
    ladder_ctx.memory.Td[0] = false;

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_TON, 0), TON);
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_T;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][1].data[1].type = LADDER_BASETIME_MS;
    ladder_ctx.network[0].cells[0][1].data[1].value.i32 = 10;

    ladder_ctx.network[0].enable = true;

    // Timer requires sustained input
    ladder_ctx.network[0].cells[0][1].state = true;
    ladder_task((void*) &ladder_ctx);
    CHECK(ladder_ctx.memory.Td[0] == false, "TON should not be done immediately", true);

    test_deinit();
}

void test_fn_TP(void) {
    TEST_INIT("TP");
    // Initialize timer
    ladder_ctx.timers[0].acc = 0;
    ladder_ctx.memory.Tr[0] = false;
    ladder_ctx.memory.Td[0] = false;

    // Use a NO contact in column 0 to provide a rising edge
    SET_REG_M(2, 0); // Contact input variable
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_NO, 0), NO);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_M;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 2; // M[2] drives the contact

    // Create TP at column 1 (spans rows 0‑1)
    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 1, LADDER_INS_TP, 0), TP);
    ladder_ctx.network[0].cells[0][1].data[0].type = LADDER_REGISTER_T;
    ladder_ctx.network[0].cells[0][1].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][1].data[1].type = LADDER_BASETIME_MS;
    ladder_ctx.network[0].cells[0][1].data[1].value.i32 = 10; // 10 ms preset

    ladder_ctx.network[0].enable = true;

    // Rising edge: contact closes
    SET_REG_M(2, 1);
    ladder_task((void*) &ladder_ctx);
    // Timer should be running after rising edge
    CHECK(ladder_ctx.memory.Tr[0] == true, "TP should start pulsing on rising edge", true);

    test_deinit();
}

void test_fn_XOR(void) {
    TEST_INIT("XOR");

    SET_REG_D(0, 0x0F);
    SET_REG_D(1, 0x33);
    SET_REG_D(2, 0);

    CHECK_LADDER_FN_CELL(ladder_fn_cell(&ladder_ctx, 0, 0, 0, LADDER_INS_XOR, 0), XOR);
    ladder_ctx.network[0].cells[0][0].data[0].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[0].value.i32 = 0;
    ladder_ctx.network[0].cells[0][0].data[1].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[1].value.i32 = 1;
    ladder_ctx.network[0].cells[0][0].data[2].type = LADDER_REGISTER_D;
    ladder_ctx.network[0].cells[0][0].data[2].value.i32 = 2;

    ladder_ctx.network[0].enable = true;
    ladder_ctx.network[0].cells[0][0].state = true;
    ladder_task((void*) &ladder_ctx);

    CHECK_REG_D(2, 0x3C, "XOR should bitwise XOR values");

    test_deinit();
}

/////////////////////////////////////////////////////////////////

bool test_ladder_instructions(void) {
    printf("\e[1;1H\e[2J- [START TESTS] -\n\n");

    test_fn_ADD();
    test_fn_AND();
    test_fn_COIL();
    test_fn_COILL();
    test_fn_COILU();
    test_fn_CONN();
    test_fn_CTD();
    test_fn_CTU();
    test_fn_DIV();
    test_fn_EQ();
    test_fn_FE();
    test_fn_FOREIGN();
    test_fn_GE();
    test_fn_GT();
    test_fn_LE();
    test_fn_LT();
    test_fn_MOD();
    test_fn_MOVE();
    test_fn_MUL();
    test_fn_NC();
    test_fn_NE();
    test_fn_NEG();
    test_fn_NO();
    test_fn_NOP();
    test_fn_NOT();
    test_fn_OR();
    test_fn_RE();
    test_fn_ROL();
    test_fn_ROR();
    test_fn_SHL();
    test_fn_SHR();
    test_fn_SUB();
    test_fn_TOF();
    test_fn_TON();
    test_fn_TP();
    test_fn_XOR();
    test_fn_TMOVE();

    printf("\n- [END TESTS] -\n\n");

    if (tests_failed != 0) {
        printf("\033[91m[ERROR] %d tests failed out of %d\033[0m\n\n", tests_failed, tests_qty);
        return false;
    }

    printf("\033[92m[SUCCESS] All %d tests passed\033[0m\n\n", tests_qty);
    return true;
}
