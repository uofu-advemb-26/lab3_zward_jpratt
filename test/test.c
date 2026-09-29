#include <stdio.h>
#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include <semphr.h>
#include "safe.h"

SemaphoreHandle_t count_semaphore;
SemaphoreHandle_t uart_semaphore;

void setUp(void)
{
    count_semaphore = xSemaphoreCreateCounting(1, 1);
    uart_semaphore = xSemaphoreCreateCounting(1, 1);
}

void tearDown(void)
{
    vSemaphoreDelete(count_semaphore);
    vSemaphoreDelete(uart_semaphore);
}

void test_safe_increment_state_busy()
{
    int count = 1;

    // Acquire a semaphore prior to the test
    xSemaphoreTake(count_semaphore, portMAX_DELAY);
    {
        // Attempt to increment count while the semaphore is held (with a timeout)
        safe_increment(&count, count_semaphore, 10);
    }
    xSemaphoreGive(count_semaphore);

    // The counter shouldn't have incremented while the semaphore was held
    TEST_ASSERT_EQUAL_MESSAGE(1, count, "Count state incremented while semaphore was held");
}


void test_safe_increment_return_busy()
{
    int count = 1;
    int new_count = 0;

    // Acquire a semaphore prior to the test
    xSemaphoreTake(count_semaphore, portMAX_DELAY);
    {
        // Attempt to increment count while the semaphore is held (and timeout)
        new_count = safe_increment(&count, count_semaphore, 10);
    }
    xSemaphoreGive(count_semaphore);

    // The counter shouldn't have incremented while the semaphore was held
    TEST_ASSERT_EQUAL_MESSAGE(-1, new_count, "New counter value incremented while semaphore was held");
}

int main (void)
{
    stdio_init_all();
    while (1) {
        sleep_ms(5000); // Give time for TTY to attach.
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_safe_increment_state_busy);
        RUN_TEST(test_safe_increment_return_busy);
        sleep_ms(5000);
        UNITY_END();
    }
}
