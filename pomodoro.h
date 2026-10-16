/*
 * pomodoro.h
 * Header for the Pomodoro timer application
 * Target: PIC24FJ64GA002, XC16
 */

#ifndef POMODORO_H
#define POMODORO_H

#include <stdint.h>
#include <stdio.h>
#include <libpic30.h>       /* for __delay_ms / __delay_us */

/* ---------- I2C / LCD driver (defined in lcd_write.c / i2c.c) ---------- */

/* Write one byte to an I2C slave (PCF8574 backpack at 0x70).
 * Implemented in i2c.c */
void Sendbyte_I2C(uint8_t addr, uint8_t data);

/* Full LCD init (4-bit mode, 2 lines, display on).
 * Implemented in lcd_write.c */
void LCD_INIT(void);

/* Clear display (HD44780 command 0x01). Implemented in lcd_write.c */
void LCD_Clear(void);

/* Write a null-terminated string at the current cursor position.
 * Implemented in lcd_write.c */
void LCD_WriteString(const char *str);

/* ---------- Low-level LCD helpers (defined in pomodoro.c) ------------- */

/* Send a raw HD44780 command (splits into high/low nibble with E pulse). */
void lcd_command(uint8_t cmd);

/* Position cursor. row = 0..3, col = 0..19 (or 0..15 for 16x4). */
void CURSOR_SET(uint8_t row, uint8_t col);

/* ---------- Pomodoro state machine (defined in pomodoro.c) ------------ */

/* Runs the full Pomodoro cycle: FOCUS -> SHORT/LONG BREAK -> repeat. */
void pomodoro(void);

#endif /* POMODORO_H */