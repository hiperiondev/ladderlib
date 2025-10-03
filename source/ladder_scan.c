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
#include <stdlib.h>

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

// Array to flag instructions with side effects or required execution on false input (e.g., accumulation, resets, or power inversion).
// This makes the check exhaustive and easier to maintain/expand.
static const bool has_side_effects_on_false[LADDER_INS_INV] = { //
        false, // LADDER_INS_NOP
        false, // LADDER_INS_CONN
        true,  // LADDER_INS_NEG (inverts false to true, must execute for correct power flow)
        false, // LADDER_INS_NO
        false, // LADDER_INS_NC
        false, // LADDER_INS_RE
        false, // LADDER_INS_FE
        true,  // LADDER_INS_COIL (sets value=false on false)
        false, // LADDER_INS_COILL (retains on false, no side)
        false, // LADDER_INS_COILU (retains on false, no side)
        true,  // LADDER_INS_TON (resets acc/flags on false)
        true,  // LADDER_INS_TOF (accumulates on false)
        true,  // LADDER_INS_TP (may time on false depending on state)
        true,  // LADDER_INS_CTU (checks reset, sets flags on false)
        true,  // LADDER_INS_CTD (checks reset, sets flags on false)
        false, // LADDER_INS_MOVE (no action on false)
        false, // LADDER_INS_SUB (no action on false)
        false, // LADDER_INS_ADD (no action on false)
        false, // LADDER_INS_MUL (no action on false)
        false, // LADDER_INS_DIV (no action on false)
        false, // LADDER_INS_MOD (no action on false)
        false, // LADDER_INS_SHL (no action on false)
        false, // LADDER_INS_SHR (no action on false)
        false, // LADDER_INS_ROL (no action on false)
        false, // LADDER_INS_ROR (no action on false)
        false, // LADDER_INS_AND (no action on false)
        false, // LADDER_INS_OR (no action on false)
        false, // LADDER_INS_XOR (no action on false)
        false, // LADDER_INS_NOT (no action on false)
        false, // LADDER_INS_EQ (no action on false)
        false, // LADDER_INS_GT (no action on false)
        false, // LADDER_INS_GE (no action on false)
        false, // LADDER_INS_LT (no action on false)
        false, // LADDER_INS_LE (no action on false)
        false, // LADDER_INS_NE (no action on false)
        true,  // LADDER_INS_FOREIGN (unknown, assume sides)
        true   // LADDER_INS_TMOVE (potential sides)
        };

