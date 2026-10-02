#include "test_safe_hello.h"

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