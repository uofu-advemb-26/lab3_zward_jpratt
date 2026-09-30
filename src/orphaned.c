#include "orphaned.h"

void orphaned_lock(void)
{
    while (1) {
        xSemaphoreTake(&semaphore, portMAX_DELAY);
        counter++;
        if (counter % 2) {
            continue;
        }
        printf("Count %d\n", counter);
        xSemaphoreGive(&semaphore);
    }
}