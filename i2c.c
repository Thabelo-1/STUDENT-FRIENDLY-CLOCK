
#include "xc.h"
#include "i2c.h"
#define FOSC 8000000        
#define FCY FOSC / 2
#include <libpic30.h>


void Init_I2C(int BRG)
{
    
    
    // Configure I2C1 pins as digital inputs
    TRISBbits.TRISB8 = 1;   // SCL1
    TRISBbits.TRISB9 = 1;   // SDA1
   
    
//ODCB |= 0x0300;   // Sets ODCB8 and ODCB9 (0x0300 = bits 8 and 9)

    I2C1CONbits.I2CEN = 0;  // Disable before config
    
    //I2C1CON = 0x8000;
    // P disabled; S disabled; I2COV disabled; IWCOL disabled; 
    I2C1STAT = 0x00;

    /* MI2C1 - I2C1 Master Events */
    // clear the master interrupt flag
    //IFS1bits.MI2C1IF = 0;
    // enable the master interrupt
   // IEC1bits.MI2C1IE = 1;

    
    I2C1BRG = BRG;
    I2C1CONbits.I2CSIDL = 0;
    I2C1CONbits.A10M = 0;
    I2C1CONbits.I2CEN = 1;  // Enable
}

void Start_I2C(void)
{
    I2C1CONbits.SEN = 1; //initiates start condition 
    
    while(I2C1CONbits.SEN); //wait until start condition finishes 
    
    LATBbits.LATB1 = 1;
}

void Stop_I2C(void)
{
    I2C1CONbits.PEN = 1; //initiates stop condition
    
    while(I2C1CONbits.PEN); //wait until stop condition finishes 
    
}

void Sendbyte_I2C(uint8_t ADDRESS, uint8_t data)
{
    Start_I2C();

    I2C1TRN = ADDRESS;
    while(I2C1STATbits.TRSTAT);
    
    
    if(I2C1STATbits.ACKSTAT)
    {

        // error
        
    }
    
  
    
    I2C1TRN = data;
    while(I2C1STATbits.TRSTAT);

   if(I2C1STATbits.ACKSTAT)
    {
        //LATAbits.LATA0 = 1;  
        
    }

    Stop_I2C();
}




uint8_t getbyte_I2C(uint8_t address)
{
    // start i2c  
    Start_I2C();
    
    //send the device address byte to the slave with a read indication
    I2C1TRN = address;              //high byte address of the slave address
    while(I2C1STATbits.TRSTAT);     //wait until the transfer is complete 
    I2C1CONbits.RCEN = 1;          //enable receive mode 
        
    while(I2C1STATbits.ACKSTAT);        //wait for and verify an ack from the slave 
    
    while(I2C1STATbits.RBF == 0);       //wait until the receive is complete 
    
    uint8_t receivedData = I2C1RCV;     //grab the received bit 
    
    I2C1CONbits.ACKEN = 1;             //Generate an ACK or NACK 
    
    //stop  i2c
    Stop_I2C();
    
    return receivedData;
}