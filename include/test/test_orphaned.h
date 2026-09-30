#ifndef TEST_ORPHANED_H
#define TEST_ORPHANED_H

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <unity.h>

void test_orphaned_lock_deadlocks();

#endif