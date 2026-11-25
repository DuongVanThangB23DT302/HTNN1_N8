#include "app_ipc.h"

QueueHandle_t sensor_queue, command_queue, oled_queue, uart_queue;

void app_ipc_init(void)
{
    sensor_queue = xQueueCreate(1, sizeof(Sensors));
    command_queue = xQueueCreate(COMMAND_CAPACITY, sizeof(Command));
    oled_queue = xQueueCreate(1, sizeof(Status));
    uart_queue = xQueueCreate(1, sizeof(Status));
    if (!sensor_queue || !command_queue || !oled_queue || !uart_queue)
        app_fault();
}
