#include "test_safe_increment.h"

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