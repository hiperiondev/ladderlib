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

#include <stdio.h>
#include <stdlib.h>

#include "ladder.h"
#include "port_dummy.h"
#include "ladder_print.h"
#include "ladder_instructions.h"
#include "fn_dummy.h"
#include "ladder_program_json.h"
#include "ladder_program_check.h"
#ifdef OPTIONAL_CRON
#include "ladderlib_cron.h"
#endif

#define EP(x) [x] = #x
const char *json_errors[] = {                   //
                EP(JSON_ERROR_OK),              //
                EP(JSON_ERROR_OPENFILE),        //
                EP(JSON_ERROR_PARSE),           //
                EP(JSON_ERROR_ALLOC_STRING),    //
                EP(JSON_ERROR_ALLOC_NETWORK),   //
                EP(JSON_ERROR_INS_INV),         //
                EP(JSON_ERROR_TYPE_INV),        //
                EP(JSON_ERROR_CREATEARRAY),     //
                EP(JSON_ERROR_CREATENETOBJT),   //
                EP(JSON_ERROR_CREATENETDATA),   //
                EP(JSON_ERROR_CREATEROWARRAY),  //
                EP(JSON_ERROR_CREATECELLOBJ),   //
                EP(JSON_ERROR_CREATEDATAARRAY), //
                EP(JSON_ERROR_CREATEDATAOBJ),   //
                EP(JSON_ERROR_PRINTOBJ),        //
                EP(JSON_ERROR_COMPACTFILE),     //
                EP(JSON_ERROR_WRITEFILE),       //
                EP(JSON_ERROR_INVALIDVALUE),    //
                EP(JSON_ERROR_SCHEMA),          //
                EP(JSON_ERROR_FAIL),            //
        };

const char *program_test_errors[] = {                       //
                EP(LADDER_ERR_PRG_CHECK_OK),                //
                EP(LADDER_ERR_PRG_CHECK_I_INV_MODULE),      //
                EP(LADDER_ERR_PRG_CHECK_I_INV_PORT),        //
                EP(LADDER_ERR_PRG_CHECK_Q_INV_MODULE),      //
                EP(LADDER_ERR_PRG_CHECK_Q_INV_PORT),        //
                EP(LADDER_ERR_PRG_CHECK_IW_INV_MODULE),     //
                EP(LADDER_ERR_PRG_CHECK_IW_INV_PORT),       //
                EP(LADDER_ERR_PRG_CHECK_QW_INV_MODULE),     //
                EP(LADDER_ERR_PRG_CHECK_QW_INV_PORT),       //
                EP(LADDER_ERR_PRG_CHECK_NO_INPUT_MODULES),  //
                EP(LADDER_ERR_PRG_CHECK_NO_OUTPUT_MODULES), //
                EP(LADDER_ERR_PRG_CHECK_T_INV_TYPE),        //
                EP(LADDER_ERR_PRG_CHECK_T_INV_INDEX),       //
                EP(LADDER_ERR_PRG_CHECK_INV_BASE_TIME),     //
                EP(LADDER_ERR_PRG_CHECK_MISSING_MULTI),     //
                EP(LADDER_ERR_PRG_CHECK_MULTI_HAS_DATA),    //
                EP(LADDER_ERR_PRG_CHECK_DANGLING_MULTI),    //
                EP(LADDER_ERR_PRG_CHECK_FAIL )              //
        };

const char *functions_str[] = {         //
                EP(LADDER_INS_NOP),     //
                EP(LADDER_INS_CONN),    //
                EP(LADDER_INS_NEG),     //
                EP(LADDER_INS_NO),      //
                EP(LADDER_INS_NC),      //
                EP(LADDER_INS_RE),      //
                EP(LADDER_INS_FE),      //
                EP(LADDER_INS_COIL),    //
                EP(LADDER_INS_COILL),   //
                EP(LADDER_INS_COILU),   //
                EP(LADDER_INS_TON),     //
                EP(LADDER_INS_TOF),     //
                EP(LADDER_INS_TP),      //
                EP(LADDER_INS_CTU),     //
                EP(LADDER_INS_CTD),     //
                EP(LADDER_INS_MOVE),    //
                EP(LADDER_INS_SUB),     //
                EP(LADDER_INS_ADD),     //
                EP(LADDER_INS_MUL),     //
                EP(LADDER_INS_DIV),     //
                EP(LADDER_INS_MOD),     //
                EP(LADDER_INS_SHL),     //
                EP(LADDER_INS_SHR),     //
                EP(LADDER_INS_ROL),     //
                EP(LADDER_INS_ROR),     //
                EP(LADDER_INS_AND),     //
                EP(LADDER_INS_OR),      //
                EP(LADDER_INS_XOR),     //
                EP(LADDER_INS_NOT),     //
                EP(LADDER_INS_EQ),      //
                EP(LADDER_INS_GT),      //
                EP(LADDER_INS_GE),      //
                EP(LADDER_INS_LT),      //
                EP(LADDER_INS_LE),      //
                EP(LADDER_INS_NE),      //
                EP(LADDER_INS_FOREIGN), //
                EP(LADDER_INS_TMOVE),   //
                EP(LADDER_INS_INV),     //
                EP(LADDER_INS_MULTI),   //
        };

