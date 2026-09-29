#include "test_deadlocks.h"

DeadlockParams_t deadlock_params = {0};

void test_deadlock()
{
    // Initialize semaphores
    deadlock_params.lock_a = xSemaphoreCreateCounting(1, 1);
    deadlock_params.lock_b = xSemaphoreCreateCounting(1, 1);

    // Launch the tasks to create the deadlock; save task handles
    TaskHandle_t thread1, thread2;
    create_deadlock(&deadlock_params, &thread1, &thread2);

    // Give some delay for the deadlock to occur
    vTaskDelay(500);

    // Check that both tasks are in the blocked state
    TEST_ASSERT_EQUAL_MESSAGE(eBlocked, eTaskGetState(thread1), "Thread 1 isn't blocked");
    TEST_ASSERT_EQUAL_MESSAGE(eBlocked, eTaskGetState(thread2), "Thread 2 isn't blocked");

    // Clean up tasks
    vTaskDelete(thread1);
    vTaskDelete(thread2);

    // Clean up semaphores
    vSemaphoreDelete(deadlock_params.lock_a);
    vSemaphoreDelete(deadlock_params.lock_b);
}
