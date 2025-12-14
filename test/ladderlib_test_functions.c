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

#include "ladder_instructions.h"
#include "ladder.h"
#include "ladder_internals.h"

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

static int32_t tests_qty = 0, tests_failed = 0;
ladder_ctx_t ladder_ctx;

/////////////////////////////////////////////////////////////////

void test_delay(long msec) {
}

uint64_t test_millis(void) {
    return 0;
}

void test_read(ladder_ctx_t *ladder_ctx, uint32_t id) {
}

void test_write(ladder_ctx_t *ladder_ctx, uint32_t id) {
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
    return false;
}

void test_on_panic(ladder_ctx_t *ladder_ctx) {
}

void test_on_end_task(ladder_ctx_t *ladder_ctx) {
}

bool test_init_read(ladder_ctx_t *ladder_ctx, uint32_t id, bool init) {
    return true;
}

bool test_init(void) {
    if (!ladder_ctx_init(&ladder_ctx, 15, 15, 1, TEST_QTY_M, TEST_QTY_C, TEST_QTY_T, TEST_QTY_D, TEST_QTY_R, 10, 0, true, true, 1000000UL, 100)) {
        printf("   ERROR Initializing\n");
        return true;
    }

    return false;
}

bool test_deinit(void) {
    ladder_ctx_deinit(&ladder_ctx);
    return false;
}

/////////////////////////////////////////////////////////////////

void test_fn_ADD(void) {
    printf("-- TEST: ADD\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_AND(void) {
    printf("-- TEST: AND\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_COIL(void) {
    printf("-- TEST: COIL\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_COILL(void) {
    printf("-- TEST: COILL\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_COILU(void) {
    printf("-- TEST: COILU\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_CONN(void) {
    printf("-- TEST: CONN\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_CTD(void) {
    printf("-- TEST: CTD\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_CTU(void) {
    printf("-- TEST: CTU\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_DIV(void) {
    printf("-- TEST: DIV\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_EQ(void) {
    printf("-- TEST: EQ\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_FE(void) {
    printf("-- TEST: FE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_FOREIGN(void) {
    printf("-- TEST: FOREIGN\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_GE(void) {
    printf("-- TEST: GE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_GT(void) {
    printf("-- TEST: GT\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_LE(void) {
    printf("-- TEST: LE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_LT(void) {
    printf("-- TEST: LT\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_MOD(void) {
    printf("-- TEST: MOD\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_MOVE(void) {
    printf("-- TEST: MOVE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_MUL(void) {
    printf("-- TEST: MUL\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_NC(void) {
    printf("-- TEST: NC\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_NE(void) {
    printf("-- TEST: NE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_NEG(void) {
    printf("-- TEST: NEG\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_NO(void) {
    printf("-- TEST: NO\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_NOP(void) {
    printf("-- TEST: NOP\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_NOT(void) {
    printf("-- TEST: NOT\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_OR(void) {
    printf("-- TEST: OR\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_RE(void) {
    printf("-- TEST: RE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_ROL(void) {
    printf("-- TEST: ROL\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_ROR(void) {
    printf("-- TEST: ROR\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_SHL(void) {
    printf("-- TEST: SHL\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_SHR(void) {
    printf("-- TEST: SHR\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_SUB(void) {
    printf("-- TEST: SUB\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_TMOVE(void) {
    printf("-- TEST: TMOVE\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_TOF(void) {
    printf("-- TEST: TOF\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_TON(void) {
    printf("-- TEST: TON\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_TP(void) {
    printf("-- TEST: TP\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

    test_deinit();
}

void test_fn_XOR(void) {
    printf("-- TEST: XOR\n");
    tests_qty++;
    if (test_init()) {
        tests_failed++;
        return;
    }

// start test

// end test

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
    test_fn_TMOVE();
    test_fn_TOF();
    test_fn_TON();
    test_fn_TP();
    test_fn_XOR();

    printf("\n- [END TESTS] -\n\n");

    if (tests_failed != 0) {
        printf("\033[91m[ERROR] %d tests failed out of %d\033[0m\n\n", tests_failed, tests_qty);
        return false;
    }

    printf("\033[92m[SUCCESS] All %d tests passed\033[0m\n\n", tests_qty);
    return true;
}
