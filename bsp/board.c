#include "board.h"

void bsp_gpio_mode(GPIO_TypeDef *gpio, unsigned pin, unsigned mode)
{
    volatile uint32_t *reg = pin < 8U ? &gpio->CRL : &gpio->CRH;
    unsigned shift = (pin % 8U) * 4U;
    *reg = (*reg & ~(15U << shift)) | (mode << shift);
}

void bsp_system_init(void)
{
    // Reset clock configuration: HSI 8 MHz, AHB/APB1/APB2 undivided.
    SCB->VTOR = FLASH_BASE;
    __DSB();
    __ISB();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN |
                    RCC_APB2ENR_ADC1EN | RCC_APB2ENR_USART1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    (void)RCC->APB2ENR;
    (void)RCC->APB1ENR;
}
