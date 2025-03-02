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

#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ladder.h"
#include "port_esp32.h"

static const char* TAG = "port_esp32";

bool esp32_port_initialize(void) { return true; }

int esp32_delay(long msec) {
    vTaskDelay(msec / portTICK_PERIOD_MS);

    return 1;
}

uint64_t esp32_millis(void) { return esp_timer_get_time() / 1000; }

void esp32_read_inputs_local(ladder_ctx_t* ladder_ctx) {}

void esp32_write_outputs_local(ladder_ctx_t* ladder_ctx) {}

void esp32_read_inputs_remote(ladder_ctx_t* ladder_ctx) {}

void esp32_write_outputs_remote(ladder_ctx_t* ladder_ctx) {}

bool esp32_on_scan_end(ladder_ctx_t* ladder_ctx) { return false; }

bool esp32_on_instruction(ladder_ctx_t* ladder_ctx) { return false; }

bool esp32_on_task_before(ladder_ctx_t* ladder_ctx) { return false; }

bool esp32_on_task_after(ladder_ctx_t* ladder_ctx) {
    if ((*ladder_ctx).scan_internals.actual_scan_time < 1)
        esp32_delay(1);

    return false;
}

void esp32_on_panic(ladder_ctx_t* ladder_ctx) {
     ESP_LOGI(TAG, "Panic");
}

void esp32_on_end_task(ladder_ctx_t* ladder_ctx) {
    ESP_LOGI(TAG, "End Task Ladder");
    vTaskDelete(NULL);
}
