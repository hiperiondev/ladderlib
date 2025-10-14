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

uint32_t basetime_factor[] = { 1, 10, 100, 1000, 60000 };

const ladder_instructions_iocd_t ladder_fn_iocd[] = { //
        { 1, 1, 1, 0 }, // NOP
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

void ladder_set_data_value(ladder_ctx_t *ladder_ctx, uint32_t row, uint32_t column, uint32_t pos, void *value, uint8_t *error) {
    *error = LADDER_INS_ERR_OK;
    ladder_register_t __type = ladder_cell_data_exec(ladder_ctx, row, column, pos).type;
    int32_t __idx = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.i32;
    uint8_t __port = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.mp.port;
    uint32_t __module = ladder_cell_data_exec(ladder_ctx, row, column, pos).value.mp.module;
    uint32_t __qty = 0;
    size_t __data_size = 0;
    uint32_t __index = 0;

    if (__type == LADDER_REGISTER_S) {
        *error = LADDER_INS_ERR_SETDATAVAL;
        return;
    }

    ladder_ins_err_t err = LADDER_INS_ERR_OK;
    switch (__type) {
        case LADDER_REGISTER_M:
        case LADDER_REGISTER_Cd:
        case LADDER_REGISTER_Cr:
        case LADDER_REGISTER_Td:
        case LADDER_REGISTER_Tr:
        case LADDER_REGISTER_C:
        case LADDER_REGISTER_D:
        case LADDER_REGISTER_R: {
            int32_t idx = safe_get_register_index(ladder_ctx, __type, __idx, &err);
            if (err != LADDER_INS_ERR_OK) {
                *error = err;
                return;
            }
            __index = (uint32_t) idx;
            __data_size = (__type == LADDER_REGISTER_C) ? sizeof(uint32_t) : (__type == LADDER_REGISTER_D) ? sizeof(int32_t) :
                          (__type == LADDER_REGISTER_R) ? sizeof(float) : sizeof(bool);  // For Cd/Cr/Td/Tr; M is uint8_t but treated as bool-like
            if (__type == LADDER_REGISTER_M)
                __data_size = sizeof(uint8_t);
            break;
        }
        case LADDER_REGISTER_Q:
        case LADDER_REGISTER_I:
        case LADDER_REGISTER_IW:
        case LADDER_REGISTER_QW: {
            if (!safe_check_module_port(ladder_ctx, __type, __module, __port, &__qty, &err)) {
                *error = err;
                return;
            }
            __index = __port;
            __data_size = (__type == LADDER_REGISTER_IW || __type == LADDER_REGISTER_QW) ? sizeof(int32_t) : sizeof(uint8_t);
            break;
        }
        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            return;
    }

    switch (__type) {
        case LADDER_REGISTER_M:
            memcpy(&ladder_ctx->memory.M[__index], value, __data_size);
            break;
        case LADDER_REGISTER_Q:
            memcpy(&ladder_ctx->output[__module].Q[__index], value, __data_size);
            break;
        case LADDER_REGISTER_I:
            memcpy(&ladder_ctx->input[__module].I[__index], value, __data_size);
            break;
        case LADDER_REGISTER_Cd:
            memcpy(&ladder_ctx->memory.Cd[__index], value, __data_size);
            break;
        case LADDER_REGISTER_Cr:
            memcpy(&ladder_ctx->memory.Cr[__index], value, __data_size);
            break;
        case LADDER_REGISTER_Td:
            memcpy(&ladder_ctx->memory.Td[__index], value, __data_size);
            break;
        case LADDER_REGISTER_Tr:
            memcpy(&ladder_ctx->memory.Tr[__index], value, __data_size);
            break;
        case LADDER_REGISTER_IW:
            memcpy(&ladder_ctx->input[__module].IW[__index], value, __data_size);
            break;
        case LADDER_REGISTER_QW:
            memcpy(&ladder_ctx->output[__module].QW[__index], value, __data_size);
            break;
        case LADDER_REGISTER_C:
            memcpy(&ladder_ctx->registers.C[__index], value, __data_size);
            break;
        case LADDER_REGISTER_D:
            memcpy(&ladder_ctx->registers.D[__index], value, __data_size);
            break;
        case LADDER_REGISTER_R:
            memcpy(&ladder_ctx->registers.R[__index], value, __data_size);
            break;
        default:
            *error = LADDER_INS_ERR_SETDATAVAL;
            break;
    }
}
