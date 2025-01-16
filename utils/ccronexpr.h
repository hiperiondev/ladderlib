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

///////////////////////////////////////// expression /////////////////////////////////////////
//                                                                                          //
// .------------------- seconds (0 - 59)                                                    //
// |  .---------------- minute (0 - 59)                                                     //
// |  |  .------------- hour (0 - 23)                                                       //
// |  |  |  .---------- day of month (1 - 31)                                               //
// |  |  |  |  .------- month (1 - 12) or jan,feb,mar,apr ...                               //
// |  |  |  |  |  .---- day of week (0 - 6) (sunday=0 or 7) or sun,mon,tue,wed,thu,fri,sat  //
// |  |  |  |  |  |                                                                         //
// *  *  *  *  *  *                                                                         //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

// Cron doesn't decide calendar, it follows it.
#ifndef CCRONEXPR_H
#define CCRONEXPR_H

#include <time.h>
#include <stdint.h>

#define CRON_INVALID_INSTANT ((time_t) -1)
#define EXPR_YEARS_LENGTH    29

/**
 * @enum CRONEXPR_ERR
 * @brief cronexpr errors
 *
 */
typedef enum CRONEXPR_ERR {
    CRONEXPR_ERR_OK,                       // ok
    CRONEXPR_ERR_NULLEXP,                  // invalid NULL expression
    CRONEXPR_ERR_NULLTARG,                 // invalid NULL target
    CRONEXPR_ERR_INVNUMFIELDS,             // invalid number of fields, expression must consist of 5-7 fields
    CRONEXPR_ERR_REBOOT,                   // reboot not implemented
} cron_expr_err_t;

/**
 * @struct cron_expr_s
 * @brief Parsed cron representation
 *
 */
typedef struct cron_expr_s {
    uint8_t seconds[8];
    uint8_t minutes[8];
    uint8_t hours[3];
    uint8_t days_of_week[1];
    uint8_t days_of_month[4];
    uint8_t months[2];
     int8_t day_in_month[1];

    // Flags:
    // 0 last day of the month
    // 1 last weekday of the month
    // 2 closest weekday to day in month
    uint8_t flags[1];
#ifndef CRON_DISABLE_YEARS
    uint8_t years[EXPR_YEARS_LENGTH];
#endif
} cron_expr_t;

/**
 * @fn cron_expr_err_t cron_parse_expr(const char *expression, cron_expr_t *target, const char **error)
 * @brief Uses the specified expression to calculate the next 'fire' date after the specified date.
 *
 * @param expression cron expression as null-terminated string, should be no longer that 256 bytes
 * @param pointer    to cron expression structure, it's client code responsibility to free/destroy it afterwards
 * @return error     output error.
 */
cron_expr_err_t cron_parse_expr(const char *expression, cron_expr_t *target);

/**
 * @fn time_t cron_next(cron_expr_t *expr, time_t date)
 * @brief Uses the specified expression to calculate the next 'fire' date after the specified date.
 *
 * @param expr  parsed cron expression to use in next date calculation
 * @param date  start date to start calculation from
 * @return next 'fire' date in case of success, '((time_t) -1)' in case of error.
 */
time_t cron_next(cron_expr_t *expr, time_t date);

/**
 * @fn time_t cron_prev(cron_expr_t *expr, time_t date)
 * @brief Uses the specified expression to calculate the previous 'fire' date after the specified date.
 *
 * @param expr parsed cron expression to use in previous date calculation
 * @param date start date to start calculation from
 * @return previous 'fire' date in case of success, '((time_t) -1)' in case of error.
 */
time_t cron_prev(cron_expr_t *expr, time_t date);

/**
 * @fn int32_t cron_generate_expr(cron_expr_t *source, char *buffer, uint32_t buffer_len, uint32_t expr_len, cron_expr_err_t *error)
 * @brief Generate cron expression from cron_expr structure
 *
 * @param expr parsed cron expression to use
 * @param buffer buffer for the result
 * @param buffer_len maximum length of the buffer
 * @param expr_len number of cron fields produced
 * @param error output error message, will be set to string literal
 * @return used length of the buffer or -1 on error
 */
int32_t cron_generate_expr(cron_expr_t *source, char *buffer, uint32_t buffer_len, uint32_t expr_len, cron_expr_err_t *error);

#endif /* CCRONEXPR_H */
