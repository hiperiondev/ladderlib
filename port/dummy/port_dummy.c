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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#include "ladder.h"
#include "port_dummy.h"

#define DUMMY_QTY_I  8
#define DUMMY_QTY_Q  8
#define DUMMY_QTY_IW 8
#define DUMMY_QTY_QW 8

static const char *_ladder_status_str[] = { //
        "STOPPED",  //
        "RUNNING",  //
        "ERROR",    //
        "EXIT_TSK", //
        "NULLFN",   //
        "INVALID",  //
        };

static const char *_fn_str[] = { //
        "NOP",     //
        "CONN",    //
        "NEG",     //
        "NO",      //
        "NC",      //
        "RE",      //
        "FE",      //
        "COIL",    //
        "COILL",   //
        "COILU",   //
        "TON",     //
        "TOF",     //
        "TP",      //
        "CTU",     //
        "CTD",     //
        "MOVE",    //
        "SUB",     //
        "ADD",     //
        "MUL",     //
        "DIV",     //
        "MOD",     //
        "SHL",     //
        "SHR",     //
        "ROL",     //
        "ROR",     //
        "AND",     //
        "OR",      //
        "XOR",     //
        "NOT",     //
        "EQ",      //
        "GT",      //
        "GE",      //
        "LT",      //
        "LE",      //
        "NE",      //
        "FOREIGN", //
        "TMOVE",   //
        };

static const char *_fn_err_str[] = { //
        "OK",         //
        "GETPREVVAL", //
        "GETDATAVAL", //
        "SETDATAVAL", //
        "NOFOREIGN",  //
        "NOTABLE",    //
        "OUTOFRANGE", //
        "DIVBYZERO",  //
        //    [...]   //
        "FAIL",       //
        };

