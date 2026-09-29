#ifndef _TEST_SAFE_INCREMENT_H_
#define _TEST_SAFE_INCREMENT_H_

#include <FreeRTOS.h>
#include <unity.h>
#include <semphr.h>
#include "safe.h"

extern SemaphoreHandle_t semaphore;
extern int counter;

void test_increment_updates_counter();
void test_increment_updates_count();
void test_increment_multiple_times();
void test_increment_from_nonzero_count();
void test_safe_increment_state_busy();
void test_safe_increment_return_busy();

#endif