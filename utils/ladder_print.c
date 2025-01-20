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

#include <stdio.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_print.h"
#include "ladder_instructions.h"

static const uint8_t fn_cells_qty[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1};

static const char *fn_str_graph_1[] = {
        "----------------", // 1
        "----------------", // 1
        "--------!-------", // 1
        "-------| |------", // 1
        "-------|/|------", // 1
        "-------(RE )----", // 1
        "-------(FE )----", // 1
        "-------( )------", // 1
        "-------(L)------", // 1
        "-------(U)------", // 1
        "-------(TON)----", // 2
        "-------(TFF)----", // 2
        "-------(TP )----", // 2
        "-------(CTU)----", // 2
        "-------(CTD)----", // 2
        "-------(MOV)----", // 2
        "-------(SUB)----", // 3
        "-------(ADD)----", // 3
        "-------(MUL)----", // 3
        "-------(DIV)----", // 3
        "-------(MOD)----", // 3
        "-------(SHL)----", // 2
        "-------(SHR)----", // 2
        "-------(ROL)----", // 2
        "-------(ROR)----", // 2
        "-------(AND)----", // 3
        "-------(OR )----", // 3
        "-------(XOR)----", // 3
        "-------(NOT)----", // 2
        "-------(EQ )----", // 2
        "-------(GT )----", // 2
        "-------(GE )----", // 2
        "-------(LT )----", // 2
        "-------(LE )----", // 2
        "-------(NE )----", // 2
        "-------(-?-)----", // 1
        "-------|--------", // 1
        };

static const char *fn_masked_str_graph_open[] = {
        "----------------",           // 1
        "----------------",           // 1
        "-------!--------",           // 1
        "------| |-------",           // 1
        "------|/|-------",           // 1
        "------(RE )-----",           // 1
        "------(FE )-----",           // 1
        "------( )-------",           // 1
        "------(L)-------",           // 1
        "------(U)-------",           // 1
        "---\u2308  TON    \u2309--", // 2
        "---\u2308  TFF    \u2309--", // 2
        "---\u2308  TP     \u2309--", // 2
        "---\u2308  CTU    \u2309--", // 2
        "---\u2308  CTD    \u2309--", // 2
        "---\u2308  MOV    \u2309--", // 2
        "---\u2308  SUB    \u2309--", // 3
        "---\u2308  ADD    \u2309--", // 3
        "---\u2308  MUL    \u2309--", // 3
        "---\u2308  DIV    \u2309--", // 3
        "---\u2308  MOD    \u2309--", // 3
        "---\u2308  SHL    \u2309--", // 2
        "---\u2308  SHR    \u2309--", // 2
        "---\u2308  ROL    \u2309--", // 2
        "---\u2308  ROR    \u2309--", // 2
        "---\u2308  AND    \u2309--", // 3
        "---\u2308  OR     \u2309--", // 3
        "---\u2308  XOR    \u2309--", // 3
        "---\u2308  NOT    \u2309--", // 2
        "---\u2308  EQ     \u2309--", // 2
        "---\u2308  GT     \u2309--", // 2
        "---\u2308  GE     \u2309--", // 2
        "---\u2308  LT     \u2309--", // 2
        "---\u2308  LE     \u2309--", // 2
        "---\u2308  NE     \u2309--", // 2
        "------(-?-)-----",           // 1
        "------|---------",           // 1
        };


