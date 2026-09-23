#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include "safe.h"
#include "led.h"

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SIDE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

SemaphoreHandle_t count_semaphore;
SemaphoreHandle_t uart_semaphore;

int counter;

void side_thread(void *params)
{
    int local_count = 0;
    char *thread_name = "thread";

	while (1) {
        vTaskDelay(100);

        local_count = safe_increment(&counter, count_semaphore);
        safe_hello(thread_name, local_count, uart_semaphore);
	}
}

void main_thread(void *params)
{
    int local_count = 0;
    char *thread_name = "main";
    int led_state = 0;

	while (1) {
        led_state = update_led(led_state);

        vTaskDelay(100);

        local_count = safe_increment(&counter, count_semaphore);
        safe_hello(thread_name, local_count, uart_semaphore);
	}
}

int main(void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);

    counter = 0;
    count_semaphore = xSemaphoreCreateCounting(1, 1);
    uart_semaphore = xSemaphoreCreateCounting(1, 1);

    TaskHandle_t main, side;
    xTaskCreate(main_thread, "MainThread",
                MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main);
    xTaskCreate(side_thread, "SideThread",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &side);

    vTaskStartScheduler();
	return 0;
}
