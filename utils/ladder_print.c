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
                "TOF", // 2
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
    if (fabsf(f) > 999999.99f) {
        strcpy(str, "OVF     ");
        return str;
    }

    if (decimals == 0) {
        snprintf(str, n, "%d      ", (int) (f + (f < 0 ? -0.5 : 0.5)));
        return str;
    } else if (decimals < 0) {
        float scale = powf(10.0f, -decimals);
        int val = (int) (f / scale + (f < 0 ? -0.5f : 0.5f));
        snprintf(str, n, "%d      ", val);
        return str;
    } else {
        int sign = (f < 0) ? -1 : 1;
        int ifl = (int) f;
        float frac = fabsf(f - (float) ifl);
        float g = powf(10.0f, decimals);
        frac *= g;
        frac += 0.5f;
        if (frac >= g) {
            frac -= g;
            ifl += sign; // Key fix: use sign for carry-over
        }
        int E = (int) frac;
        char fmt[128];
        sprintf(fmt, "%%d.%%0%dd  ", decimals);
        snprintf(str, n, fmt, ifl, E);
        return str;
    }
}

static const char *basetime_graph[] = { "ms   ", "10ms ", "100ms", "seg  ", "min  " };

static void fn_to_str(ladder_ctx_t ladder_ctx, uint32_t net, char (*cells)[6][32], uint32_t row, uint32_t column) {
    char strtmp[13];

    memset(cells, 0, 6 * 32 * sizeof(char));

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_NOP) {
        snprintf((*cells)[0], sizeof((*cells)[0]), "                  %s", (LADDER_VERTICAL_BAR(&ladder_ctx, net, row, column) ? "+" : " "));
        snprintf((*cells)[1], sizeof((*cells)[0]), "                   ");
        return;
    }

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_MULTI) {
        printf("MULTI\n");
        snprintf((*cells)[0], sizeof((*cells)[0]), "---+-ERR MULTI--+--");
        snprintf((*cells)[1], sizeof((*cells)[0]), "---++++++++++++++--");
        return;
    }

    ladder_instructions_iocd_t actual_ioc;

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_FOREIGN) {
        memcpy(&actual_ioc, &(ladder_ctx.foreign.fn[ladder_ctx.network[net].cells[row][column].data[0].value.u32]).description,
                sizeof(ladder_instructions_iocd_t));

        snprintf((*cells)[0], sizeof((*cells)[0]), "---+-%s--------+%s",
                ladder_ctx.network[net].cells[row][column].data[0].value.u32 >= ladder_ctx.foreign.qty ? "FN?" :
                strlen(ladder_ctx.foreign.fn[ladder_ctx.network[net].cells[row][column].data[0].value.i32].name) == 0 ?
                        "FN?" : ladder_ctx.foreign.fn[ladder_ctx.network[net].cells[row][column].data[0].value.u32].name,
                PIN_OUT(ladder_ctx, net, row, column));
    } else {
        memcpy(&actual_ioc, &(ladder_fn_iocd[ladder_ctx.network[net].cells[row][column].code]), sizeof(ladder_instructions_iocd_t));
        if (actual_ioc.cells != 1) {
            snprintf((*cells)[0], sizeof((*cells)[0]), "---+-%s--------+%s", fn_symbol[ladder_ctx.network[net].cells[row][column].code],
                    PIN_OUT(ladder_ctx, net, row, column));
        } else {
            snprintf((*cells)[0], sizeof((*cells)[0]), "--------%s------%s", fn_symbol[ladder_ctx.network[net].cells[row][column].code],
                    PIN_OUT(ladder_ctx, net, row, column));
        }
    }

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_FOREIGN
            && ladder_ctx.network[net].cells[row][column].data[0].value.u32 >= ladder_ctx.foreign.qty)
        return;

    switch (actual_ioc.cells) {
        case 1:
            memset(strtmp, 0, sizeof(strtmp));

            if (ladder_ctx.network[net].cells[row][column].data_qty > 0) {
                switch (ladder_ctx.network[net].cells[row][column].data[0].type) {
                    case LADDER_REGISTER_S:
                        snprintf((*cells)[1], sizeof((*cells)[0]), "     %.10s   %s", ladder_ctx.network[net].cells[row][column].data[0].value.cstr,
                                SPACE_BAR(ladder_ctx, net, row, column));
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_Q)
                            snprintf(strtmp, sizeof(strtmp), "%d.%d", ladder_ctx.network[net].cells[row][column].data[0].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[0].value.mp.port);
                        else ftos(ladder_ctx.network[net].cells[row][column].data[0].value.real, strtmp, 12, 2); // Modified: Increased n from 8 to 12.

                        if (strlen(strtmp) < 4)
                            strtmp[3] = ' ';
                        snprintf((*cells)[1], sizeof((*cells)[0]), "     %s %s      %s", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                strtmp, SPACE_BAR(ladder_ctx, net, row, column));
                        break;
                    default:
                        if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_CONN)
                            snprintf((*cells)[1], sizeof((*cells)[0]), "                  %s", SPACE_BAR(ladder_ctx, net, row, column));
                        else snprintf((*cells)[1], sizeof((*cells)[0]), "     %s %04d      %s",
                                dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[0].value.u32, SPACE_BAR(ladder_ctx, net, row, column));
                }
            } else snprintf((*cells)[1], sizeof((*cells)[0]), "                  %s", SPACE_BAR(ladder_ctx, net, row, column));

            break;
        case 2:
            memset(strtmp, 0, sizeof(strtmp));
            if (row > ladder_ctx.network[net].rows - 1)
                break;
            if (ladder_ctx.network[net].cells[row][column].data_qty > 0) {
                switch (ladder_ctx.network[net].cells[row][column].data[0].type) {
                    case LADDER_REGISTER_S:
                        snprintf((*cells)[1], sizeof((*cells)[0]), "   | %.10s |  ", ladder_ctx.network[net].cells[row][column].data[0].value.cstr);
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_Q)
                            snprintf(strtmp, sizeof(strtmp), "%d.%d", ladder_ctx.network[net].cells[row][column].data[0].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[0].value.mp.port);
                        else ftos(ladder_ctx.network[net].cells[row][column].data[0].value.real, strtmp, 12, 2); // Modified: Increased n from 8 to 12.

                        snprintf((*cells)[1], sizeof((*cells)[0]), "   | %s %s    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                strtmp);
                        break;
                    default:
                        if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_CONN)
                            snprintf((*cells)[1], sizeof((*cells)[0]), "                   ");
                        else snprintf((*cells)[1], sizeof((*cells)[0]), "   | %s %04d    |  ",
                                dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[0].value.u32);
                }

                memset(strtmp, 0, sizeof(strtmp));
                if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_TON || ladder_ctx.network[net].cells[row][column].code == LADDER_INS_TOF
                        || ladder_ctx.network[net].cells[row][column].code == LADDER_INS_TP) {
                    const char *bt_str = ((int)ladder_ctx.network[net].cells[row][column].data[1].type >= (int)LADDER_BASETIME_MS &&
                                          (int)ladder_ctx.network[net].cells[row][column].data[1].type <= (int)LADDER_BASETIME_MIN) ?
                                         basetime_graph[ladder_ctx.network[net].cells[row][column].data[1].type] : "INV   ";

                    snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %04d %s |%s", actual_ioc.inputs == 1 ? "   " : "---",
                            (int) ladder_ctx.network[net].cells[row][column].data[1].value.i32, bt_str,
                            actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                } else {
                    switch (ladder_ctx.network[net].cells[row][column].data[1].type) {
                        case LADDER_REGISTER_S:
                            snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %.10s |%s", actual_ioc.inputs == 1 ? "   " : "---",
                                    ladder_ctx.network[net].cells[row][column].data[1].value.cstr,
                                    actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                            break;
                        case LADDER_REGISTER_I:
                        case LADDER_REGISTER_Q:
                        case LADDER_REGISTER_R:
                            if (ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_I
                                    || ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_Q)
                                snprintf(strtmp, sizeof(strtmp), "%d.%d", ladder_ctx.network[net].cells[row][column].data[1].value.mp.module,
                                        ladder_ctx.network[net].cells[row][column].data[1].value.mp.port);
                            else ftos(ladder_ctx.network[net].cells[row][column].data[1].value.real, strtmp, 12, 2); // Modified: Increased n from 8 to 12.

                            snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %s %s    |%s", actual_ioc.inputs == 1 ? "   " : "---",
                                    dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type], strtmp,
                                    actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                            break;
                        default:
                            snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %s %04d    |%s", actual_ioc.inputs == 1 ? "   " : "---",
                                    dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type],
                                    (uint32_t) ladder_ctx.network[net].cells[row][column].data[1].value.u32,
                                    actual_ioc.outputs == 1 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                    }
                }
            } else {
                snprintf((*cells)[1], sizeof((*cells)[0]), "                   ");
                snprintf((*cells)[2], sizeof((*cells)[0]), "                   ");
            }
            snprintf((*cells)[3], sizeof((*cells)[0]), "   +------------+ %s", SPACE_BAR(ladder_ctx, net, row, column));

            break;
        case 3:
            memset(strtmp, 0, sizeof(strtmp));
            if (row > ladder_ctx.network[net].rows - 3)
                break;
            if (ladder_ctx.network[net].cells[row][column].data_qty > 0) {
                switch (ladder_ctx.network[net].cells[row][column].data[0].type) {
                    case LADDER_REGISTER_S:
                        snprintf((*cells)[1], sizeof((*cells)[0]), "   | %.10s |  ", ladder_ctx.network[net].cells[row][column].data[0].value.cstr);
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[0].type == LADDER_REGISTER_Q)
                            snprintf(strtmp, sizeof(strtmp), "%d.%d", ladder_ctx.network[net].cells[row][column].data[0].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[0].value.mp.port);
                        else ftos(ladder_ctx.network[net].cells[row][column].data[0].value.real, strtmp, 12, 2); // Modified: Increased n from 8 to 12.

                        snprintf((*cells)[1], sizeof((*cells)[0]), "   | %s %s    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                strtmp);
                        break;
                    default:
                        snprintf((*cells)[1], sizeof((*cells)[0]), "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[0].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[0].value.u32);
                }

                memset(strtmp, 0, sizeof(strtmp));
                switch (ladder_ctx.network[net].cells[row][column].data[1].type) {
                    case LADDER_REGISTER_S:
                        snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %.10s |%s", actual_ioc.inputs < 2 ? "   " : "---",
                                ladder_ctx.network[net].cells[row][column].data[1].value.cstr,
                                actual_ioc.outputs < 2 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[1].type == LADDER_REGISTER_Q)
                            snprintf(strtmp, sizeof(strtmp), "%d.%d", ladder_ctx.network[net].cells[row][column].data[1].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[1].value.mp.port);
                        else ftos(ladder_ctx.network[net].cells[row][column].data[1].value.real, strtmp, 12, 2); // Modified: Increased n from 8 to 12.

                        snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %s %s    |%s", actual_ioc.inputs < 2 ? "   " : "---",
                                dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type], strtmp,
                                actual_ioc.outputs < 2 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                        break;
                    default:
                        snprintf((*cells)[2], sizeof((*cells)[0]), "%s| %s %04d    |%s", actual_ioc.inputs < 2 ? "   " : "---",
                                dt_graph[ladder_ctx.network[net].cells[row][column].data[1].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[1].value.u32,
                                actual_ioc.outputs < 2 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
                }

                memset(strtmp, 0, sizeof(strtmp));
                switch (ladder_ctx.network[net].cells[row][column].data[2].type) {
                    case LADDER_REGISTER_S:
                        snprintf((*cells)[3], sizeof((*cells)[0]), "   | %.10s |  ", ladder_ctx.network[net].cells[row][column].data[2].value.cstr);
                        break;
                    case LADDER_REGISTER_I:
                    case LADDER_REGISTER_Q:
                    case LADDER_REGISTER_R:
                        if (ladder_ctx.network[net].cells[row][column].data[2].type == LADDER_REGISTER_I
                                || ladder_ctx.network[net].cells[row][column].data[2].type == LADDER_REGISTER_Q)
                            snprintf(strtmp, sizeof(strtmp), "%d.%d", ladder_ctx.network[net].cells[row][column].data[2].value.mp.module,
                                    ladder_ctx.network[net].cells[row][column].data[2].value.mp.port);
                        else ftos(ladder_ctx.network[net].cells[row][column].data[2].value.real, strtmp, 12, 2); // Modified: Increased n from 8 to 12.

                        snprintf((*cells)[3], sizeof((*cells)[0]), "   | %s %s |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[2].type], strtmp);
                        break;
                    default:
                        snprintf((*cells)[3], sizeof((*cells)[0]), "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].data[2].type],
                                (uint32_t) ladder_ctx.network[net].cells[row][column].data[2].value.u32);
                }
            } else {
                snprintf((*cells)[1], sizeof((*cells)[0]), "                   ");
                snprintf((*cells)[2], sizeof((*cells)[0]), "                   ");
                snprintf((*cells)[3], sizeof((*cells)[0]), "                   ");
            }

            snprintf((*cells)[4], sizeof((*cells)[0]), "%s|            |%s", actual_ioc.inputs < 3 ? "   " : "---",
                    actual_ioc.outputs < 3 ? "  " : PIN_OUT(ladder_ctx, net, row, column));
            snprintf((*cells)[5], sizeof((*cells)[0]), "   +------------+ %s", SPACE_BAR(ladder_ctx, net, row, column));

            break;
        default:
            break;
    }
}

