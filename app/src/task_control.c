#include "task_control.h"
#include "app_ipc.h"
#include "hal_gpio.h"
#include "app_cfg.h"

typedef struct {
    bool timing;
    TickType_t started;
} Confirmation;

static bool confirmed(Confirmation *timer, bool exceeded, TickType_t now, TickType_t delay)
{
    if (!exceeded) {
        timer->timing = false;
        return false;
    }
    if (!timer->timing) {
        timer->timing = true;
        timer->started = now;
    }
    return now - timer->started >= delay;
}

static void apply_command(Status *status, Command command,
                          Confirmation *gas_timer, Confirmation *fire_timer)
{
    switch (command.type) {
    case TOGGLE_ALARM: status->alarm_manual = !status->alarm_manual; break;
    case TOGGLE_FAN: status->fan_manual = !status->fan_manual; break;
    case SET_ALARM: status->alarm_manual = command.value != 0; break;
    case SET_FAN: status->fan_manual = command.value != 0; break;
    case SET_THRESHOLD:
        if (status->threshold != command.value) gas_timer->timing = false;
        status->threshold = command.value;
        break;
    case SET_FLAME_THRESHOLD:
        if (status->flame_threshold != command.value) fire_timer->timing = false;
        status->flame_threshold = command.value;
        break;
    case SET_DELAY: status->delay_seconds = command.value; break;
    }
}

static void task_control_run(void *argument)
{
    (void)argument;
    Status status = {
        .threshold = DEFAULT_MQ_THRESHOLD,
        .flame_threshold = DEFAULT_FLAME_THRESHOLD,
        .delay_seconds = DEFAULT_DELAY_SECONDS
    };
    Confirmation gas_timer = {0}, fire_timer = {0};
    xQueueReceive(sensor_queue, &status.sensors, portMAX_DELAY);
    TickType_t wake = xTaskGetTickCount();

    for (;;) {
        xQueueReceive(sensor_queue, &status.sensors, 0);
        Command command;
        for (unsigned i = 0; i < COMMAND_CAPACITY; ++i) {
            if (xQueueReceive(command_queue, &command, 0) != pdPASS) break;
            apply_command(&status, command, &gas_timer, &fire_timer);
        }
        TickType_t now = xTaskGetTickCount();
        TickType_t delay = pdMS_TO_TICKS(status.delay_seconds * 1000U);
        bool gas_input = status.sensors.mq_adc >= status.threshold;
        bool fire_input = status.sensors.flame_adc <= status.flame_threshold;
#if TEST_MODE == 2
        // Button demo: disable automatic alarms for this firmware.
        gas_input = false;
        fire_input = false;
#endif
        status.gas = confirmed(&gas_timer, gas_input, now, delay);
        status.flame = confirmed(&fire_timer, fire_input, now, delay);
        status.pending = (gas_input && !status.gas) || (fire_input && !status.flame);
        status.alarm_on = status.flame || status.gas || status.alarm_manual;
        status.fan_on = !status.flame && (status.gas || status.fan_manual);
#if TEST_MODE != 5
        hal_gpio_outputs(status.alarm_on, status.fan_on);
#endif
        xQueueOverwrite(oled_queue, &status);
        xQueueOverwrite(uart_queue, &status);
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(10));
    }
}

BaseType_t task_control_create(void)
{
    return xTaskCreate(task_control_run, "Control", CONTROL_STACK_WORDS,
                       NULL, CONTROL_PRIORITY, NULL);
}