static const char *fn_masked_str_graph_close[] = {
        "----------------",           // 1
        "----------------",           // 1
        "-------!--------",           // 1
        "------| |-------",           // 1
        "------|/|-------",           // 1
        "------(RE )-----",           // 1
        "------(FE )-----",           // 1
        "------( )-------",           // 1
        "------(L)-------",           // 1
        "------(U)-------",           // 1
        "---\u230a  TON    \u230b--", // 2
        "---\u230a  TFF    \u230b--", // 2
        "---\u230a  TP     \u230b--", // 2
        "---\u230a  CTU    \u230b--", // 2
        "---\u230a  CTD    \u230b--", // 2
        "---\u230a  MOV    \u230b--", // 2
        "---\u230a  SUB    \u230b--", // 3
        "---\u230a  ADD    \u230b--", // 3
        "---\u230a  MUL    \u230b--", // 3
        "---\u230a  DIV    \u230b--", // 3
        "---\u230a  MOD    \u230b--", // 3
        "---\u230a  SHL    \u230b--", // 2
        "---\u230a  SHR    \u230b--", // 2
        "---\u230a  ROL    \u230b--", // 2
        "---\u230a  ROR    \u230b--", // 2
        "---\u230a  AND    \u230b--", // 3
        "---\u230a  OR     \u230b--", // 3
        "---\u230a  XOR    \u230b--", // 3
        "---\u230a  NOT    \u230b--", // 2
        "---\u230a  EQ     \u230b--", // 2
        "---\u230a  GT     \u230b--", // 2
        "---\u230a  GE     \u230b--", // 2
        "---\u230a  LT     \u230b--", // 2
        "---\u230a  LE     \u230b--", // 2
        "---\u230a  NE     \u230b--", // 2
        "------(-?-)-----",           // 1
        "------|---------",           // 1
        };

static const char *dt_graph[] = {
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
        " K", //
        " R", //
        "KR", //
        };

void ladder_print(ladder_ctx_t *ladder_ctx) {
    for (uint32_t n = 0; n < (*ladder_ctx).ladder.total_networks; n++) {
        printf("Network: %d\n", n);
        (*ladder_ctx).exec_network = (*ladder_ctx).network[n];

        for (uint32_t r = 0; r < NET_ROWS; r++) {
            printf("|");

            // function
            for (uint32_t c = 0; c < NET_COLUMNS; c++) {
                if ((*ladder_ctx).exec_network.cells[r][c].code < LADDER_INS_INV) {
                    if (fn_cells_qty[(*ladder_ctx).exec_network.cells[r][c].code] == 1) {
                        printf("%s", fn_str_graph_1[(*ladder_ctx).exec_network.cells[r][c].code]);
                    } else {
                        printf("%s", fn_masked_str_graph_open[(*ladder_ctx).exec_network.cells[r][c].code]);
                    }
                } else {
                    if (((*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK) < LADDER_INS_INV) {
                        if (fn_cells_qty[(*ladder_ctx).exec_network.cells[r][c].code] == 2) {
                            printf("%s", fn_masked_str_graph_close[(*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK]);
                        } else {
                            if ((*ladder_ctx).exec_network.cells[r - 1][c].code > LADDER_INS_INV) {
                                printf("---\u230a %s %04d \u230b--", dt_graph[(*ladder_ctx).exec_network.cells[r][c].type],
                                        (*ladder_ctx).exec_network.cells[r][c].data);
                            } else {
                                printf("---| %s %04d |--", dt_graph[(*ladder_ctx).exec_network.cells[r][c].type], (*ladder_ctx).exec_network.cells[r][c].data);
                            }
                        }
                    } else {
                        printf("------(-?-)-----");
                    }
                }
            }

            printf("|");
            printf("\n|");

            // data
            for (uint32_t c = 0; c < NET_COLUMNS; c++) {
                if ((*ladder_ctx).exec_network.cells[r][c].code < LADDER_INS_INV && (*ladder_ctx).exec_network.cells[r][c].code > 1) {
                    printf("   | %s %04d |  ", dt_graph[(*ladder_ctx).exec_network.cells[r][c].type], (*ladder_ctx).exec_network.cells[r][c].data);
                } else {
                    if ((*ladder_ctx).exec_network.cells[r][c].code > LADDER_INS_INV
                            && (*ladder_ctx).exec_network.cells[(r < NET_COLUMNS - 1 ? r + 1 : NET_COLUMNS - 1)][c].code > LADDER_INS_INV)
                        printf("   |         |  ");
                    else
                        printf("                ");
                }
            }
            printf("|\n");
        }
        printf("\n\n");
    }
}
