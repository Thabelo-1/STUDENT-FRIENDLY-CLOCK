/*
 * File: ma.c
 * Device: PIC24FJ64GA002
 *
 * CLK -> RB0 / CN4 / AN2 / pin 4
 * DT  -> RB1 / CN5 / AN3 / pin 5
 * LED -> RA0
 *
 * CW  -> LED ON
 * CCW -> LED OFF
 */



#include <xc.h>
#include <stdint.h>
#include <libpic30.h>
#include "lcd_write.h"
#include "i2c.h"

volatile uint8_t prevCLK = 1;
volatile int encoderCount = 1;

void initEncoderCN(void)
{
    // Digital pins
    //AD1PCFGbits.PCFG0 = 1;  // RA0
    AD1PCFGbits.PCFG2 = 1;  // RB0
    AD1PCFGbits.PCFG3 = 1;  // RB1

    // Encoder inputs
    TRISBbits.TRISB0 = 1;   // CLK
    TRISBbits.TRISB1 = 1;   // DT

    // LED output
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 0;

    // Pull-ups on CN4 and CN5
    CNPU1bits.CN4PUE = 1;
    CNPU1bits.CN5PUE = 1;

    // Enable CN on CN4 and CN5
    CNEN1bits.CN4IE = 1;
    CNEN1bits.CN5IE = 1;

    // Clear mismatch
    volatile uint16_t dummy = PORTB;
    (void)dummy;

    // Remember initial CLK level
    prevCLK = PORTBbits.RB0;

    // CN interrupt
    IPC4bits.CNIP = 4;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void __attribute__((interrupt, no_auto_psv))
_CNInterrupt(void)
{    
    //LCD_Clear();

    
    //if(encoderCount >=5){encoderCount = 1;}
    
    // Read PORTB first (clears mismatch)
    uint16_t port = PORTB;

    // Get current CLK and DT
    uint8_t clk = (port & 0x0001u) ? 1 : 0;   // RB0
    uint8_t dt  = (port & 0x0002u) ? 1 : 0;   // RB1

    // Did CLK fall? (1 -> 0)
    if (prevCLK == 1 && clk == 0)
    {
        if (dt == 1)
        {
           encoderCount++;    // CW -> LED ON
        }
        else
        {
           encoderCount--;    // CCW -> LED OFF
        }
    }

    if (encoderCount > 5)
{
    encoderCount = 1;
}
else if (encoderCount < 1)
{
    encoderCount = 5;
}
    
    // Save CLK for next time
    prevCLK = clk;

    // Clear flag
    IFS1bits.CNIF = 0;
}