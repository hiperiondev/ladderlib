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

#include <stdint.h>

#include "ladder.h"
#include "ladder_instructions.h"
#include "ladder_internals.h"

static ladder_fn_t const ladder_function[] = { //
        fn_NOP,      // 00
        fn_CONN,     // 01
        fn_NEG,      // 02
        fn_NO,       // 03
        fn_NC,       // 04
        fn_RE,       // 05
        fn_FE,       // 06
        fn_COIL,     // 07
        fn_COILL,    // 08
        fn_COILU,    // 09
        fn_TON,      // 10
        fn_TOFF,     // 11
        fn_TP,       // 12
        fn_CTU,      // 13
        fn_CTD,      // 14
        fn_MOVE,     // 15
        fn_SUB,      // 16
        fn_ADD,      // 17
        fn_MUL,      // 18
        fn_DIV,      // 19
        fn_MOD,      // 20
        fn_SHL,      // 21
        fn_SHR,      // 22
        fn_ROL,      // 23
        fn_ROR,      // 24
        fn_AND,      // 25
        fn_OR,       // 26
        fn_XOR,      // 27
        fn_NOT,      // 28
        fn_EQ,       // 29
        fn_GT,       // 30
        fn_GE,       // 31
        fn_LT,       // 32
        fn_LE,       // 33
        fn_NE,       // 34
        fn_FOREIGN,  // 35
        fn_TMOVE,    // 36
        };

void ladder_scan(ladder_ctx_t *ladder_ctx) {
    ladder_ins_err_t ins_err = 0;

    for (uint32_t network = 0; network < (*ladder_ctx).ladder.quantity.networks; network++) {
        if (!(*ladder_ctx).network[network].enable)
            continue;

        (*ladder_ctx).exec_network = &((*ladder_ctx).network[network]);

        // resets dynamic flags before to start each network
        for (uint32_t nfl = 0; nfl < (*ladder_ctx).ladder.quantity.net_columns - 1; nfl++)
            (*ladder_ctx).scan_internals.network_flags[nfl] = 0;

        // call ladder instructions
        for (uint32_t column = 0; column < (*ladder_ctx).ladder.quantity.net_columns; column++) {
            for (uint32_t row = 0; row < (*ladder_ctx).ladder.quantity.net_rows; row++) {

                // save this execution
                (*ladder_ctx).ladder.last.instr = (*(*ladder_ctx).exec_network).cells[row][column].code;
                (*ladder_ctx).ladder.last.err = ins_err;
                (*ladder_ctx).ladder.last.network = network;
                (*ladder_ctx).ladder.last.cell_row = row;
                (*ladder_ctx).ladder.last.cell_column = column;

                // evaluation for an invalid code if the cell is not part of an instruction that uses more than one cell
                if ((*(*ladder_ctx).exec_network).cells[row][column].code >= LADDER_INS_INV
                        && (*(*ladder_ctx).exec_network).cells[row][column].code != LADDER_INS_MULTI) {
                    (*ladder_ctx).ladder.state = LADDER_ST_INV;
                    return;
                }

                // execute instruction
                if ((*(*ladder_ctx).exec_network).cells[row][column].code != LADDER_INS_MULTI) {
                    ins_err = ladder_function[(*(*ladder_ctx).exec_network).cells[row][column].code](ladder_ctx, column, row,
                            column == 0 ? true : ((*ladder_ctx).scan_internals.network_flags[column - 1] & LADDER_FLAG_MASK(row)));

                    if (ins_err != LADDER_INS_ERR_OK) {
                        (*ladder_ctx).ladder.state = LADDER_ST_INV;
                        return;
                    }
                }

                if((*ladder_ctx).on.instruction != NULL)
                    (*ladder_ctx).on.instruction(ladder_ctx);
            }

            // update dynamic flags vs bars (not for last column)
            if ((column < (*ladder_ctx).ladder.quantity.net_columns - 1) && ((*ladder_ctx).scan_internals.network_flags[column] != 0)) {
                for (uint32_t i = 0; i < (*ladder_ctx).ladder.quantity.net_rows - 1; i++) {
                    (*ladder_ctx).scan_internals.network_flags[column] = (*ladder_ctx).scan_internals.network_flags[column]
                            | (((*ladder_ctx).scan_internals.network_flags[column] & (*(*ladder_ctx).exec_network).bars[column]) << 1);
                    (*ladder_ctx).scan_internals.network_flags[column] = (*ladder_ctx).scan_internals.network_flags[column]
                            | (((*ladder_ctx).scan_internals.network_flags[column] & ((*(*ladder_ctx).exec_network).bars[column] << 1)) >> 1);
                }
            }
        }

        if ((*ladder_ctx).on.scan_end != NULL)
            (*ladder_ctx).on.scan_end(ladder_ctx);
    }
}
