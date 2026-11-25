#ifndef HAL_TIMER_H
#define HAL_TIMER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void hal_timer_init(void);
void hal_timer_wait_us(uint16_t us);
#endif
