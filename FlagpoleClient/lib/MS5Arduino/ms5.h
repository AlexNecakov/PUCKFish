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

// I2C Command Bytes
#define MS5_RESET 0x1E
#define MS5_READ_PROM 0xA2
#define MS5_D1_CONV_SEQ 0x48
#define MS5_D2_CONV_SEQ 0x58
#define MS5_READ_ADC 0x00

// Conversion mode num
#define D1_CONV_MODE false
#define D2_CONV_MODE true

class MS5
{

public:
    MS5(byte addr = 0x76);
    bool begin(byte addr = 0x76,
               TwoWire *i2c = nullptr);
    bool reset();
    int32_t readPressure();
    int32_t readTemperature();

private:
    TwoWire *I2C;
    byte MS5_I2CADDR;
    uint16_t calib[6];
    int32_t dt;

    bool readPROM();
    bool conversion(bool mode);
};

#endif
