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

void execNop(plc_ctx_t *plc_ctx, int c, int r, int f) {
    ;
}

void execConn(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execNeg(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (!f) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execNO(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if ((f) && (ladder_get_data_value(plc_ctx, r, c))) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execNC(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if ((f) && !(ladder_get_data_value(plc_ctx, r, c))) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execRE(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if ((f) && (ladder_get_data_value(plc_ctx, r, c)) && !(ladder_get_previous_value(plc_ctx, r, c))) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execFE(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if ((f) && !(ladder_get_data_value(plc_ctx, r, c)) && (ladder_get_previous_value(plc_ctx, r, c))) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execCoil(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r, c, 1);
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    } else {
        ladder_set_data_value(plc_ctx, r, c, 0);
    }
}

void execCoilL(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r, c, 1);
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execCoilU(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r, c, 0);
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execTON(plc_ctx_t *plc_ctx, int c, int r, int f) {
    // timer is not active --> reset
    if (!f) {
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = 0;
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = 0;
    }
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (f && !(*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] && !(*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp = (micros() / 1000);
    }
    // timer is running, update acc value
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (uint16_t) (((micros() / 1000)
                - (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp) / (*plc_ctx).exec_network.cells[r + 1][c].type);
    }
    // timer done --> activate timer done flag and set acc value to his set point
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]
            && (((micros() / 1000) - (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp)
                    >= ((*plc_ctx).exec_network.cells[r + 1][c].data * (*plc_ctx).exec_network.cells[r + 1][c].type))) {
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (*plc_ctx).exec_network.cells[r + 1][c].data;
    }
    // copy timer flags to dynamic flags on network
    if ((*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 1];
    }
}

void execTOFF(plc_ctx_t *plc_ctx, int c, int r, int f) {
    // timer is activated
    if (f && !(*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] && !(*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = 1;
    }
    // timer reactivated while running --> reset
    if (f && (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
    }
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (!f && (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] && !(*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp = (micros() / 1000);
    }
    // timer is running, update acc value
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (uint16_t) (((micros() / 1000)
                - (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp) / (*plc_ctx).exec_network.cells[r + 1][c].type);
    }
    // timer done --> activate timer done flag and set acc value to his set point
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]
            && (((micros() / 1000) - (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp)
                    >= ((*plc_ctx).exec_network.cells[r + 1][c].data * (*plc_ctx).exec_network.cells[r + 1][c].type))) {
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (*plc_ctx).exec_network.cells[r + 1][c].data;
    }
    // copy timer flags to dynamic flags on network
    if ((*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 1];
    }
}

void execTP(plc_ctx_t *plc_ctx, int c, int r, int f) {
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (f && !(*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] && !(*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp = (micros() / 1000);
    }
    // reset timer running when input goes false to avoid continuously running the timer if input stays true
    if (!f && !(*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] && (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
    }
    // timer is running, update acc value
    if ((*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (uint16_t) (((micros() / 1000)
                - (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp) / (*plc_ctx).exec_network.cells[r + 1][c].type);
    }
    // timer done --> activate timer done flag and set acc value to his setpoint
    if ((*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data]
            && (((micros() / 1000) - (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].time_stamp)
                    >= ((*plc_ctx).exec_network.cells[r + 1][c].data * (*plc_ctx).exec_network.cells[r + 1][c].type))) {
        (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (*plc_ctx).exec_network.cells[r + 1][c].data;
    }
    // copy timer flags to dynamic flags on network
    if ((*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
    if ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 1];
    }
}

void execCTU(plc_ctx_t *plc_ctx, int c, int r, int f) {
    // reset counter
    if (c == 0) {
        if ((*plc_ctx).ladder.plc_state == PLC_ST_RUNNING) {
            (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] = 0;
            (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = 0;
            (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        }
    } else {
        if ((*plc_ctx).internals.ladder_network_flags[c - 1] & (*plc_ctx).internals.flags_mask[r + 1]) {
            (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] = 0;
            (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = 0;
            (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        }
    }
    // counter is activated in this scan, change count
    if (f && !(*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] && !(*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data]++;
    }
    // reset counter edge detection
    if (!f) {
        (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
    }
    // counter done
    if ((*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] >= (*plc_ctx).exec_network.cells[r + 1][c].data) {
        (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = 1;
    }
    // copy counter flags to dynamic flags on network
    if ((*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] && f) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
    if ((*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] && f) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 1];
    }
}

void execCTD(plc_ctx_t *plc_ctx, int c, int r, int f) {
    // reset counter
    if (c == 0) {
        if ((*plc_ctx).ladder.plc_state == PLC_ST_RUNNING) {
            (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] = (*plc_ctx).exec_network.cells[r + 1][c].data;
            (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = 0;
            (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        }
    } else {
        if ((*plc_ctx).internals.ladder_network_flags[c - 1] & (*plc_ctx).internals.flags_mask[r + 1]) {
            (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] = (*plc_ctx).exec_network.cells[r + 1][c].data;
            (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = 0;
            (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
        }
    }
    // counter is activated in this scan, change count
    if (f && !(*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] && !(*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data]) {
        (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 1;
        (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data]--;
    }
    // reset counter edge detection
    if (!f) {
        (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = 0;
    }
    // counter done
    if ((*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] == 0) {
        (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = 1;
    }
    // copy counter flags to dynamic flags on network
    if ((*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] && f) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
    if ((*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] && f) {
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 1];
    }
}

void execMOVE(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 1, c, ladder_get_data_value(plc_ctx, r, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execSUB(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        int16_t auxValue1 = ladder_get_data_value(plc_ctx, r, c);
        int16_t auxValue2 = ladder_get_data_value(plc_ctx, r + 1, c);

        if (auxValue1 > auxValue2) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        } else if (auxValue1 == auxValue2) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 1];
        } else {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 2];
        }
        ladder_set_data_value(plc_ctx, r + 2, c, auxValue1 - auxValue2);
    }
}

void execADD(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) + ladder_get_data_value(plc_ctx, r + 1, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execMUL(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) * ladder_get_data_value(plc_ctx, r + 1, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execDIV(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r + 1, c) == 0) {
            ladder_set_data_value(plc_ctx, r + 2, c, 0);
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r + 2];
        } else {
            ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) / ladder_get_data_value(plc_ctx, r + 1, c));
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

void execMOD(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) % ladder_get_data_value(plc_ctx, r + 1, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execSHL(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 1, c, ladder_get_data_value(plc_ctx, r, c) << 1);
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execSHR(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 1, c, ladder_get_data_value(plc_ctx, r, c) >> 1);
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execROL(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        uint16_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(plc_ctx, r, c);
        auxCarryBit = auxValue & 0x8000;
        auxValue = auxValue << 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x0001;
        }
        ladder_set_data_value(plc_ctx, r + 1, c, auxValue);

        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execROR(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        uint16_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(plc_ctx, r, c);
        auxCarryBit = auxValue & 0x0001;
        auxValue = auxValue >> 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x8000;
        }
        ladder_set_data_value(plc_ctx, r + 1, c, auxValue);

        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execAND(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) & ladder_get_data_value(plc_ctx, r + 1, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execOR(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) | ladder_get_data_value(plc_ctx, r + 1, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execXOR(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 2, c, ladder_get_data_value(plc_ctx, r, c) ^ ladder_get_data_value(plc_ctx, r + 1, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execNOT(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(plc_ctx, r + 1, c, ~ladder_get_data_value(plc_ctx, r, c));
        (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
    }
}

void execEQ(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r, c) == ladder_get_data_value(plc_ctx, r + 1, c)) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

void execGT(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r, c) > ladder_get_data_value(plc_ctx, r + 1, c)) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

void execGE(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r, c) >= ladder_get_data_value(plc_ctx, r + 1, c)) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

void execLT(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r, c) < ladder_get_data_value(plc_ctx, r + 1, c)) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

void execLE(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r, c) <= ladder_get_data_value(plc_ctx, r + 1, c)) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

void execNE(plc_ctx_t *plc_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(plc_ctx, r, c) != ladder_get_data_value(plc_ctx, r + 1, c)) {
            (*plc_ctx).internals.ladder_network_flags[c] = (*plc_ctx).internals.ladder_network_flags[c] | (*plc_ctx).internals.flags_mask[r];
        }
    }
}

// get and set memory values
int ladder_get_previous_value(plc_ctx_t *plc_ctx, int r, int c) {
    int returnData = 0;
    switch ((*plc_ctx).exec_network.cells[r][c].type) {
        case PLC_DT_M:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Mh[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Q:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Qh[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_I:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Ih[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Cd:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Cdh[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Cr:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Crh[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Td:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Tdh[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Tr:
            returnData = (int) ((*plc_ctx).prev_scan_vals.Trh[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        default:
            printf("GetPreviousValue ERROR in SwitchCase index\n");
            break;
    }
    return returnData;
}
int ladder_get_data_value(plc_ctx_t *plc_ctx, int r, int c) { // issue #55 - change to uint16_t ?
    int returnData = 0;
    switch ((*plc_ctx).exec_network.cells[r][c].type) {
        case PLC_DT_M:
            returnData = (int) ((*plc_ctx).memory.M[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Q:
            returnData = (int) ((*plc_ctx).memory.Q[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_I:
            returnData = (int) ((*plc_ctx).memory.I[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Cd:
            returnData = (int) ((*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Cr:
            returnData = (int) ((*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Td:
            returnData = (int) ((*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_Tr:
            returnData = (int) ((*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_IW:
            returnData = (int) ((*plc_ctx).registers.IW[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_QW:
            returnData = (int) ((*plc_ctx).registers.QW[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_C:
            returnData = (int) ((*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_D:
            returnData = (int) ((*plc_ctx).registers.D[(*plc_ctx).exec_network.cells[r][c].data]);
            break;
        case PLC_DT_T:
            returnData = (int) ((*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc);
            break;
        case PLC_DT_K:
            returnData = (int) ((*plc_ctx).exec_network.cells[r][c].data);
            break;
            //    case PLC_DT_R:
            //      returnData = R[(*plc_ctx).exec_network.cells[r][c].data];
            //      break;
        default:
            printf("GetDataValue ERROR in SwitchCase index\n");
            break;
    }
    return returnData;
}

void ladder_set_data_value(plc_ctx_t *plc_ctx, int r, int c, int Value) { // Issue #55 - Change to uint16_t ?
    switch ((*plc_ctx).exec_network.cells[r][c].type) {
        case PLC_DT_M:
            (*plc_ctx).memory.M[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_Q:
            (*plc_ctx).memory.Q[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_I:
            (*plc_ctx).memory.I[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_Cd:
            (*plc_ctx).memory.Cd[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_Cr:
            (*plc_ctx).memory.Cr[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_Td:
            (*plc_ctx).memory.Td[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_Tr:
            (*plc_ctx).memory.Tr[(*plc_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case PLC_DT_IW:
            (*plc_ctx).registers.IW[(*plc_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case PLC_DT_QW:
            (*plc_ctx).registers.QW[(*plc_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case PLC_DT_C:
            (*plc_ctx).registers.C[(*plc_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case PLC_DT_D:
            (*plc_ctx).registers.D[(*plc_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case PLC_DT_T:
            (*plc_ctx).timers[(*plc_ctx).exec_network.cells[r][c].data].acc = (uint16_t) (Value);
            break;
            //    case PLC_DT_R:
            //      R[(*plc_ctx).exec_network.cells[r][c].data] = Value;
            //      break;
        default:
            printf("SetDataValue ERROR in SwitchCase index\n");
            break;
    }
}
