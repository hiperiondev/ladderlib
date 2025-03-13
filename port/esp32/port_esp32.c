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

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ladder.h"
#include "port_esp32.h"

static const char *TAG = "port_esp32";

static const uint32_t inputs[] = {
    INPUT_00, //
    INPUT_01, //
    INPUT_02, //
    INPUT_03, //
    INPUT_04, //
    INPUT_05, //
    INPUT_06, //
    INPUT_07, //
};

static const uint32_t outputs[] = {
    OUTPUT_00, //
    OUTPUT_01, //
    OUTPUT_02, //
    OUTPUT_03, //
    OUTPUT_04, //
    OUTPUT_05, //
};

bool esp32_port_initialize(ladder_ctx_t *ladder_ctx) {
    gpio_config_t GPIO_Config;

    if ((*ladder_ctx).ladder.quantity.i > 8 - (*ladder_ctx).ladder.quantity.iw || (*ladder_ctx).ladder.quantity.q > 6 - (*ladder_ctx).ladder.quantity.qw ||
        (*ladder_ctx).ladder.quantity.iw > 2 || (*ladder_ctx).ladder.quantity.qw > 2) {
        ESP_LOGI(TAG, "ERROR quantity");
        return false;
    }

    GPIO_Config.pin_bit_mask = (1ULL << INPUT_00) | (1ULL << INPUT_01) | (1ULL << INPUT_02) | (1ULL << INPUT_03) | (1ULL << INPUT_04) | (1ULL << INPUT_05) |
                               ((*ladder_ctx).ladder.quantity.iw > 0 ? 0 : (1ULL << INPUT_06)) |
                               ((*ladder_ctx).ladder.quantity.iw > 1 ? 0 : (1ULL << INPUT_07));
    GPIO_Config.pull_up_en = GPIO_PULLUP_DISABLE;
    GPIO_Config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    GPIO_Config.mode = GPIO_MODE_INPUT;
    GPIO_Config.intr_type = GPIO_INTR_DISABLE;
    if (gpio_config(&GPIO_Config) != ESP_OK) {
        ESP_LOGI(TAG, "ERROR input");
        return false;
    }

    GPIO_Config.pin_bit_mask = (1ULL << OUTPUT_00) | (1ULL << OUTPUT_01) | (1ULL << OUTPUT_02) | (1ULL << OUTPUT_03) |
                               ((*ladder_ctx).ladder.quantity.qw > 0 ? 0 : (1ULL << OUTPUT_04)) |
                               ((*ladder_ctx).ladder.quantity.qw > 1 ? 0 : (1ULL << OUTPUT_05));
    GPIO_Config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    GPIO_Config.pull_up_en = GPIO_PULLUP_DISABLE;
    GPIO_Config.mode = GPIO_MODE_OUTPUT;
    GPIO_Config.intr_type = GPIO_INTR_DISABLE;
    if (gpio_config(&GPIO_Config) != ESP_OK) {
        ESP_LOGI(TAG, "ERROR output");
        return false;
    }

    return true;
}

void esp32_port_output_test(void) {
    ladder_ctx_t ladder_ctx;

    ladder_ctx.ladder.quantity.i = 8;
    ladder_ctx.ladder.quantity.q = 6;
    ladder_ctx.ladder.quantity.iw = 0;
    ladder_ctx.ladder.quantity.qw = 0;

    if (!esp32_port_initialize(&ladder_ctx)) {
        ESP_LOGI(TAG, "ERROR esp32 port initialize");
        return;
    }

    for (uint32_t qs = 0; qs < ladder_ctx.ladder.quantity.q; qs++) {
        ESP_LOGI(TAG, "test port %d", (int)qs);
        gpio_set_level(outputs[qs], 1);
        esp32_delay(1000);
        gpio_set_level(outputs[qs], 0);
    }
}

void esp32_port_input_test(void) {
    uint64_t start = 0;

    ladder_ctx_t ladder_ctx;
    bool port = false;
    bool ports[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    ladder_ctx.ladder.quantity.i = 8;
    ladder_ctx.ladder.quantity.q = 6;
    ladder_ctx.ladder.quantity.iw = 0;
    ladder_ctx.ladder.quantity.qw = 0;

    if (!esp32_port_initialize(&ladder_ctx)) {
        ESP_LOGI(TAG, "ERROR esp32 port initialize");
        return;
    }

    ESP_LOGI(TAG, "(Stop after 10 sec of no input)");
    start = esp32_millis();
    while (1) {
        for (uint32_t is = 0; is < ladder_ctx.ladder.quantity.i; is++) {
            port = gpio_get_level(inputs[is]);
            if (port != ports[is]) {
                ports[is] = port;
                ESP_LOGI(TAG, "PORT %d %s", (int)is, (port == 0 ? "OFF" : "ON"));
                start = esp32_millis();
            }
        }
        esp32_delay(1);

        if (esp32_millis() - start > 10000)
            break;
    }
}

void esp32_delay(long msec) {
    vTaskDelay(msec / portTICK_PERIOD_MS);
}

uint64_t esp32_millis(void) {
    return esp_timer_get_time() / 1000;
}

void esp32_read_inputs_local(ladder_ctx_t *ladder_ctx) {
    for (uint32_t is = 0; is < (*ladder_ctx).ladder.quantity.i; is++)
        (*ladder_ctx).memory.I[is] = gpio_get_level(inputs[is]);
}

void esp32_write_outputs_local(ladder_ctx_t *ladder_ctx) {
    for (uint32_t qs = 0; qs < (*ladder_ctx).ladder.quantity.q; qs++)
        gpio_set_level(outputs[qs], (*ladder_ctx).memory.Q[qs]);
}

void esp32_read_inputs_remote(ladder_ctx_t *ladder_ctx) {
}

void esp32_write_outputs_remote(ladder_ctx_t *ladder_ctx) {
}

bool esp32_on_scan_end(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool esp32_on_instruction(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool esp32_on_task_before(ladder_ctx_t *ladder_ctx) {
    return false;
}

bool esp32_on_task_after(ladder_ctx_t *ladder_ctx) {
    if ((*ladder_ctx).scan_internals.actual_scan_time < 1)
        esp32_delay(1);

    return false;
}

void esp32_on_panic(ladder_ctx_t *ladder_ctx) {
    ESP_LOGI(TAG, "Panic");
}

void esp32_on_end_task(ladder_ctx_t *ladder_ctx) {
    ESP_LOGI(TAG, "End Task Ladder");
    vTaskDelete(NULL);
}
