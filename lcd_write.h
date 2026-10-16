#ifndef lcd_write_H
#define lcd_write_H

#include  <stdint.h>

/* I2C address of the LCD backpack (PCF8574) */
#define LCD_I2C_ADDR    0x70

/* Function prototypes */
void LCD_INIT(void);
void LCD_WriteChar(char c);
void LCD_WriteString(const char *str);
void LCD_Clear(void);

#endif /* LCD_WRITE_H */