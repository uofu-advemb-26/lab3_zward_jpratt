#include "test_orphaned.h"
#include "orphaned.h"

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