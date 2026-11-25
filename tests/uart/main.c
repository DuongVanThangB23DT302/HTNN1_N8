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


int main(void)
{
    bsp_system_init();
    hal_gpio_init();
    hal_timer_init();
    hal_uart_init();
    app_ipc_init();

    const Sensors sample = {1234, 4095};
    xQueueOverwrite(sensor_queue, &sample);
    if (task_control_create() != pdPASS) app_fault();
    if (task_uart_create() != pdPASS) app_fault();
    vTaskStartScheduler();
    app_fault();
    return 1;
}
