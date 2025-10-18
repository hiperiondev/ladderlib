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

#ifndef FN_COMMONS_H
#define FN_COMMONS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ladder.h"


#define exnet(lctx)  ((lctx)->exec_network ? *(*lctx).exec_network : (ladder_network_t){0})

/**
 * @def ladder_cell_data_exec
 * @brief
 *
 */
#define ladder_cell_data_exec(lctx, r, c, i)  (exnet(lctx).cells[r][c].data[i])
/**
 * @def ladder_get_type
 * @brief
 *
 */
#define ladder_get_type(lctx, r, c, i)  (exnet(lctx).cells[r][c].data[i].type)

/**
 * @def ladder_table_pos_row
 * @brief
 *
 */
#define ladder_table_pos_row(lctx, pos)  ((pos) / (*lctx).network[(*lctx).ladder.last.network].cols)

/**
 * @def ladder_table_pos_column
 * @brief
 *
 */
#define ladder_table_pos_column(lctx, pos)  ((pos) - (ladder_table_pos_row(lctx, pos) * (*lctx).network[(*lctx).ladder.last.network].cols))

/**
 * @def CELL_STATE
 * @brief Cell state
 *
 */
#define CELL_STATE(lctx, c, r)  (exnet(lctx).cells[r][c].state)

/**
 * @def CELL_STATE_LEFT
 * @brief Left cell state
 *
 */
#define CELL_STATE_LEFT(lctx, c, r)  \
    (c == 0 ? true : CELL_STATE(lctx, c - 1, r))

/**
 * @def MAKE_BOOL
 * @brief Ensure bool value
 *
 */
#define MAKE_BOOL(v)  ((v) == 0 ? false : true)

/**
 * @fn ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_Conn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Connector
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_CONN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_Neg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Negate
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NEG(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Normally open contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Normally closed contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief  Rise Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Fall Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_Coil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Coil
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 * @return
 */
ladder_ins_err_t fn_COIL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Latch coil
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 * @return
 */
ladder_ins_err_t fn_COILL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Unlatch coil
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 * @return
 */
ladder_ins_err_t fn_COILU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Timer on
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Timer off
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TOF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Timer pulse
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Counter up
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Counter down
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Register move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic subtraction
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic addition
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic multiplication
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic division
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Arithmetic division module
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit shifting left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit shifting right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit rotate left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bit rotate right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise AND
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise OR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise XOR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Bitwise NOT
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison equal to
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison greater than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison lesser than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison lesser or equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Comparison not equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Execute external functions
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

/**
 * @fn ladder_ins_err_t  fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row)
 * @brief Execute table data move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @return Status
 */
ladder_ins_err_t fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row);

#endif /* FN_COMMONS_H */
