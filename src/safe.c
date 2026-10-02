#include "safe.h"

// Increment a shared variable in a thread safe manner.
// Return the new value of that variable;
// If the semaphore couldn't be acquired, return -1
int safe_increment(int *counter, SemaphoreHandle_t semaphore, TickType_t wait_ticks)
{
    int local_count = -1;

    // Critical section: capture and increment the counter
    if (xSemaphoreTake(semaphore, wait_ticks) == pdTRUE)
    {
        local_count = ++(*counter);
        xSemaphoreGive(semaphore);
    }

    return local_count;
}

void safe_hello(const char *thread_name, const int count, SemaphoreHandle_t semaphore)
{
    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        printf("hello world from %s! Count %d\n", thread_name, count);
    }
    xSemaphoreGive(semaphore);
}