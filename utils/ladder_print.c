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
#include <string.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_print.h"
#include "ladder_instructions.h"

static const char *fn_symbol[] = {
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
        "-|-", //
        "FGN", // ?
        "???", // 1
        };

static const char *dt_graph[] = {
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
        " K", //
        " R", //
        "KR", //
        };

static const char *basetime_graph[] = {
        "ms   ",
        "10ms ",
        "100ms",
        "seg  ",
        "min  "
};

/**
 * @struct ladder_instructions_ioc_s
 * @brief Instruction inputs, outputs, occupied cells and timer (for basetime)
 *
 */
typedef struct ladder_instructions_ioc_s {
    uint8_t inputs;   /**< Inputs quantity */
    uint8_t outputs;  /**< Outputs quantity */
    uint8_t cells;    /**< Cells quantity */
       bool basetime; /**< it's timer, have basetime */
} ladder_instructions_ioc_t;

static const ladder_instructions_ioc_t ladder_fn_ioc[] = {
        { 1, 1, 1, false }, // NOP
        { 1, 1, 1, false }, // CONN
        { 1, 1, 1, false }, // NEG
        { 1, 1, 1, false }, // NO
        { 1, 1, 1, false }, // NC
        { 1, 1, 1, false }, // RE
        { 1, 1, 1, false }, // FE
        { 1, 1, 1, false }, // COIL
        { 1, 1, 1, false }, // COILL
        { 1, 1, 1, false }, // COILU
        { 1, 2, 2, true  }, // TON
        { 1, 2, 2, true  }, // TOFF
        { 1, 2, 2, true  }, // TP
        { 2, 2, 2, false }, // CTU
        { 2, 2, 2, false }, // CTD
        { 1, 1, 2, false }, // MOVE
        { 1, 3, 3, false }, // SUB
        { 1, 1, 3, false }, // ADD
        { 1, 1, 3, false }, // MUL
        { 1, 2, 3, false }, // DIV
        { 1, 1, 3, false }, // MOD
        { 1, 1, 2, false }, // SHL
        { 1, 1, 2, false }, // SHR
        { 1, 1, 2, false }, // ROL
        { 1, 1, 2, false }, // ROR
        { 1, 1, 3, false }, // AND
        { 1, 1, 3, false }, // OR
        { 1, 1, 3, false }, // XOR
        { 1, 1, 2, false }, // NOT
        { 1, 1, 2, false }, // EQ
        { 1, 1, 2, false }, // GT
        { 1, 1, 2, false }, // GE
        { 1, 1, 2, false }, // LT
        { 1, 1, 2, false }, // LE
        { 1, 1, 2, false }, // NE
        { 0, 0, 0, false }, // FOREINGN // TODO: get from definition
        { 1, 1, 1, false }, // INV
        };

static void fn_to_str(ladder_ctx_t ladder_ctx, uint32_t net, char (*cells)[6][32], uint32_t row, uint32_t column) {
    memset(cells, 0, 6 * 32 * sizeof(char));

    if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_NOP || ladder_ctx.network[net].cells[row][column].code >= LADDER_INS_INV)
        return;

    if (ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].cells != 1) {
        sprintf((*cells)[0], "---+-%s--------+--", fn_symbol[ladder_ctx.network[net].cells[row][column].code]);
    } else {
        sprintf((*cells)[0], "--------%s--------", fn_symbol[ladder_ctx.network[net].cells[row][column].code]);
    }

    switch (ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].cells) {
        case 0:
            sprintf((*cells)[1], "                   ");
            break;
        case 1:
            if (ladder_ctx.network[net].cells[row][column].code == LADDER_INS_CONN)
                sprintf((*cells)[1], "                   ");

            else
                sprintf((*cells)[1], "     %s %04d       ", dt_graph[ladder_ctx.network[net].cells[row][column].type],
                        ladder_ctx.network[net].cells[row][column].data);
            break;
        case 2:
            if (row > ladder_ctx.ladder.quantity.net_rows - 1)
                break;

            sprintf((*cells)[1], "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].type],
                    ladder_ctx.network[net].cells[row][column].data);

            if (ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].basetime) {
                sprintf((*cells)[2], "%s| %04d %s |%s", ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].inputs == 1 ? "   " : "---",
                        ladder_ctx.network[net].cells[row + 1][column].data, basetime_graph[ladder_ctx.network[net].cells[row + 1][column].type],
                        ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].outputs == 1 ? "  " : "--");
            } else {
                sprintf((*cells)[2], "%s| %s %04d    |%s", ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].inputs == 1 ? "   " : "---",
                        dt_graph[ladder_ctx.network[net].cells[row + 1][column].type], ladder_ctx.network[net].cells[row + 1][column].data,
                        ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].outputs == 1 ? "  " : "--");
            }

            sprintf((*cells)[3], "   +------------+  ");

            break;
        case 3:
            if (row > ladder_ctx.ladder.quantity.net_rows - 3)
                break;

            sprintf((*cells)[1], "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row][column].type],
                    ladder_ctx.network[net].cells[row][column].data);

            sprintf((*cells)[2], "%s| %s %04d    |%s",
                    (ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].inputs == 2
                            || ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].outputs == 3) ? "---" : "   ",
                    dt_graph[ladder_ctx.network[net].cells[row + 1][column].type], ladder_ctx.network[net].cells[row + 1][column].data,
                    (ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].outputs == 2
                            || ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].outputs == 3) ? "--" : "  ");

            sprintf((*cells)[3], "   | %s %04d    |  ", dt_graph[ladder_ctx.network[net].cells[row + 2][column].type],
                    ladder_ctx.network[net].cells[row + 1][column].data);

            sprintf((*cells)[4], "%s|            |%s", ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].inputs == 1 ? "   " : "---",
                    ladder_fn_ioc[ladder_ctx.network[net].cells[row][column].code].outputs == 1 ? "  " : "--");

            sprintf((*cells)[5], "   +------------+  ");
            break;
        default:
            break;
    }
}

void ladder_print(ladder_ctx_t ladder_ctx) {
    char network_str[ladder_ctx.ladder.quantity.net_rows + 3][ladder_ctx.ladder.quantity.net_columns + 3][2][32];
    char fn_str[6][32];
    char (*fn_str_ptr)[6][32] = &fn_str;

    for (uint32_t n = 0; n < ladder_ctx.ladder.quantity.networks; n++) {
        memset(network_str, 0, ladder_ctx.ladder.quantity.net_rows * ladder_ctx.ladder.quantity.net_columns * 2 * 32 * sizeof(char));
        printf("[Network %d (%s)]\n\n", n, ladder_ctx.network[n].enable ? "enabled" : "disabled");

        for (uint32_t r = 0; r < ladder_ctx.ladder.quantity.net_rows; r++) {
            for (uint32_t c = 0; c < ladder_ctx.ladder.quantity.net_columns; c++) {
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

        for (uint32_t r = 0; r < ladder_ctx.ladder.quantity.net_rows; r++) {
            printf("    |");
            for (uint32_t c = 0; c < ladder_ctx.ladder.quantity.net_columns; c++) {
                if (network_str[r][c][0][0] == '\0')
                    printf("                   ");
                else
                    printf("%s", network_str[r][c][0]);
            }
            printf("|\n    |");
            for (uint32_t c = 0; c < ladder_ctx.ladder.quantity.net_columns; c++) {
                if (network_str[r][c][0][0] == '\0')
                    printf("                   ");
                else
                    printf("%s", network_str[r][c][1]);
            }
            printf("|\n");
        }

        printf("\n\n");
    }
}
