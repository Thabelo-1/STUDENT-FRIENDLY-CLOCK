

#pragma once
#include <stdint.h>

//Initilize the i2c module
void Init_I2C(int BRG);

//start the i2c module
void Start_I2C(void);

//start the i2c module
void Stop_I2C(void);

//send data to the i2c module
void Sendbyte_I2C(uint8_t address, uint8_t data);

uint8_t getbyte_I2C(uint8_t address);

//get data from i2c module
uint8_t Setbyte_I2C(uint8_t address);