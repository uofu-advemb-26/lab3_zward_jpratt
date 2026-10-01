#include "orphaned.h"

void orphaned_lock(SemaphoreHandle_t semaphore, int *counter)
{
    while (1)
    {
        orphaned_lock_iteration(semaphore, counter, portMAX_DELAY, orphaned_output);
    }
}

void orphaned_lock_iteration(SemaphoreHandle_t semaphore, int *counter, const TickType_t semaphore_delay, OrphanedLockFunc_t output_logic)
{
    xSemaphoreTake(semaphore, semaphore_delay);
    (*counter)++;
    if ((*counter) % 2) {
        return;
    }
    output_logic(*counter);
    xSemaphoreGive(semaphore);
}

void orphaned_output(int counter)
{
    printf("Count %d\n", counter);
}