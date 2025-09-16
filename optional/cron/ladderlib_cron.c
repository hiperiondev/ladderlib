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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ladder.h"
#include "ladderlib_cron.h"

#ifdef OPTIONAL_CRON

ladder_ins_err_t ladderlib_cron_init(ladder_ctx_t *ladder_ctx, uint32_t qty) {
    if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx != NULL)
        return LADDER_INS_ERR_FAIL;

    ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx = calloc(qty, sizeof(ladderlib_cron_ctx_t));
    ((ladderlib_cron_t*) (*ladder_ctx).cron)->qty = qty;
    ((ladderlib_cron_t*) (*ladder_ctx).cron)->used = 0;

    for (uint32_t n = 0; n < qty; n++)
        ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].enabled = false;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t ladderlib_cron_deinit(ladder_ctx_t *ladder_ctx) {
    if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx != NULL)
        free(((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx);

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t ladderlib_cron_add(ladder_ctx_t *ladder_ctx, bool auto_reset, uint32_t enable_reg, uint32_t flag_reg, const char *cron_str) {
    if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx == NULL || ((ladderlib_cron_t*) (*ladder_ctx).cron)->used == ((ladderlib_cron_t*) (*ladder_ctx).cron)->qty
            || enable_reg > (*ladder_ctx).ladder.quantity.m - 1 || flag_reg > (*ladder_ctx).ladder.quantity.m - 1)
        return LADDER_INS_ERR_FAIL;

    if (lwdtc_cron_parse(&(((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[((ladderlib_cron_t*) (*ladder_ctx).cron)->used].cron), cron_str) != lwdtcOK)
        return LADDER_INS_ERR_FAIL;
    ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[((ladderlib_cron_t*) (*ladder_ctx).cron)->used].auto_reset = auto_reset;
    ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[((ladderlib_cron_t*) (*ladder_ctx).cron)->used].enable_reg = enable_reg;
    ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[((ladderlib_cron_t*) (*ladder_ctx).cron)->used].flag_reg = flag_reg;
    ((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[((ladderlib_cron_t*) (*ladder_ctx).cron)->used].enabled = true;
    ++((ladderlib_cron_t*) (*ladder_ctx).cron)->used;

    return LADDER_INS_ERR_OK;
}

ladder_ins_err_t ladderlib_cron_eval(ladder_ctx_t *ladder_ctx) {
    if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx == NULL || ((ladderlib_cron_t*) (*ladder_ctx).cron)->used == 0)
        return LADDER_INS_ERR_FAIL;

    struct tm *timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    for (uint32_t n = 0; n < ((ladderlib_cron_t*) (*ladder_ctx).cron)->used; n++) {
        if (((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].enabled
                && (lwdtc_cron_is_valid_for_time(timeinfo, &(((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].cron)) == lwdtcOK)) {
            (*ladder_ctx).memory.M[((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].flag_reg] =
                    (*ladder_ctx).memory.M[((ladderlib_cron_t*) (*ladder_ctx).cron)->ctx[n].enable_reg];
        }
    }

    return LADDER_INS_ERR_OK;
}

#endif