void ladder_scan(ladder_ctx_t *ladder_ctx) {
    uint32_t network = 0;

#ifdef OPTIONAL_CRON
    // Auto_reset clearing before evaluation to prevent race conditions where set flags are immediately cleared, ensuring triggers are not missed in fast scans.
    // clean auto reset cron registers
    if ((ladderlib_cron_t*) (*ladder_ctx).cron != NULL)
        for (uint32_t n = 0; n < ((ladderlib_cron_t*) (*ladder_ctx).cron)->used; n++)
            if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].enabled && ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].auto_reset)
                (*ladder_ctx).memory.M[((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].flag_reg] = false;

    // evaluate cron for actual time
    if (ladderlib_cron_eval(ladder_ctx) != LADDER_INS_ERR_OK) {
        (*ladder_ctx).ladder.state = LADDER_ST_INV;
        return;
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

        // Power rail tracking to enable short-circuiting of columns with no incoming power.
        // Initialize to true, simulating the energized left power rail.
        bool has_power = true;

        // Detect vertical groups per column, uniform input from top, OR outputs uniformly
        for (uint32_t column = 0; column < (*ladder_ctx).exec_network->cols; column++) {
            // Check if safe to skip before breaking. Scan column for instructions without side effects on false input.
            bool skip_safe = true;
            for (uint32_t r = 0; r < (*ladder_ctx).exec_network->rows; r++) {
                ladder_instruction_t code = (*ladder_ctx).exec_network->cells[r][column].code;
                if (code == LADDER_INS_MULTI) {
                    // Treat MULTI as potentially unsafe (part of multi-cell like timers/counters).
                    skip_safe = false;
                    break;
                }
                if (has_side_effects_on_false[code]) {
                    skip_safe = false;
                    break;
                }
            }

            // Short-circuit only if no power and column is safe to skip (no side effects on false).
            if (!has_power && skip_safe) {
                continue;
            }

            uint32_t row = 0;
            while (row < (*ladder_ctx).exec_network->rows) {
                uint32_t group_start = row;
                // Compute group input from top row's left (power rail or prev col top state)
                bool group_input = (column == 0) ? true : (*ladder_ctx).exec_network->cells[group_start][column - 1].state;

                // Tightened loop condition to prevent OOB access to cells[rows][column].
                // Checks group_end + 1 < rows before deref, ensuring group_end never >= rows.
                uint32_t group_end = group_start;
                while (group_end + 1 < (*ladder_ctx).exec_network->rows && (*ladder_ctx).exec_network->cells[group_end + 1][column].vertical_bar) {
                    group_end++;
                }

                // Execute instructions in group (using uniform group_input as left where needed)
                bool group_output = false;
                bool group_error = false;

                bool *original_lefts = NULL;
                uint32_t num_saved = 0;
                // Pre-compute group size and check against array limit before loop to prevent any potential overflow attempts.
                if (column > 0) {
                    uint32_t group_size = group_end - group_start + 1;
                    // Assertion for group_size <= rows to catch malformed ladder configurations in debug builds (tautological but enforces invariant).
                    if (group_size > (*ladder_ctx).exec_network->rows) {
                        (*ladder_ctx).ladder.last.err = LADDER_INS_ERR_FAIL;  // Treat allocation failure as generic error
                        goto cleanup;
                    }
                    original_lefts = (bool*) malloc(group_size * sizeof(bool));
                    if (original_lefts == NULL) {
                        group_error = true;
                        (*ladder_ctx).ladder.last.err = LADDER_INS_ERR_FAIL;  // Treat allocation failure as generic error
                        goto cleanup;
                    }
                    // Save originals for batch override
                    for (uint32_t gr = group_start; gr <= group_end; gr++) {
                        original_lefts[num_saved++] = (*ladder_ctx).exec_network->cells[gr][column - 1].state;
                        (*ladder_ctx).exec_network->cells[gr][column - 1].state = group_input;
                    }
                }

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
                        // Jump to cleanup on invalid code to restore left states immediately
                        goto cleanup;
                    }

                    // execute instruction
                    if (code != LADDER_INS_MULTI) {
                        (*ladder_ctx).ladder.last.err = ladder_function[code](ladder_ctx, column, current_row_for_exec);

                        if ((*ladder_ctx).ladder.last.err != LADDER_INS_ERR_OK) {
                            group_error = true;
                            // Jump to cleanup on execution error to ensure restoration before halting
                            goto cleanup;
                        }

                        if ((*ladder_ctx).on.instruction != NULL)
                            (*ladder_ctx).on.instruction(ladder_ctx);

                        // Collect individual output for OR
                        group_output |= (*(*ladder_ctx).exec_network).cells[current_row_for_exec][column].state;
                    }
                }

                cleanup:
                // Always restore left states after execution, regardless of error, to prevent corruption for next groups/columns
                // This ensures overrides are isolated to the current group, fixing cascading errors in parallel rungs
                if (column > 0) {
                    uint32_t restore_idx = 0;
                    for (uint32_t gr = group_start; gr <= group_end && restore_idx < num_saved; gr++) {
                        (*ladder_ctx).exec_network->cells[gr][column - 1].state = original_lefts[restore_idx++];
                    }
                }

                // Modified: Free the dynamically allocated array after use to prevent memory leaks.
                if (original_lefts != NULL) {
                    free(original_lefts);
                    original_lefts = NULL;
                }

                // On group error, propagate error
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

            if ((*ladder_ctx).on.scan_end != NULL)
                (*ladder_ctx).on.scan_end(ladder_ctx);

            has_power = false;
            for (uint32_t r = 0; r < (*ladder_ctx).exec_network->rows; ++r) {
                if ((*ladder_ctx).exec_network->cells[r][column].state) {
                    has_power = true;
                    break;
                }
            }
        }
    }
}
