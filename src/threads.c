#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SIDE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

SemaphoreHandle_t count_semaphore;
SemaphoreHandle_t uart_semaphore;

int counter;
int on;

void side_thread(void *params)
{
    int local_count = 0;

	while (1) {
        vTaskDelay(100);

        // Critical section: capture and increment the global counter
        // to minimize critical section code
        // NOTE: setting the time-out to portMAX_DELAY means that 
        // xSemaphoreTake only returns when the semaphore was acquired,
        // so there's no need to check the return value.
        xSemaphoreTake(count_semaphore, portMAX_DELAY);
        {
            local_count = ++counter;
        }
        xSemaphoreGive(count_semaphore);

        // The printf to the UART is a separate resource, so it should be protected separately from the counter
        xSemaphoreTake(uart_semaphore, portMAX_DELAY);
        {
            printf("hello world from %s! Count %d\n", "thread", local_count);
        }
        xSemaphoreGive(uart_semaphore);
	}
}

void main_thread(void *params)
{
    int local_count = 0;

	while (1) {
        // Commit the LED's state
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);

        vTaskDelay(100);

        // Critical section: capture and increment the global counter
        // to minimize critical section code
        // NOTE: setting the time-out to portMAX_DELAY means that 
        // xSemaphoreTake only returns when the semaphore was acquired,
        // so there's no need to check the return value.
        xSemaphoreTake(count_semaphore, portMAX_DELAY);
        {
            local_count = ++counter;
        }
        xSemaphoreGive(count_semaphore);

        // The printf to the UART is a separate resource, so it should be protected separately from the counter
        xSemaphoreTake(uart_semaphore, portMAX_DELAY);
        {
            printf("hello world from %s! Count %d\n", "main", local_count);
        }
        xSemaphoreGive(uart_semaphore);

        // Update the LED's state
        on = !on;
	}
}

int main(void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);

    on = false;
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
