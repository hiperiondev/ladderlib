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
#include <stdbool.h>

#include "ladder.h"
#include "ladder_instructions.h"
#include "ladder_internals.h"
#ifdef OPTIONAL_CRON
#include "ladderlib_cron.h"
#endif

static ladder_fn_t const ladder_function[] = { //
        fn_NOP,     // 00
        fn_CONN,    // 01
        fn_NEG,     // 02
        fn_NO,      // 03
        fn_NC,      // 04
        fn_RE,      // 05
        fn_FE,      // 06
        fn_COIL,    // 07
        fn_COILL,   // 08
        fn_COILU,   // 09
        fn_TON,     // 10
        fn_TOF,     // 11
        fn_TP,      // 12
        fn_CTU,     // 13
        fn_CTD,     // 14
        fn_MOVE,    // 15
        fn_SUB,     // 16
        fn_ADD,     // 17
        fn_MUL,     // 18
        fn_DIV,     // 19
        fn_MOD,     // 20
        fn_SHL,     // 21
        fn_SHR,     // 22
        fn_ROL,     // 23
        fn_ROR,     // 24
        fn_AND,     // 25
        fn_OR,      // 26
        fn_XOR,     // 27
        fn_NOT,     // 28
        fn_EQ,      // 29
        fn_GT,      // 30
        fn_GE,      // 31
        fn_LT,      // 32
        fn_LE,      // 33
        fn_NE,      // 34
        fn_FOREIGN, // 35
        fn_TMOVE,   // 36
        };

void ladder_scan(ladder_ctx_t *ladder_ctx) {
    uint32_t network = 0;

#ifdef OPTIONAL_CRON
    // evaluate cron for actual time
    if (ladderlib_cron_eval(ladder_ctx) != LADDER_INS_ERR_OK) {
        (*ladder_ctx).ladder.state = LADDER_ST_INV;
        goto exit;
    }
#endif

    for (network = 0; network < (*ladder_ctx).ladder.quantity.networks; network++) {
        if (!(*ladder_ctx).network[network].enable)
            continue;

        (*ladder_ctx).exec_network = &((*ladder_ctx).network[network]);

        // Clear all cell states to ensure fresh evaluation each scan cycle
        // This prevents retention of states from previous scans, which could lead to incorrect power flow
        for (uint32_t column = 0; column < (*ladder_ctx).exec_network->cols; column++) {
            for (uint32_t row = 0; row < (*ladder_ctx).exec_network->rows; row++) {
                (*ladder_ctx).exec_network->cells[row][column].state = false;
            }
        }

        // Detect vertical groups per column, uniform input from top, OR outputs uniformly
        for (uint32_t column = 0; column < (*ladder_ctx).exec_network->cols; column++) {
            uint32_t row = 0;
            while (row < (*ladder_ctx).exec_network->rows) {
                uint32_t group_start = row;
                // Compute group input from top row's left (power rail or prev col top state)
                bool group_input = (column == 0) ? true : (*ladder_ctx).exec_network->cells[group_start][column - 1].state;

                // Find group end: consecutive rows with vertical_bar=true after start
                uint32_t group_end = group_start;
                while (group_end < (*ladder_ctx).exec_network->rows - 1 && (*ladder_ctx).exec_network->cells[group_end + 1][column].vertical_bar) {
                    group_end++;
                }

                // Execute instructions in group (using uniform group_input as left where needed)
                bool group_output = false;
                bool group_error = false;
                for (uint32_t gr = group_start; gr <= group_end; gr++) {
                    // Set current row for last instr tracking
                    uint32_t current_row_for_exec = gr;

                    // save this execution
                    (*ladder_ctx).ladder.last.instr = (*(*ladder_ctx).exec_network).cells[current_row_for_exec][column].code;
                    (*ladder_ctx).ladder.last.err = LADDER_INS_ERR_OK;
                    (*ladder_ctx).ladder.last.network = network;
                    (*ladder_ctx).ladder.last.cell_row = current_row_for_exec;
                    (*ladder_ctx).ladder.last.cell_column = column;

                    ladder_instruction_t code = (*(*ladder_ctx).exec_network).cells[current_row_for_exec][column].code;

                    // evaluation for an invalid code if the cell is not part of an instruction that uses more than one cell
                    if (code >= LADDER_INS_INV && code != LADDER_INS_MULTI) {
                        (*ladder_ctx).ladder.state = LADDER_ST_INV;
                        (*ladder_ctx).ladder.last.err = LADDER_INS_ERR_FAIL;
                        group_error = true;
                        break; // Break group exec on error
                    }

                    // execute instruction
                    if (code != LADDER_INS_MULTI) {
                        // Tight per-row override only for executing rows and column > 0.
                        // This ensures uniform group_input during exec without group-wide mutation.
                        // For top row (gr == group_start), group_input matches original, so no effective change.
                        // For parallel lowers: sets to group_input temporarily.
                        // For multi-row lowers: skipped, no override.
                        if (column > 0) {
                            bool original_left = (*ladder_ctx).exec_network->cells[gr][column - 1].state;
                            (*ladder_ctx).exec_network->cells[gr][column - 1].state = group_input;
                            (*ladder_ctx).ladder.last.err = ladder_function[code](ladder_ctx, column, current_row_for_exec);
                            (*ladder_ctx).exec_network->cells[gr][column - 1].state = original_left;
                        } else {
                            (*ladder_ctx).ladder.last.err = ladder_function[code](ladder_ctx, column, current_row_for_exec);
                        }

                        if ((*ladder_ctx).ladder.last.err != LADDER_INS_ERR_OK) {
                            group_error = true;
                            break; // Break on exec error
                        }

                        if ((*ladder_ctx).on.instruction != NULL)
                            (*ladder_ctx).on.instruction(ladder_ctx);

                        // Collect individual output for OR
                        group_output |= (*(*ladder_ctx).exec_network).cells[current_row_for_exec][column].state;
                    }
                }

                // On group error, propagate error (no restore needed, as overrides are now per-exec)
                if (group_error) {
                    (*ladder_ctx).ladder.state = LADDER_ST_INV;
                    return;
                }

                // Set uniform group output to all rows in group (ORed flow to right)
                for (uint32_t gr = group_start; gr <= group_end; gr++) {
                    (*(*ladder_ctx).exec_network).cells[gr][column].state = group_output;
                }

                // Advance to next group
                row = group_end + 1;
            }
        }

#ifdef OPTIONAL_CRON
        exit:
        // clean auto reset cron registers
        if ((ladderlib_cron_t*) (*ladder_ctx).cron != NULL)
            for (uint32_t n = 0; n < ((ladderlib_cron_t*) (*ladder_ctx).cron)->used; n++)
                if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].enabled && ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].auto_reset)
                    (*ladder_ctx).memory.M[((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].flag_reg] = false;
#endif

        if ((*ladder_ctx).on.scan_end != NULL)
            (*ladder_ctx).on.scan_end(ladder_ctx);
    }
}
