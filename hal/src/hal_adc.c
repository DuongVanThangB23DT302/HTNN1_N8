#include "hal_adc.h"
#include "stm32f1xx.h"
#include "board.h"
#include "hal_timer.h"
#include "FreeRTOSConfig.h"

static bool adc_wait_clear(uint32_t mask)
{
    const uint16_t start = (uint16_t)TIM2->CNT;
    while (ADC1->CR2 & mask) {
        if ((uint16_t)((uint16_t)TIM2->CNT - start) >= 1000U) return false;
    }
    return true;
}

void hal_adc_init(void)
{
    bsp_gpio_mode(GPIOA, BSP_MQ_PIN, 0);
    bsp_gpio_mode(GPIOA, BSP_FLAME_PIN, 0);
    ADC1->SMPR2 = 7U | (7U << 3); // CH0, CH1, 239.5 ADC cycles.
    ADC1->CR2 = ADC_CR2_ADON;
    hal_timer_wait_us(10);
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    if (!adc_wait_clear(ADC_CR2_RSTCAL)) app_fault();
    ADC1->CR2 |= ADC_CR2_CAL;
    if (!adc_wait_clear(ADC_CR2_CAL)) app_fault();
    ADC1->CR2 |= ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL;
}

static unsigned read_adc(unsigned channel)
{
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    const uint16_t start = (uint16_t)TIM2->CNT;
    while (!(ADC1->SR & ADC_SR_EOC)) {
        if ((uint16_t)((uint16_t)TIM2->CNT - start) >= 1000U) app_fault();
    }
    return ADC1->DR & 4095U;
}

unsigned hal_adc_mq_adc(void) { return read_adc(BSP_MQ_PIN); }
unsigned hal_adc_flame_adc(void) { return read_adc(BSP_FLAME_PIN); }
