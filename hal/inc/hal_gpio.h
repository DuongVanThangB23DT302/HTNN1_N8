#ifndef HAL_GPIO_H
#define HAL_GPIO_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void hal_gpio_init(void);
bool hal_gpio_button(unsigned index);
void hal_gpio_outputs(bool alarm, bool fan);
void hal_gpio_led(bool on);
#endif
