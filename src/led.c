#include "led.h"

// Commit the LED's current state and return the LED's next state
int update_led(const int current_state)
{
    // Commit the LED's current state
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, current_state);

    // Update the LED's next state
    return !current_state;
}