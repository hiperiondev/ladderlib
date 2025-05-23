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

// registers quantity
#define QTY_M  8
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
    if (!ladder_ctx_init(&ladder_ctx, 6, 7, 3, QTY_M, QTY_C, QTY_T, QTY_D, QTY_R, false)) {
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

    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    printf("--[ laderlib version: %d.%d.%d ]--\n\n", LADDERLIB_VERSION_MAYOR, LADDERLIB_VERSION_MINOR, LADDERLIB_VERSION_PATCH);
    // add foreign
    if (!ladder_add_foreign(&ladder_ctx, dummy_init, NULL, 5)) {
        printf("ERROR Load foreign\n");
        exit(1);
    }

    printf("Load demo program: %s\n", prg_load);
    if ((err = ladder_json_to_program(prg_load, &ladder_ctx)) != JSON_ERROR_OK) {
        printf("ERROR: Load demo program %d\n", err);
        goto end;
    }

    printf("Save demo program: %s\n", prg_save);
    if ((err = ladder_program_to_json(prg_save, &ladder_ctx)) != JSON_ERROR_OK) {
        printf("ERROR: Save demo program\n");
        goto end;
    }

    printf("Compact saved program: %s\n", prg_save);
    if ((err = ladder_compact_json_file(prg_save, "compact.json")) != JSON_ERROR_OK) {
        printf("ERROR: Compact saved program\n");
        goto end;
    }

    printf("\n");

    err_prg_check = ladder_program_check(ladder_ctx);
    if (err_prg_check.error != LADDER_ERR_PRG_CHECK_OK) {
        printf("ERROR: Program not valid (%d) at network:%d [%d,%d] code: %d\n", err_prg_check.error, err_prg_check.network, err_prg_check.row,
                err_prg_check.column, err_prg_check.code);
        goto end;
    }

    ladder_print(ladder_ctx);

    printf("Start Task Ladder\n\n");
    ladder_task((void*) &ladder_ctx);

    ///////////////////////////////////////////////////////

    end:
    // release context
    if (!ladder_ctx_deinit(&ladder_ctx)) {
        printf("ERROR Deinitializing\n");
        return 1;
    }

    return 0;
}
