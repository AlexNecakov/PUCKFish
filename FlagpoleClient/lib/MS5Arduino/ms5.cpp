/*
*  This is a library for the MS5 pressure and temperature sensor
*
*  The MS5 board uses I2C for communication. Two pins are required to
*  interface to the device. 
*
*  Written by Alex Necakov, March 2022
*/

#include "MS5.h"

// Define milliseconds delay for ESP8266 platform
#if defined(ESP8266)

#include <pgmspace.h>
#define _delay_ms(ms) delayMicroseconds((ms)*1000)

// Use __delay_ms_ms from utils for AVR-based platforms
#elif defined(__avr__)
#include <util/delay.h>

// Use Wiring's delay for compability with another platforms
#else
#define _delay_ms(ms) delay(ms)
#endif

// Legacy Wire.write() function fix
#if (ARDUINO >= 100)
#define __wire_write(d) I2C->write(d)
#else
#define __wire_write(d) I2C->send(d)
#endif

// Legacy Wire.read() function fix
#if (ARDUINO >= 100)
#define __wire_read() I2C->read()
#else
#define __wire_read() I2C->receive()
#endif

/**
 * Constructor
 * @param addr Sensor address (0x76)
 */
MS5::MS5(byte addr)
{
    MS5_I2CADDR = addr;
    // Allows user to change TwoWire instance
    I2C = &Wire;
}

/**
 * Configure sensor
 * @param addr Address of the sensor
 * @param i2c TwoWire instance connected to I2C bus
 * @return bool true if successful configure
 */
bool MS5::begin(byte addr, TwoWire *i2c)
{
    // I2C is expected to be initialized outside this library
    // But, allows a different address and TwoWire instance to be used
    if (i2c)
    {
        I2C = i2c;
    }
    if (addr)
    {
        MS5_I2CADDR = addr;
    }

    // Reset sensor and read PROM for calibration
    return (reset() && readPROM());
}

/**
 * Reset MS5 sensor
 * @return bool true if successful reset
 */
bool MS5::reset()
{
    // default transmission result to a value out of normal range
    byte ack = 5;

    // Send mode to sensor
    I2C->beginTransmission(MS5_I2CADDR);
    __wire_write(MS5_RESET);
    ack = I2C->endTransmission();

    // Wait a few moments to wake up
    _delay_ms(200);

    // Check result code
    switch (ack)
    {
    case 0:
        return true;
    case 1: // too long for transmit buffer
        Serial.println(F("[MS5] ERROR: too long for transmit buffer"));
        break;
    case 2: // received NACK on transmit of address
        Serial.println(F("[MS5] ERROR: received NACK on transmit of address"));
        break;
    case 3: // received NACK on transmit of data
        Serial.println(F("[MS5] ERROR: received NACK on transmit of data"));
        break;
    case 4: // other error
        Serial.println(F("[MS5] ERROR: other error"));
        break;
    default:
        Serial.println(F("[MS5] ERROR: reset error"));
        break;
    }

    return false;
}

/**
 * Read PROM for calibration values
 * PROM must be read after reset()
 * There are 7 addresses in the PROM (2 bytes each)
 * The first is used for CRC, and not needed for calibration so it is skipped
 * @return bool true PROM read
 * 	false if PROM not successfully read
 */
bool MS5::readPROM()
{
    byte ack = 5;

    I2C->beginTransmission(MS5_I2CADDR);
    __wire_write(MS5_READ_PROM);
    for (int i = 0; i < 7; i++)
    {
        I2C->requestFrom(MS5_I2CADDR, 2, false);
        calib[i] = __wire_read();
        calib[i] <<= 8;
        calib[i] |= __wire_read();
        // Serial.print(calib[i]);
        // Serial.print(" calib ");
        // Serial.print(i);
        // Serial.println("");
    }
    ack = I2C->endTransmission();

    // Wait a few moments to wake up
    _delay_ms(200);

    // Check result code
    switch (ack)
    {
    case 0:
        return true;
    case 1: // too long for transmit buffer
        Serial.println(F("[MS5] ERROR: too long for transmit buffer"));
        break;
    case 2: // received NACK on transmit of address
        Serial.println(F("[MS5] ERROR: received NACK on transmit of address"));
        break;
    case 3: // received NACK on transmit of data
        Serial.println(F("[MS5] ERROR: received NACK on transmit of data"));
        break;
    case 4: // other error
        Serial.println(F("[MS5] ERROR: other error"));
        break;
    default:
        Serial.println(F("[MS5] Read PROM error"));
        break;
    }

    return false;
}

/**
 * Conversion sequence for temperature/pressure compensation
 * Conversion must take place before reading from ADC
 * @param mode bool to determine whether to do 
 * d1 (pressure) or d2 (temperature)
 * @return bool true if successful conversion read
 * 	false if conversion not successfully
 */
bool MS5::conversion(bool mode)
{

    byte ack = 5;

    I2C->beginTransmission(MS5_I2CADDR);
    if (mode == D1_CONV_MODE)
        __wire_write(MS5_D1_CONV_SEQ);
    else
        __wire_write(MS5_D2_CONV_SEQ);
    ack = I2C->endTransmission();

    // Wait a few moments to wake up
    _delay_ms(200);

    // Check result code
    switch (ack)
    {
    case 0:;
        return true;
    default:
        Serial.println(F("[MS5] Conversion sequence error"));
        break;
    }

    return false;
}

/**
 * Read temperature from sensor
 * @return Temperature in celsius
 *  ie 1981 = 19.81 C
 */
int32_t MS5::readTemperature()
{
    // Measurement result will be stored here
    int32_t temperature = 0;

    // d1 conversion sequence
    conversion(D2_CONV_MODE);

    // Read three bytes from the sensor
    I2C->beginTransmission(MS5_I2CADDR);
    __wire_write(MS5_READ_ADC);
    I2C->requestFrom(MS5_I2CADDR, 3, false);
    int32_t d2 = 0;
    d2 = __wire_read();
    d2 <<= 8;
    d2 |= __wire_read();
    d2 <<= 8;
    d2 |= __wire_read();
    I2C->endTransmission();

    dt = d2 - calib[5] * 256;
    temperature = 2000 + dt * calib[6] / 8388608;
    
    return temperature;
}

/**
 * Read temperature compensated pressure from sensor
 * @return Pressure in mbar
 * ie 39998 = 3999.8 mbar
 */
int32_t MS5::readPressure()
{
    // must read temp to compensate
    readTemperature();

    // Measurement result will be stored here
    int32_t pressure = 0;

    // d1 conversion sequence
    conversion(D1_CONV_MODE);

    // Read three bytes from the sensor
    I2C->beginTransmission(MS5_I2CADDR);
    __wire_write(MS5_READ_ADC);
    I2C->requestFrom(MS5_I2CADDR, 3, false);
    int32_t d1 = 0;
    d1 = __wire_read();
    d1 <<= 8;
    d1 |= __wire_read();
    d1 <<= 8;
    d1 |= __wire_read();
    I2C->endTransmission();

    int64_t off = calib[2] * 65536 + ((calib[4] * dt) / 128);
    int64_t sens = calib[1] * 32768 + ((calib[3] * dt) / 256);
    pressure = (d1 * sens / 2097152 - off) / 8192;

    return pressure;
}
