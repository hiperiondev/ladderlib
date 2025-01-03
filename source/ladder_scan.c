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
#include "ladder_logic_instructions.h"

void ladder_scan_plc(plc_ctx_t *plc_ctx, ladder_network_t networks[]) {
    typedef void (*ladder_logic)(plc_ctx_t *plc_ctx, int c, int r, int f);
    
    ladder_logic fn_ladder[] = {
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

    for (int n = 0; n < (*plc_ctx).ladder.networks_quantity; n++) {
        (*plc_ctx).exec_network = networks[n];

        // resets dynamic flags before to start each network
        for (int f = 0; f < NET_COLUMNS - 1; f++) {
            (*plc_ctx).internals.ladder_network_flags[f] = 0;
        }

        // call ladder instructions
        for (int c = 0; c < NET_COLUMNS; c++) {
            for (int r = 0; r < NET_ROWS; r++) {

                // evaluation for an invalid code if the cell is not part of an instruction that uses more than one cell
                // plc to error state and serial log else, do not process, it was processed before
                if ((*plc_ctx).exec_network.cells[r][c].code >= PLC_INS_INV) {
                    if (((*plc_ctx).exec_network.cells[r][c].code & PLC_INS_CELL_CODE_MASK) >= PLC_INS_INV) {
                        printf("TASK LADDER - CORE 1 - INSTRUCTION CODE INVALID: \n");
                        printf("   - Network: ");
                        printf("%d\n", n);
                        printf("   - Code: ");
                        printf("%u\n", (*plc_ctx).exec_network.cells[r][c].code);
                        printf("   - Data: ");
                        printf("%u\n", (*plc_ctx).exec_network.cells[r][c].data);
                        printf("   - Type: ");
                        printf("%u\n", (*plc_ctx).exec_network.cells[r][c].type);
                        (*plc_ctx).ladder.plc_state = PLC_ST_INV;
                        //saveSettings();
                    }
                    (*plc_ctx).exec_network.cells[r][c].code = 0;
                }

                // execute instruction
                if ((*plc_ctx).exec_network.cells[r][c].code != 0) {
                    if (c == 0) {
                        if ((*plc_ctx).ladder.plc_state == PLC_ST_RUNNING) {
                            fn_ladder[(*plc_ctx).exec_network.cells[r][c].code](plc_ctx, c, r, 1);
                        } else {
                            fn_ladder[(*plc_ctx).exec_network.cells[r][c].code](plc_ctx, c, r, 0);
                        }
                    } else {
                        fn_ladder[(*plc_ctx).exec_network.cells[r][c].code](plc_ctx, c, r,
                                ((*plc_ctx).internals.ladder_network_flags[c - 1] & (*plc_ctx).internals.flags_mask[r]));
                    }
                }
            }

            // update dynamic flags vs bars (not for last column)
            if ((c < NET_COLUMNS - 1) && ((*plc_ctx).internals.ladder_network_flags[c] != 0)) {
                for (int i = 0; i < NET_ROWS - 1; i++) {
                    (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c]
                            | (((*plc_ctx).internals.ladder_network_flags[c] & (*plc_ctx).exec_network.bars[c]) << 1);
                    (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c]
                            | (((*plc_ctx).internals.ladder_network_flags[c] & ((*plc_ctx).exec_network.bars[c] << 1)) >> 1);
                }
            }
        }

        // entry for edition
        if ((*plc_ctx).io.fn.external_on_scan(plc_ctx)) {
            for (int ff = 0; ff < NET_COLUMNS - 1; ff++) {
                (*plc_ctx).internals.ladder_network_flags_online[ff] = 0;
            }
        }
    }
}
