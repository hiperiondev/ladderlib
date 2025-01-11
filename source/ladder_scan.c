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

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

#ifdef DEBUG
static const char *fn_str[] = {
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
#endif

typedef ladder_ins_err_t (*ladder_logic)(ladder_ctx_t *ladder_ctx, int c, int r, int f);
static ladder_logic const fn_ladder[] = {
        execNop,   //
        execConn,  //
        execNeg,   //
        execNO,    //
        execNC,    //
        execRE,    //
        execFE,    //
        execCoil,  //
        execCoilL, //
        execCoilU, //
        execTON,   //
        execTOFF,  //
        execTP,    //
        execCTU,   //
        execCTD,   //
        execMOVE,  //
        execSUB,   //
        execADD,   //
        execMUL,   //
        execDIV,   //
        execMOD,   //
        execSHL,   //
        execSHR,   //
        execROL,   //
        execROR,   //
        execAND,   //
        execOR,    //
        execXOR,   //
        execNOT,   //
        execEQ,    //
        execGT,    //
        execGE,    //
        execLT,    //
        execLE,    //
        execNE     //
        };

void ladder_scan(ladder_ctx_t *ladder_ctx) {
    for (uint32_t n = 0; n < (*ladder_ctx).ladder.networks_quantity; n++) {
        (*ladder_ctx).exec_network = (*ladder_ctx).network[n];

        // resets dynamic flags before to start each network
        for (uint32_t f = 0; f < NET_COLUMNS - 1; f++) {
            (*ladder_ctx).internals.ladder_network_flags[f] = 0;
        }

        // call ladder instructions
        for (uint32_t c = 0; c < NET_COLUMNS; c++) {
            for (uint32_t r = 0; r < NET_ROWS; r++) {
#ifdef DEBUG
                printf("   - Network: %d [%d, %d]\n", n, r, c);
                printf("   - Code: %u (%s)\n", (*ladder_ctx).exec_network.cells[r][c].code,
                        (*ladder_ctx).exec_network.cells[r][c].code > 34 ? "???" : fn_str[(*ladder_ctx).exec_network.cells[r][c].code]);
                printf("   - Code (mask): %u (%s)\n", (*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK,
                        ((*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK) > 34 ?
                                "???" : fn_str[(*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK]);
                printf("   - Data: %u\n", (*ladder_ctx).exec_network.cells[r][c].data);
                printf("   - Type: %u\n", (*ladder_ctx).exec_network.cells[r][c].type);
                printf("-------------------------------------\n");
#endif
                // evaluation for an invalid code if the cell is not part of an instruction that uses more than one cell
                // plc to error state and serial log else, do not process, it was processed before
                if ((*ladder_ctx).exec_network.cells[r][c].code >= LADDER_INS_INV) {
                    if (((*ladder_ctx).exec_network.cells[r][c].code & LADDER_INS_CELL_CODE_MASK) >= LADDER_INS_INV) {
                        printf("TASK LADDER INSTRUCTION CODE INVALID: \n");
                        printf("   - Network: %d\n", n);
                        printf("   - Code: %u\n", (*ladder_ctx).exec_network.cells[r][c].code);
                        printf("   - Data: %u\n", (*ladder_ctx).exec_network.cells[r][c].data);
                        printf("   - Type: %u\n", (*ladder_ctx).exec_network.cells[r][c].type);
                        (*ladder_ctx).ladder.state = LADDER_ST_INV;
                    }
                    (*ladder_ctx).exec_network.cells[r][c].code = 0;
                }

                // execute instruction
                if ((*ladder_ctx).exec_network.cells[r][c].code != 0) {
                    if (c == 0) {
                        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
                            fn_ladder[(*ladder_ctx).exec_network.cells[r][c].code](ladder_ctx, c, r, 1);
                        } else {
                            fn_ladder[(*ladder_ctx).exec_network.cells[r][c].code](ladder_ctx, c, r, 0);
                        }
                    } else {
                        fn_ladder[(*ladder_ctx).exec_network.cells[r][c].code](ladder_ctx, c, r,
                                ((*ladder_ctx).internals.ladder_network_flags[c - 1] & (*ladder_ctx).internals.flags_mask[r]));
                    }
                }
            }

            // update dynamic flags vs bars (not for last column)
            if ((c < NET_COLUMNS - 1) && ((*ladder_ctx).internals.ladder_network_flags[c] != 0)) {
                for (uint32_t i = 0; i < NET_ROWS - 1; i++) {
                    (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c]
                            | (((*ladder_ctx).internals.ladder_network_flags[c] & (*ladder_ctx).exec_network.bars[c]) << 1);
                    (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c]
                            | (((*ladder_ctx).internals.ladder_network_flags[c] & ((*ladder_ctx).exec_network.bars[c] << 1)) >> 1);
                }
            }
        }

        // external function (ex. entry for edition)
        if ((*ladder_ctx).io.external_on_scan != NULL && (*ladder_ctx).io.external_on_scan(ladder_ctx)) {
            for (uint32_t ff = 0; ff < NET_COLUMNS - 1; ff++) {
                (*ladder_ctx).internals.ladder_network_flags_online[ff] = 0;
            }
        }
    }
}
