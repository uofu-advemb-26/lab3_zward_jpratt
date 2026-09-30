#ifndef ORPHANED_H
#define ORPHANED_H

#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>

void orphaned_lock(void *vargs);

#endif