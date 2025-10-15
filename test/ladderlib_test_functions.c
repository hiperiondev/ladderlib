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

uint8_t test_fn_ADD(void) {
    return 0;
}

uint8_t test_fn_AND(void) {
    return 0;
}

uint8_t test_fn_COIL(void) {
    return 0;
}

uint8_t test_fn_COILL(void) {
    return 0;
}

uint8_t test_fn_COILU(void) {
    return 0;
}

uint8_t test_fn_CONN(void) {
    return 0;
}

uint8_t test_fn_CTD(void) {
    return 0;
}

uint8_t test_fn_CTU(void) {
    return 0;
}

uint8_t test_fn_DIV(void) {
    return 0;
}

uint8_t test_fn_EQ(void) {
    return 0;
}

uint8_t test_fn_FE(void) {
    return 0;
}

uint8_t test_fn_FOREIGN(void) {
    return 0;
}

uint8_t test_fn_GE(void) {
    return 0;
}

uint8_t test_fn_GT(void) {
    return 0;
}

uint8_t test_fn_LE(void) {
    return 0;
}

uint8_t test_fn_LT(void) {
    return 0;
}

uint8_t test_fn_MOD(void) {
    return 0;
}

uint8_t test_fn_MOVE(void) {
    return 0;
}

uint8_t test_fn_MUL(void) {
    return 0;
}

uint8_t test_fn_NC(void) {
    return 0;
}

uint8_t test_fn_NE(void) {
    return 0;
}

uint8_t test_fn_NEG(void) {
    return 0;
}

uint8_t test_fn_NO(void) {
    return 0;
}

uint8_t test_fn_NOP(void) {
    return 0;
}

uint8_t test_fn_NOT(void) {
    return 0;
}

uint8_t test_fn_OR(void) {
    return 0;
}

uint8_t test_fn_RE(void) {
    return 0;
}

uint8_t test_fn_ROL(void) {
    return 0;
}

uint8_t test_fn_ROR(void) {
    return 0;
}

uint8_t test_fn_SHL(void) {
    return 0;
}

uint8_t test_fn_SHR(void) {
    return 0;
}

uint8_t test_fn_SUB(void) {
    return 0;
}

uint8_t test_fn_TMOVE(void) {
    return 0;
}

uint8_t test_fn_TOF(void) {
    return 0;
}

uint8_t test_fn_TON(void) {
    return 0;
}

uint8_t test_fn_TP(void) {
    return 0;
}

uint8_t test_fn_XOR(void) {
    return 0;
}

bool test_ladder_instructions(void) {
    uint8_t err = 0;

    err += test_fn_ADD();
    err += test_fn_AND();
    err += test_fn_COIL();
    err += test_fn_COILL();
    err += test_fn_COILU();
    err += test_fn_CONN();
    err += test_fn_CTD();
    err += test_fn_CTU();
    err += test_fn_DIV();
    err += test_fn_EQ();
    err += test_fn_FE();
    err += test_fn_FOREIGN();
    err += test_fn_GE();
    err += test_fn_GT();
    err += test_fn_LE();
    err += test_fn_LT();
    err += test_fn_MOD();
    err += test_fn_MOVE();
    err += test_fn_MUL();
    err += test_fn_NC();
    err += test_fn_NE();
    err += test_fn_NEG();
    err += test_fn_NO();
    err += test_fn_NOP();
    err += test_fn_NOT();
    err += test_fn_OR();
    err += test_fn_RE();
    err += test_fn_ROL();
    err += test_fn_ROR();
    err += test_fn_SHL();
    err += test_fn_SHR();
    err += test_fn_SUB();
    err += test_fn_TMOVE();
    err += test_fn_TOF();
    err += test_fn_TON();
    err += test_fn_TP();
    err += test_fn_XOR();

    if (err != 0) {
        printf("ERROR: %d tests fails\n\n", err);
        return false;
    }

    return true;
}
