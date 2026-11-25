#ifndef HAL_UART_H
#define HAL_UART_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void hal_uart_init(void);
int hal_uart_get(void);
void hal_uart_put(char ch);
#endif
