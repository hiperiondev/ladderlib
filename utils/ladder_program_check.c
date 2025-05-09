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

#include "ladder.h"
#include "ladder_internals.h"
#include "ladder_program_check.h"

ladder_err_prg_check_t ladder_program_check(ladder_ctx_t ladder_ctx) {
    for (uint32_t nt = 0; nt < ladder_ctx.ladder.quantity.networks; nt++)
        for (uint32_t column = 0; column < ladder_ctx.network[nt].cols; column++)
            for (uint32_t row = 0; row < ladder_ctx.network[nt].rows; row++)
                for (uint32_t d = 0; d < ladder_ctx.network[nt].cells[row][column].data_qty; d++) {
                    switch (ladder_ctx.network[nt].cells[row][column].data[d].type) {
                        case LADDER_REGISTER_Q:
                            if (ladder_ctx.hw.io.fn_read_qty == 0)
                                return LADDER_ERR_PRG_CHECK_NO_OUTPUT_MODULES;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module > ladder_ctx.hw.io.fn_read_qty)
                                return LADDER_ERR_PRG_CHECK_Q_INV_MODULE;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.port
                                    > ladder_ctx.output[ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module].q_qty - 1)
                                return LADDER_ERR_PRG_CHECK_Q_INV_PORT;
                            break;
                        case LADDER_REGISTER_QW:
                            if (ladder_ctx.hw.io.fn_write_qty == 0)
                                return LADDER_ERR_PRG_CHECK_NO_OUTPUT_MODULES;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module > ladder_ctx.hw.io.fn_read_qty)
                                return LADDER_ERR_PRG_CHECK_QW_INV_MODULE;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.port
                                    > ladder_ctx.output[ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module].qw_qty - 1)
                                return LADDER_ERR_PRG_CHECK_QW_INV_PORT;
                            break;
                        case LADDER_REGISTER_I:
                            if (ladder_ctx.hw.io.fn_read_qty == 0)
                                return LADDER_ERR_PRG_CHECK_NO_INPUT_MODULES;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module > ladder_ctx.hw.io.fn_read_qty)
                                return LADDER_ERR_PRG_CHECK_I_INV_MODULE;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.port
                                    > ladder_ctx.input[ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module].i_qty - 1)
                                return LADDER_ERR_PRG_CHECK_I_INV_PORT;
                            break;
                        case LADDER_REGISTER_IW:
                            if (ladder_ctx.hw.io.fn_write_qty == 0)
                                return LADDER_ERR_PRG_CHECK_NO_INPUT_MODULES;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module > ladder_ctx.hw.io.fn_read_qty)
                                return LADDER_ERR_PRG_CHECK_I_INV_MODULE;
                            if (ladder_ctx.network[nt].cells[row][column].data[d].value.mp.port
                                    > ladder_ctx.input[ladder_ctx.network[nt].cells[row][column].data[d].value.mp.module].iw_qty - 1)
                                return LADDER_ERR_PRG_CHECK_I_INV_PORT;
                            break;
                        default:
                    }
                }

    return LADDER_ERR_PRG_CHECK_OK;
}
