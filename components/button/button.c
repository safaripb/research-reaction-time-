#include "button.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"


#define BUTTON_PIN GPIO_NUM_27


static TaskHandle_t reaction_task_handle = NULL;
static volatile int64_t button_time_us = 0;


static void button_isr_handler(void *arg)
{
    (void)arg;

    if (button_time_us == 0)
    {
        button_time_us = esp_timer_get_time();

        BaseType_t higher_priority_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR(
            reaction_task_handle,
            &higher_priority_task_woken
        );

        if (higher_priority_task_woken == pdTRUE)
        {
            portYIELD_FROM_ISR();
        }
    }
}


esp_err_t button_init(TaskHandle_t task_to_notify)
{
    reaction_task_handle = task_to_notify;

    gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BUTTON_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    esp_err_t err = gpio_config(&button_config);
    if (err != ESP_OK)
    {
        return err;
    }

    err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    err = gpio_isr_handler_add(
        BUTTON_PIN,
        button_isr_handler,
        NULL
    );
    if (err != ESP_OK)
    {
        return err;
    }

    gpio_intr_disable(BUTTON_PIN);
    return ESP_OK;
}


void button_enable_interrupt(void)
{
    gpio_intr_enable(BUTTON_PIN);
}


void button_disable_interrupt(void)
{
    gpio_intr_disable(BUTTON_PIN);
}


void button_reset_event(void)
{
    button_time_us = 0;
}


int64_t button_get_press_time_us(void)
{
    return button_time_us;
}


bool button_is_pressed(void)
{
    return gpio_get_level(BUTTON_PIN) == 0;
}
