#ifndef HAL_I2C_H
#define HAL_I2C_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void hal_i2c_init(void);
bool hal_i2c_write(uint8_t address, uint8_t control, const uint8_t *data, size_t length);
#endif
