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

// +--------------------------------------+-------+---------+------+------------+-------+-------+------+------+------+-------+-------+-------+-------+
// |              function                | instr | symbol  | rows | val 0      | val 1 | val 2 | in 0 | in 1 | in 2 | out 0 | out 1 | out 2 | notes |
// +--------------------------------------+-------+---------+------+------------+-------+-------+------+------+------+-------+-------+-------+-------+
// | Not operation                        | NOP   | ------- |   1  | none       | none  | none  | none | none | none | ????  | ????  | ????  |       |
// | Horizontal Connection                | CONN  | ------- |   1  | none       | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Vertical Bar                         | BAR   | ---|--- |   1  | none       | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Horizontal Connection with Inversion | NEG   | ---!--- |   1  | none       | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Normal Open Contact                  | NO    | --| |-- |   1  | any        | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Normal Closed Contact                | NC    | --|/|-- |   1  | any        | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Rise Edge Contact                    | RE    | -(RE )- |   1  | any        | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Fall Edge Contact                    | FE    | -(FE )- |   1  | any        | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Coil                                 | COIL  | --( )-- |   1  | M or Q     | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Latch Coil                           | COILL | --(L)-- |   1  | M or Q     | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Unlatch Coil                         | COILU | --(U)-- |   1  | M or Q     | none  | none  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Timer On                             | TON   | -(TON)- |   2  | timer id   | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Timer Off                            | TOFF  | -(TOF)- |   2  | timer id   | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Timer Pulse                          | TP    | -(TP )- |   2  | timer id   | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Counter Up                           | CTU   | -(CTU)- |   2  | counter id | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Counter Down                         | CTD   | -(CTD)- |   2  | counter id | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Register Transfer                    | MOVE  | -(MOV)- |   2  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Arithmetic Subtraction               | SUB   | -(SUB)- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Arithmetic Addition                  | ADD   | -(ADD)- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Arithmetic Multiplication            | MUL   | -(MUL)- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Arithmetic Division                  | DIV   | -(DIV)- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Division Module                      | MOD   | -(MOD)- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bit Shifting Left                    | SHL   | -(SHL)- |   2  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bit Shifting Right                   | SHR   | -(SHR)- |   2  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bit Rotation Left                    | ROL   | -(ROL)- |   2  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bit Rotation Right                   | ROR   | -(ROR)- |   2  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bitwise AND                          | AND   | -(AND)- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bitwise OR                           | OR    | -(OR )- |   3  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bitwise XOR                          | XOR   | -(XOR)- |   3  | ????       |  ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Bitwise NOT                          | NOT   | -(NOT)- |   2  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Equal to                             | EQ    | -(EQ )- |   ?  | ????       |  ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Greater than                         | GT    | -(GT )- |   ?  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Greater than or Equal to             | GE    | -(GE )- |   ?  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Less than                            | LT    | -(LT )- |   ?  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Less than or Equal to                | LE    | -(LE )- |   ?  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// | Not Equal to                         | NE    | -(NE )- |   ?  | ????       | ????  | ????  | ???? | ???? | ???? | ????  | ????  | ????  |       |
// +--------------------------------------+-------+---------+------+------------+-------+-------+------+------+------+-------+-------+-------+-------+

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
 * @brief Instruction prototype
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
 * @brief No operation
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
 * @brief Normally open contact
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
