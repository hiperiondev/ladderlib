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

#ifndef LADDERLIB_CRON_H_
#define LADDERLIB_CRON_H_

#include <stdint.h>
#include <stdbool.h>

#include "lwdtc.h"

/**
 * @struct LADDERLIB_CRON_S
 * @brief Cron.
 *
 */
typedef struct LADDERLIB_CRON_CTX_S {
    bool enabled;          /*< Enabled */
    bool auto_reset;       /*< Reset flag at end of scan */
    uint32_t enable_reg;   /*< Register M enable cron */
    uint32_t flag_reg;     /*< Register M set on time */
    lwdtc_cron_ctx_t cron; /*< Cron */
} ladderlib_cron_ctx_t;

/**
 * @struct LADDERLIB_CRON_S
 * @brief Cron list
 *
 */
typedef struct LADDERLIB_CRON_S {
    ladderlib_cron_ctx_t *ctx; /*< Cron context */
    uint32_t qty;              /*< Cron qty */
    uint32_t used;             /*< Used crons */
} ladderlib_cron_t;

/**
 * @fn ladder_ins_err_t ladderlib_cron_init(ladder_ctx_t *ladder_ctx, uint32_t qty)
 * @brief Initialize cron context.
 *
 * @param ladder_ctx Ladder context
 * @param qty        Quantity
 * @return Status
 */
ladder_ins_err_t ladderlib_cron_init(ladder_ctx_t *ladder_ctx, uint32_t qty);

/**
 * @fn ladder_ins_err_t ladderlib_cron_deinit(ladder_ctx_t *ladder_ctx)
 * @brief Erase cron context
 *
 * @param ladder_ctx Ladder context
 * @return Status
 */
ladder_ins_err_t ladderlib_cron_deinit(ladder_ctx_t *ladder_ctx);

/**
 * @fn ladder_ins_err_t ladderlib_cron_add(ladder_ctx_t *ladder_ctx, bool auto_reset, uint32_t enable_reg, uint32_t flag_reg, const char* cron_str)
 * @brief Add cron job.
 *
 * @param ladder_ctx Ladder context
 * @param auto_reset Reset flag at end of scan
 * @param enable_reg Register M enable cron
 * @param flag_reg   Register M set on time
 * @param cron_str   Cron string
 * @return Status
 */
ladder_ins_err_t ladderlib_cron_add(ladder_ctx_t *ladder_ctx, bool auto_reset, uint32_t enable_reg, uint32_t flag_reg, const char *cron_str);

/**
 * @fn ladder_ins_err_t ladderlib_cron_eval(ladder_ctx_t *ladder_ctx)
 * @brief Evaluate cron jobs on actual time.
 *
 * @param ladder_ctx Ladder context
 * @return Status
 */
ladder_ins_err_t ladderlib_cron_eval(ladder_ctx_t *ladder_ctx);

#endif /* LADDERLIB_CRON_H_ */
