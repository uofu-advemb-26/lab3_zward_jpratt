#include <stdio.h>
#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include <semphr.h>
#include "test_safe_increment.h"
#include "test_safe_hello.h"
#include "test_led.h"
#include "test_deadlocks.h"
#include "test_orphaned.h"

// Same priority as deadlocking threads
#define MAIN_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MAIN_STACK_SIZE configMINIMAL_STACK_SIZE

SemaphoreHandle_t semaphore;
int counter;

void setUp(void) {
    semaphore = xSemaphoreCreateCounting(1, 1);
    counter = 0;
}

void tearDown(void)
{
    vSemaphoreDelete(semaphore);
}

// Generic semaphore test
void test_xSemaphore_status()
{
    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        // The semaphore should not be available
        TEST_ASSERT_EQUAL_INT(0, uxSemaphoreGetCount(semaphore));
    }
    xSemaphoreGive(semaphore);
}

void run_tests()
{
    while (1) {
        sleep_ms(5000); // Give time for TTY to attach.
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_xSemaphore_status);

        RUN_TEST(test_safe_increment_releases_semaphore);
        RUN_TEST(test_increment_updates_counter);
        RUN_TEST(test_increment_updates_count);
        RUN_TEST(test_increment_multiple_times);
        RUN_TEST(test_increment_from_nonzero_count);
        RUN_TEST(test_safe_increment_state_busy);
        RUN_TEST(test_safe_increment_return_busy);

        RUN_TEST(test_safe_hello_releases_semaphore);
        RUN_TEST(test_safe_hello_does_not_change_counter);

        RUN_TEST(test_led_off);
        RUN_TEST(test_led_on);

        RUN_TEST(test_deadlock);

        RUN_TEST(test_orphaned_lock_deadlocks);
        UNITY_END();
        sleep_ms(5000);
    }
}

int main (void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);

    xTaskCreate(run_tests, "Test Thread",
                MAIN_STACK_SIZE, NULL, MAIN_PRIORITY, NULL);
    vTaskStartScheduler();
}
