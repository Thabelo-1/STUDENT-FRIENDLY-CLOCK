#ifndef RTC_WRITE_H
#define RTC_WRITE_H

#include <xc.h>
#include <stdint.h>
#include <stdio.h>

// ==================== RTC-Specific LCD Functions ====================


void I2C1_LCD_write1ByteMemory_RTC(uint8_t device_addr, uint8_t data);


void LCD_WriteChar_RTC(char c);
void LCD_WriteString_RTC(const char *str);

// ==================== RTC I2C Functions ====================

void I2C1_write1ByteMemory_RTC(uint8_t device_addr,
                               uint8_t register_addr,
                               uint8_t data);

uint8_t I2C1_read1ByteMemory_RTC(uint8_t device_addr,
                                 uint16_t register_addr);

// ==================== RTC Application ====================

void RT_WRITE(void);

#endif /* RTC_WRITE_H */