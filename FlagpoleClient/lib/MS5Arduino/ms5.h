/*

  This is a library for the MS5 pressure and temperature sensor

  The MS5 board uses I2C for communication. Two pins are required to
  interface to the device. 

  Written by Alex Necakov, March 2022

*/

#ifndef MS5_h
#define MS5_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "Wire.h"

// Uncomment, to enable debug messages
// #define MS5_DEBUG

#define MS5_I2C_ADDR 0x76

// Reg Addresses
#define MS5_RESET 0x1E
#define MS5_READ_PROM 0xA6
#define MS5_D1_CONV_SEQ 0x48
#define MS5_D2_CONV_SEQ 0x58
#define MS5_READ_ADC 0x00

class MS5
{

public:
    MS5(byte addr = MS5_I2C_ADDR);
    bool begin(byte addr = MS5_I2C_ADDR,
               TwoWire *i2c = nullptr);
    bool reset();
    float readPressure();
    float readTemperature();

private:
    byte addr;
    byte MS5_MTreg = (byte)MS5_DEFAULT_MTREG;
    bool readPROM();
    bool d1Conversion();
    bool d2Conversion();
    TwoWire *I2C;
};

#endif
