#include <stdio.h>
#include <xc.h>
#include <stdint.h>

//#include "mcc_generated_files/system.h"
#include "TEMP_READ.h"
#include "lcd_write.h"
#define FCY 4000000UL
#include <libpic30.h>

#include "LCD_WRITE.h"

extern volatile int16_t encoderCount;


/* =========================================================
   GLOBAL CALIBRATION (BMP280 - no humidity)
   ========================================================= */
uint16_t dig_T1, dig_P1;
int16_t  dig_T2, dig_T3;
int16_t  dig_P2, dig_P3, dig_P4;
int16_t  dig_P5, dig_P6, dig_P7;
int16_t  dig_P8, dig_P9;

int32_t t_fine;



/* BMP280 8-bit address form (0x76 << 1).
 * Your I2C1_read1ByteMemory_TEMP() masks with 0xFE for the
 * write byte and ORs 0x01 for the read byte, so it wants the
 * pre-shifted form. This is what your working code used. */
#define BMP280_ADDR   0xEC


/* =========================================================
   BMP280 CALIBRATION
   ========================================================= */
void BME280_ReadCalibration(void)
{
    /* --- Temperature --- */
    dig_T1 = ((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x89) << 8) |
              I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x88);

    dig_T2 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x8B) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x8A));

    dig_T3 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x8D) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x8C));

    /* --- Pressure --- */
    dig_P1 = ((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x8F) << 8) |
              I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x8E);

    dig_P2 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x91) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x90));

    dig_P3 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x93) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x92));

    dig_P4 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x95) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x94));

    dig_P5 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x97) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x96));

    dig_P6 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x99) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x98));

    dig_P7 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x9B) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x9A));

    dig_P8 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x9D) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x9C));

    dig_P9 = (int16_t)(((uint16_t)I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x9F) << 8) |
                        I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0x9E));
}


/* =========================================================
   TEMPERATURE COMPENSATION
   Returns temperature in 0.01 DegC units (5123 = 51.23 C)
   ========================================================= */
int32_t BME280_compensate_T_int32(int32_t adc_T)
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) *
            ((int32_t)dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
             ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
             ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    T = (t_fine * 5 + 128) >> 8;

    return T;
}


/* =========================================================
   PRESSURE COMPENSATION
   Returns pressure in Pa (100653 = 100653 Pa)
   ========================================================= */
uint32_t BME280_compensate_P_int32(int32_t adc_P)
{
    int32_t  var1, var2;
    uint32_t p;

    var1 = (t_fine >> 1) - 64000;

    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * dig_P6;
    var2 = var2 + ((var1 * dig_P5) << 1);
    var2 = (var2 >> 2) + (dig_P4 << 16);

    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) +
            ((dig_P2 * var1) >> 1)) >> 18;

    var1 = ((32768 + var1) * dig_P1) >> 15;

    if (var1 == 0)
        return 0;

    p = (((uint32_t)(((int32_t)1048576 - adc_P) -
         (var2 >> 12))) * 3125);

    if (p < 0x80000000)
        p = (p << 1) / ((uint32_t)var1);
    else
        p = (p / (uint32_t)var1) * 2;

    var1 = (dig_P9 *
            (int32_t)(((p >> 3) * (p >> 3)) >> 13)) >> 12;

    var2 = ((int32_t)(p >> 2) * dig_P8) >> 13;

    p = (uint32_t)((int32_t)p +
         ((var1 + var2 + dig_P7) >> 4));

    return p;
}


/* =========================================================
   I2C HELPERS (unchanged from your working version)
   ========================================================= */
void I2C1_LCD_write1ByteMemory_TEMP(uint8_t device_addr, uint8_t data)
{
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    I2C1TRN = device_addr & 0xFE;
    while(I2C1STATbits.TRSTAT);

    I2C1TRN = data;
    while(I2C1STATbits.TRSTAT);

    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
}


void I2C1_write1ByteMemory_TEMP(uint8_t device_addr,
                                uint8_t REGISTER_,
                                uint8_t data)
{
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    I2C1TRN = device_addr & 0xFE;
    while(I2C1STATbits.TRSTAT);

    if(I2C1STATbits.ACKSTAT)
    {
        I2C1CONbits.PEN = 1;
        while(I2C1CONbits.PEN);
        LCD_WriteString_TEMP(" NACK FIRST ");
        return;
    }

    I2C1TRN = REGISTER_;
    while(I2C1STATbits.TRSTAT);

    if(I2C1STATbits.ACKSTAT)
    {
        I2C1CONbits.PEN = 1;
        while(I2C1CONbits.PEN);
        LCD_WriteString_TEMP(" NACK SECOND ");
        return;
    }

    I2C1TRN = data;
    while(I2C1STATbits.TRSTAT);

    if(I2C1STATbits.ACKSTAT)
    {
        I2C1CONbits.PEN = 1;
        while(I2C1CONbits.PEN);
        LCD_WriteString_TEMP(" NACK THIRD ");
        return;
    }

    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
}


