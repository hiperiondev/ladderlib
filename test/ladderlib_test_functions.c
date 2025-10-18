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

uint8_t assert_count = 0;
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

    // initialize context
    if (!ladder_ctx_init(&ladder_ctx, 6, 7, 3, TEST_QTY_M, TEST_QTY_C, TEST_QTY_T, TEST_QTY_D, TEST_QTY_R, 10, 0, true, true, 1000000UL, 100)) {
        printf("ERROR Initializing\n");
        return 1;
    }

    return false;
}

bool test_deinit(void) {
    return false;
}

/////////////////////////////////////////////////////////////////

void test_fn_ADD(void) {

}

void test_fn_AND(void) {

}

void test_fn_COIL(void) {

}

void test_fn_COILL(void) {

}

void test_fn_COILU(void) {

}

void test_fn_CONN(void) {

}

void test_fn_CTD(void) {

}

void test_fn_CTU(void) {

}

void test_fn_DIV(void) {

}

void test_fn_EQ(void) {

}

void test_fn_FE(void) {

}

void test_fn_FOREIGN(void) {

}

void test_fn_GE(void) {

}

void test_fn_GT(void) {

}

void test_fn_LE(void) {

}

void test_fn_LT(void) {

}

void test_fn_MOD(void) {

}

void test_fn_MOVE(void) {

}

void test_fn_MUL(void) {

}

void test_fn_NC(void) {

}

void test_fn_NE(void) {

}

void test_fn_NEG(void) {

}

void test_fn_NO(void) {

}

void test_fn_NOP(void) {

}

void test_fn_NOT(void) {

}

void test_fn_OR(void) {

}

void test_fn_RE(void) {

}

void test_fn_ROL(void) {

}

void test_fn_ROR(void) {

}

void test_fn_SHL(void) {

}

void test_fn_SHR(void) {

}

void test_fn_SUB(void) {

}

void test_fn_TMOVE(void) {

}

void test_fn_TOF(void) {

}

void test_fn_TON(void) {

}

void test_fn_TP(void) {

}

void test_fn_XOR(void) {

}

/////////////////////////////////////////////////////////////////

bool test_ladder_instructions(void) {
    // clear screen
    printf("\e[1;1H\e[2J");

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

    if (assert_count != 0) {
        printf("ERROR: %d tests fails\n\n", assert_count);
        return false;
    }

    return true;
}
