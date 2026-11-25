#include "hal_i2c.h"
#include "stm32f1xx.h"
#include "board.h"
#include "hal_timer.h"

#define SCL (1U << BSP_SCL_PIN)
#define SDA (1U << BSP_SDA_PIN)

void hal_i2c_init(void)
{
    GPIOB->BSRR = SCL | SDA;
    bsp_gpio_mode(GPIOB, BSP_SCL_PIN, 6);
    bsp_gpio_mode(GPIOB, BSP_SDA_PIN, 6);
}

static void set_pin(uint32_t pin, bool release)
{
    GPIOB->BSRR = release ? pin : pin << 16;
    hal_timer_wait_us(3);
}

static bool send_byte(uint8_t byte)
{
    for (unsigned bit = 0; bit < 8U; ++bit) {
        set_pin(SDA, (byte & 0x80U) != 0);
        set_pin(SCL, true);
        set_pin(SCL, false);
        byte <<= 1;
    }
    set_pin(SDA, true);
    set_pin(SCL, true);
    bool ack = !(GPIOB->IDR & SDA);
    set_pin(SCL, false);
    return ack;
}

bool hal_i2c_write(uint8_t address, uint8_t control, const uint8_t *data, size_t length)
{
    set_pin(SDA, true);
    set_pin(SCL, true);
    if ((GPIOB->IDR & (SCL | SDA)) != (SCL | SDA)) return false;
    set_pin(SDA, false);
    set_pin(SCL, false);

    bool ack = send_byte(address << 1) && send_byte(control);
    for (size_t i = 0; ack && i < length; ++i) ack = send_byte(data[i]);

    set_pin(SDA, false);
    set_pin(SCL, true);
    set_pin(SDA, true);
    return ack;
}
