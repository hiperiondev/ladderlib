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
#include <string.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

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
    ladder_ins_err_t ins_err;

    for (uint32_t network = 0; network < (*ladder_ctx).ladder.total_networks; network++) {
        (*ladder_ctx).exec_network = (*ladder_ctx).network[network];

        // resets dynamic flags before to start each network
        memset((*ladder_ctx).scan_internals.ladder_network_flags, 0, (NET_COLUMNS - 1) * sizeof(uint16_t));

        // call ladder instructions
        for (uint32_t column = 0; column < NET_COLUMNS; column++) {
            for (uint32_t row = 0; row < NET_ROWS; row++) {
#ifdef DEBUG
                printf("   -          Network: %d [r:%d, c:%d]\n", network, row, column);
                printf("   -    Code (%s): %u (%s)\n", (*ladder_ctx).exec_network.cells[row][column].code >= LADDER_INS_INV ? "masked" : "notmsk",
                        (*ladder_ctx).exec_network.cells[row][column].code & LADDER_INS_CELL_CODE_MASK,
                        ((*ladder_ctx).exec_network.cells[row][column].code & LADDER_INS_CELL_CODE_MASK) > 34 ?
                                "???" : fn_str[(*ladder_ctx).exec_network.cells[row][column].code & LADDER_INS_CELL_CODE_MASK]);
                printf("   -             Type: %s (%u)\n",
                        ladder_type_str[(
                                (*ladder_ctx).exec_network.cells[row][column].type >= LADDER_TYPE_INV ?
                                        LADDER_TYPE_INV : (*ladder_ctx).exec_network.cells[row][column].type)],
                        (*ladder_ctx).exec_network.cells[row][column].type);
                printf("   -             Data: %u\n", (*ladder_ctx).exec_network.cells[row][column].data);

                printf("-------------------------------------\n");
#endif

                (*ladder_ctx).ladder.last_instr = (*ladder_ctx).exec_network.cells[row][column].code;
                (*ladder_ctx).ladder.last_instr_err = ins_err;
                (*ladder_ctx).ladder.last_instr_network = network;
                (*ladder_ctx).ladder.last_instr_cell_row = row;
                (*ladder_ctx).ladder.last_instr_cell_column = column;

                // evaluation for an invalid code if the cell is not part of an instruction that uses more than one cell
                // plc to error state and serial log else, do not process, it was processed before
                if ((*ladder_ctx).exec_network.cells[row][column].code >= LADDER_INS_INV) {
                    if (((*ladder_ctx).exec_network.cells[row][column].code & (~LADDER_INS_MASK)) >= LADDER_INS_INV) {
#ifdef DEBUG
                        printf("TASK LADDER INSTRUCTION CODE INVALID: \n");
                        printf("   - Network: %d\n", network);
                        printf("   - Code: %u\n", (*ladder_ctx).exec_network.cells[row][column].code);
                        printf("   - Type: %u\n", (*ladder_ctx).exec_network.cells[row][column].type);
                        printf("   - Data: %u\n", (*ladder_ctx).exec_network.cells[row][column].data);
#endif
                        (*ladder_ctx).ladder.state = LADDER_ST_INV;
                        return;
                    }
                    (*ladder_ctx).exec_network.cells[row][column].code = 0;
                }

                // execute instruction
                if ((*ladder_ctx).exec_network.cells[row][column].code != 0) {
                    if (column == 0) {
                        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
                            ins_err = fn_ladder[(*ladder_ctx).exec_network.cells[row][column].code](ladder_ctx, column, row, 1);
                        } else {
                            ins_err = fn_ladder[(*ladder_ctx).exec_network.cells[row][column].code](ladder_ctx, column, row, 0);
                        }
                    } else {
                        ins_err = fn_ladder[(*ladder_ctx).exec_network.cells[row][column].code](ladder_ctx, column, row,
                                ((*ladder_ctx).scan_internals.ladder_network_flags[column - 1] & (*ladder_ctx).scan_internals.flags_mask[row]));
                    }

                    if (ins_err != LADDER_INS_ERR_OK) {
                        (*ladder_ctx).ladder.state = LADDER_ST_INV;
                        return;
                    }
                }
            }

            // update dynamic flags vs bars (not for last column)
            if ((column < NET_COLUMNS - 1) && ((*ladder_ctx).scan_internals.ladder_network_flags[column] != 0)) {
                for (uint32_t i = 0; i < NET_ROWS - 1; i++) {
                    (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                            | (((*ladder_ctx).scan_internals.ladder_network_flags[column] & (*ladder_ctx).exec_network.bars[column]) << 1);
                    (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                            | (((*ladder_ctx).scan_internals.ladder_network_flags[column] & ((*ladder_ctx).exec_network.bars[column] << 1)) >> 1);
                }
            }
        }

        // external function (ex. entry for edition or events)
        if ((*ladder_ctx).io.external_on_scan != NULL)
            (*ladder_ctx).io.external_on_scan(ladder_ctx);
    }
}
