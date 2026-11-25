#ifndef HAL_ADC_H
#define HAL_ADC_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void hal_adc_init(void);
unsigned hal_adc_mq_adc(void);
unsigned hal_adc_flame_adc(void);
#endif
