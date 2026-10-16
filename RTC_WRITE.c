#include <stdio.h>
#include <xc.h>
#include <stdint.h>

//#include "mcc_generated_files/system.h"
#include "RTC_WRITE.h"
#include "LCD_WRITE.h"

#define FCY 4000000UL
#include <libpic30.h>

extern volatile int16_t encoderCount;



void I2C1_LCD_write1ByteMemory_RTC(uint8_t device_addr, uint8_t data)
{
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN);

    I2C1TRN = device_addr & 0xFE;
    while (I2C1STATbits.TRSTAT);

    I2C1TRN = data;
    while (I2C1STATbits.TRSTAT);

    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
}



void LCD_WriteChar_RTC(char c)
{
    uint8_t high = c & 0xF0;
    uint8_t low  = (c << 4) & 0xF0;

    I2C1_LCD_write1ByteMemory_RTC(0x4E, high | 0x0D);   //ENABLE HIGH RS HIGH
    I2C1_LCD_write1ByteMemory_RTC(0x4E, high | 0x09);   //ENABLE LOW RS HIGH

    I2C1_LCD_write1ByteMemory_RTC(0x4E, low | 0x0D);   // SAME AS ABOVE
    I2C1_LCD_write1ByteMemory_RTC(0x4E, low | 0x09);
}

void LCD_WriteString_RTC(const char *str)
{
    while(*str)
    {
        LCD_WriteChar_RTC(*str++);
    }
}

/*=========================================================
                ADD THESE TWO FUNCTIONS HERE
=========================================================*/

void I2C1_write1ByteMemory_RTC(uint8_t device_addr, uint8_t REGISTER_, uint8_t data)
{
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    I2C1TRN = device_addr & 0xFE;
    while(I2C1STATbits.TRSTAT);
    
      if(I2C1STATbits.ACKSTAT)
    {
        LCD_WriteString_RTC("NACK ON i2c"); while(1);
        // error
        
    }
    
    I2C1TRN = REGISTER_;
    while(I2C1STATbits.TRSTAT);
    
     if(I2C1STATbits.ACKSTAT)
    {
        LCD_WriteString_RTC("NACK ON i2c"); while(1);
        // error
        
    }

    I2C1TRN = data;
    while(I2C1STATbits.TRSTAT);

    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
}

uint8_t I2C1_read1ByteMemory_RTC(uint8_t device_addr, uint16_t mem_addr)
{
    uint8_t received_data;

    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    I2C1TRN = device_addr & 0xFE;
    while(I2C1STATbits.TRSTAT);
    
    

    I2C1TRN = mem_addr & 0xFF;
    while(I2C1STATbits.TRSTAT);

    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN);

    I2C1TRN = device_addr | 0x01;
    while(I2C1STATbits.TRSTAT);

    I2C1CONbits.RCEN = 1;
    while(!I2C1STATbits.RBF);

    received_data = I2C1RCV;

    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    while(I2C1CONbits.ACKEN);

    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);

    return received_data;
}

/*=========================================================*/

// Convert BCD to decimal
uint8_t BCD_TO_DEC(uint8_t value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}


void RT_WRITE(void)
{
    AD1PCFG = 0xFFFF;

   

/*void RTC_SET_TIME(void)
{
    I2C1_write1ByteMemory_RTC(0xD0, 0x00, 0x00); // Seconds: 00
    I2C1_write1ByteMemory_RTC(0xD0, 0x01, 0x30); // Minutes: 30
    I2C1_write1ByteMemory_RTC(0xD0, 0x02, 0x14); // Hours: 14
    I2C1_write1ByteMemory_RTC(0xD0, 0x03, 0x05); // Thursday
    I2C1_write1ByteMemory_RTC(0xD0, 0x04, 0x15); // Date: 15
    I2C1_write1ByteMemory_RTC(0xD0, 0x05, 0x10); // October
    I2C1_write1ByteMemory_RTC(0xD0, 0x06, 0x26); // Year: 2026
}*/

  



    uint8_t time;
    
    
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;

    uint8_t hour12;
    uint8_t pm;

    char buffer[21];

    while (encoderCount == 2)
    {
        // Read all DS3231 registers
        seconds = I2C1_read1ByteMemory_RTC(0xD1, 0x00);
        minutes = I2C1_read1ByteMemory_RTC(0xD1, 0x01);
        hours   = I2C1_read1ByteMemory_RTC(0xD1, 0x02);
        day     = I2C1_read1ByteMemory_RTC(0xD1, 0x03);
        date    = I2C1_read1ByteMemory_RTC(0xD1, 0x04);
        month   = I2C1_read1ByteMemory_RTC(0xD1, 0x05);
        year    = I2C1_read1ByteMemory_RTC(0xD1, 0x06);

        // Mask control bits
        seconds &= 0x7F;
        minutes &= 0x7F;
        day     &= 0x07;
        date    &= 0x3F;
        month   &= 0x1F;

        // Convert seconds and minutes
        seconds = BCD_TO_DEC(seconds);
        minutes = BCD_TO_DEC(minutes);

        // Convert hours according to the mode bit
        if (hours & 0x40)
        {
            // 12-hour mode
            pm = (hours & 0x20) ? 1 : 0;

            hour12 = BCD_TO_DEC(hours & 0x1F);

            hours = hour12 % 12;

            if (pm)
            {
                hours += 12;
            }
        }
        else
        {
            // 24-hour mode
            hours &= 0x3F;
            hours = BCD_TO_DEC(hours);
        }

        // Convert date, month and year
        date  = BCD_TO_DEC(date);
        month = BCD_TO_DEC(month);
        year  = BCD_TO_DEC(year);

        // Display time on LCD row 1
        sprintf(buffer, "%02u:%02u:%02u",
                (unsigned int)hours,
                (unsigned int)minutes,
                (unsigned int)seconds);

        CURSOR_SET(0, 0);
        LCD_WriteString_RTC(buffer);

        // Display date on LCD row 2
        sprintf(buffer, "%02u/%02u/20%02u",
                (unsigned int)date,
                (unsigned int)month,
                (unsigned int)year);

        CURSOR_SET(1, 0);
        LCD_WriteString_RTC(buffer);

        // Refresh display without continuously hammering I2C
        __delay_ms(200);
    }
}