#ifndef _COUNT_H_
#define _COUNT_H_

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>

int safe_increment(int *counter, SemaphoreHandle_t semaphore, TickType_t wait_ticks);
void safe_hello(const char *thread_id, const int count, SemaphoreHandle_t semaphore);

#endif