void dummy_delay(long msec) {
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

uint64_t dummy_millis(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (uint64_t) ((time.tv_sec * 1000000L + time.tv_usec) / 1000);
}

void dummy_read(ladder_ctx_t *ladder_ctx, uint32_t id) {
    char ch = 0;
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

    if (ch == 81 || ch == 113) {
        (*ladder_ctx).ladder.state = LADDER_ST_EXIT_TSK;
    }

    if (ch > 47 && ch < 56)
        (*ladder_ctx).input[id].I[ch - 48] = (*ladder_ctx).input[id].I[ch - 48] > 0 ? 0 : 1;

    printf("[scan time: %lu ms]                                                               \n", (*ladder_ctx).scan_internals.actual_scan_time);
    printf("Toggle I: 0-7  (q: exit) (fn: %d)                                                                                                     \n", id);
    printf("-----------------------                                                           \n");

    printf("I0.0|I0.1|I0.2|I0.3|I0.4|I0.5|I0.6|I0.7                              \n");
    printf("%04d|%04d|%04d|%04d|%04d|%04d|%04d|%04d                              \n", (*ladder_ctx).input[id].I[0], (*ladder_ctx).input[id].I[1],
            (*ladder_ctx).input[id].I[2], (*ladder_ctx).input[id].I[3], (*ladder_ctx).input[id].I[4], (*ladder_ctx).input[id].I[5],
            (*ladder_ctx).input[id].I[6], (*ladder_ctx).input[id].I[7]);
    printf("                                                     \n");
}

void dummy_write(ladder_ctx_t *ladder_ctx, uint32_t id) {
    uint32_t rets = 0;

    printf("M0|M1|M2|M3|M4|M5|M6|M7                                \n");
    printf("%02d|%02d|%02d|%02d|%02d|%02d|%02d|%02d                              \n", (*ladder_ctx).memory.M[0], (*ladder_ctx).memory.M[1],
            (*ladder_ctx).memory.M[2], (*ladder_ctx).memory.M[3], (*ladder_ctx).memory.M[4], (*ladder_ctx).memory.M[5], (*ladder_ctx).memory.M[6],
            (*ladder_ctx).memory.M[7]);

    printf("                                                     \n");

#ifdef OPTIONAL_CRON
    printf("M10|M11|M12|M13|M14|M15|M16|M17                                \n");
    printf("%02d |%02d |%02d |%02d |%02d |%02d |%02d |%02d                              \n", (*ladder_ctx).memory.M[10], (*ladder_ctx).memory.M[11],
            (*ladder_ctx).memory.M[12], (*ladder_ctx).memory.M[13], (*ladder_ctx).memory.M[14], (*ladder_ctx).memory.M[15], (*ladder_ctx).memory.M[16],
            (*ladder_ctx).memory.M[17]);

    printf("                                                     \n");
#endif

    printf("Q0.0|Q0.1|Q0.2|Q0.3|Q0.4|Q0.5|Q0.6|Q0.7                                \n");
    printf("%04d|%04d|%04d|%04d|%04d|%04d|%04d|%04d                                \n", (*ladder_ctx).output[id].Q[0], (*ladder_ctx).output[id].Q[1],
            (*ladder_ctx).output[id].Q[2], (*ladder_ctx).output[id].Q[3], (*ladder_ctx).output[id].Q[4], (*ladder_ctx).output[id].Q[5],
            (*ladder_ctx).output[id].Q[6], (*ladder_ctx).output[id].Q[7]);

    printf("-----------------------                                                     \n");
    printf("        +----+----+-------+\n");
    printf("        | Td | Tr |  acc  |\n");
    printf("        +----+----+-------+\n");
    printf("Timer 0 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[0], (*ladder_ctx).memory.Tr[0], (*ladder_ctx).timers[0].acc);
    printf("Timer 1 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[1], (*ladder_ctx).memory.Tr[1], (*ladder_ctx).timers[1].acc);
    printf("Timer 2 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[2], (*ladder_ctx).memory.Tr[2], (*ladder_ctx).timers[2].acc);
    printf("Timer 3 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[3], (*ladder_ctx).memory.Tr[3], (*ladder_ctx).timers[3].acc);
    printf("Timer 4 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[4], (*ladder_ctx).memory.Tr[4], (*ladder_ctx).timers[4].acc);
    printf("Timer 5 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[5], (*ladder_ctx).memory.Tr[5], (*ladder_ctx).timers[5].acc);
    printf("Timer 6 | %d  | %d  | %05u |\n", (*ladder_ctx).memory.Td[6], (*ladder_ctx).memory.Tr[6], (*ladder_ctx).timers[6].acc);
    printf("        +----+----+-------+\n");

    printf("          +----+----+-------+\n");
    printf("          | Cd | Cr |   C   |\n");
    printf("          +----+----+-------+\n");
    printf("Counter 0 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[0], (*ladder_ctx).memory.Cr[0], (*ladder_ctx).registers.C[0]);
    printf("Counter 1 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[1], (*ladder_ctx).memory.Cr[1], (*ladder_ctx).registers.C[1]);
    printf("Counter 2 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[2], (*ladder_ctx).memory.Cr[2], (*ladder_ctx).registers.C[2]);
    printf("Counter 3 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[3], (*ladder_ctx).memory.Cr[3], (*ladder_ctx).registers.C[3]);
    printf("Counter 4 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[4], (*ladder_ctx).memory.Cr[4], (*ladder_ctx).registers.C[4]);
    printf("Counter 5 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[5], (*ladder_ctx).memory.Cr[5], (*ladder_ctx).registers.C[5]);
    printf("Counter 6 | %d  | %d  | %05d |\n", (*ladder_ctx).memory.Cd[6], (*ladder_ctx).memory.Cr[6], (*ladder_ctx).registers.C[6]);
    printf("          +----+----+-------+\n");

    ////////////////////////////

    for (uint32_t nt = 0; nt < (*ladder_ctx).ladder.quantity.networks; nt++) {
        printf("network %d:\n", nt);
        ++rets;
        for (uint32_t r = 0; r < (*ladder_ctx).network[nt].rows - 1; r++) {
            printf("|");
            for (uint32_t c = 0; c < (*ladder_ctx).network[nt].cols; c++) {
                printf("%s|", (*ladder_ctx).network[nt].cells[r][c].state == true ? "#" : "-");
            }
            printf("\n");
            ++rets;
        }
        printf("\n");
        ++rets;
    }

    // data
    printf("\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A"
            "\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A");
#ifdef OPTIONAL_CRON
    printf("\033[A\033[A\033[A");
#endif

    //networks
    for (uint32_t n = 0; n < rets; n++)
        printf("\033[A");

    (*ladder_ctx).hw.time.delay(5);
}

bool dummy_on_scan_end(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool dummy_on_instruction(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool dummy_on_task_before(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool dummy_on_task_after(ladder_ctx_t *ladder_ctx) {
    return false;
}

void dummy_on_panic(ladder_ctx_t *ladder_ctx) {
    printf("--------- dummy PANIC ---------\n");
    printf("        STATE: %s (%d)                     \n", _ladder_status_str[(*ladder_ctx).ladder.state], (*ladder_ctx).ladder.state);
    printf("  INSTRUCTION: %s (%d)                     \n", _fn_str[(*ladder_ctx).ladder.last.instr], (*ladder_ctx).ladder.last.instr);
    printf("      NETWORK: %d                          \n", (*ladder_ctx).ladder.last.network);
    printf("         CELL: %d, %d                      \n", (*ladder_ctx).ladder.last.cell_row, (*ladder_ctx).ladder.last.cell_column);
    printf("        ERROR: %s (%d)                     \n", _fn_err_str[(*ladder_ctx).ladder.last.err], (*ladder_ctx).ladder.last.err);
    printf("-------------------------------\n\n");
}

void dummy_on_end_task(ladder_ctx_t *ladder_ctx) {
}

bool dummy_init_read(ladder_ctx_t *ladder_ctx, uint32_t id, bool init) {
    if (init) {
        (*ladder_ctx).input[id].I = calloc(DUMMY_QTY_I, sizeof(uint8_t));
        (*ladder_ctx).input[id].IW = calloc(DUMMY_QTY_IW, sizeof(int32_t));
        (*ladder_ctx).input[id].Ih = calloc(DUMMY_QTY_I, sizeof(uint8_t));
        (*ladder_ctx).input[id].i_qty = DUMMY_QTY_I;
        (*ladder_ctx).input[id].iw_qty = DUMMY_QTY_IW;

        for (uint32_t n = 0; n < DUMMY_QTY_I; n++) {
            (*ladder_ctx).input[id].I[n] = 0;
            (*ladder_ctx).input[id].Ih[n] = 0;
        }
        for (uint32_t n = 0; n < DUMMY_QTY_IW; n++) {
            (*ladder_ctx).input[id].IW[n] = 0;
        }
    } else {
        free((*ladder_ctx).input[id].I);
        free((*ladder_ctx).input[id].IW);
        free((*ladder_ctx).input[id].Ih);
        (*ladder_ctx).input[id].i_qty = 0;
        (*ladder_ctx).input[id].iw_qty = 0;
    }

    return true;
}

bool dummy_init_write(ladder_ctx_t *ladder_ctx, uint32_t id, bool init) {
    if (init) {
        (*ladder_ctx).output[id].Q = calloc(DUMMY_QTY_Q, sizeof(uint8_t));
        (*ladder_ctx).output[id].QW = calloc(DUMMY_QTY_QW, sizeof(int32_t));
        (*ladder_ctx).output[id].Qh = calloc(DUMMY_QTY_Q, sizeof(uint8_t));
        (*ladder_ctx).output[id].q_qty = DUMMY_QTY_Q;
        (*ladder_ctx).output[id].qw_qty = DUMMY_QTY_QW;
    } else {
        free((*ladder_ctx).output[id].Q);
        free((*ladder_ctx).output[id].QW);
        free((*ladder_ctx).output[id].Qh);
        (*ladder_ctx).output[id].q_qty = 0;
        (*ladder_ctx).output[id].qw_qty = 0;
    }

    return true;
}
