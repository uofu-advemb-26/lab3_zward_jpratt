#ifndef _DEADLOCKS_H_
#define _DEADLOCKS_H_

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>

// The two threads have the same priority
#define THREAD1_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define THREAD1_STACK_SIZE configMINIMAL_STACK_SIZE
#define THREAD2_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define THREAD2_STACK_SIZE configMINIMAL_STACK_SIZE

typedef struct {
    SemaphoreHandle_t lock_a;
    SemaphoreHandle_t lock_b;
} DeadlockParams_t;

void create_deadlock(const DeadlockParams_t *deadlock_params, TaskHandle_t *thread1, TaskHandle_t *thread2);
void do_thread1(void *void_params);
void do_thread2(void *void_params);

#endif