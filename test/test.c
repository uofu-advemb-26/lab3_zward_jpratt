#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "safe.h"

SemaphoreHandle_t semaphore;
int counter;

void setUp(void) {
    semaphore = xSemaphoreCreateCounting(1, 1);
    counter = 0;
}

void tearDown(void) 
{
    xSemaphoreDelete(semaphore);
}

// Test safe_increment function to ensure it correctly increments the counter and returns the new value.
void test_increment_updates_counter()
{
    int count = safe_increment(&counter, semaphore);
    TEST_ASSERT_EQUAL_INT(1, counter);
}

void test_increment_updates_count()
{
    int count = safe_increment(&counter, semaphore);
    TEST_ASSERT_EQUAL_INT(1, count);
}

void test_increment_multiple_times()
{
    for (int i = 0; i < 5; i++) {
        safe_increment(&counter, semaphore);
    }
    TEST_ASSERT_EQUAL_INT(5, counter);
}

void test_increment_from_nonzero_count()
{
    counter = 10;
    int count = safe_increment(&counter, semaphore);
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

void test_xSemaphore_status(){
    xSemamoreTake(semaphore, portMAX_DELAY);
    {
        TESTASSERT_EQUAL(TRUE, xSemaphoreTake)
    }
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
        sleep_ms(5000);
        UNITY_END();
    }
}
