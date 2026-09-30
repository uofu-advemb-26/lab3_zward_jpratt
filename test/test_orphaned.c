#include test_orphaned.h
#include orphaned.c


void test_orphaned_lock_deadlocks()
{
    TaskHandle_t thread1;
    xTaskCreate(orphaned_lock, "OrphanedLock", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &thread1);
    
    vTaskDelay(100);

    TEST_ASSERT_EQUAL_MESSAGE(eBlocked, eTaskGetState(thread1), "Thread 1 isn't blocked");

    vTaskDelete(thread1);
}