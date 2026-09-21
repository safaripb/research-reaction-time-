#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "freertos/task.h"

esp_err_t button_init(TaskHandle_t task_to_notify);

void button_enable_interrupt(void);

void button_disable_interrupt(void);

void button_reset_event(void);

int64_t button_get_press_time_us(void);

bool button_is_pressed(void);
