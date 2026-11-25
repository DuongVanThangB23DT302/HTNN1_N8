#include "app_ipc.h"
#include "stm32f1xx.h"
#include "hal_gpio.h"
#include "hal_timer.h"

void app_fault(void)
{
    __disable_irq();
    hal_gpio_outputs(false, false);
    for (;;) {
        hal_gpio_led(true);
        for (unsigned i = 0; i < 10U; ++i) hal_timer_wait_us(20000);
        hal_gpio_led(false);
        for (unsigned i = 0; i < 10U; ++i) hal_timer_wait_us(20000);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    (void)name;
    app_fault();
}
