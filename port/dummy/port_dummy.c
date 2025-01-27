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
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "ladder.h"
#include "port_dummy.h"

static const char *_ladder_status_str[] = {
        "STOPPED",   //
        "RUNNING",   //
        "ERROR",     //
        "EXIT_TASK", //
        "INVALID",   //
        };

static const char *_fn_str[] = {
        "NOP",   //
        "CONN",  //
        "NEG",   //
        "NO",    //
        "NC",    //
        "RE",    //
        "FE",    //
        "COIL",  //
        "COILL", //
        "COILU", //
        "TON",   //
        "TOFF",  //
        "TP",    //
        "CTU",   //
        "CTD",   //
        "MOVE",  //
        "SUB",   //
        "ADD",   //
        "MUL",   //
        "DIV",   //
        "MOD",   //
        "SHL",   //
        "SHR",   //
        "ROL",   //
        "ROR",   //
        "AND",   //
        "OR",    //
        "XOR",   //
        "NOT",   //
        "EQ",    //
        "GT",    //
        "GE",    //
        "LT",    //
        "LE",    //
        "NE",    //
        };

static const char *_fn_err_str[] = {
        "OK",         //
        "GETPREVVAL", //
        "GETDATAVAL", //
        // [...] //
        "FAIL",       //
        };

char ch = 0;

int dummy_delay(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int32_t dummy_micros(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (int32_t) (ts.tv_nsec * 1000);
}

void dummy_read_inputs_local(ladder_ctx_t *ladder_ctx) {
    struct termios orig_term, raw_term;

    tcgetattr(STDIN_FILENO, &orig_term);
    raw_term = orig_term;
    raw_term.c_lflag &= ~(ECHO | ICANON);
    raw_term.c_cc[VMIN] = 0;
    raw_term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_term);

    int len = read(STDIN_FILENO, &ch, 1);
    if (len != 1)
        ch = 0;

    while (read(STDIN_FILENO, &ch, 1) == 1)
        ;
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);

    if (ch > 47 && ch < 56)
        (*ladder_ctx).memory.I[ch - 48] = (*ladder_ctx).memory.I[ch - 48] > 0 ? 0 : 1;

    printf("Toggle I: 0-7                                        \n");
    printf("-----------------------                              \n");

    printf("I0-I1-I2-I3-I4-I5-I6-I7                              \n");
    printf("%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d                              \n", (*ladder_ctx).memory.I[0], (*ladder_ctx).memory.I[1],
            (*ladder_ctx).memory.I[2], (*ladder_ctx).memory.I[3], (*ladder_ctx).memory.I[4], (*ladder_ctx).memory.I[5], (*ladder_ctx).memory.I[6],
            (*ladder_ctx).memory.I[7]);
    printf("                                                     \n");
}

void dummy_write_outputs_local(ladder_ctx_t *ladder_ctx) {
    printf("M0-M1-M2-M3-M4-M5-M6-M7                                \n");
    printf("%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d                              \n", (*ladder_ctx).memory.M[0], (*ladder_ctx).memory.M[1],
            (*ladder_ctx).memory.M[2], (*ladder_ctx).memory.M[3], (*ladder_ctx).memory.M[4], (*ladder_ctx).memory.M[5], (*ladder_ctx).memory.M[6],
            (*ladder_ctx).memory.M[7]);

    printf("                                                     \n");

    printf("Q0-Q1-Q2-Q3-Q4-Q5-Q6-Q7                                \n");
    printf("%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d                              \n", (*ladder_ctx).memory.Q[0], (*ladder_ctx).memory.Q[1], (*ladder_ctx).memory.Q[2],
            (*ladder_ctx).memory.Q[3], (*ladder_ctx).memory.Q[4], (*ladder_ctx).memory.Q[5], (*ladder_ctx).memory.Q[6], (*ladder_ctx).memory.Q[7]);

    printf("-----------------------                                                     \n");
    printf("\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A");
}

void dummy_read_inputs_remote(ladder_ctx_t *ladder_ctx) {

}

void dummy_write_outputs_remote(ladder_ctx_t *ladder_ctx) {

}

bool dummy_external_on_scan(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool dummy_external_on_task(ladder_ctx_t *ladder_ctx) {
    return false;
}

void dummy_panic(ladder_ctx_t *ladder_ctx) {
    printf("--------- dummy PANIC ---------\n");
    printf("        STATE: %s (%d)\n", _ladder_status_str[(*ladder_ctx).ladder.status], (*ladder_ctx).ladder.status);
    printf("  INSTRUCTION: %s (%d)\n", _fn_str[(*ladder_ctx).ladder.last_instr], (*ladder_ctx).ladder.last_instr);
    printf("      NETWORK: %d\n", (*ladder_ctx).ladder.last_instr_network);
    printf("         CELL: %d, %d\n", (*ladder_ctx).ladder.last_instr_cell_row, (*ladder_ctx).ladder.last_instr_cell_column);
    printf("        ERROR: %s (%d)\n", _fn_err_str[(*ladder_ctx).ladder.last_instr_err], (*ladder_ctx).ladder.last_instr_err);
    printf("-------------------------------\n\n");
}