// registers quantity
#define QTY_M  18
#define QTY_C  8
#define QTY_T  8
#define QTY_D  8
#define QTY_R  8

int main(void) {
    const char prg_load[] = "ladder_networks.json";
    const char prg_save[] = "ladder_networks_save.json";
    uint8_t err = 0;
    ladder_prg_check_t err_prg_check;

    // main context
    ladder_ctx_t ladder_ctx;

    // clear screen
    printf("\e[1;1H\e[2J");

    // initialize context
    if (!ladder_ctx_init(&ladder_ctx, 6, 7, 3, QTY_M, QTY_C, QTY_T, QTY_D, QTY_R, 10, true)) {
        printf("ERROR Initializing\n");
        return 1;
    }

    ///////////////////////////////////////////////////////

    // assign port functions
    if (!ladder_add_read_fn(&ladder_ctx, dummy_read, dummy_init_read)) {
        printf("ERROR Adding io read function\n");
        return 1;
    }

    if (!ladder_add_write_fn(&ladder_ctx, dummy_write, dummy_init_write)) {
        printf("ERROR Adding io write function\n");
        return 1;
    }

    ladder_ctx.on.scan_end = dummy_on_scan_end;
    ladder_ctx.on.instruction = dummy_on_instruction;
    ladder_ctx.on.task_before = dummy_on_task_before;
    ladder_ctx.on.task_after = dummy_on_task_after;
    ladder_ctx.on.panic = dummy_on_panic;
    ladder_ctx.on.end_task = dummy_on_end_task;
    ladder_ctx.hw.time.millis = dummy_millis;
    ladder_ctx.hw.time.delay = dummy_delay;

#ifdef OPTIONAL_CRON
    if (ladderlib_cron_init(&ladder_ctx, 3) != LADDER_INS_ERR_OK) {
        printf("ERROR Cron init\n");
        exit(1);
    }

    if (ladderlib_cron_add(&ladder_ctx, false, 10, 11, "*/2 * * * * * *") != LADDER_INS_ERR_OK) {
        printf("ERROR Cron add 1\n");
        exit(1);
    }

    if (ladderlib_cron_add(&ladder_ctx, false, 12, 13, "*/10 * * * * * *") != LADDER_INS_ERR_OK) {
        printf("ERROR Cron add 2\n");
        exit(1);
    }

    if (ladderlib_cron_add(&ladder_ctx, false, 14, 15, "0 58 22 * * * *") != LADDER_INS_ERR_OK) {
        printf("ERROR Cron add 3\n");
        exit(1);
    }
#endif

    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    printf("--[ laderlib version: %d.%d.%d ]--\n\n", LADDERLIB_VERSION_MAYOR, LADDERLIB_VERSION_MINOR, LADDERLIB_VERSION_PATCH);
    // add foreign
    if (!ladder_add_foreign(&ladder_ctx, dummy_init, NULL, 5)) {
        printf("ERROR Load foreign\n");
        exit(1);
    }

    printf("Check schema demo program: %s\n", prg_load);
    if (!ladder_validate_json_file(prg_load, "ladder_networks_schema.json")) {
        printf("ERROR: Check schema demo program\n");
        goto end;
    }

    printf("Load demo program: %s\n", prg_load);
    if ((err = ladder_json_to_program(prg_load, &ladder_ctx)) != JSON_ERROR_OK) {
        printf("ERROR: Load demo program (%d) %s\n", err, json_errors[err]);
        goto end;
    }

    printf("Save demo program: %s\n", prg_save);
    if ((err = ladder_program_to_json(prg_save, &ladder_ctx)) != JSON_ERROR_OK) {
        printf("ERROR: Save demo program (%d) %s\n", err, json_errors[err]);
        goto end;
    }

    printf("Compact saved program: %s\n", prg_save);
    if ((err = ladder_compact_json_file(prg_save, "compact.json")) != JSON_ERROR_OK) {
        printf("ERROR: Compact saved program (%d) %s\n", err, json_errors[err]);
        goto end;
    }

    printf("\n");

    err_prg_check = ladder_program_check(&ladder_ctx);
    if (err_prg_check.error != LADDER_ERR_PRG_CHECK_OK) {
        printf("ERROR: Program not valid (%d) %s at network:%d [%d,%d] code: (%d) %s\n", err_prg_check.error, program_test_errors[err_prg_check.error],
                err_prg_check.network, err_prg_check.row, err_prg_check.column, err_prg_check.code, functions_str[err_prg_check.code]);
        goto end;
    }

    ladder_print(ladder_ctx);

    printf("Start Task Ladder\n\n");
    ladder_task((void*) &ladder_ctx);

    ///////////////////////////////////////////////////////

    end:
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    // release context
    if (!ladder_ctx_deinit(&ladder_ctx)) {
        printf("ERROR Deinitializing\n");
        return 1;
    }

    return 0;
}
