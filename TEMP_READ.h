#ifndef TEMP_READ_H
#define TEMP_READ_H

#include <xc.h>
#include <stdint.h>
#include <stdio.h>

// ==================== RTC-Specific LCD Functions ====================


void I2C1_LCD_write1ByteMemory_TEMP(uint8_t device_addr,uint8_t data);

void LCD_INIT_TEMP(void);
void LCD_WriteChar_TEMP(char c);
void LCD_WriteString_TEMP(const char *str);

// ==================== TEMP I2C Functions ====================

void I2C1_write1ByteMemory_TEMP(uint8_t device_addr,
                                uint8_t reg,
                                uint8_t data);

uint8_t I2C1_read1ByteMemory_TEMP(uint8_t device_addr, uint8_t reg);

// ==================== RTC Application ====================

void TEMP_WRITE(void);

#endif /* TEMP_READ_H */