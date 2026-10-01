#include "test_orphaned.h"
#include "orphaned.h"

int test_function_output = -1;

// Test even operation of the orphaned function (assuming deadlock is expected behavior)
void test_orphaned_lock_even()
{
    int state = 0;
    test_function_output = -1;

    // Run the code with the testing semaphore, a timeout, and the custom testing output function
    orphaned_lock_iteration(semaphore, &state, 10, orphaned_output_test);

    // Should not have released the semaphore
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, uxSemaphoreGetCount(semaphore), "orphaned_lock() released semaphore (even)");
    // Should have incremented the counter
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, state, "orphaned_lock() didn't increment counter (even)");
    // Should not have run the output function
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, test_function_output, "orphaned_lock() ran output function (even)");
}

// Test odd operation of the orphaned function (no deadlock in this case)
void test_orphaned_lock_odd()
{
    int state = 1;
    test_function_output = -1;

    // Run the code with the testing semaphore, a timeout, and the custom testing output function
    orphaned_lock_iteration(semaphore, &state, 10, orphaned_output_test);

    // Should have released the semaphore
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, uxSemaphoreGetCount(semaphore), "orphaned_lock() didn't release semaphore (odd)");
    // Should have incremented the counter
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, state, "orphaned_lock() didn't increment counter (odd)");
    // Should have run the output function
    TEST_ASSERT_EQUAL_INT_MESSAGE(state, test_function_output, "orphaned_lock() didn't run output function (odd)");
}

// Used to test when orphaned_lock runs the output function
void orphaned_output_test(int counter)
{
    // Indicate this function has run by changing the global output variable to the current counter value
    test_function_output = counter;
}

// Test whether the original code deadlocks by letting it run and then seeing if it's blocked
void test_orphaned_lock_deadlocks()
{
    // Launch the original code in its own thread
    TaskHandle_t thread1;
    xTaskCreate(orphaned_lock_wrapper, "OrphanedLock", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &thread1);

    // Give it sufficient time to deadlock
    vTaskDelay(100);

    // Test whether it has deadlocked
    TEST_ASSERT_EQUAL_MESSAGE(eBlocked, eTaskGetState(thread1), "Thread 1 isn't blocked");

    vTaskDelete(thread1);
}

void orphaned_lock_wrapper(void *vargs)
{
    // Call with the global semaphore and counter setup prior to the test in setUp()
    orphaned_lock(semaphore, &counter);
}

// Fixed versions of the orphaned lock function tests

// Counter becomes odd: releases the semaphore, no output
void test_fixed_lock_odd()
{
    int state = 0;
    test_function_output = -1;

    int new_count = fixed_lock_iteration(semaphore, &state, 10, orphaned_output_test);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, new_count, "fixed_lock() returned wrong count (odd)");

    //the semaphore is released even on odd counts
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, uxSemaphoreGetCount(semaphore), "fixed_lock() didn't release semaphore (odd)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, state, "fixed_lock() didn't increment counter (odd)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, test_function_output, "fixed_lock() ran output function (odd)");
}

// The counter becomes even and releases the semaphore and outputs
void test_fixed_lock_even()
{
    int state = 1;
    test_function_output = -1;

    int new_count = fixed_lock_iteration(semaphore, &state, 10, orphaned_output_test);

    TEST_ASSERT_EQUAL_INT_MESSAGE(2, new_count, "fixed_lock() returned wrong count (even)");

    //the semaphore is released on even counts
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, uxSemaphoreGetCount(semaphore), "fixed_lock() didn't release semaphore (even)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, state, "fixed_lock() didn't increment counter (even)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(state, test_function_output, "fixed_lock() didn't run output function (even)");
}

// The semaphore is already held so it returns -1 without touching anything
void test_fixed_lock_busy()
{
    int state = 1;
    int new_count = 0;
    test_function_output = -1;

    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        new_count = fixed_lock_iteration(semaphore, &state, 10, orphaned_output_test);
    }
    xSemaphoreGive(semaphore);

    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, new_count, "fixed_lock() didn't return -1 while semaphore was held");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, state, "fixed_lock() incremented counter while semaphore was held");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, test_function_output, "fixed_lock() ran output function while semaphore was held");
}

void test_fixed_lock_does_not_deadlock()
{
    // Launch the fixed code in its own thread
    TaskHandle_t thread1;
    xTaskCreate(fixed_lock_wrapper, "FixedLock", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &thread1);

    // Let the thread run, then sample the counter twice
    vTaskDelay(50);
    int first_count = counter;
    
    vTaskDelay(50);
    int second_count = counter;

    vTaskSuspend(thread1);
    TEST_ASSERT_EQUAL_MESSAGE(eSuspended, eTaskGetState(thread1), "Fixed thread didn't suspend");

    // A deadlocked thread would be stuck at the same value
    TEST_ASSERT_GREATER_THAN_INT_MESSAGE(first_count, second_count, "Fixed thread stopped making progress");

    vTaskDelete(thread1);
}

void fixed_lock_wrapper(void *vargs)
  {
      // Call with the global semaphore and counter setup
      fixed_lock(semaphore, &counter, orphaned_output_test);
  }