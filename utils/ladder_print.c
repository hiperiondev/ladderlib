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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_print.h"
#include "ladder_instructions.h"

#define PIN_OUT(lctx,n,r,c) \
                        (LADDER_VERTICAL_BAR(&lctx,n,r,c) || ((r > lctx.network[n].rows - 2) ? 0 : LADDER_VERTICAL_BAR(&lctx,n,r + 1,c)) ? "-+" : "--")

#define SPACE_BAR(lctx,n,r,c)  (((r > lctx.network[n].rows - 2) ? 0 : LADDER_VERTICAL_BAR(&lctx,n,r + 1,c)) ? "|" : " ")

static const char *fn_symbol[] = { //
        "   ", // 1
                "---", // 1
                "(!)", // 1
                "| |", // 1
                "|/|", // 1
                "-RE", // 1
                "-FE", // 1
                "( )", // 1
                "(L)", // 1
                "(U)", // 1
                "TON", // 2
                "TFF", // 2
                "-TP", // 2
                "CTU", // 2
                "CTD", // 2
                "MOV", // 2
                "SUB", // 3
                "ADD", // 3
                "MUL", // 3
                "DIV", // 3
                "MOD", // 3
                "SHL", // 2
                "SHR", // 2
                "ROL", // 2
                "ROR", // 2
                "AND", // 3
                "OR-", // 3
                "XOR", // 3
                "NOT", // 2
                "EQ-", // 2
                "GT-", // 2
                "GE-", // 2
                "LT-", // 2
                "LE-", // 2
                "NE-", // 2
                "FGN", // variable
                "TMV", // 3
                "???", // 1
        };

static const char *dt_graph[] = { //
        "--", //
                " M", //
                " Q", //
                " I", //
                "Cd", //
                "Cr", //
                "Td", //
                "Tr", //
                "IW", //
                "QW", //
                " C", //
                " T", //
                " D", //
                "ST", //
                "RE", //
                "??", //
        };

static char* ftos(float f, char *str, size_t n, int decimals) {
    if (decimals == 0)
        snprintf(str, n, "%d      ", (int) (f + (f < 0 ? -0.5 : 0.5)));
    else if (decimals < 0) {
        int i;
        for (i = 0; i < -decimals && fabs(f) >= 10; i++)
            f /= 10;
        char fmt[128];
        sprintf(fmt, "%%d%%0%dd      ", i);
        snprintf(str, n, fmt, (int) (f + (f < 0 ? -0.5 : 0.5)), 0);
    } else {
        int ifl = (int) f;
        f = fabs(f - (float) ifl);
        float g = 1;
        for (int i = decimals; i > 0; i--)
            g *= 10;
        f *= g;
        f += 0.5;
        if (f >= g) {
            f -= g;
            ifl += 1;
        }
        int E = (int) f;
        char fmt[128];
        sprintf(fmt, "%%d.%%0%dd  ", decimals);
        snprintf(str, n, fmt, ifl, E);
    }
    return (str);
}

static const char *basetime_graph[] = { "ms   ", "10ms ", "100ms", "seg  ", "min  " };

