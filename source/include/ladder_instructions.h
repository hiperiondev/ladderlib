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
 * @fn void execNop(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execNop(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execConn(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execConn(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execNeg(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execNeg(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execNO(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execNO(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execNC(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execNC(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execRE(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execRE(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execFE(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execFE(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execCoil(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execCoil(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execCoilL(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execCoilL(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execCoilU(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execCoilU(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execTON(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execTON(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execTOFF(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execTOFF(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execTP(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execTP(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execCTU(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execCTU(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execCTD(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execCTD(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execMOVE(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execMOVE(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execSUB(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execSUB(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execADD(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execADD(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execMUL(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execMUL(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execDIV(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execDIV(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execMOD(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execMOD(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execSHL(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execSHL(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execSHR(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execSHR(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execROL(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execROL(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execROR(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execROR(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execAND(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execAND(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execOR(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execOR(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execXOR(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execXOR(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execNOT(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execNOT(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execEQ(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execEQ(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execGT(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execGT(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execGE(plc_ctx_t*, int, int, int)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execGE(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execLT(plc_ctx_t*, int, int, int)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execLT(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execLE(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execLE(plc_ctx_t *plc_ctx, int c, int r, int f);

/**
 * @fn void execNE(plc_ctx_t *plc_ctx, int c, int r, int f)
 * @brief
 *
 * @param plc_ctx
 * @param c
 * @param r
 * @param f
 */
void execNE(plc_ctx_t *plc_ctx, int c, int r, int f);

#endif /* LADDER_LOGIC_INSTRUCTIONS_H */