void ladder_print(ladder_ctx_t ladder_ctx) {
    char fn_str[6][32];
    char (*fn_str_ptr)[6][32] = &fn_str;

    for (uint32_t n = 0; n < ladder_ctx.ladder.quantity.networks; n++) {
        uint32_t rows = ladder_ctx.network[n].rows;
        uint32_t cols = ladder_ctx.network[n].cols;

        if (rows > LADDER_MAX_ROWS || cols > LADDER_MAX_COLS) {
            printf("Error: Network %d dimensions (%u rows, %u cols) exceed maximum limits (%d rows, %d cols). Skipping print.\n", (int) n, rows, cols,
                    LADDER_MAX_ROWS, LADDER_MAX_COLS);
            continue;
        }

        size_t total_size = (rows + 3) * (cols + 3) * 2 * 32 * sizeof(char);
        char *network_str_raw = calloc(1, total_size);
        if (!network_str_raw) {
            printf("Error: Failed to allocate memory for network %d print (size: %zu bytes). Skipping.\n", (int) n, total_size);
            continue;
        }

#define NET_STR(r, c, l, o) network_str_raw[((( (r) * (cols + 3) + (c) ) * 2 + (l) ) * 32) + (o)]

        printf("[Network %d (%s)]\n\n", (int) n, ladder_ctx.network[n].enable ? "enabled" : "disabled");

        for (uint32_t r = 0; r < rows; r++) {
            for (uint32_t c = 0; c < cols; c++) {
                if (NET_STR(r, c, 0, 0)== '\0') {
                    fn_to_str(ladder_ctx, n, fn_str_ptr, r, c);
                    strncpy(&NET_STR(r, c, 0, 0), fn_str[0], 32);
                    strncpy(&NET_STR(r, c, 1, 0), fn_str[1], 32);
                    strncpy(&NET_STR(r + 1, c, 0, 0), fn_str[2], 32);
                    strncpy(&NET_STR(r + 1, c, 1, 0), fn_str[3], 32);
                    strncpy(&NET_STR(r + 2, c, 0, 0), fn_str[4], 32);
                    strncpy(&NET_STR(r + 2, c, 1, 0), fn_str[5], 32);
                }
            }
        }

        for (uint32_t r = 0; r < rows; r++) {
            printf("    |");
            for (uint32_t c = 0; c < cols; c++) {
                if (NET_STR(r, c, 0, 0)== '\0')
                printf("                   ");
                else printf("%s", &NET_STR(r, c, 0, 0));
            }
            printf("|\n    |");
            for (uint32_t c = 0; c < cols; c++) {
                if (NET_STR(r, c, 1, 0)== '\0')
                printf("                   ");
                else printf("%s", &NET_STR(r, c, 1, 0));
            }
            printf("|\n");
        }

        printf("\n\n");

        free(network_str_raw);
#undef NET_STR
    }
}
