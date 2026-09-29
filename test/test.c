#include <stdio.h>
#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include <semphr.h>
#include "safe.h"

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

// Test safe_increment function to ensure it correctly increments the counter and returns the new value.
void test_increment_updates_counter()
{
    safe_increment(&counter, semaphore, 10);
    TEST_ASSERT_EQUAL_INT(1, counter);
}

void test_increment_updates_count()
{
    int count = safe_increment(&counter, semaphore, 10);
    TEST_ASSERT_EQUAL_INT(1, count);
}

void test_increment_multiple_times()
{
    for (int i = 0; i < 5; i++) {
        safe_increment(&counter, semaphore, 10);
    }
    TEST_ASSERT_EQUAL_INT(5, counter);
}

void test_increment_from_nonzero_count()
{
    counter = 10;
    int count = safe_increment(&counter, semaphore, 10);
    TEST_ASSERT_EQUAL_INT(11, counter);
    TEST_ASSERT_EQUAL_INT(11, count);
}

// Test safe hello function to ensure it releases the semaphore after execution.
void test_safe_hello_releases_semaphore()
{
    safe_hello("TestThread", 12, semaphore);

    // After calling safe_hello, the semaphore should be available for other threads.
    TEST_ASSERT_EQUAL_INT(1, uxSemaphoreGetCount(semaphore));
}

void test_safe_hello_does_not_change_counter()
{
    int initial_counter = counter;
    safe_hello("TestThread", 8, semaphore);
    TEST_ASSERT_EQUAL_INT(initial_counter, counter);
}

void test_xSemaphore_status()
{
    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        // The semaphore should not be available
        TEST_ASSERT_EQUAL_INT(0, uxSemaphoreGetCount(semaphore));
    }
    xSemaphoreGive(semaphore);
}

void test_safe_increment_state_busy()
{
    int count = 1;

    // Acquire a semaphore prior to the test
    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        // Attempt to increment count while the semaphore is held (with a timeout)
        safe_increment(&count, semaphore, 10);
    }
    xSemaphoreGive(semaphore);

    // The counter shouldn't have incremented while the semaphore was held
    TEST_ASSERT_EQUAL_MESSAGE(1, count, "Count state incremented while semaphore was held");
}

void test_safe_increment_return_busy()
{
    int count = 1;
    int new_count = 0;

    // Acquire a semaphore prior to the test
    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        // Attempt to increment count while the semaphore is held (and timeout)
        new_count = safe_increment(&count, semaphore, 10);
    }
    xSemaphoreGive(semaphore);

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
        RUN_TEST(test_increment_updates_counter);
        RUN_TEST(test_increment_updates_count);
        RUN_TEST(test_increment_multiple_times);
        RUN_TEST(test_increment_from_nonzero_count);
        RUN_TEST(test_safe_hello_releases_semaphore);
        RUN_TEST(test_safe_hello_does_not_change_counter);
        RUN_TEST(test_xSemaphore_status);
        RUN_TEST(test_safe_increment_state_busy);
        RUN_TEST(test_safe_increment_return_busy);
        UNITY_END();
        sleep_ms(5000);
    }
}
