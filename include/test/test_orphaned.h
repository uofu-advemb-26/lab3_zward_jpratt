#ifndef TEST_ORPHANED_H
#define TEST_ORPHANED_H

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <unity.h>

extern SemaphoreHandle_t semaphore;
extern int counter;

void test_orphaned_lock_even();
void test_orphaned_lock_odd();
void orphaned_output_test(int counter);

void test_orphaned_lock_deadlocks();
void orphaned_lock_wrapper(void *vargs);

#endif