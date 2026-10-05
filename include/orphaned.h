#ifndef ORPHANED_H
#define ORPHANED_H

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>

typedef void (*OrphanedLockFunc_t)(const int);

void orphaned_lock(SemaphoreHandle_t semaphore, int *counter);
void orphaned_lock_iteration(SemaphoreHandle_t semaphore, int *counter, const TickType_t semaphore_delay, OrphanedLockFunc_t output_logic);
void orphaned_output(int counter);
void fixed_lock(SemaphoreHandle_t semaphore, int *counter, OrphanedLockFunc_t output_logic);
int fixed_lock_iteration(SemaphoreHandle_t semaphore, int *counter, const TickType_t semaphore_delay, OrphanedLockFunc_t output_logic);

#endif