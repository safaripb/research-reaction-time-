# Register the button folder as an ESP-IDF component.
idf_component_register(

    # button.c contains this component's implementation.
    SRCS "button.c"

    # Files inside include/ can be included by other components.
    INCLUDE_DIRS "include"

    # The button component depends on GPIO, esp_timer, and FreeRTOS.
    REQUIRES esp_driver_gpio esp_timer freertos
)