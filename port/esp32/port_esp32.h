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

#ifndef PORT_ESP32_H_
#define PORT_ESP32_H_

#include <stdbool.h>
#include <stdint.h>

#include "ladder.h"

#define INPUT_00          GPIO_NUM_34
#define INPUT_01          GPIO_NUM_35
#define INPUT_02          GPIO_NUM_32
#define INPUT_03          GPIO_NUM_13
#define INPUT_04          GPIO_NUM_15
#define INPUT_05          GPIO_NUM_0
#define INPUT_06          GPIO_NUM_39
#define INPUT_07          GPIO_NUM_36
#define AN_INPUT_00       GPIO_NUM_39
#define AN_INPUT_01       GPIO_NUM_36
#define OUTPUT_00         GPIO_NUM_4
#define OUTPUT_01         GPIO_NUM_16
#define OUTPUT_02         GPIO_NUM_2
#define OUTPUT_03         GPIO_NUM_17
#define OUTPUT_04         GPIO_NUM_26
#define OUTPUT_05         GPIO_NUM_25
#define AN_OUTPUT_00      GPIO_NUM_26
#define AN_OUTPUT_01      GPIO_NUM_25

/**
 * @fn bool esp32_port_initialize(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
bool esp32_port_initialize(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_port_output_test(void)
 * @brief
 */
void esp32_port_output_test(void);

/**
 * @fn void esp32_port_input_test(void)
 * @brief
 */
void esp32_port_input_test(void);

/**
 * @fn void esp32_read_inputs_local(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 */
void esp32_read_inputs_local(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_write_outputs_local(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 */
void esp32_write_outputs_local(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_read_inputs_remote(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 */
void esp32_read_inputs_remote(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_write_outputs_remote(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 */
void esp32_write_outputs_remote(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool esp32_on_scan_end(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
bool esp32_on_scan_end(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool esp32_on_instruction(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
bool esp32_on_instruction(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool esp32_on_task_before(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
bool esp32_on_task_before(ladder_ctx_t *ladder_ctx);

/**
 * @fn bool esp32_on_task_after(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 * @return
 */
bool esp32_on_task_after(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_on_panic(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 */
void esp32_on_panic(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_on_end_task(ladder_ctx_t*)
 * @brief
 *
 * @param ladder_ctx
 */
void esp32_on_end_task(ladder_ctx_t *ladder_ctx);

/**
 * @fn void esp32_delay(long)
 * @brief
 *
 * @param msec
 */
void esp32_delay(long msec);

/**
 * @fn uint64_t esp32_millis(void)
 * @brief
 *
 * @return
 */
uint64_t esp32_millis(void);

#endif /* PORT_ESP32_H_ */
