#include "app_ipc.h"
#include "board.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "task_sensors.h"
#include "task_buttons.h"
#include "task_control.h"
#include "task_oled.h"
#include "task_uart.h"
#include "app_cfg.h"

static void sensor_monitor(void *argument)
{
    (void)argument;
    Status status = { .threshold = DEFAULT_MQ_THRESHOLD,
                      .flame_threshold = DEFAULT_FLAME_THRESHOLD };
    for (;;) {
        xQueueReceive(sensor_queue, &status.sensors, portMAX_DELAY);
        xQueueOverwrite(uart_queue, &status);
    }
}

int main(void)
{
    bsp_system_init();
    hal_gpio_init();
    hal_timer_init();
    hal_adc_init();
    hal_uart_init();
    app_ipc_init();

    if (task_sensors_create() != pdPASS) app_fault();
    if (xTaskCreate(sensor_monitor, "Monitor", 128, NULL, 2, NULL) != pdPASS) app_fault();
    if (task_uart_create() != pdPASS) app_fault();
    vTaskStartScheduler();
    app_fault();
    return 1;
}
