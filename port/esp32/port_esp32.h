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

#ifndef PORT_ESP32_H_
#define PORT_ESP32_H_

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

#define INPUT_00    34
#define INPUT_01    35
#define INPUT_02    32
#define INPUT_03    13
#define INPUT_04    04
#define INPUT_05    00
#define INPUT_06    39
#define INPUT_07    36
#define AN_INPUT_00 39
#define AN_INPUT_01 36

#define OUTPUT_00    17
#define OUTPUT_01    16
#define OUTPUT_02    02
#define OUTPUT_03    15
#define OUTPUT_04    26
#define OUTPUT_05    25
#define AN_OUTPUT_00 26
#define AN_OUTPUT_01 25

bool esp32_port_initialize(void);

void esp32_read_inputs_local(ladder_ctx_t *ladder_ctx);
void esp32_write_outputs_local(ladder_ctx_t *ladder_ctx);
void esp32_read_inputs_remote(ladder_ctx_t *ladder_ctx);
void esp32_write_outputs_remote(ladder_ctx_t *ladder_ctx);
bool esp32_on_scan_end(ladder_ctx_t *ladder_ctx);
bool esp32_on_instruction(ladder_ctx_t *ladder_ctx);
bool esp32_on_task_before(ladder_ctx_t *ladder_ctx);
bool esp32_on_task_after(ladder_ctx_t *ladder_ctx);
void esp32_on_panic(ladder_ctx_t *ladder_ctx);
void esp32_on_end_task(ladder_ctx_t *ladder_ctx);

int esp32_delay(long msec);
uint64_t esp32_millis(void);

#endif /* PORT_ESP32_H_ */
