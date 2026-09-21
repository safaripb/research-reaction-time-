# Register the application's main component.
idf_component_register(

    # Compile the application's main source file.
    SRCS "main.c"

    # Allow headers from the main directory.
    INCLUDE_DIRS "."

    # main depends on our button component plus
    # GPIO, timers, random-number support, and FreeRTOS.
    REQUIRES button esp_driver_gpio esp_timer esp_hw_support freertos
)