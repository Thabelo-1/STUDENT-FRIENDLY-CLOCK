/*
 * File: encodercount.h
 * Device: PIC24FJ64GA002
 *
 * Rotary encoder header.
 *
 * CLK -> RB0 / CN4 / AN2 / pin 4
 * DT  -> RB1 / CN5 / AN3 / pin 5
 * LED -> RA0
 */

#ifndef ENCODERCOUNT_H
#define ENCODERCOUNT_H

#include <xc.h>
#include <stdint.h>

/* Shared state ----------------------------------------------------- */

/* Last seen CLK level (1 = high, 0 = low) */
extern volatile uint8_t prevCLK;

/* Encoder step counter (updated by the CN ISR) */
extern volatile int encoderCount;

/* Functions -------------------------------------------------------- */

/* Configure RB0/RB1 as encoder inputs, RA0 as LED, enable CN4/CN5. */
void initEncoderCN(void);

#endif /* ENCODERCOUNT_H */