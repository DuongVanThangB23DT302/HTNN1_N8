#ifndef BOARD_H
#define BOARD_H
#include "stm32f1xx.h"

enum {
    BSP_MQ_PIN = 0,      // PA0 / ADC1 channel 0
    BSP_FLAME_PIN = 1,   // PA1 / ADC1 channel 1
    BSP_UART_TX_PIN = 9, // PA9
    BSP_UART_RX_PIN = 10,// PA10
    BSP_BUZZER_PIN = 0,  // PB0
    BSP_LED_PIN = 1,     // PB1
    BSP_SCL_PIN = 6,     // PB6
    BSP_SDA_PIN = 7,     // PB7
    BSP_RELAY_PIN = 12,  // PB12
    BSP_BUTTON1_PIN = 13,// PB13
    BSP_BUTTON2_PIN = 14 // PB14
};

void bsp_system_init(void);
void bsp_gpio_mode(GPIO_TypeDef *gpio, unsigned pin, unsigned mode);
#endif
