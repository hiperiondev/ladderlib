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

static uint32_t basetime_factor[] = { 1, 10, 100, 1000, 60000 };
static uint8_t error;

ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (!flag) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if ((flag) && ladder_get_data_value(ladder_ctx, row, column)) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if ((flag) && !ladder_get_data_value(ladder_ctx, row, column)) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if ((flag) && ladder_get_data_value(ladder_ctx, row, column) && !ladder_get_previous_value(ladder_ctx, row, column)) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if ((flag) && !ladder_get_data_value(ladder_ctx, row, column) && ladder_get_previous_value(ladder_ctx, row, column)) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    uint32_t val;

    if (flag) {
        val = 1;
        ladder_set_data_value(ladder_ctx, row, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    } else {
        val = 0;
        ladder_set_data_value(ladder_ctx, row, column, (void*) &val, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = 1;
        ladder_set_data_value(ladder_ctx, row, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = 0;
        ladder_set_data_value(ladder_ctx, row, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // timer is not active --> reset
    if (!flag) {
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = 0;
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
    }

    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (flag && !(*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && !(*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp = (*ladder_ctx).hw.time.millis();
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                - (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp)
                / basetime_factor[(*(*ladder_ctx).exec_network).cells[row + 1][column].type]);
    }

    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp)
                    >= ((*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32
                            * basetime_factor[(*(*ladder_ctx).exec_network).cells[row + 1][column].type]))) {
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = (*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32;
    }

    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row + 1);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // timer is activated
    if (flag && !(*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && !(*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
    }

    // timer reactivated while running --> reset
    if (flag && (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
    }

    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (!flag && (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && !(*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp = (*ladder_ctx).hw.time.millis();
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                - (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp)
                / basetime_factor[(*(*ladder_ctx).exec_network).cells[row + 1][column].type]);
    }

    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp)
                    >= ((*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32
                            * basetime_factor[(*(*ladder_ctx).exec_network).cells[row + 1][column].type]))) {
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = (*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32;
    }

    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row + 1);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (flag && !(*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && !(*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp = (*ladder_ctx).hw.time.millis();
    }

    // reset timer running when input goes false to avoid continuously running the timer if input stays true
    if (!flag && !(*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
    }

    // timer is running, update acc value
    if ((*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = (uint16_t) (((*ladder_ctx).hw.time.millis()
                - (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp)
                / basetime_factor[(*(*ladder_ctx).exec_network).cells[row + 1][column].type]);
    }

    // timer done --> activate timer done flag and set acc value to his setpoint
    if ((*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && (((*ladder_ctx).hw.time.millis() - (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].time_stamp)
                    >= ((*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32
                            * basetime_factor[(*(*ladder_ctx).exec_network).cells[row + 1][column].type]))) {
        (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        (*ladder_ctx).timers[(*(*ladder_ctx).exec_network).cells[row][column].data.i32].acc = (*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32;
    }

    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    if ((*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row + 1);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = 0;
            (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
            (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        }
    } else {
        if ((*ladder_ctx).scan_internals.network_flags[column - 1] & LADDER_FLAG_MASK(row + 1)) {
            (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = 0;
            (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
            (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        }
    }

    // counter is activated in this scan, change count
    if (flag && !(*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && !(*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]++;
    }

    // reset counter edge detection
    if (!flag) {
        (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] >= (*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32) {
        (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
    }

    // copy counter flags to dynamic flags on network
    if ((*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    if ((*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row + 1);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    // reset counter
    if (column == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] =
                    (*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32;
            (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
            (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        }
    } else {
        if ((*ladder_ctx).scan_internals.network_flags[column - 1] & LADDER_FLAG_MASK(row + 1)) {
            (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] =
                    (*(*ladder_ctx).exec_network).cells[row + 1][column].data.i32;
            (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
            (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
        }
    }

    // counter is activated in this scan, change count
    if (flag && !(*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]
            && !(*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
        (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]--;
    }

    // reset counter edge detection
    if (!flag) {
        (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = false;
    }

    // counter done
    if ((*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] == 0) {
        (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = true;
    }

    // copy counter flags to dynamic flags on network
    if ((*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    if ((*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32]) {
        LADDER_ACTUALIZE_FLAGS(column, row + 1);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = ladder_get_data_value(ladder_ctx, row, column);
        ladder_set_data_value(ladder_ctx, row + 1, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    int32_t auxValue1 = ladder_get_data_value(ladder_ctx, row, column);
    int32_t auxValue2 = ladder_get_data_value(ladder_ctx, row + 1, column);

    if (flag) {
        if (auxValue1 > auxValue2) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        } else if (auxValue1 == auxValue2) {
            LADDER_ACTUALIZE_FLAGS(column, row + 1);
        } else {
            LADDER_ACTUALIZE_FLAGS(column, row + 2);
        }

        uint32_t res = auxValue1 - auxValue2;
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &res, &error);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        int32_t val = ladder_get_data_value(ladder_ctx, row, column) + ladder_get_data_value(ladder_ctx, row + 1, column);
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        int32_t val = ladder_get_data_value(ladder_ctx, row, column) * ladder_get_data_value(ladder_ctx, row + 1, column);
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row +1, column) == 0) {
            ladder_set_data_value(ladder_ctx, row + 2, column, 0, &error);
            LADDER_ACTUALIZE_FLAGS(column, row + 2);
        } else {
            int32_t val = ladder_get_data_value(ladder_ctx, row, column) / ladder_get_data_value(ladder_ctx, row + 1, column);
            ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        int32_t val = forcecast(ladder_get_data_value(ladder_ctx, row, column), 1) % forcecast(ladder_get_data_value(ladder_ctx, row + 1, column), 1);
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = forcecast(ladder_get_data_value(ladder_ctx, row, column), 1) << 1;
        ladder_set_data_value(ladder_ctx, row + 1, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = forcecast(ladder_get_data_value(ladder_ctx, row, column), 1) >> 1;
        ladder_set_data_value(ladder_ctx, row + 1, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(ladder_ctx, row, column);
        auxCarryBit = auxValue & 0x8000;
        auxValue = auxValue << 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x0001;
        }
        ladder_set_data_value(ladder_ctx, row + 1, column, (void*) &auxValue, &error);

        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(ladder_ctx, row, column);
        auxCarryBit = auxValue & 0x0001;
        auxValue = auxValue >> 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x8000;
        }
        ladder_set_data_value(ladder_ctx, row + 1, column, (void*) &auxValue, &error);

        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = forcecast(ladder_get_data_value(ladder_ctx, row, column), 1) & forcecast(ladder_get_data_value(ladder_ctx, row + 1, column), 1);
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = forcecast(ladder_get_data_value(ladder_ctx, row, column), 1) | forcecast(ladder_get_data_value(ladder_ctx, row + 1, column), 1);
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = forcecast(ladder_get_data_value(ladder_ctx, row, column), 1) ^ forcecast(ladder_get_data_value(ladder_ctx, row + 1, column), 1);
        ladder_set_data_value(ladder_ctx, row + 2, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        uint32_t val = ~forcecast(ladder_get_data_value(ladder_ctx, row, column), 1);
        ladder_set_data_value(ladder_ctx, row + 1, column, (void*) &val, &error);
        LADDER_ACTUALIZE_FLAGS(column, row);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column) == ladder_get_data_value(ladder_ctx, row + 1, column)) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column) > ladder_get_data_value(ladder_ctx, row + 1, column)) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execGE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column) >= ladder_get_data_value(ladder_ctx, row + 1, column)) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execLT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column) < ladder_get_data_value(ladder_ctx, row + 1, column)) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column) <= ladder_get_data_value(ladder_ctx, row + 1, column)) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    if (flag) {
        if (ladder_get_data_value(ladder_ctx, row, column) != ladder_get_data_value(ladder_ctx, row + 1, column)) {
            LADDER_ACTUALIZE_FLAGS(column, row);
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execFOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag) {
    return (*ladder_ctx).foreign.fn[forcecast(ladder_get_data_value(ladder_ctx, row, column), 0)].exec(ladder_ctx, column, row, flag);
}

void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, void *value, uint8_t *error) {
    *error = LADDER_INS_ERR_OK;

    switch ((*(*ladder_ctx).exec_network).cells[row][column].type) {
        case LADDER_TYPE_M:
            (*ladder_ctx).memory.M[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_Q:
            (*ladder_ctx).memory.Q[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_I:
            (*ladder_ctx).memory.I[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_Cd:
            (*ladder_ctx).memory.Cd[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_Cr:
            (*ladder_ctx).memory.Cr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_Td:
            (*ladder_ctx).memory.Td[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_Tr:
            (*ladder_ctx).memory.Tr[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_IW:
            (*ladder_ctx).registers.IW[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_QW:
            (*ladder_ctx).registers.QW[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_C:
            (*ladder_ctx).registers.C[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_D:
            (*ladder_ctx).registers.D[(*(*ladder_ctx).exec_network).cells[row][column].data.i32] = *((int32_t*) value);
            break;
        case LADDER_TYPE_REAL:
            (*(*ladder_ctx).exec_network).cells[row][column].data.real = *((float*) value);
            break;

        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            break;
    }
}
