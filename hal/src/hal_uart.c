#include "hal_uart.h"
#include "stm32f1xx.h"
#include "board.h"

#define RX_MASK 127U

static volatile uint8_t rx_data[RX_MASK + 1U];
static volatile unsigned rx_head, rx_tail;
static volatile bool rx_error;

void hal_uart_init(void)
{
    bsp_gpio_mode(GPIOA, BSP_UART_TX_PIN, 10);  // USART1 TX, alternate push-pull.
    GPIOA->BSRR = 1U << BSP_UART_RX_PIN;
    bsp_gpio_mode(GPIOA, BSP_UART_RX_PIN, 8);  // USART1 RX, pull-up.
    USART1->BRR = (8000000U + 57600U) / 115200U;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    NVIC_SetPriority(USART1_IRQn, 6);
    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
    uint32_t flags = USART1->SR;
    if (!(flags & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))) return;
    uint8_t byte = (uint8_t)USART1->DR;
    unsigned next = (rx_head + 1U) & RX_MASK;
    if (next == rx_tail || (flags & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))) {
        rx_error = true;
    } else {
        rx_data[rx_head] = byte;
        rx_head = next;
    }
}

int hal_uart_get(void)
{
    int result = -1;
    NVIC_DisableIRQ(USART1_IRQn);
    if (rx_error) {
        rx_error = false;
        rx_tail = rx_head;
        result = -2;
    } else if (rx_tail != rx_head) {
        result = rx_data[rx_tail];
        rx_tail = (rx_tail + 1U) & RX_MASK;
    }
    NVIC_EnableIRQ(USART1_IRQn);
    return result;
}

void hal_uart_put(char ch)
{
    const uint16_t start = (uint16_t)TIM2->CNT;
    while (!(USART1->SR & USART_SR_TXE)) {
        if ((uint16_t)((uint16_t)TIM2->CNT - start) >= 1000U) return;
    }
    USART1->DR = (uint8_t)ch;
}

