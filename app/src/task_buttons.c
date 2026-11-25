#include "task_buttons.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "hal_gpio.h"

typedef struct {
    bool raw, stable;
    TickType_t changed_at;
} Button;

static void task_buttons_run(void *argument)
{
    (void)argument;
    Button buttons[2] = {0};
    TickType_t wake = xTaskGetTickCount();

    for (;;) {
        TickType_t now = xTaskGetTickCount();
        for (unsigned index = 0; index < 2U; ++index) {
            Button *button = &buttons[index];
            bool pressed = hal_gpio_button(index);
            if (pressed != button->raw) {
                button->raw = pressed;
                button->changed_at = now;
            }
            if (button->stable != pressed &&
                now - button->changed_at >= pdMS_TO_TICKS(35)) {
                button->stable = pressed;
                if (pressed) {
                    Command command = { index == 0 ? TOGGLE_ALARM : TOGGLE_FAN, 0 };
                    xQueueSend(command_queue, &command, portMAX_DELAY);
                }
            }
        }
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(10));
    }
}

BaseType_t task_buttons_create(void)
{
    return xTaskCreate(task_buttons_run, "Buttons", BUTTONS_STACK_WORDS,
                       NULL, BUTTONS_PRIORITY, NULL);
}