static void fn_to_str(ladder_ctx_t ladder_ctx, uint32_t net, char (*cells)[6][32], uint32_t row, uint32_t column) {
    char strtmp[9];

    memset(cells, 0, 6 * 32 * sizeof(char));

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_NOP) {
        sprintf((*cells)[0], "                  %s", (LADDER_VERTICAL_BAR(&ladder_ctx, net, row, column) ? "+" : " "));
        sprintf((*cells)[1], "                   ");
        return;
    }

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_MULTI) {
        printf("MULTI\n");
        sprintf((*cells)[0], "---+-ERR MULTI--+--");
        sprintf((*cells)[1], "---++++++++++++++--");
        return;
    }

    ladder_instructions_iocd_t actual_ioc;

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_FOREIGN) {
        memcpy(&actual_ioc, &(ladder_ctx.foreign.fn[ladder_ctx.network[net].cells[row][column].data[0].value.u32]).description,
                sizeof(ladder_instructions_iocd_t));

        sprintf((*cells)[0], "---+-%s--------+%s",
                ladder_ctx.network[net].cells[row][column].data[0].value.u32 >= ladder_ctx.foreign.qty ? "FN?" :
                strlen(ladder_ctx.foreign.fn[ladder_ctx.network[net].cells[row][column].data[0].value.i32].name) == 0 ?
                        "FN?" : ladder_ctx.foreign.fn[ladder_ctx.network[net].cells[row][column].data[0].value.u32].name,
                PIN_OUT(ladder_ctx, net, row, column));
    } else {
        memcpy(&actual_ioc, &(ladder_fn_iocd[ladder_ctx.network[net].cells[row][column].code]), sizeof(ladder_instructions_iocd_t));
        if (actual_ioc.cells != 1) {
            sprintf((*cells)[0], "---+-%s--------+%s", fn_symbol[ladder_ctx.network[net].cells[row][column].code], PIN_OUT(ladder_ctx, net, row, column));
        } else {
            sprintf((*cells)[0], "--------%s------%s", fn_symbol[ladder_ctx.network[net].cells[row][column].code], PIN_OUT(ladder_ctx, net, row, column));
        }
    }

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_FOREIGN
            && ladder_ctx.network[net].cells[row][column].data[0].value.u32 >= ladder_ctx.foreign.qty)
        return;

    switch (actual_ioc.cells) {
        case 1:
            memset(strtmp, 0, 9 * sizeof(char));

            if (ladder_ctx.network[net].cells[row][column].data_qty > 0) {
                switch (ladder_ctx.network[net].cells[row][column].data[0].type) {
                    case LADDER_REGISTER_S:
                        sprintf((*cells)[1], "     %.10s   %s", ladder_ctx.network[net].cells[row][column].data[0].value.cstr,
                                SPACE_BAR(ladder_ctx, net, row, column));
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_Q)
                            sprintf(strtmp, "%d.%d", ladder_ctx.network[net].cells[row][column].data[0].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[0].value.mp.port);
                        else
                            ftos(ladder_ctx.network[net].cells[row][column].data[0].value.real, strtmp, 8, 2);

                        if (strlen(strtmp) < 4)
                            strtmp[3] = ' ';
                        sprintf((*cells)[1], "     %s %s      %s", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type], strtmp,
                                SPACE_BAR(ladder_ctx, net, row, column));
                        break;
                    default:
                        if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_CONN)
                            sprintf((*cells)[1], "                  %s", SPACE_BAR(ladder_ctx, net, row, column));
                        else sprintf((*cells)[1], "     %s %04d      %s", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[0].value.u32, SPACE_BAR(ladder_ctx, net, row, column));
                }
            } else sprintf((*cells)[1], "                  %s", SPACE_BAR(ladder_ctx, net, row, column));

            break;
        case 2:
            memset(strtmp, 0, 9 * sizeof(char));
            if (row > ladder_ctx.network[net].rows - 1)
                break;
            if (ladder_ctx.network[net].cells[row][column].data_qty > 0) {
                switch (ladder_ctx.network[net].cells[row][column].data[0].type) {
                    case LADDER_REGISTER_S:
                        sprintf((*cells)[1], "   | %.10s |  ", ladder_ctx.network[net].cells[row][column].data[0].value.cstr);
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_Q)
                            sprintf(strtmp, "%d.%d", ladder_ctx.network[net].cells[row][column].data[0].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[0].value.mp.port);
                        else
                            ftos(ladder_ctx.network[net].cells[row][column].data[0].value.real, strtmp, 8, 2);

                        sprintf((*cells)[1], "   | %s %s    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type], strtmp);
                        break;
                    default:
                        if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_CONN)
                            sprintf((*cells)[1], "                   ");
                        else sprintf((*cells)[1], "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[0].value.u32);
                }

                memset(strtmp, 0, 9 * sizeof(char));
                if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_TON || ladder_ctx.network[net].cells[row][column].code == LADDER_INS_TOF
                        || ladder_ctx.network[net].cells[row][column].code == LADDER_INS_TP) {
                    sprintf((*cells)[2], "%s| %04d %s |%s", actual_ioc.inputs == 1 ? "   " : "---",
                            (int) ladder_ctx.network[net].cells[row][column].data[1].value.i32,
                            basetime_graph[ladder_ctx.network[net].cells[row][column].data[1].type],
                            actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                } else {
                    switch (ladder_ctx.network[net].cells[row][column].data[1].type) {
                        case LADDER_REGISTER_S:
                            sprintf((*cells)[2], "%s| %.10s |%s", actual_ioc.inputs == 1 ? "   " : "---",
                                    ladder_ctx.network[net].cells[row][column].data[1].value.cstr,
                                    actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                            break;
                        case LADDER_REGISTER_I:
                        case LADDER_REGISTER_Q:
                        case LADDER_REGISTER_R:
                            if (ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_I
                                    || ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_Q)
                                sprintf(strtmp, "%d.%d", ladder_ctx.network[net].cells[row][column].data[1].value.mp.module,
                                        ladder_ctx.network[net].cells[row][column].data[1].value.mp.port);
                            else ftos(ladder_ctx.network[net].cells[row][column].data[1].value.real, strtmp, 8, 2);

                            sprintf((*cells)[2], "%s| %s %s    |%s", actual_ioc.inputs == 1 ? "   " : "---",
                                    dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type], strtmp,
                                    actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                            break;
                        default:
                            sprintf((*cells)[2], "%s| %s %04d    |%s", actual_ioc.inputs == 1 ? "   " : "---",
                                    dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type],
                                    (uint32_t) ladder_ctx.network[net].cells[row][column].data[1].value.u32,
                                    actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                    }
                }
            } else {
                sprintf((*cells)[1], "                   ");
                sprintf((*cells)[2], "                   ");
            }
            sprintf((*cells)[3], "   +------------+ %s", SPACE_BAR(ladder_ctx, net, row, column));

            break;
        case 3:
            memset(strtmp, 0, 9 * sizeof(char));
            if (row > ladder_ctx.network[net].rows - 3)
                break;
            if (ladder_ctx.network[net].cells[row][column].data_qty > 0) {
                switch (ladder_ctx.network[net].cells[row][column].data[0].type) {
                    case LADDER_REGISTER_S:
                        sprintf((*cells)[1], "   | %.10s |  ", ladder_ctx.network[net].cells[row][column].data[0].value.cstr);
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_Q)
                            sprintf(strtmp, "%d.%d", ladder_ctx.network[net].cells[row][column].data[0].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[0].value.mp.port);
                        else
                            ftos(ladder_ctx.network[net].cells[row][column].data[0].value.real, strtmp, 8, 2);

                        sprintf((*cells)[1], "   | %s %s    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type], strtmp);
                        break;
                    default:
                        if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_CONN)
                            sprintf((*cells)[1], "                   ");
                        else sprintf((*cells)[1], "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[0].value.u32);
                }

                memset(strtmp, 0, 9 * sizeof(char));
                switch (ladder_ctx.network[net].cells[row][column].data[1].type) {
                    case LADDER_REGISTER_S:
                        sprintf((*cells)[2], "%s| %.10s |%s", actual_ioc.inputs < 2 ? "   " : "---",
                                ladder_ctx.network[net].cells[row][column].data[1].value.cstr,
                                actual_ioc.outputs < 2 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_Q)
                            sprintf(strtmp, "%d.%d", ladder_ctx.network[net].cells[row][column].data[1].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[1].value.mp.port);
                        else
                            ftos(ladder_ctx.network[net].cells[row][column].data[1].value.real, strtmp, 8, 2);

                        sprintf((*cells)[2], "%s| %s %s    |%s", actual_ioc.inputs < 2 ? "   " : "---",
                                dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type], strtmp,
                                actual_ioc.outputs < 2 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                        break;
                    default:
                        sprintf((*cells)[2], "%s| %s %04d    |%s", actual_ioc.inputs < 2 ? "   " : "---",
                                dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[1].value.u32,
                                actual_ioc.outputs < 2 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                }

                memset(strtmp, 0, 9 * sizeof(char));
                switch (ladder_ctx.network[net].cells[row][column].data[2].type) {
                    case LADDER_REGISTER_S:
                        sprintf((*cells)[3], "   | %.10s |  ", ladder_ctx.network[net].cells[row][column].data[2].value.cstr);
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[2].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[2].type == LADDER_REGISTER_Q)
                            sprintf(strtmp, "%d.%d", ladder_ctx.network[net].cells[row][column].data[2].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[2].value.mp.port);
                        else
                            ftos(ladder_ctx.network[net].cells[row][column].data[2].value.real, strtmp, 8, 2);

                        sprintf((*cells)[3], "   | %s %s |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[2].type], strtmp);
                        break;
                    default:
                        sprintf((*cells)[3], "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[2].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[2].value.u32);
                }
            } else {
                sprintf((*cells)[1], "                   ");
                sprintf((*cells)[2], "                   ");
                sprintf((*cells)[3], "                   ");
            }

            sprintf((*cells)[4], "%s|            |%s", actual_ioc.inputs < 3 ? "   " : "---",
                    actual_ioc.outputs < 3 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
            sprintf((*cells)[5], "   +------------+ %s", SPACE_BAR(ladder_ctx, net, row, column));

            break;
        default:
            break;
    }
}

void ladder_print(ladder_ctx_t ladder_ctx) {
    char fn_str[6][32];
    char (*fn_str_ptr)[6][32] = &fn_str;

    for (uint32_t n = 0; n < ladder_ctx.ladder.quantity.networks; n++) {
        char network_str[ladder_ctx.network[n].rows + 3][ladder_ctx.network[n].cols + 3][2][32];
        memset(network_str, 0, ladder_ctx.network[n].rows * ladder_ctx.network[n].cols * 2 * 32 * sizeof(char));

        printf("[Network %d (%s)]\n\n", (int) n, ladder_ctx.network[n].enable ? "enabled" : "disabled");

        for (uint32_t r = 0; r < ladder_ctx.network[n].rows; r++) {
            for (uint32_t c = 0; c < ladder_ctx.network[n].cols; c++) {
                if (network_str[r][c][0][0] == '\0') {
                    fn_to_str(ladder_ctx, n, fn_str_ptr, r, c);
                    strcpy(network_str[r][c][0], fn_str[0]);
                    strcpy(network_str[r][c][1], fn_str[1]);
                    strcpy(network_str[r + 1][c][0], fn_str[2]);
                    strcpy(network_str[r + 1][c][1], fn_str[3]);
                    strcpy(network_str[r + 2][c][0], fn_str[4]);
                    strcpy(network_str[r + 2][c][1], fn_str[5]);
                }
            }
        }

        for (uint32_t r = 0; r < ladder_ctx.network[n].rows; r++) {
            printf("    |");
            for (uint32_t c = 0; c < ladder_ctx.network[n].cols; c++) {
                if (network_str[r][c][0][0] == '\0')
                    printf("                   ");
                else printf("%s", network_str[r][c][0]);
            }
            printf("|\n    |");
            for (uint32_t c = 0; c < ladder_ctx.network[n].cols; c++) {
                if (network_str[r][c][0][0] == '\0')
                    printf("                   ");
                else printf("%s", network_str[r][c][1]);
            }
            printf("|\n");
        }

        printf("\n\n");
    }
}
