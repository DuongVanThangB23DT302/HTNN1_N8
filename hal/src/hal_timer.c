#include "hal_timer.h"
#include "stm32f1xx.h"

void hal_timer_init(void)
{
    TIM2->PSC = 7;
    TIM2->ARR = 65535;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 = TIM_CR1_CEN;
}

void hal_timer_wait_us(uint16_t us)
{
    uint16_t start = (uint16_t)TIM2->CNT;
    while ((uint16_t)((uint16_t)TIM2->CNT - start) < us) {}
}
