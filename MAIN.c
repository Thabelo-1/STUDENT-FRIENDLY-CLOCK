
// PIC24FJ64GA002 Configuration Bit Settings

// 'C' source line config statements

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = ON             // IOLOCK Protection (Once IOLOCK is set, cannot be changed)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator (FRC))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Windowed Watchdog Timer enabled; FWDTEN must be 1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator EMUC2/EMUD2 pins are shared with PGC2/PGD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define FCY 4000000UL          // Set to your actual FOSC/2 (see note below)
#include <libpic30.h>          // Provides __delay_ms() / __delay_us()
#include <stdint.h>            // For uint16_t etc.
#include <xc.h>


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "xc.h"
#include "lcd_write.h"
#include "i2c.h"
#include "pomodoro.h"
#include "TEMP_READ.h"
#include "encodercount.h"
#define FOSC 8000000        
#define FCY FOSC / 2

#define BRG 0x28
#define SLAVE_ADDRESS_WRITE 0x70  //LCD WRITE
#define SLAVE_ADDRESS_READ 0x71

//volatile int encoderCount = 3;
extern volatile int16_t encoderCount;






int main (void)
{ 
    
    
    LATAbits.LATA0 = 0;       // DIAG_LED initially OFF
    TRISAbits.TRISA0 = 0;     // RA0 as output


 

    TMR5 = 0;
    TMR4 = 0;


    PR5 = 0x0026;
    PR4 = 0x259F;


    T4CON = 0x8018;



    IFS1bits.T5IF = 0;        // Clear Timer 5 flag

    IPC7bits.T5IP = 4;        // Priority = 4

    IEC1bits.T5IE = 1;        // Enable Timer 5 interrupt

    
    
   TRISA = 0;
     
    AD1PCFG = 0xFFFF;
    
  
    
    Init_I2C(BRG);   //I2C Initialization

    
   
   
   LCD_INIT();
    
 
     
  
int lastScreen = 0;   // outside while(1)
     
   
    
  
    BOOT();
    
    initEncoderCN();
    
    
    BME280_Init();  
    


while (1)
{
    
    if (encoderCount != lastScreen)
    {
        lastScreen = encoderCount;
        
        LCD_Clear();

        switch (encoderCount)
        {
            case 1: LCD_WriteString("VISCA BARCA")  ;    break;
            case 2: RT_WRITE();     break;
            case 3: TEMP_WRITE(); break;
            case 4 : pomodoro();  encoderCount = 1; break; 
        }
    }

   
}

    
    
    
    return 0;
    
}

void _ISR _T5Interrupt(void) {

    // Toggle DIAG_LED
    LATAbits.LATA0 ^= 1;

    if(LATAbits.LATA0 == 1) {

        PR5 = 0x0000;
        PR4 = 0xEA60;
    }


    else {

        PR5 = 0x0026;
        PR4 = 0x25A0;
    }


    // Clear interrupt flag
    IFS1bits.T5IF = 0;
}


   