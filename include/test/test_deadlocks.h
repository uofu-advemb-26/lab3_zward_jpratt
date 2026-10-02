#ifndef _TEST_DEADLOCKS_H_
#define _TEST_DEADLOCKS_H_

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <unity.h>
#include "deadlocks.h"

void test_deadlock();

#endif