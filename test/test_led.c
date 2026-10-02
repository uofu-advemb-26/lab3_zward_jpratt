#include "test_led.h"

// Test turning the LED off
void test_led_off() {
    int current_state = 1;
    int next_state = update_led(current_state);
    TEST_ASSERT_FALSE_MESSAGE(next_state, "Next LED state after on isn't off");
}

// Test turning the LED on
void test_led_on() {
    int current_state = 0;
    int next_state = update_led(current_state);
    TEST_ASSERT_TRUE_MESSAGE(next_state, "Next LED state after off isn't on");
}