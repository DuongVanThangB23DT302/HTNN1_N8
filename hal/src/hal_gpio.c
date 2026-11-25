#include "hal_gpio.h"
#include "stm32f1xx.h"
#include "board.h"

// Set each output's active level according to the module.
#ifndef BUZZER_ACTIVE_HIGH
#define BUZZER_ACTIVE_HIGH 1
#endif
#ifndef RELAY_ACTIVE_HIGH
#define RELAY_ACTIVE_HIGH 1
#endif
#if (BUZZER_ACTIVE_HIGH != 0 && BUZZER_ACTIVE_HIGH != 1) || \
    (RELAY_ACTIVE_HIGH != 0 && RELAY_ACTIVE_HIGH != 1)
#error "Active level must be 0 or 1"
#endif
#define LED (1U << BSP_LED_PIN)
#define BUZZER (1U << BSP_BUZZER_PIN)
#define RELAY (1U << BSP_RELAY_PIN)

void hal_gpio_init(void)
{
    hal_gpio_outputs(false, false);
    bsp_gpio_mode(GPIOB, BSP_BUZZER_PIN, 2);
    bsp_gpio_mode(GPIOB, BSP_LED_PIN, 2);
    bsp_gpio_mode(GPIOB, BSP_RELAY_PIN, 2);

    GPIOB->BSRR = (1U << BSP_BUTTON1_PIN) | (1U << BSP_BUTTON2_PIN);
    bsp_gpio_mode(GPIOB, BSP_BUTTON2_PIN, 8);
    bsp_gpio_mode(GPIOB, BSP_BUTTON1_PIN, 8);
}

bool hal_gpio_button(unsigned index) { return !(GPIOB->IDR & (1U << (BSP_BUTTON1_PIN + index))); }

void hal_gpio_outputs(bool alarm, bool fan)
{
    bool buzzer_high = alarm == (BUZZER_ACTIVE_HIGH != 0);
    uint32_t on = (alarm ? LED : 0U) | (buzzer_high ? BUZZER : 0U) |
                  (fan == (RELAY_ACTIVE_HIGH != 0) ? RELAY : 0U);
    GPIOB->BSRR = on | (((LED | BUZZER | RELAY) & ~on) << 16);
}

void hal_gpio_led(bool on) { GPIOB->BSRR = on ? LED : LED << 16; }
