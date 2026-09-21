#include "button.h"

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define LED_PIN GPIO_NUM_25
#define TOTAL_TRIALS 10
#define PREP_DELAY_MIN_MS 1000
#define PREP_DELAY_RANGE_MS 3001
#define POLL_INTERVAL_MS 10

static const char *TAG = "REACTION";

static void wait_for_button_release(void)
{
    while (button_is_pressed())
    {
        vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
    }
}

static bool wait_for_clean_prep_delay(uint32_t delay_ms)
{
    uint32_t elapsed_ms = 0;

    while (elapsed_ms < delay_ms)
    {
        if (button_is_pressed())
        {
            return false;
        }

        uint32_t step_ms = delay_ms - elapsed_ms;
        if (step_ms > POLL_INTERVAL_MS)
        {
            step_ms = POLL_INTERVAL_MS;
        }

        vTaskDelay(pdMS_TO_TICKS(step_ms));
        elapsed_ms += step_ms;
    }

    return true;
}


void app_main(void)
{
    gpio_config_t led_config = {
        .pin_bit_mask = 1ULL << LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&led_config));
    ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));
    ESP_ERROR_CHECK(button_init(xTaskGetCurrentTaskHandle()));


    double total_reaction_ms = 0.0;
    double best_reaction_ms = 1000000.0;
    double worst_reaction_ms = 0.0;
    double total_wake_latency_us = 0.0;
    int false_starts = 0;


    ESP_LOGI(TAG, "Starting modular interrupt reaction test");


    for (int trial = 1; trial <= TOTAL_TRIALS; trial++)
    {
        wait_for_button_release();
        ulTaskNotifyTake(pdTRUE, 0);
        button_reset_event();


        ESP_LOGI(
            TAG,
            "Trial %d/%d - GET READY...",
            trial,
            TOTAL_TRIALS
        );


        uint32_t random_delay_ms =
            PREP_DELAY_MIN_MS + (esp_random() % PREP_DELAY_RANGE_MS);

        if (!wait_for_clean_prep_delay(random_delay_ms))
        {
            false_starts++;
            ESP_LOGW(
                TAG,
                "False start detected. Release the button to retry trial %d.",
                trial
            );
            trial--;
            continue;
        }


        gpio_set_level(LED_PIN, 1);

        int64_t start_time_us = esp_timer_get_time();
        button_enable_interrupt();

        ESP_LOGI(TAG, "GO!");


        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);


        int64_t task_wake_time_us = esp_timer_get_time();

        button_disable_interrupt();
        gpio_set_level(LED_PIN, 0);


        int64_t button_time_us =
            button_get_press_time_us();

        int64_t reaction_time_us =
            button_time_us - start_time_us;

        double reaction_time_ms =
            reaction_time_us / 1000.0;

        int64_t wake_latency_us =
            task_wake_time_us - button_time_us;


        total_reaction_ms += reaction_time_ms;
        total_wake_latency_us += wake_latency_us;


        if (reaction_time_ms < best_reaction_ms)
        {
            best_reaction_ms = reaction_time_ms;
        }


        if (reaction_time_ms > worst_reaction_ms)
        {
            worst_reaction_ms = reaction_time_ms;
        }


        ESP_LOGI(
            TAG,
            "Reaction: %.1f ms",
            reaction_time_ms
        );

        ESP_LOGI(
            TAG,
            "ISR -> task latency: %lld us",
            (long long)wake_latency_us
        );


        vTaskDelay(pdMS_TO_TICKS(1500));
    }


    double average_reaction_ms =
        total_reaction_ms / TOTAL_TRIALS;

    double average_wake_latency_us =
        total_wake_latency_us / TOTAL_TRIALS;


    ESP_LOGI(TAG, "------------------------------");

    ESP_LOGI(
        TAG,
        "Average reaction: %.1f ms",
        average_reaction_ms
    );

    ESP_LOGI(
        TAG,
        "Best reaction: %.1f ms",
        best_reaction_ms
    );

    ESP_LOGI(
        TAG,
        "Worst reaction: %.1f ms",
        worst_reaction_ms
    );

    ESP_LOGI(
        TAG,
        "Average ISR -> task latency: %.1f us",
        average_wake_latency_us
    );

    ESP_LOGI(
        TAG,
        "False starts: %d",
        false_starts
    );

    ESP_LOGI(TAG, "------------------------------");
}
