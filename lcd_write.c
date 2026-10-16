/*
 * File:   lcd_write.c
 * Author: Thabelo
 *
 * Created on 28 September 2026, 8:00 AM
 */


#include "lcd_write.h"
#include "i2c.h"

#define FCY 4000000UL
#include <libpic30.h>
#define FOSC 8000000 
#include <libpic30.h>          
#include <stdint.h>  
#include <xc.h>


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "xc.h"




void LCD_INIT(void)
{
    for (int i = 0; i < 3; i++) {
        Sendbyte_I2C(0x4E, 0x2C);
        __delay_ms(50);
        Sendbyte_I2C(0x4E, 0x28);
    }

    Sendbyte_I2C(0x4E, 0x2C);
    __delay_ms(50);
    Sendbyte_I2C(0x4E, 0x28);

    Sendbyte_I2C(0x4E, 0xCC);
    __delay_ms(50);
    Sendbyte_I2C(0x4E, 0xC8);

    Sendbyte_I2C(0x4E, 0x0C);
    __delay_ms(50);
    Sendbyte_I2C(0x4E, 0x08);

    Sendbyte_I2C(0x4E, 0xEC);
    __delay_ms(50);
    Sendbyte_I2C(0x4E, 0xE8);
}


void LCD_WriteChar(char c)
{
    uint8_t high = c & 0xF0;
    uint8_t low  = (c << 4) & 0xF0;

    Sendbyte_I2C(0x4E, high | 0x0D);   // E=1, RS=1
    Sendbyte_I2C(0x4E, high | 0x09);   // E=0, RS=1

    Sendbyte_I2C(0x4E, low  | 0x0D);   // E=1, RS=1
    Sendbyte_I2C(0x4E, low  | 0x09);   // E=0, RS=1
}


void LCD_WriteString(const char *str)
{
    while (*str) {
        LCD_WriteChar(*str++);
    }
}



void lcd_command(uint8_t cmd)
{
    uint8_t high = cmd & 0xF0;
    uint8_t low  = (cmd << 4) & 0xF0;

   Sendbyte_I2C (0x4E, high | 0x0C);
    Sendbyte_I2C(0x4E, high | 0x08);

    Sendbyte_I2C(0x4E, low | 0x0C);
    Sendbyte_I2C(0x4E, low | 0x08);

    __delay_ms(2);
}

void LCD_Clear(void){
    
        Sendbyte_I2C(0x4E, 0x0C);   // E=1, RS=0
    Sendbyte_I2C(0x4E, 0x08);   // E=0, RS=0

    Sendbyte_I2C(0x4E, 0x1C);   // E=1, RS=0
    Sendbyte_I2C(0x4E, 0x18);   // E=0, RS=0

    __delay_ms(2);
}

/*void lcd_command(uint8_t cmd)
{
    uint8_t high = cmd & 0xF0;
    uint8_t low  = (cmd << 4) & 0xF0;

    I2C1_LCD_write1ByteMemory(0x70, high | 0x08);
    I2C1_LCD_write1ByteMemory(0x70, high | 0x00);

    I2C1_LCD_write1ByteMemory(0x70, low | 0x08);
    I2C1_LCD_write1ByteMemory(0x70, low | 0x00);

    __delay_ms(2);
}*/

void CURSOR_SET(uint8_t row, uint8_t col)
{
    const uint8_t row_addr[] = {0x80, 0xC0, 0x94, 0xD4};
    lcd_command(row_addr[row] + col);
}


     void BOOT(void){
         
     CURSOR_SET(0,2);
     
     LCD_WriteString("HELLO WORLD ");
     CURSOR_SET(2,0);
     LCD_WriteString("SMILE");
     
     __delay_ms(1000);
     
     LCD_Clear();
 
     
     }