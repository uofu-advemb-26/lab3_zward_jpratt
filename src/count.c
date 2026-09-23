#include "count.h"

// Increment a shared variable in a thread safe manner.
// Return the new value of that variable
int safe_increment(int *counter, SemaphoreHandle_t semaphore)
{
    int local_count = 0;

    // Critical section: capture and increment the counter
    // to minimize critical section code
    // NOTE: setting the time-out to portMAX_DELAY means that 
    // xSemaphoreTake only returns when the semaphore was acquired,
    // so there's no need to check the return value.
    xSemaphoreTake(semaphore, portMAX_DELAY);
    {
        local_count = ++(*counter);
    }
    xSemaphoreGive(semaphore);

    return local_count;
}
