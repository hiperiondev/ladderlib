/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
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

#include "ladder.h"
#include "port_dummy.h"

static const char *_ladder_state_str[] = {
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
        "OK",
        "GETPREVVAL",
        "GETDATAVAL",
        // [...] //
        "FAIL",
};

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

}

void dummy_write_outputs_local(ladder_ctx_t *ladder_ctx) {

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
    printf("------------ PANIC ------------\n");
    printf("        STATE: %s (%d)\n", _ladder_state_str[(*ladder_ctx).ladder.state], (*ladder_ctx).ladder.state);
    printf("  INSTRUCTION: %s (%d)\n", _fn_str[(*ladder_ctx).ladder.last_instr], (*ladder_ctx).ladder.last_instr);
    printf("      NETWORK: %d\n", (*ladder_ctx).ladder.last_instr_network);
    printf("         CELL: %d, %d\n", (*ladder_ctx).ladder.last_instr_cell_row, (*ladder_ctx).ladder.last_instr_cell_column);
    printf("        ERROR: %s (%d)\n", _fn_err_str[(*ladder_ctx).ladder.last_instr_err], (*ladder_ctx).ladder.last_instr_err);
    printf("-------------------------------\n\n");
}
