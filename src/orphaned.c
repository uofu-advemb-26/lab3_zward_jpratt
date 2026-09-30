#include "orphaned.h"

extern SemaphoreHandle_t semaphore;
extern int counter;

void orphaned_lock(void *vargs)
{
    while (1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        counter++;
        if (counter % 2) {
            continue;
        }
        printf("Count %d\n", counter);
        xSemaphoreGive(semaphore);
    }
}