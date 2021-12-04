//
// Created by 14456 on 2021/12/4.
//

#ifndef U8G2PLATFORM_OLEDSCREEN_H
#define U8G2PLATFORM_OLEDSCREEN_H
#include "u8g2.h"
#include "spi.h"
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,U8X8_UNUSED void *arg_ptr);
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                               void *arg_ptr);

u8g2_t u8g2;
#endif //U8G2PLATFORM_OLEDSCREEN_H
