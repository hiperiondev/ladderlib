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

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

#ifdef DEBUG
const char *fn_str[] = {
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

const char *fn_err_str[] = {
        "OK",
        "GETPREVVAL",
        "GETDATAVAL",
        // [...] //
        "FAIL",
};
#endif

ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (!flag) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if ((flag) && data) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if ((flag) && !data) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int prev_val;
    int data;

    if (ladder_get_previous_value(ladder_ctx, row, column, &prev_val) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETPREVVAL;

    if ((flag) && data && !prev_val) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int prev_val;
    int data;

    if (ladder_get_previous_value(ladder_ctx, row, column, &prev_val) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETPREVVAL;

    if ((flag) && !data && prev_val) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        ladder_set_data_value(ladder_ctx, row, column, 1);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    } else {
        ladder_set_data_value(ladder_ctx, row, column, 0);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        ladder_set_data_value(ladder_ctx, row, column, 1);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        ladder_set_data_value(ladder_ctx, row, column, 0);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // timer is not active --> reset
    if (!flag) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc = 0;
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = false;
    }

    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (flag && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]
            && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp = ((*ladder_ctx).io.micros() / 1000);
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc =
                (uint16_t) ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp)
                        / (*ladder_ctx).exec_network.cells[row + 1][column].type);
    }

    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]
            && ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp)
                    >= ((*ladder_ctx).exec_network.cells[row + 1][column].data * (*ladder_ctx).exec_network.cells[row + 1][column].type))) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc = (*ladder_ctx).exec_network.cells[row + 1][column].data;
    }

    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // timer is activated
    if (flag && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]
            && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = true;
    }

    // timer reactivated while running --> reset
    if (flag && (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
    }

    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (!flag && (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]
            && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp = ((*ladder_ctx).io.micros() / 1000);
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc =
                (uint16_t) ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp)
                        / (*ladder_ctx).exec_network.cells[row + 1][column].type);
    }

    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]
            && ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp)
                    >= ((*ladder_ctx).exec_network.cells[row + 1][column].data * (*ladder_ctx).exec_network.cells[row + 1][column].type))) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc = (*ladder_ctx).exec_network.cells[row + 1][column].data;
    }

    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (flag && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]
            && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp = ((*ladder_ctx).io.micros() / 1000);
    }

    // reset timer running when input goes false to avoid continuously running the timer if input stays true
    if (!flag && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]
            && (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc =
                (uint16_t) ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp)
                        / (*ladder_ctx).exec_network.cells[row + 1][column].type);
    }

    // timer done --> activate timer done flag and set acc value to his setpoint
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]
            && ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].time_stamp)
                    >= ((*ladder_ctx).exec_network.cells[row + 1][column].data * (*ladder_ctx).exec_network.cells[row + 1][column].type))) {
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc = (*ladder_ctx).exec_network.cells[row + 1][column].data;
    }

    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] = 0;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = false;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        }
    } else {
        if ((*ladder_ctx).scan_internals.ladder_network_flags[column - 1] & (*ladder_ctx).scan_internals.flags_mask[row + 1]) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] = 0;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = false;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        }
    }

    // counter is activated in this scan, change count
    if (flag && !(*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data]
            && !(*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data]++;
    }

    // reset counter edge detection
    if (!flag) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] >= (*ladder_ctx).exec_network.cells[row + 1][column].data) {
        (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = true;
    }

    // copy counter flags to dynamic flags on network
    if ((*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] && flag) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    if ((*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] && flag) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] = (*ladder_ctx).exec_network.cells[row + 1][column].data;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = false;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        }
    } else {
        if ((*ladder_ctx).scan_internals.ladder_network_flags[column - 1] & (*ladder_ctx).scan_internals.flags_mask[row + 1]) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] = (*ladder_ctx).exec_network.cells[row + 1][column].data;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = false;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
        }
    }

    // counter is activated in this scan, change count
    if (flag && !(*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data]
            && !(*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data]) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = true;
        (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data]--;
    }

    // reset counter edge detection
    if (!flag) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] == 0) {
        (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = true;
    }

    // copy counter flags to dynamic flags on network
    if ((*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] && flag) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    if ((*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] && flag) {
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 1, column, data);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int auxValue1, auxValue2;

    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column, &auxValue1) != LADDER_INS_ERR_OK
                || ladder_get_data_value(ladder_ctx, row + 1, column, &auxValue2) != LADDER_INS_ERR_OK)
            return LADDER_INS_ERR_GETDATAVAL;

        if (auxValue1 > auxValue2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        } else if (auxValue1 == auxValue2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row + 1];
        } else {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row + 2];
        }
        ladder_set_data_value(ladder_ctx, row + 2, column, auxValue1 - auxValue2);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 2, column, data1 + data2);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 2, column, data1 * data2);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data2 == 0) {
            ladder_set_data_value(ladder_ctx, row + 2, column, 0);
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row + 2];
        } else {
            ladder_set_data_value(ladder_ctx, row + 2, column, data1 / data2);
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 2, column, data1 % data2);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 1, column, data << 1);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 1, column, data >> 1);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        uint16_t auxCarryBit, auxValue;

        auxValue = data;
        auxCarryBit = auxValue & 0x8000;
        auxValue = auxValue << 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x0001;
        }
        ladder_set_data_value(ladder_ctx, row + 1, column, auxValue);

        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        uint16_t auxCarryBit, auxValue;

        auxValue = data;
        auxCarryBit = auxValue & 0x0001;
        auxValue = auxValue >> 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x8000;
        }
        ladder_set_data_value(ladder_ctx, row + 1, column, auxValue);

        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 2, column, data1 & data2);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 2, column, data1 | data2);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 2, column, data1 ^ data2);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data;

    if (ladder_get_data_value(ladder_ctx, row, column, &data) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        ladder_set_data_value(ladder_ctx, row + 1, column, ~data);
        (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                | (*ladder_ctx).scan_internals.flags_mask[row];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data1 == data2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data1 > data2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execGE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data1 >= data2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execLT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data1 < data2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data1 <= data2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int data1, data2;

    if (ladder_get_data_value(ladder_ctx, row, column, &data1) != LADDER_INS_ERR_OK
            || ladder_get_data_value(ladder_ctx, row + 1, column, &data2) != LADDER_INS_ERR_OK)
        return LADDER_INS_ERR_GETDATAVAL;

    if (flag) {
        if (data1 != data2) {
            (*ladder_ctx).scan_internals.ladder_network_flags[column] = (*ladder_ctx).scan_internals.ladder_network_flags[column]
                    | (*ladder_ctx).scan_internals.flags_mask[row];
        }
    }

    return LADDER_INS_ERR_OK;
}

// get and set memory values
ladder_ins_err_t ladder_get_previous_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int *value) {
    *value = 0;

    switch ((*ladder_ctx).exec_network.cells[row][column].type) {
        case LADDER_TYPE_M:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Mh[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Q:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Qh[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_I:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Ih[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Cd:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Cdh[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Cr:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Crh[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Td:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Tdh[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Tr:
            (*value) = (int) ((*ladder_ctx).prev_scan_vals.Trh[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        default:
            return LADDER_INS_ERR_GETPREVVAL;
            break;
    }

    return LADDER_INS_ERR_OK;
}
ladder_ins_err_t ladder_get_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int *value) { // issue #55 - change to uint16_t ?
    *value = 0;

    switch ((*ladder_ctx).exec_network.cells[row][column].type) {
        case LADDER_TYPE_M:
            (*value) = (int) ((*ladder_ctx).memory.M[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Q:
            (*value) = (int) ((*ladder_ctx).memory.Q[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_I:
            (*value) = (int) ((*ladder_ctx).memory.I[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Cd:
            (*value) = (int) ((*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Cr:
            (*value) = (int) ((*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Td:
            (*value) = (int) ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_Tr:
            (*value) = (int) ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_IW:
            (*value) = (int) ((*ladder_ctx).registers.IW[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_QW:
            (*value) = (int) ((*ladder_ctx).registers.QW[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_C:
            (*value) = (int) ((*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_D:
            (*value) = (int) ((*ladder_ctx).registers.D[(*ladder_ctx).exec_network.cells[row][column].data]);
            break;
        case LADDER_TYPE_T:
            (*value) = (int) ((*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc);
            break;
        case LADDER_TYPE_K:
            (*value) = (int) ((*ladder_ctx).exec_network.cells[row][column].data);
            break;
            //    case LADDER_TYPE_R:
            //      (*value) = R[(*ladder_ctx).exec_network.cells[row][column].data];
            //      break;
        default:
            return LADDER_INS_ERR_GETDATAVAL;
            break;
    }

    return LADDER_INS_ERR_OK;
}

void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, int value) { // Issue #55 - Change to uint16_t ?
    switch ((*ladder_ctx).exec_network.cells[row][column].type) {
        case LADDER_TYPE_M:
            (*ladder_ctx).memory.M[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_Q:
            (*ladder_ctx).memory.Q[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_I:
            (*ladder_ctx).memory.I[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_Cd:
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_Cr:
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_Td:
            (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_Tr:
            (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[row][column].data] = (uint8_t) (value);
            break;
        case LADDER_TYPE_IW:
            (*ladder_ctx).registers.IW[(*ladder_ctx).exec_network.cells[row][column].data] = (uint16_t) (value);
            break;
        case LADDER_TYPE_QW:
            (*ladder_ctx).registers.QW[(*ladder_ctx).exec_network.cells[row][column].data] = (uint16_t) (value);
            break;
        case LADDER_TYPE_C:
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[row][column].data] = (uint16_t) (value);
            break;
        case LADDER_TYPE_D:
            (*ladder_ctx).registers.D[(*ladder_ctx).exec_network.cells[row][column].data] = (uint16_t) (value);
            break;
        case LADDER_TYPE_T:
            (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[row][column].data].acc = (uint16_t) (value);
            break;
            //    case LADDER_TYPE_R:
            //      R[(*ladder_ctx).exec_network.cells[row][column].data] = value;
            //      break;
        default:
            //printf("SetDataValue ERROR in SwitchCase index\n");
            break;
    }
}
