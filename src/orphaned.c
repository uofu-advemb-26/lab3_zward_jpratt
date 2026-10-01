#include "orphaned.h"
#include <stdint.h>

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


void fixed_lock(SemaphoreHandle_t semaphore, int *counter, OrphanedLockFunc_t output_logic)
  {
      while (1)
      {
          fixed_lock_iteration(semaphore, counter, portMAX_DELAY, output_logic);

          // Don't starve the other tasks
          vTaskDelay(1);
      }
  }

// Return the new value of the counter.
// If the semaphore couldn't be acquired, return -1
int fixed_lock_iteration(SemaphoreHandle_t semaphore, int *counter, const TickType_t semaphore_delay, OrphanedLockFunc_t output_logic)
{
    int local_count = 0;

    if (xSemaphoreTake(semaphore, semaphore_delay) != pdTRUE) {
        return -1;
    }
    {
        local_count = ++(*counter);
    }
    xSemaphoreGive(semaphore);

    // Output happens outside the lock, so the odd/even branch can't skip the give
    if (!(local_count % 2)) 
    {
        output_logic(local_count);
    }

    return local_count;
}