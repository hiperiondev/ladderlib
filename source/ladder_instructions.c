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

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_instructions.h"

ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (!f) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if ((f) && (ladder_get_data_value(ladder_ctx, r, c))) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if ((f) && !(ladder_get_data_value(ladder_ctx, r, c))) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if ((f) && (ladder_get_data_value(ladder_ctx, r, c)) && !(ladder_get_previous_value(ladder_ctx, r, c))) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if ((f) && !(ladder_get_data_value(ladder_ctx, r, c)) && (ladder_get_previous_value(ladder_ctx, r, c))) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r, c, 1);
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    } else {
        ladder_set_data_value(ladder_ctx, r, c, 0);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r, c, 1);
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r, c, 0);
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    // timer is not active --> reset
    if (!f) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = 0;
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
    }
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (f && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp = ((*ladder_ctx).io.micros() / 1000);
    }
    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (uint16_t) ((((*ladder_ctx).io.micros() / 1000)
                - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp) / (*ladder_ctx).exec_network.cells[r + 1][c].type);
    }
    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]
            && ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp)
                    >= ((*ladder_ctx).exec_network.cells[r + 1][c].data * (*ladder_ctx).exec_network.cells[r + 1][c].type))) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (*ladder_ctx).exec_network.cells[r + 1][c].data;
    }
    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    // timer is activated
    if (f && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
    }
    // timer reactivated while running --> reset
    if (f && (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
    }
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (!f && (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp = ((*ladder_ctx).io.micros() / 1000);
    }
    // timer is running, update acc value
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (uint16_t) ((((*ladder_ctx).io.micros() / 1000)
                - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp) / (*ladder_ctx).exec_network.cells[r + 1][c].type);
    }
    // timer done --> activate timer done flag and set acc value to his set point
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]
            && ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp)
                    >= ((*ladder_ctx).exec_network.cells[r + 1][c].data * (*ladder_ctx).exec_network.cells[r + 1][c].type))) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (*ladder_ctx).exec_network.cells[r + 1][c].data;
    }
    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    // timer is activated in this scan, set timer running flag and snapshot the timestamp
    if (f && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] && !(*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp = ((*ladder_ctx).io.micros() / 1000);
    }
    // reset timer running when input goes false to avoid continuously running the timer if input stays true
    if (!f && !(*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] && (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
    }
    // timer is running, update acc value
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (uint16_t) ((((*ladder_ctx).io.micros() / 1000)
                - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp) / (*ladder_ctx).exec_network.cells[r + 1][c].type);
    }
    // timer done --> activate timer done flag and set acc value to his setpoint
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data]
            && ((((*ladder_ctx).io.micros() / 1000) - (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].time_stamp)
                    >= ((*ladder_ctx).exec_network.cells[r + 1][c].data * (*ladder_ctx).exec_network.cells[r + 1][c].type))) {
        (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (*ladder_ctx).exec_network.cells[r + 1][c].data;
    }
    // copy timer flags to dynamic flags on network
    if ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }
    if ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    // reset counter
    if (c == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        }
    } else {
        if ((*ladder_ctx).internals.ladder_network_flags[c - 1] & (*ladder_ctx).internals.flags_mask[r + 1]) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        }
    }
    // counter is activated in this scan, change count
    if (f && !(*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] && !(*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data]++;
    }
    // reset counter edge detection
    if (!f) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
    }
    // counter done
    if ((*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] >= (*ladder_ctx).exec_network.cells[r + 1][c].data) {
        (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
    }
    // copy counter flags to dynamic flags on network
    if ((*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] && f) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }
    if ((*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] && f) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    // reset counter
    if (c == 0) {
        if ((*ladder_ctx).ladder.state == LADDER_ST_RUNNING) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] = (*ladder_ctx).exec_network.cells[r + 1][c].data;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        }
    } else {
        if ((*ladder_ctx).internals.ladder_network_flags[c - 1] & (*ladder_ctx).internals.flags_mask[r + 1]) {
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] = (*ladder_ctx).exec_network.cells[r + 1][c].data;
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
        }
    }
    // counter is activated in this scan, change count
    if (f && !(*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] && !(*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data]) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
        (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data]--;
    }
    // reset counter edge detection
    if (!f) {
        (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = 0;
    }
    // counter done
    if ((*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] == 0) {
        (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = 1;
    }
    // copy counter flags to dynamic flags on network
    if ((*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] && f) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }
    if ((*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] && f) {
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 1];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 1, c, ladder_get_data_value(ladder_ctx, r, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        int16_t auxValue1 = ladder_get_data_value(ladder_ctx, r, c);
        int16_t auxValue2 = ladder_get_data_value(ladder_ctx, r + 1, c);

        if (auxValue1 > auxValue2) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        } else if (auxValue1 == auxValue2) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 1];
        } else {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 2];
        }
        ladder_set_data_value(ladder_ctx, r + 2, c, auxValue1 - auxValue2);
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) + ladder_get_data_value(ladder_ctx, r + 1, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) * ladder_get_data_value(ladder_ctx, r + 1, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r + 1, c) == 0) {
            ladder_set_data_value(ladder_ctx, r + 2, c, 0);
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r + 2];
        } else {
            ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) / ladder_get_data_value(ladder_ctx, r + 1, c));
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) % ladder_get_data_value(ladder_ctx, r + 1, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 1, c, ladder_get_data_value(ladder_ctx, r, c) << 1);
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 1, c, ladder_get_data_value(ladder_ctx, r, c) >> 1);
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        uint16_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(ladder_ctx, r, c);
        auxCarryBit = auxValue & 0x8000;
        auxValue = auxValue << 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x0001;
        }
        ladder_set_data_value(ladder_ctx, r + 1, c, auxValue);

        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        uint16_t auxCarryBit, auxValue;

        auxValue = ladder_get_data_value(ladder_ctx, r, c);
        auxCarryBit = auxValue & 0x0001;
        auxValue = auxValue >> 1;
        if (auxCarryBit) {
            auxValue = auxValue | 0x8000;
        }
        ladder_set_data_value(ladder_ctx, r + 1, c, auxValue);

        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) & ladder_get_data_value(ladder_ctx, r + 1, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) | ladder_get_data_value(ladder_ctx, r + 1, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 2, c, ladder_get_data_value(ladder_ctx, r, c) ^ ladder_get_data_value(ladder_ctx, r + 1, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        ladder_set_data_value(ladder_ctx, r + 1, c, ~ladder_get_data_value(ladder_ctx, r, c));
        (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r, c) == ladder_get_data_value(ladder_ctx, r + 1, c)) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r, c) > ladder_get_data_value(ladder_ctx, r + 1, c)) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execGE(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r, c) >= ladder_get_data_value(ladder_ctx, r + 1, c)) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execLT(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r, c) < ladder_get_data_value(ladder_ctx, r + 1, c)) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r, c) <= ladder_get_data_value(ladder_ctx, r + 1, c)) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, int c, int r, int f) {
    if (f) {
        if (ladder_get_data_value(ladder_ctx, r, c) != ladder_get_data_value(ladder_ctx, r + 1, c)) {
            (*ladder_ctx).internals.ladder_network_flags[c] = (*ladder_ctx).internals.ladder_network_flags[c] | (*ladder_ctx).internals.flags_mask[r];
        }
    }

    return LADDER_INS_ERR_OK;
}

