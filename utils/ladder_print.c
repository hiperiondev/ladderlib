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

static const char *fn_str_graph[] = {
        "----------", //
        "----------", //
        "-----!----", //
        "----| |---", //
        "----|/|---", //
        "---(RE )--", //
        "---(FE )--", //
        "----( )---", //
        "----(L)---", //
        "----(U)---", //
        "---(TON)--", //
        "---(TFF)--", //
        "---(TP )--", //
        "---(CTU)--", //
        "---(CTD)--", //
        "---(MOV)--", //
        "---(SUB)--", //
        "---(ADD)--", //
        "---(MUL)--", //
        "---(DIV)--", //
        "---(MOD)--", //
        "---(SHL)--", //
        "---(SHR)--", //
        "---(ROL)--", //
        "---(ROR)--", //
        "---(AND)--", //
        "---(OR )--", //
        "---(XOR)--", //
        "---(NOT)--", //
        "---(EQ )--", //
        "---(GT )--", //
        "---(GE )--", //
        "---(LT )--", //
        "---(LE )--", //
        "---(NE )--", //
        };

static const char *fn_masked_str_graph[] = {
        "----------", //
        "----------", //
        "----[!]---", //
        "---[| |]--", //
        "---[|/|]--", //
        "---[RE ]--", //
        "---[FE ]--", //
        "---[( )]--", //
        "---[(L)]--", //
        "---[(U)]--", //
        "---[TON]--", //
        "---[TFF]--", //
        "---[TP ]--", //
        "---[CTU]--", //
        "---[CTD]--", //
        "---[MOV]--", //
        "---[SUB]--", //
        "---[ADD]--", //
        "---[MUL]--", //
        "---[DIV]--", //
        "---[MOD]--", //
        "---[SHL]--", //
        "---[SHR]--", //
        "---[ROL]--", //
        "---[ROR]--", //
        "---[AND]--", //
        "---[OR ]--",  //
        "---[XOR]--", //
        "---[NOT]--", //
        "---[EQ ]--", //
        "---[GT ]--", //
        "---[GE ]--", //
        "---[LT ]--", //
        "---[LE ]--", //
        "---[NE ]--", //
        };

static const char *dt_graph[] = {
        " M",  //
        " Q",  //
        " I",  //
        "Cd", //
        "Cr", //
        "Td", //
        "Tr", //
        "IW", //
        "QW", //
        " C",  //
        " T",  //
        " D",  //
        " K",  //
        " R",  //
        "KR", //
        };

void ladder_print(ladder_ctx_t *ladder_ctx) {
    for (uint32_t n = 0; n < (*ladder_ctx).ladder.total_networks; n++) {
        printf("Network: %d\n", n);
        (*ladder_ctx).exec_network = (*ladder_ctx).network[n];

        for (uint32_t r = 0; r < NET_ROWS; r++) {
            printf("|");

            for (uint32_t c = 0; c < NET_COLUMNS; c++) {
                if ((*ladder_ctx).exec_network.cells[r][c].code < LADDER_INS_INV) {
                    printf("%s", fn_str_graph[(*ladder_ctx).exec_network.cells[r][c].code]);
                } else {
                    if (((*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK) < LADDER_INS_INV) {
                        printf("%s", fn_masked_str_graph[(*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK]);
                    } else {
                        printf("-(-?-)-");
                    }
                }
            }

            printf("|\n|");

            for (uint32_t c = 0; c < NET_COLUMNS; c++) {
                if ((*ladder_ctx).exec_network.cells[r][c].code < LADDER_INS_INV && (*ladder_ctx).exec_network.cells[r][c].code > 1) {
                    printf(" {%s %04d}", dt_graph[(*ladder_ctx).exec_network.cells[r][c].type], (*ladder_ctx).exec_network.cells[r][c].data);
                } else {
                    if (((*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK) < LADDER_INS_INV
                            && (*ladder_ctx).exec_network.cells[r][c].code > 1) {
                        printf(" {%s %04d}", dt_graph[(*ladder_ctx).exec_network.cells[r][c].type], (*ladder_ctx).exec_network.cells[r][c].data);
                    } else {
                        printf("          ");
                    }
                }
            }
            printf("|\n");
            printf( "|%*s|\n", NET_COLUMNS * 10, "");
        }
        printf("\n\n");
    }
}