uint8_t I2C1_read1ByteMemory_TEMP(uint8_t device_addr, uint8_t reg)
{
    uint8_t received_data;

    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    I2C1TRN = device_addr & 0xFE;
    while(I2C1STATbits.TRSTAT);

    I2C1TRN = reg;
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


/* =========================================================
   LCD HELPERS (unchanged from your working version)
   ========================================================= */
void LCD_WriteChar_TEMP(char c)
{
    uint8_t high = c & 0xF0;
    uint8_t low  = (c << 4) & 0xF0;

    I2C1_LCD_write1ByteMemory_TEMP(0x4E, high | 0x0D);
    I2C1_LCD_write1ByteMemory_TEMP(0x4E, high | 0x09);

    I2C1_LCD_write1ByteMemory_TEMP(0x4E, low | 0x0D);
    I2C1_LCD_write1ByteMemory_TEMP(0x4E, low | 0x09);
}


void LCD_WriteString_TEMP(const char *str)
{
    while(*str)
    {
        LCD_WriteChar_TEMP(*str++);
    }
}


/* =========================================================
   BMP280 INIT  -- call ONCE from main(), not from TEMP_WRITE()
   ========================================================= */
void BME280_Init(void)
{
    __delay_ms(10);   /* let BMP280 finish power-on reset */

    I2C1_write1ByteMemory_TEMP(BMP280_ADDR, 0xF4, 0x27);  /* ctrl_meas: x1/x1, normal */
    I2C1_write1ByteMemory_TEMP(BMP280_ADDR, 0xF5, 0xA0);  /* config: 1s standby       */

    BME280_ReadCalibration();
}


/* =========================================================
   MAIN DISPLAY ROUTINE  -- non-blocking, no init, no while(1)
   ========================================================= */
void TEMP_WRITE(void)
{  
    LCD_Clear();
  while(encoderCount == 3){  
    uint8_t msb, lsb, xlsb;
    uint8_t temp_msb, temp_lsb, temp_xlsb;

    int32_t  adc_T, adc_P;
    int32_t  temperature;
    uint32_t pressure;

    char buffer[32];

    /* --- raw temperature (0xFA..0xFC) --- */
    temp_msb  = I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0xFA);
    temp_lsb  = I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0xFB);
    temp_xlsb = I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0xFC);

    adc_T = ((int32_t)temp_msb << 12) |
            ((int32_t)temp_lsb << 4)  |
            ((int32_t)temp_xlsb >> 4);

    temperature = BME280_compensate_T_int32(adc_T);

    /* --- raw pressure (0xF7..0xF9) --- */
    msb  = I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0xF7);
    lsb  = I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0xF8);
    xlsb = I2C1_read1ByteMemory_TEMP(BMP280_ADDR, 0xF9);

    adc_P = ((int32_t)msb << 12) |
            ((int32_t)lsb << 4)  |
            ((int32_t)xlsb >> 4);

    pressure = BME280_compensate_P_int32(adc_P);

    /* --- Temperature line ---
       Format string kept identical to your original.
       Sign is split off before sprintf so negatives show
       as "-5.50" instead of "-5.-50". */
    
        
        int32_t mag;
        char    sign;

        if (temperature < 0) { sign = '-'; mag = -temperature; }
        else                 { sign = ' '; mag =  temperature; }

        CURSOR_SET(1, 0);
        sprintf(buffer, "%c%ld.%02ld      Deg Cel",
                sign,
                (long)(mag / 100),
                (long)(mag % 100));
        LCD_WriteString_TEMP(buffer);
    

    /* --- Pressure line ---
       Format string kept identical to your original. */
    CURSOR_SET(2, 0);
    sprintf(buffer, "%lu.%03lu     kPa",
            (unsigned long)(pressure / 1000),
            (unsigned long)(pressure % 1000));
    LCD_WriteString_TEMP(buffer);
    
}
}