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

// TODO: incomplete!
// +--------------------------------------+-------+---------+------+------------+-------+-------+------+------+------+-------+-------+-------+-------+
// |              function                | instr | symbol  | rows | val 0      | val 1 | val 2 | in 0 | in 1 | in 2 | out 0 | out 1 | out 2 | notes |
// +--------------------------------------+-------+---------+------+------------+-------+-------+------+------+------+-------+-------+-------+-------+
// | Not operation                        | NOP   | ------- |   1  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Horizontal Connection                | CONN  | ------- |   1  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Vertical Bar                         | BAR   | ---|--- |   1  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Horizontal Connection with Inversion | NEG   | ---!--- |   1  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Normal Open Contact                  | NO    | --| |-- |   1  | any        | none  | none  | none | none | none | none  | none  | none  |       |
// | Normal Closed Contact                | NC    | --|/|-- |   1  | any        | none  | none  | none | none | none | none  | none  | none  |       |
// | Rise Edge Contact                    | RE    | -(RE )- |   1  | any        | none  | none  | none | none | none | none  | none  | none  |       |
// | Fall Edge Contact                    | FE    | -(FE )- |   1  | any        | none  | none  | none | none | none | none  | none  | none  |       |
// | Coil                                 | COIL  | --( )-- |   1  | M or Q     | none  | none  | none | none | none | none  | none  | none  |       |
// | Latch Coil                           | COILL | --(L)-- |   1  | M or Q     | none  | none  | none | none | none | none  | none  | none  |       |
// | Unlatch Coil                         | COILU | --(U)-- |   1  | M or Q     | none  | none  | none | none | none | none  | none  | none  |       |
// | Timer On                             | TON   | -(TON)- |   2  | timer id   | none  | none  | none | none | none | none  | none  | none  |       |
// | Timer Off                            | TOFF  | -(TOF)- |   2  | timer id   | none  | none  | none | none | none | none  | none  | none  |       |
// | Timer Pulse                          | TP    | -(TP )- |   2  | timer id   | none  | none  | none | none | none | none  | none  | none  |       |
// | Counter Up                           | CTU   | -(CTU)- |   2  | counter id | none  | none  | none | none | none | none  | none  | none  |       |
// | Counter Down                         | CTD   | -(CTD)- |   2  | counter id | none  | none  | none | none | none | none  | none  | none  |       |
// | Register Transfer                    | MOVE  | -(MOV)- |   2  | any        | any   | none  | none | none | none | none  | none  | none  |       |
// | Arithmetic Subtraction               | SUB   | -(SUB)- |   3  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Arithmetic Addition                  | ADD   | -(ADD)- |   3  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Arithmetic Multiplication            | MUL   | -(MUL)- |   3  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Arithmetic Division                  | DIV   | -(DIV)- |   3  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Division Module                      | MOD   | -(MOD)- |   3  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Bit Shifting Left                    | SHL   | -(SHL)- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Bit Shifting Right                   | SHR   | -(SHR)- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Bit Rotation Left                    | ROL   | -(ROL)- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Bit Rotation Right                   | ROR   | -(ROR)- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Bitwise AND                          | AND   | -(AND)- |   3  | any        | any   | any   | none | none | none | none  | none  | none  |       |
// | Bitwise OR                           | OR    | -(OR )- |   3  | any        | any   | any   | none | none | none | none  | none  | none  |       |
// | Bitwise XOR                          | XOR   | -(XOR)- |   3  | any        | any   | any   | none | none | none | none  | none  | none  |       |
// | Bitwise NOT                          | NOT   | -(NOT)- |   2  | any        | any   | none  | none | none | none | none  | none  | none  |       |
// | Equal to                             | EQ    | -(EQ )- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Greater than                         | GT    | -(GT )- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Greater than or Equal to             | GE    | -(GE )- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Less than                            | LT    | -(LT )- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Less than or Equal to                | LE    | -(LE )- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
// | Not Equal to                         | NE    | -(NE )- |   2  | none       | none  | none  | none | none | none | none  | none  | none  |       |
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
 * @param ladder_ctx Ladder context
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
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execNop(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Connector
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execConn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Negate
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execNeg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Normally open contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execNO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Normally closed contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execNC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief  Rise Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execRE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Fall Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execFE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Coil
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCoil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Latch coil
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Unlatch coil
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execCoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Timer on
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t execTON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Timer off
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execTOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Timer pulse
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execTP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Counter up
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execCTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Counter down
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execCTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Register move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic subtraction
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execSUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic addition
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic multiplication
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execMUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic division
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execDIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic division module
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execMOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit shifting left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execSHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit shifting right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execSHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit rotate left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit rotate right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise AND
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execAND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise OR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise XOR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execXOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise NOT
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execNOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execEQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison equal to
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execGT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execGE(ladder_ctx_t*, int, int, int)
 * @brief Comparison greater than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execGE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execLT(ladder_ctx_t*, int, int, int)
 * @brief Comparison lesser than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execLT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison lesser or equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execLE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison not equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t execNE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

#endif /* LADDER_LOGIC_INSTRUCTIONS_H */
