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

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

static uint32_t basetime_factor[] = { 1, 10, 100, 1000, 60000 };

const ladder_instructions_iocd_t ladder_fn_iocd[] = { { 1, 1, 1, 0 }, // NOP
        { 1, 1, 1, 0 }, // CONN
        { 1, 1, 1, 1 }, // NEG
        { 1, 1, 1, 1 }, // NO
        { 1, 1, 1, 1 }, // NC
        { 1, 1, 1, 1 }, // RE
        { 1, 1, 1, 1 }, // FE
        { 1, 1, 1, 1 }, // COIL
        { 1, 1, 1, 1 }, // COILL
        { 1, 1, 1, 1 }, // COILU
        { 1, 2, 2, 2 }, // TON
        { 1, 2, 2, 2 }, // TOF
        { 1, 2, 2, 2 }, // TP
        { 2, 2, 2, 2 }, // CTU
        { 2, 2, 2, 2 }, // CTD
        { 1, 1, 2, 2 }, // MOVE
        { 1, 3, 3, 3 }, // SUB
        { 1, 1, 3, 3 }, // ADD
        { 1, 1, 3, 3 }, // MUL
        { 1, 2, 3, 3 }, // DIV
        { 1, 1, 3, 3 }, // MOD
        { 1, 1, 2, 2 }, // SHL
        { 1, 1, 2, 2 }, // SHR
        { 1, 1, 2, 2 }, // ROL
        { 1, 1, 2, 2 }, // ROR
        { 1, 1, 3, 3 }, // AND
        { 1, 1, 3, 3 }, // OR
        { 1, 1, 3, 3 }, // XOR
        { 1, 1, 2, 2 }, // NOT
        { 1, 1, 2, 2 }, // EQ
        { 1, 1, 2, 2 }, // GT
        { 1, 1, 2, 2 }, // GE
        { 1, 1, 2, 2 }, // LT
        { 1, 1, 2, 2 }, // LE
        { 1, 1, 2, 2 }, // NE
        { 0, 0, 0, 0 }, // FOREIGN
        { 1, 1, 1, 5 }, // TMOVE
        { 1, 1, 1, 0 }, // INV
        };

static int32_t ladder_get_table_i32(ladder_ctx_t *ladder_ctx, uint32_t table_net, uint32_t flat_pos, uint8_t *err) {
    *err = LADDER_INS_ERR_OK;
    if (table_net >= ladder_ctx->ladder.quantity.networks) {
        *err = LADDER_INS_ERR_NOTABLE;
        return 0;
    }
    ladder_network_t *net = &ladder_ctx->network[table_net];

    if (flat_pos >= net->rows * net->cols) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return 0;
    }
    uint32_t r = flat_pos / net->cols;
    uint32_t c = flat_pos % net->cols;
    if (r >= net->rows || c >= net->cols || net->cells[r][c].data_qty < 1) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return 0;
    }
    // Check type for i32 compatibility (D/IW/QW as proxy)
    if (net->cells[r][c].data[0].type != LADDER_REGISTER_D && net->cells[r][c].data[0].type != LADDER_REGISTER_IW
            && net->cells[r][c].data[0].type != LADDER_REGISTER_QW) {
        *err = LADDER_INS_ERR_TYPEMISMATCH;
        return 0;
    }
    return (int32_t) net->cells[r][c].data[0].value.i32;
}

