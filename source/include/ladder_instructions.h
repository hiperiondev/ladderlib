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

#ifndef LADDER_LOGIC_INSTRUCTIONS_H
#define LADDER_LOGIC_INSTRUCTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

#ifdef DEBUG
extern const char *fn_str[];
extern const char *fn_err_str[];
#endif

typedef enum LADDER_INS_ERROR {
    LADDER_INS_ERR_OK,         //
    LADDER_INS_ERR_GETPREVVAL, //
    LADDER_INS_ERR_GETDATAVAL, //
    // [...] //
    LADDER_INS_ERR_FAIL,       //
} ladder_ins_err_t;

/**
 * @fn  ladder_ins_err_t (*ladder_logic)(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
typedef ladder_ins_err_t (*ladder_logic)(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execGE(ladder_ctx_t*, int, int, int)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execGE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execLT(ladder_ctx_t*, int, int, int)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execLT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

#endif /* LADDER_LOGIC_INSTRUCTIONS_H */
