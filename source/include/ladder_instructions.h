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

/**
 * @enum LADDER_INSTRUCTIONS
 * @brief Ladder Instructions codes
 * Must match with array of functions in TaskLadder
 */
enum LADDER_INSTRUCTIONS {
    LADDER_INS_NOP   = 0,  /**< LADDER_INS_NOP */
    LADDER_INS_CONN  = 1,  /**< LADDER_INS_CONN */
    LADDER_INS_NEG   = 2,  /**< LADDER_INS_NEG */
    LADDER_INS_NO    = 3,  /**< LADDER_INS_NO */
    LADDER_INS_NC    = 4,  /**< LADDER_INS_NC */
    LADDER_INS_RE    = 5,  /**< LADDER_INS_RE */
    LADDER_INS_FE    = 6,  /**< LADDER_INS_FE */
    LADDER_INS_COIL_ = 7,  /**< LADDER_INS_COIL_ */
    LADDER_INS_COILL = 8,  /**< LADDER_INS_COILL */
    LADDER_INS_COILU = 9,  /**< LADDER_INS_COILU */
    LADDER_INS_TON   = 10, /**< LADDER_INS_TON */
    LADDER_INS_TOFF  = 11, /**< LADDER_INS_TOFF */
    LADDER_INS_TP    = 12, /**< LADDER_INS_TP */
    LADDER_INS_CTU   = 13, /**< LADDER_INS_CTU */
    LADDER_INS_CTD   = 14, /**< LADDER_INS_CTD */
    LADDER_INS_MOVE  = 15, /**< LADDER_INS_MOVE */
    LADDER_INS_SUB   = 16, /**< LADDER_INS_SUB */
    LADDER_INS_ADD   = 17, /**< LADDER_INS_ADD */
    LADDER_INS_MUL   = 18, /**< LADDER_INS_MUL */
    LADDER_INS_DIV   = 19, /**< LADDER_INS_DIV */
    LADDER_INS_MOD   = 20, /**< LADDER_INS_MOD */
    LADDER_INS_SHL   = 21, /**< LADDER_INS_SHL */
    LADDER_INS_SHR   = 22, /**< LADDER_INS_SHR */
    LADDER_INS_ROL   = 23, /**< LADDER_INS_ROL */
    LADDER_INS_ROR   = 24, /**< LADDER_INS_ROR */
    LADDER_INS_AND   = 25, /**< LADDER_INS_AND */
    LADDER_INS_OR    = 26, /**< LADDER_INS_OR */
    LADDER_INS_XOR   = 27, /**< LADDER_INS_XOR */
    LADDER_INS_NOT   = 28, /**< LADDER_INS_NOT */
    LADDER_INS_EQ    = 29, /**< LADDER_INS_EQ */
    LADDER_INS_GT    = 30, /**< LADDER_INS_GT */
    LADDER_INS_GE    = 31, /**< LADDER_INS_GE */
    LADDER_INS_LT    = 32, /**< LADDER_INS_LT */
    LADDER_INS_LE    = 33, /**< LADDER_INS_LE */
    LADDER_INS_NE    = 34, /**< LADDER_INS_NE */
    LADDER_INS_INV   = 35, /**< first invalid */
    LADDER_INS_BAR   = 99, /**< bar mnemonic */
};

typedef enum LADDER_INS_ERROR {
    LADDER_INS_ERR_OK   = 0x00, //
    // [...] //
    LADDER_INS_ERR_FAIL = 0xff, //
} ladder_ins_err_t;

/**
 * @fn ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execGE(ladder_ctx_t*, int, int, int)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execGE(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execLT(ladder_ctx_t*, int, int, int)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execLT(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, int c, int r, int f);

/**
 * @fn ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, int c, int r, int f)
 * @brief
 *
 * @param ladder_ctx
 * @param c
 * @param r
 * @param f
 */
ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, int c, int r, int f);

#endif /* LADDER_LOGIC_INSTRUCTIONS_H */