static void ladder_set_table_i32(ladder_ctx_t *ladder_ctx, uint32_t table_net, uint32_t flat_pos, int32_t val, uint8_t *err) {
    *err = LADDER_INS_ERR_OK;
    if (table_net >= ladder_ctx->ladder.quantity.networks) {
        *err = LADDER_INS_ERR_NOTABLE;
        return;
    }
    ladder_network_t *net = &ladder_ctx->network[table_net];

    if (flat_pos >= net->rows * net->cols) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return;
    }
    uint32_t r = flat_pos / net->cols;
    uint32_t c = flat_pos % net->cols;
    if (r >= net->rows || c >= net->cols || net->cells[r][c].data_qty < 1) {
        *err = LADDER_INS_ERR_OUTOFRANGE;
        return;
    }
    // Check dest type for i32 compatibility
    if (net->cells[r][c].data[0].type != LADDER_REGISTER_D && net->cells[r][c].data[0].type != LADDER_REGISTER_IW
            && net->cells[r][c].data[0].type != LADDER_REGISTER_QW) {
        *err = LADDER_INS_ERR_TYPEMISMATCH;
        return;
    }
    net->cells[r][c].data[0].value.i32 = val;
}

ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_CONN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_NEG(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) = !CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) = ladder_get_data_value(ladder_ctx, row, column, 0) && CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) = (!ladder_get_data_value(ladder_ctx, row, column, 0)) && CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    MAKE_BOOL(ladder_get_data_value(ladder_ctx, row, column, 0) &&
            !ladder_get_previous_value(ladder_ctx, row, column, 0)) && CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    MAKE_BOOL(
            !ladder_get_data_value(ladder_ctx, row, column, 0)
            && ladder_get_previous_value(ladder_ctx, row, column, 0)) && CELL_STATE_LEFT(ladder_ctx, column, row);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_COIL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    uint32_t val = 0;
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE(ladder_ctx, column, row))
        val = 1;

    ladder_set_data_value(ladder_ctx, row, column, 0, (void*) &val, &error);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_COILL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    bool state = CELL_STATE_LEFT(ladder_ctx, column, row);
    bool prev = ladder_get_previous_value(ladder_ctx, row, column, 0); // From history (Mh/Qh)
    uint32_t val = (prev || state) ? 1 : 0;

    CELL_STATE(ladder_ctx, column, row) = (bool) val; // Power flow = latched output

    ladder_set_data_value(ladder_ctx, row, column, 0, (void*) &val, &error);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_COILU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    bool state = CELL_STATE_LEFT(ladder_ctx, column, row);
    bool prev = ladder_get_previous_value(ladder_ctx, row, column, 0); // From history (Mh/Qh)
    uint32_t val = (prev && !state) ? 1 : 0;

    CELL_STATE(ladder_ctx, column, row) = (bool) val; // Power flow = latched output

    ladder_set_data_value(ladder_ctx, row, column, 0, (void*) &val, &error);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // timer is not active --> reset
    if (!CELL_STATE_LEFT(ladder_ctx, column, row)) {
        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = 0;

        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
        (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
    }

    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (CELL_STATE_LEFT(ladder_ctx, column, row) && !(*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && !(*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp = (*ladder_ctx).hw.time.millis();
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                / basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]);
    }

    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                    >= ((*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32
                            * basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]))) {
        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;

        (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc =
                (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
    }

    CELL_STATE(ladder_ctx, column, row + 1) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    // Set the state to Td every scan to persist across clears
    CELL_STATE(ladder_ctx, column, row) = (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_TOF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // input active --> reset
    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = 0;

        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
        (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
    } else {
        // timer is activated on falling edge, set timer running flag and snapshot the timestamp
        if (!(*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
                && !(*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
            (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

            (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp = (*ladder_ctx).hw.time.millis();
        }

        // timer is running, update acc value
        if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
            (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                    - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                    / basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]);
        }

        // timer done --> activate timer done flag and set acc value to his set point
        if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
                && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                        >= ((*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32
                                * basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]))) {
            (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;

            (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

            (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc =
                    (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
        }
    }

    CELL_STATE(ladder_ctx, column, row + 1) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    // Set the state to input || Tr every scan to persist across clears
    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row)
            || (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // detect rising edge to start pulse
    if (CELL_STATE_LEFT(ladder_ctx, column, row) && !ladder_get_previous_value(ladder_ctx, row, column, -1)
            && !(*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp = (*ladder_ctx).hw.time.millis();
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                / basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]);
    }

    // timer done --> deactivate
    if ((*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].time_stamp)
                    >= ((*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32
                            * basetime_factor[(*(*ladder_ctx).exec_network).cells[row][column].data[1].type]))) {
        (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;

        (*ladder_ctx).memory.Td[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;

        (*ladder_ctx).timers[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32].acc =
                (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
    }

    CELL_STATE(ladder_ctx, column, row + 1) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    // Set the state to Tr every scan to persist across clears
    CELL_STATE(ladder_ctx, column, row) = (*ladder_ctx).memory.Tr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32];

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = 0;
            (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            CELL_STATE(ladder_ctx, column, row) = false;
            CELL_STATE(ladder_ctx, column, row + 1) = false;
        }
    } else {
        if (CELL_STATE_LEFT(ladder_ctx, column, row + 1)) {
            (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = 0;
            (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            CELL_STATE(ladder_ctx, column, row) = false;
            CELL_STATE(ladder_ctx, column, row + 1) = false;
        }
    }

    // counter is activated in this scan, change count
    if (CELL_STATE_LEFT(ladder_ctx, column, row) && !(*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && !(*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;
        CELL_STATE(ladder_ctx, column, row + 1) = true;
        (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]++;
    }

    // reset counter edge detection
    if (!CELL_STATE_LEFT(ladder_ctx, column, row)) {
        (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
        CELL_STATE(ladder_ctx, column, row + 1) = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            >= (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32) {
        (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;
        CELL_STATE(ladder_ctx, column, row) = true;
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] =
                    (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
            (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            CELL_STATE(ladder_ctx, column, row) = false;
            CELL_STATE(ladder_ctx, column, row + 1) = false;
        }
    } else {
        if (CELL_STATE_LEFT(ladder_ctx, column, row + 1)) {
            (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] =
                    (*(*ladder_ctx).exec_network).cells[row][column].data[1].value.i32;
            (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
            CELL_STATE(ladder_ctx, column, row) = false;
            CELL_STATE(ladder_ctx, column, row + 1) = false;
        }
    }

    // counter is activated in this scan, change count
    if (CELL_STATE_LEFT(ladder_ctx, column, row) && !(*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]
            && !(*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]) {
        (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;
        CELL_STATE(ladder_ctx, column, row + 1) = true;
        (*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32]--;
    }

    // reset counter edge detection
    if (!CELL_STATE_LEFT(ladder_ctx, column, row)) {
        (*ladder_ctx).memory.Cr[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = false;
        CELL_STATE(ladder_ctx, column, row + 1) = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] == 0) {
        (*ladder_ctx).memory.Cd[ladder_cell_data_exec(ladder_ctx, row, column, 0).value.i32] = true;
        CELL_STATE(ladder_ctx, column, row) = true;
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = ladder_get_data_value(ladder_ctx, row, column, 0);
        ladder_set_data_value(ladder_ctx, row, column, 1, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);
    int32_t auxValue1 = ladder_get_data_value(ladder_ctx, row, column, 0);
    int32_t auxValue2 = ladder_get_data_value(ladder_ctx, row, column, 1);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        if (auxValue1 > auxValue2) {
            CELL_STATE(ladder_ctx, column, row) = true;
        } else if (auxValue1 == auxValue2) {
            CELL_STATE(ladder_ctx, column, row + 1) = true;
        } else {
            CELL_STATE(ladder_ctx, column, row + 2) = true;
        }

        uint32_t res = auxValue1 - auxValue2;
        ladder_set_data_value(ladder_ctx, row, 2, column, (void*) &res, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        int32_t val = ladder_get_data_value(ladder_ctx, row, column, 0) + ladder_get_data_value(ladder_ctx, row, column, 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        int32_t val = ladder_get_data_value(ladder_ctx, row, column, 0) * ladder_get_data_value(ladder_ctx, row, column, 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);
    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        if (ladder_get_data_value(ladder_ctx, row +1, column, 0) == 0) {
            ladder_set_data_value(ladder_ctx, row, 2, column, 0, &error);
        } else {
            int32_t val = ladder_get_data_value(ladder_ctx, row, column, 0) / ladder_get_data_value(ladder_ctx, row, column, 1);
            ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        int32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) % to_integer(ladder_get_data_value(ladder_ctx, row, column, 1), 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) << 1;
        ladder_set_data_value(ladder_ctx, row, column, 1, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) >> 1;
        ladder_set_data_value(ladder_ctx, row, column, 1, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(ladder_ctx, row, column, 0);
        auxCarryBit = auxValue & 0x8000;
        auxValue = auxValue << 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x0001;
        }
        ladder_set_data_value(ladder_ctx, row, column, 1, (void*) &auxValue, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(ladder_ctx, row, column, 0);
        auxCarryBit = auxValue & 0x0001;
        auxValue = auxValue >> 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x8000;
        }
        ladder_set_data_value(ladder_ctx, row, 1, column, (void*) &auxValue, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) & to_integer(ladder_get_data_value(ladder_ctx, row, column, 1), 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) | to_integer(ladder_get_data_value(ladder_ctx, row, column, 1), 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1) ^ to_integer(ladder_get_data_value(ladder_ctx, row, column, 1), 1);
        ladder_set_data_value(ladder_ctx, row, column, 2, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    static uint8_t error = 0;

    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);

    if (CELL_STATE_LEFT(ladder_ctx, column, row)) {
        uint32_t val = ~to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 1);
        ladder_set_data_value(ladder_ctx, row, column, 1, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) == ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) > ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) >= ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) < ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) <= ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    CELL_STATE(ladder_ctx, column, row) =
    CELL_STATE_LEFT(ladder_ctx, column, row) ? (ladder_get_data_value(ladder_ctx, row, column, 0) != ladder_get_data_value(ladder_ctx, row, column, 1)) : false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    if (to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 0) >= (*ladder_ctx).foreign.qty)
        return LADDER_INS_ERR_NOFOREIGN;

    return (*ladder_ctx).foreign.fn[to_integer(ladder_get_data_value(ladder_ctx, row, column, 0), 0)].exec(ladder_ctx, column, row);
}

ladder_ins_err_t fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row) {
    // MAX_MOVE=1024 (safe for 32x255=8160, but cap low)
#define MAX_MOVE 1024
    int32_t dest_net = ladder_get_data_value(ladder_ctx, row, column, 0);
    int32_t dest_pos = ladder_get_data_value(ladder_ctx, row, column, 1);
    int32_t src_net = ladder_get_data_value(ladder_ctx, row, column, 2);
    int32_t src_pos = ladder_get_data_value(ladder_ctx, row, column, 3);
    int32_t qty_raw = ladder_get_data_value(ladder_ctx, row, column, 4);
    uint32_t qty = (qty_raw < 0 || qty_raw > MAX_MOVE) ? 0 : (uint32_t) qty_raw; // Handle negative as 0, cap

    if (qty == 0) {
        CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row); // Success for zero
        return LADDER_INS_ERR_OK;
    }

    // Check table enables (disabled for data tables)
    if (ladder_ctx->network[dest_net].enable || ladder_ctx->network[src_net].enable) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return LADDER_INS_ERR_NOTABLE;
    }

    // Bounds: Ensure full range fits
    uint32_t dest_size = ladder_ctx->network[dest_net].rows * ladder_ctx->network[dest_net].cols;
    uint32_t src_size = ladder_ctx->network[src_net].rows * ladder_ctx->network[src_net].cols;
    if (dest_pos + qty > dest_size || src_pos + qty > src_size) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return LADDER_INS_ERR_OUTOFRANGE;
    }

    // Atomic via temp buffer (all read or err, all write or none)
    int32_t temp[MAX_MOVE];
    uint8_t read_err = LADDER_INS_ERR_OK;
    for (uint32_t k = 0; k < qty; ++k) {
        temp[k] = ladder_get_table_i32(ladder_ctx, src_net, src_pos + k, &read_err);
        if (read_err != LADDER_INS_ERR_OK) {
            break;
        }
    }
    if (read_err != LADDER_INS_ERR_OK) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return read_err;
    }

    uint8_t write_err = LADDER_INS_ERR_OK;
    for (uint32_t k = 0; k < qty; ++k) {
        ladder_set_table_i32(ladder_ctx, dest_net, dest_pos + k, temp[k], &write_err);
        if (write_err != LADDER_INS_ERR_OK) {
            break;
        }
    }
    if (write_err != LADDER_INS_ERR_OK) {
        CELL_STATE(ladder_ctx, column, row) = false;
        return write_err;
    }

    // Success: Propagate power
    CELL_STATE(ladder_ctx, column, row) = CELL_STATE_LEFT(ladder_ctx, column, row);
    return LADDER_INS_ERR_OK;
}

void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, uint32_t pos, void *value, uint8_t *error) {
    *error = LADDER_INS_ERR_OK;

    ladder_register_t __type = ladder_cell_data_exec(ladder_ctx, row, column, pos).type;
    uint32_t __idx = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.i32; /* For registers */
    uint8_t __port = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.mp.port; /* For modules */
    uint32_t __module = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.mp.module;
    uint32_t __qty = 0;
    size_t __data_size = 0;
    uint32_t __index = 0;

    switch (__type) {
        case LADDER_REGISTER_M:
            __qty = ladder_ctx->ladder.quantity.m;
            __data_size = sizeof(uint8_t);
            __index = __idx;
            break;
        case LADDER_REGISTER_Q:
            __qty = ladder_ctx->output[__module].q_qty;
            __data_size = sizeof(uint8_t);
            __index = __port;
            break;
        case LADDER_REGISTER_I:
            __qty = ladder_ctx->input[__module].i_qty;
            __data_size = sizeof(uint8_t);
            __index = __port;
            break;
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
            __qty = ladder_ctx->ladder.quantity.c;
            __data_size = sizeof(bool);
            __index = __idx;
            break;
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
            __qty = ladder_ctx->ladder.quantity.t;
            __data_size = sizeof(bool);
            __index = __idx;
            break;
        case LADDER_REGISTER_IW:
            __qty = ladder_ctx->input[__module].iw_qty;
            __data_size = sizeof(int32_t);
            __index = __port;
            break;
        case LADDER_REGISTER_QW:
            __qty = ladder_ctx->output[__module].qw_qty;
            __data_size = sizeof(int32_t);
            __index = __port;
            break;
        case LADDER_REGISTER_C:
            __qty = ladder_ctx->ladder.quantity.c;
            __data_size = sizeof(uint32_t);
            __index = __idx;
            break;
        case LADDER_REGISTER_D:
            __qty = ladder_ctx->ladder.quantity.d;
            __data_size = sizeof(int32_t);
            __index = __idx;
            break;
        case LADDER_REGISTER_R:
            __qty = ladder_ctx->ladder.quantity.r;
            __data_size = sizeof(float);
            __index = __idx;
            break;
        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            return;
    }

    if (__index >= __qty) {
        *error = LADDER_INS_ERR_OUTOFRANGE;
        return;
    }

    switch (__type) {
        case LADDER_REGISTER_M:
            memcpy(&ladder_ctx->memory.M[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_Q:
            memcpy(&ladder_ctx->output[__module].Q[__port], value, __data_size);
            break;
        case LADDER_REGISTER_I:
            memcpy(&ladder_ctx->input[__module].I[__port], value, __data_size);
            break;
        case LADDER_REGISTER_Cd:
            memcpy(&ladder_ctx->memory.Cd[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_Cr:
            memcpy(&ladder_ctx->memory.Cr[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_Td:
            memcpy(&ladder_ctx->memory.Td[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_Tr:
            memcpy(&ladder_ctx->memory.Tr[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_IW:
            memcpy(&ladder_ctx->input[__module].IW[__port], value, __data_size); /* MODIFIED: Use __port */
            break;
        case LADDER_REGISTER_QW:
            memcpy(&ladder_ctx->output[__module].QW[__port], value, __data_size); /* MODIFIED: Use __port */
            break;
        case LADDER_REGISTER_C:
            memcpy(&ladder_ctx->registers.C[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_D:
            memcpy(&ladder_ctx->registers.D[__idx], value, __data_size);
            break;
        case LADDER_REGISTER_R:
            memcpy(&ladder_ctx->registers.R[__idx], value, __data_size);
            break;
        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            break;
    }
}
