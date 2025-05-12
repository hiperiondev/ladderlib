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

#ifndef LADDER_PROGRAM_CHECK_H_
#define LADDER_PROGRAM_CHECK_H_

#include "ladder.h"
#include "ladder_internals.h"

/**
 * @enum LADDER_ERR_PRG_CHECK
 * @brief Ladder program check errors
 *
 */
typedef enum LADDER_ERR_PRG_CHECK {
    LADDER_ERR_PRG_CHECK_OK,                //
    LADDER_ERR_PRG_CHECK_I_INV_MODULE,      //
    LADDER_ERR_PRG_CHECK_I_INV_PORT,        //
    LADDER_ERR_PRG_CHECK_Q_INV_MODULE,      //
    LADDER_ERR_PRG_CHECK_Q_INV_PORT,        //
    LADDER_ERR_PRG_CHECK_IW_INV_MODULE,     //
    LADDER_ERR_PRG_CHECK_IW_INV_PORT,       //
    LADDER_ERR_PRG_CHECK_QW_INV_MODULE,     //
    LADDER_ERR_PRG_CHECK_QW_INV_PORT,       //
    LADDER_ERR_PRG_CHECK_NO_INPUT_MODULES,  //
    LADDER_ERR_PRG_CHECK_NO_OUTPUT_MODULES, //
    //////////////////////////////////////////
    LADDER_ERR_PRG_CHECK_FAIL               //
} ladder_err_prg_check_t;

/**
 * @struct ladder_prg_check_s
 * @brief Return status of program check
 *
 */
typedef struct ladder_prg_check_s {
                  uint32_t network; //
                  uint32_t row;     //
                  uint32_t column;  //
      ladder_instruction_t code;    //
    ladder_err_prg_check_t error;   //
} ladder_prg_check_t;

/**
 * @fn ladder_prg_check_t ladder_program_check(ladder_ctx_t*)
 * @brief Check if program is valid
 *
 * @param ladder_ctx Ladder context
 * @return Status
 */
ladder_prg_check_t ladder_program_check(ladder_ctx_t ladder_ctx);

#endif /* LADDER_PROGRAM_CHECK_H_ */
