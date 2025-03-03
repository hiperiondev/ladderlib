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

#ifndef LADDER_INSTRUCTIONS_H
#define LADDER_INSTRUCTIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

#define to_integer(val, type)      \
    (                              \
    (val == 0) ? (uint32_t)(val) : \
    (val == 1) ? (int32_t)(val)  : \
    0                              \
    )

#define exnet(lctx)  (*(*lctx).exec_network)
/**
 * @def ladder_get_data_value
 * @brief Get data values
 *
 */
#define ladder_get_data_value(lctx, r, c, i)                                                                                              \
(                                                                                                                                         \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_NONE) ? (uint8_t)(exnet(lctx).cells[r][c].data[i].value.i32)                       : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_M)    ? (uint8_t)((*lctx).memory.M[exnet(lctx).cells[r][c].data[i].value.i32])     : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Q)    ? (uint8_t)((*lctx).memory.Q[exnet(lctx).cells[r][c].data[i].value.i32])     : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_I)    ? (uint8_t)((*lctx).memory.I[exnet(lctx).cells[r][c].data[i].value.i32])     : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Cd)   ? (bool)((*lctx).memory.Cd[exnet(lctx).cells[r][c].data[i].value.i32])       : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Cr)   ? (bool)((*lctx).memory.Cr[exnet(lctx).cells[r][c].data[i].value.i32])       : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Td)   ? (bool)((*lctx).memory.Td[exnet(lctx).cells[r][c].data[i].value.i32])       : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Tr)   ? (bool)((*lctx).memory.Tr[exnet(lctx).cells[r][c].data[i].value.i32])       : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_IW)   ? (int32_t)((*lctx).registers.IW[exnet(lctx).cells[r][c].data[i].value.i32]) : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_QW)   ? (int32_t)((*lctx).registers.QW[exnet(lctx).cells[r][c].data[i].value.i32]) : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_C)    ? (uint32_t)((*lctx).registers.C[exnet(lctx).cells[r][c].data[i].value.i32]) : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_T)    ? (uint64_t)((*lctx).timers[exnet(lctx).cells[r][c].data[i].value.i32].acc)  : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_D)    ? (int32_t)((*lctx).registers.D[exnet(lctx).cells[r][c].data[i].value.i32])  : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_REAL) ? (float)((*lctx).registers.IW[exnet(lctx).cells[r][c].data[i].value.i32])   : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_INV)  ? 0                                                                          : \
0                                                                                                                                         \
)

/**
 * @def ladder_get_previous_value
 * @brief Get previous value
 *
 */
#define ladder_get_previous_value(lctx, r, c, i)                                                                                             \
(                                                                                                                                            \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_M)  ? (uint8_t)((*lctx).prev_scan_vals.Mh[exnet(lctx).cells[r][c].data[i].value.i32]) : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Q)  ? (uint8_t)((*lctx).prev_scan_vals.Qh[exnet(lctx).cells[r][c].data[i].value.i32]) : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_I)  ? (uint8_t)((*lctx).prev_scan_vals.Ih[exnet(lctx).cells[r][c].data[i].value.i32]) : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Cd) ? (bool)((*lctx).prev_scan_vals.Cdh[exnet(lctx).cells[r][c].data[i].value.i32])   : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Cr) ? (bool)((*lctx).prev_scan_vals.Crh[exnet(lctx).cells[r][c].data[i].value.i32])   : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Td) ? (bool)((*lctx).prev_scan_vals.Tdh[exnet(lctx).cells[r][c].data[i].value.i32])   : \
(exnet(lctx).cells[r][c].data[i].type == LADDER_TYPE_Tr) ? (bool)((*lctx).prev_scan_vals.Trh[exnet(lctx).cells[r][c].data[i].value.i32])   : \
0                                                                                                                                            \
)

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
#define ladder_table_pos_row(lctx, pos)  ((pos) / (*lctx).ladder.quantity.net_columns)

/**
 * @def ladder_table_pos_column
 * @brief
 *
 */
#define ladder_table_pos_column(lctx, pos)  ((pos) - (ladder_table_pos_row(lctx, pos) * (*lctx).ladder.quantity.net_columns))

/**
 * @def ladder_get_table_i32
 * @brief
 *
 */
#define ladder_get_table_i32(lctx, t, pos)                                                                                    \
    (                                                                                                                         \
        ((table) > (*lctx).ladder.quantity.networks || (*lctx).exec_network[(table)].enable) ? 0                            : \
        ((pos) > (*lctx).ladder.quantity.net_rows * (*lctx).ladder.quantity.net_columns)     ? 0                            : \
        (*lctx).network[table].cells[ladder_table_pos_row(lctx, pos)][ladder_table_pos_column(lctx, pos)].data[0].value.i32   \
    )

/**
 * @def LADDER_ACTUALIZE_FLAGS
 * @brief Actualize flags in/after execution
 *
 */
#define LADDER_ACTUALIZE_FLAGS(column, row)                                              \
        (*ladder_ctx).scan_internals.network_flags[(column)] =                           \
            (*ladder_ctx).scan_internals.network_flags[(column)] | LADDER_FLAG_MASK(row)

/**
 * @fn ladder_ins_err_t  fn_Nop(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief No operation
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_NOP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_Conn(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Connector
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_CONN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_Neg(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Negate
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_NEG(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Normally open contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_NO(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Normally closed contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_NC(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief  Rise Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_RE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Fall Edge Contact
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_FE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_Coil(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Coil
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t fn_COIL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_CoilL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Latch coil
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t fn_COILL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_CoilU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Unlatch coil
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t fn_COILU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Timer on
 *
 * @param ladder_ctx Ladder context
 * @param column
 * @param row
 * @param flag
 * @return
 */
ladder_ins_err_t fn_TON(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_TOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Timer off
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_TOFF(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Timer pulse
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_TP(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Counter up
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_CTU(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Counter down
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_CTD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Register move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_MOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic subtraction
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_SUB(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic addition
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_ADD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic multiplication
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_MUL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic division
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_DIV(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Arithmetic division module
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_MOD(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit shifting left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_SHL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit shifting right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_SHR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit rotate left
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_ROL(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bit rotate right
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_ROR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise AND
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_AND(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise OR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_OR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise XOR
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_XOR(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Bitwise NOT
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_NOT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_EQ(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison equal to
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_GT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison greater than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_GE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison lesser than
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_LT(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison lesser or equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_LE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Comparison not equal
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_NE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Execute external functions
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_FOREIGN(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

/**
 * @fn ladder_ins_err_t  fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag)
 * @brief Execute table data move
 *
 * @param ladder_ctx Ladder context
 * @param column Column
 * @param row Row
 * @param flag Flag
 * @return Status
 */
ladder_ins_err_t fn_TMOVE(ladder_ctx_t *ladder_ctx, uint32_t column, uint32_t row, bool flag);

#endif /* LADDER_INSTRUCTIONS_H */
