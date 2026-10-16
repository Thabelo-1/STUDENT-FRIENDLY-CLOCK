
#define FCY 4000000UL
#define FOSC 8000000

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <libpic30.h>

#include "lcd_write.h"
#include "pomodoro.h"
#include "encodercount.h"

volatile int count = 0;

extern volatile int16_t encoderCount;

uint8_t longbreak = 0;
uint8_t shortbreak = 0;


/* =====================================================
   TIMER 1 INITIALIZATION
   FCY = 4 MHz
   Prescaler = 1:64
   Interrupt period = 1 second
   ===================================================== */

static void Timer1_Init(void)
{
    T1CONbits.TON = 0;

    T1CONbits.TCS = 0;          // Internal instruction clock
    T1CONbits.TCKPS = 0b10;     // Prescaler 1:64

    TMR1 = 0;
    PR1 = 62499;                // 1 second

    IPC0bits.T1IP = 4;          // Interrupt priority
    IFS0bits.T1IF = 0;           // Clear interrupt flag
    IEC0bits.T1IE = 1;           // Enable Timer1 interrupt
}


/* =====================================================
   DISPLAY TIME ON LCD
   ===================================================== */

static void DisplayTime(int seconds)
{
    uint16_t minutes = (uint16_t)(seconds / 60);
    uint16_t secs = (uint16_t)(seconds % 60);

    char timeStr[10];

    sprintf(timeStr, "%02u:%02u",
            (unsigned int)minutes,
            (unsigned int)secs);

    CURSOR_SET(1, 0);
    LCD_WriteString(timeStr);
}


/* =====================================================
   RUN A COUNTDOWN
   durationSeconds = countdown duration in seconds
   title = FOCUS, SHORT BREAK or LONG BREAK
   ===================================================== */

static void RunCountdown(uint16_t durationSeconds,
                         const char *title)
{
    T1CONbits.TON = 0;

    count = 0;
    TMR1 = 0;
    IFS0bits.T1IF = 0;

    LCD_Clear();

    CURSOR_SET(0, 0);
    LCD_WriteString(title);

    CURSOR_SET(2, 0);
    LCD_WriteString("COMPLETED");

    DisplayTime(0);

    T1CONbits.TON = 1;

    int previousCount = 0;

    while (count < durationSeconds && encoderCount == 4)
    {
        int currentCount = count;

        // Update LCD only when the count changes
        if (currentCount != previousCount)
        {
            DisplayTime(currentCount);
            previousCount = currentCount;
        }
    }

    T1CONbits.TON = 0;

    IFS0bits.T1IF = 0;
}


/* =====================================================
   POMODORO MAIN FUNCTION
   30-minute focus
   5-minute short break
   15-minute long break after 3 focus sessions
   ===================================================== */

void pomodoro(void)
{
    Timer1_Init();

    longbreak = 0;
    shortbreak = 0;

    while (encoderCount == 4)
    {
        // 30-minute focus session
        RunCountdown(1800, "FOCUS");

        if (encoderCount != 4) {break;}

        longbreak++;

        // Long break after 3 focus sessions
        if (longbreak >= 3)
        {
            RunCountdown(900, "LONG BREAK");

            if (encoderCount != 3)
                break;

            longbreak = 0;
            shortbreak = 0;
        }
        else
        {
            // 5-minute short break
            shortbreak = 1;

            RunCountdown(300, "SHORT BREAK");

            if (encoderCount != 3)
                break;

            shortbreak = 0;
        }
    }

    // Stop Timer1 when leaving Pomodoro
    T1CONbits.TON = 0;
}


/* =====================================================
   TIMER 1 INTERRUPT
   Executes once per second
   ===================================================== */

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;

    count++;
}
