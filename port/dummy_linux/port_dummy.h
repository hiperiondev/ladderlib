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

#ifndef PORT_DUMMY_H_
#define PORT_DUMMY_H_

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

void dummy_read_inputs_local(ladder_ctx_t *ladder_ctx);
void dummy_write_outputs_local(ladder_ctx_t *ladder_ctx);
void dummy_read_inputs_remote(ladder_ctx_t *ladder_ctx);
void dummy_write_outputs_remote(ladder_ctx_t *ladder_ctx);
bool dummy_on_scan_end(ladder_ctx_t *ladder_ctx);
bool dummy_on_instruction(ladder_ctx_t *ladder_ctx);
bool dummy_on_task_before(ladder_ctx_t *ladder_ctx);
bool dummy_on_task_after(ladder_ctx_t *ladder_ctx);
void dummy_on_panic(ladder_ctx_t *ladder_ctx);
void dummy_on_end_task(ladder_ctx_t *ladder_ctx);

void dummy_delay(long msec);
uint64_t dummy_millis(void);

#endif /* PORT_DUMMY_H_ */