// get and set memory values
int ladder_get_previous_value(ladder_ctx_t *ladder_ctx, int r, int c) {
    int returnData = 0;
    switch ((*ladder_ctx).exec_network.cells[r][c].type) {
        case LADDER_TYPE_M:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Mh[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Q:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Qh[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_I:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Ih[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Cd:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Cdh[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Cr:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Crh[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Td:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Tdh[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Tr:
            returnData = (int) ((*ladder_ctx).prev_scan_vals.Trh[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        default:
            //printf("GetPreviousValue ERROR in SwitchCase index\n");
            break;
    }
    return returnData;
}
int ladder_get_data_value(ladder_ctx_t *ladder_ctx, int r, int c) { // issue #55 - change to uint16_t ?
    int returnData = 0;
    switch ((*ladder_ctx).exec_network.cells[r][c].type) {
        case LADDER_TYPE_M:
            returnData = (int) ((*ladder_ctx).memory.M[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Q:
            returnData = (int) ((*ladder_ctx).memory.Q[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_I:
            returnData = (int) ((*ladder_ctx).memory.I[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Cd:
            returnData = (int) ((*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Cr:
            returnData = (int) ((*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Td:
            returnData = (int) ((*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_Tr:
            returnData = (int) ((*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_IW:
            returnData = (int) ((*ladder_ctx).registers.IW[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_QW:
            returnData = (int) ((*ladder_ctx).registers.QW[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_C:
            returnData = (int) ((*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_D:
            returnData = (int) ((*ladder_ctx).registers.D[(*ladder_ctx).exec_network.cells[r][c].data]);
            break;
        case LADDER_TYPE_T:
            returnData = (int) ((*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc);
            break;
        case LADDER_TYPE_K:
            returnData = (int) ((*ladder_ctx).exec_network.cells[r][c].data);
            break;
            //    case LADDER_TYPE_R:
            //      returnData = R[(*ladder_ctx).exec_network.cells[r][c].data];
            //      break;
        default:
            //printf("GetDataValue ERROR in SwitchCase index\n");
            break;
    }
    return returnData;
}

void ladder_set_data_value(ladder_ctx_t *ladder_ctx, int r, int c, int Value) { // Issue #55 - Change to uint16_t ?
    switch ((*ladder_ctx).exec_network.cells[r][c].type) {
        case LADDER_TYPE_M:
            (*ladder_ctx).memory.M[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_Q:
            (*ladder_ctx).memory.Q[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_I:
            (*ladder_ctx).memory.I[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_Cd:
            (*ladder_ctx).memory.Cd[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_Cr:
            (*ladder_ctx).memory.Cr[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_Td:
            (*ladder_ctx).memory.Td[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_Tr:
            (*ladder_ctx).memory.Tr[(*ladder_ctx).exec_network.cells[r][c].data] = (uint8_t) (Value);
            break;
        case LADDER_TYPE_IW:
            (*ladder_ctx).registers.IW[(*ladder_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case LADDER_TYPE_QW:
            (*ladder_ctx).registers.QW[(*ladder_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case LADDER_TYPE_C:
            (*ladder_ctx).registers.C[(*ladder_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case LADDER_TYPE_D:
            (*ladder_ctx).registers.D[(*ladder_ctx).exec_network.cells[r][c].data] = (uint16_t) (Value);
            break;
        case LADDER_TYPE_T:
            (*ladder_ctx).timers[(*ladder_ctx).exec_network.cells[r][c].data].acc = (uint16_t) (Value);
            break;
            //    case LADDER_TYPE_R:
            //      R[(*ladder_ctx).exec_network.cells[r][c].data] = Value;
            //      break;
        default:
            //printf("SetDataValue ERROR in SwitchCase index\n");
            break;
    }
}
