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

typedef struct {
    SemaphoreHandle_t count_semaphore;
    SemaphoreHandle_t uart_semaphore;
    int counter;
} Params_t;
Params_t params = {0};

void side_thread(void *void_params)
{
    // Prepare params for access
    Params_t *params = (Params_t *)void_params;

    // Locals
    int local_count = 0;
    char *thread_name = "thread";

	while (1) {
        vTaskDelay(100);

        local_count = safe_increment(&params->counter, params->count_semaphore);
        safe_hello(thread_name, local_count, params->uart_semaphore);
	}
}

void main_thread(void *void_params)
{
    // Prepare params for access
    Params_t *params = (Params_t *)void_params;

    // Locals
    int local_count = 0;
    char *thread_name = "main";
    int led_state = 0;

	while (1) {
        led_state = update_led(led_state);

        vTaskDelay(100);

        local_count = safe_increment(&params->counter, params->count_semaphore);
        safe_hello(thread_name, local_count, params->uart_semaphore);
	}
}

int main(void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);

    params.count_semaphore = xSemaphoreCreateCounting(1, 1);
    params.uart_semaphore = xSemaphoreCreateCounting(1, 1);
    params.counter = 0;

    TaskHandle_t main, side;
    xTaskCreate(main_thread, "MainThread",
                MAIN_TASK_STACK_SIZE, (void *)&params, MAIN_TASK_PRIORITY, &main);
    xTaskCreate(side_thread, "SideThread",
                SIDE_TASK_STACK_SIZE, (void *)&params, SIDE_TASK_PRIORITY, &side);

    vTaskStartScheduler();
	return 0;
}
