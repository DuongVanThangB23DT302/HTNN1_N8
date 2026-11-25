#include "task_sensors.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "hal_adc.h"

static void task_sensors_run(void *argument)
{
    (void)argument;
    TickType_t wake = xTaskGetTickCount();

    for (;;) {
        unsigned mq_sum = 0, flame_sum = 0;
        for (unsigned i = 0; i < 8; ++i) {
            mq_sum += hal_adc_mq_adc();
            flame_sum += hal_adc_flame_adc();
        }
        const Sensors sensors = { (mq_sum + 4U) / 8U, (flame_sum + 4U) / 8U };
        xQueueOverwrite(sensor_queue, &sensors);
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(50));
    }
}

BaseType_t task_sensors_create(void)
{
    return xTaskCreate(task_sensors_run, "Sensors", SENSORS_STACK_WORDS,
                       NULL, SENSORS_PRIORITY, NULL);
}
