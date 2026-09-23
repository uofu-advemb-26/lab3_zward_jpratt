#ifndef _COUNT_H_
#define _COUNT_H_

#include <FreeRTOS.h>
#include <semphr.h>

int safe_increment(int *counter, SemaphoreHandle_t semaphore);

#endif