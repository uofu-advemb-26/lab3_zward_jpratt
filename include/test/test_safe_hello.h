#ifndef _TEST_SAFE_HELLO_H_
#define _TEST_SAFE_HELLO_H_

#include <FreeRTOS.h>
#include <unity.h>
#include <semphr.h>
#include "safe.h"

extern SemaphoreHandle_t semaphore;
extern int counter;

void test_safe_hello_releases_semaphore();
void test_safe_hello_does_not_change_counter();

#endif