#include "deadlocks.h"

void create_deadlock(const DeadlockParams_t *deadlock_params, TaskHandle_t *thread1, TaskHandle_t *thread2)
{
    xTaskCreate(do_thread1, "Thread1",
                THREAD1_STACK_SIZE, (void *)deadlock_params, THREAD1_PRIORITY, thread1);
    xTaskCreate(do_thread2, "Thread2",
                THREAD2_STACK_SIZE, (void *)deadlock_params, THREAD2_PRIORITY, thread2);
}

void do_thread1(void *void_params)
{
    DeadlockParams_t *params = (DeadlockParams_t *)void_params;

    // Take Lock A, then Lock B
    xSemaphoreTake(params->lock_a, portMAX_DELAY);
    {
        // Give enough time for the other thread to run
        vTaskDelay(100);

        // Attempt to take and release the second lock
        xSemaphoreTake(params->lock_b, portMAX_DELAY);
        xSemaphoreGive(params->lock_b);
    }
    xSemaphoreGive(params->lock_a);
}

void do_thread2(void *void_params)
{
    DeadlockParams_t *params = (DeadlockParams_t *)void_params;

    // Take Lock B, then Lock A
    xSemaphoreTake(params->lock_b, portMAX_DELAY);
    {
        // Give enough time for the other thread to run
        vTaskDelay(100);

        // Attempt to take and release the second lock
        xSemaphoreTake(params->lock_a, portMAX_DELAY);
        xSemaphoreGive(params->lock_a);
    }
    xSemaphoreGive(params->lock_b);
}