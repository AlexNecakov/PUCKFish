/*

  This is a library for the MS5 pressure and temperature sensor

  The MS5 board uses I2C for communication. Two pins are required to
  interface to the device. 

  Written by Alex Necakov, March 2022

*/

#include "MS5.h"

// Legacy Wire.write() function fix
#if (ARDUINO >= 100)
#  define __wire_write(d) I2C->write(d)
#else
#  define __wire_write(d) I2C->send(d)
#endif

// Legacy Wire.read() function fix
#if (ARDUINO >= 100)
#  define __wire_read() I2C->read()
#else
#  define __wire_read() I2C->receive()
#endif

/**
 * Constructor
 * @params addr Sensor address (0x76)
 */
MS5::MS5(byte addr) {

  MS5_I2CADDR = addr;
  // Allows user to change TwoWire instance
  I2C = &Wire;
}

/**
 * Configure sensor
 * @param mode Measurement mode
 * @param addr Address of the sensor
 * @param i2c TwoWire instance connected to I2C bus
 */
bool MS5::begin(Mode mode, byte addr, TwoWire* i2c) {

  // I2C is expected to be initialized outside this library
  // But, allows a different address and TwoWire instance to be used
  if (i2c) {
    I2C = i2c;
  }
  if (addr) {
    MS5_I2CADDR = addr;
  }

  // Configure sensor in specified mode and set default MTreg
  return (configure(mode) && setMTreg(MS5_DEFAULT_MTREG));
}

/**
 * Configure MS5 with specified mode
 * @param mode Measurement mode
 */
bool MS5::configure(Mode mode) {

  // default transmission result to a value out of normal range
  byte ack = 5;

  // Check measurement mode is valid
  switch (mode) {

  case MS5::CONTINUOUS_HIGH_RES_MODE:
  case MS5::CONTINUOUS_HIGH_RES_MODE_2:
  case MS5::CONTINUOUS_LOW_RES_MODE:
  case MS5::ONE_TIME_HIGH_RES_MODE:
  case MS5::ONE_TIME_HIGH_RES_MODE_2:
  case MS5::ONE_TIME_LOW_RES_MODE:

    // Send mode to sensor
    I2C->beginTransmission(MS5_I2CADDR);
    __wire_write((uint8_t)mode);
    ack = I2C->endTransmission();

    // Wait a few moments to wake up
    _delay_ms(10);
    break;

  default:
    // Invalid measurement mode
    Serial.println(F("[MS5] ERROR: Invalid mode"));
    break;
  }

  // Check result code
  switch (ack) {
  case 0:
    MS5_MODE = mode;
    lastReadTimestamp = millis();
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
    Serial.println(F("[MS5] ERROR: undefined error"));
    break;
  }

  return false;
}

/**
 * Configure MS5 MTreg value
 * MT reg = Measurement Time register
 * @param MTreg a value between 31 and 254. Default: 69
 * @return bool true if MTReg successful set
 * 		false if MTreg not changed or parameter out of range
 */
bool MS5::setMTreg(byte MTreg) {
  if (MTreg < MS5_MTREG_MIN || MTreg > MS5_MTREG_MAX) {
    Serial.println(F("[MS5] ERROR: MTreg out of range"));
    return false;
  }
  byte ack = 5;
  // Send MTreg and the current mode to the sensor
  //   High bit: 01000_MT[7,6,5]
  //    Low bit: 011_MT[4,3,2,1,0]
  I2C->beginTransmission(MS5_I2CADDR);
  __wire_write((0b01000 << 3) | (MTreg >> 5));
  ack = I2C->endTransmission();
  I2C->beginTransmission(MS5_I2CADDR);
  __wire_write((0b011 << 5) | (MTreg & 0b11111));
  ack = ack | I2C->endTransmission();
  I2C->beginTransmission(MS5_I2CADDR);
  __wire_write(MS5_MODE);
  ack = ack | I2C->endTransmission();

  // Wait a few moments to wake up
  _delay_ms(10);

  // Check result code
  switch (ack) {
  case 0:
    MS5_MTreg = MTreg;
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
    Serial.println(F("[MS5] ERROR: undefined error"));
    break;
  }

  return false;
}

/**
 * Checks whether enough time has gone to read a new value
 * @param maxWait a boolean if to wait for typical or maximum delay
 * @return a boolean if a new measurement is possible
 *
 */
bool MS5::measurementReady(bool maxWait) {
  unsigned long delaytime = 0;
  switch (MS5_MODE) {
  case MS5::CONTINUOUS_HIGH_RES_MODE:
  case MS5::CONTINUOUS_HIGH_RES_MODE_2:
  case MS5::ONE_TIME_HIGH_RES_MODE:
  case MS5::ONE_TIME_HIGH_RES_MODE_2:
    maxWait ? delaytime = (180 * MS5_MTreg / (byte)MS5_DEFAULT_MTREG)
            : delaytime = (120 * MS5_MTreg / (byte)MS5_DEFAULT_MTREG);
    break;
  case MS5::CONTINUOUS_LOW_RES_MODE:
  case MS5::ONE_TIME_LOW_RES_MODE:
    // Send mode to sensor
    maxWait ? delaytime = (24 * MS5_MTreg / (byte)MS5_DEFAULT_MTREG)
            : delaytime = (16 * MS5_MTreg / (byte)MS5_DEFAULT_MTREG);
    break;
  default:
    break;
  }
  // Wait for new measurement to be possible.
  // Measurements have a maximum measurement time and a typical measurement
  // time. The maxWait argument determines which measurement wait time is
  // used when a one-time mode is being used. The typical (shorter)
  // measurement time is used by default and if maxWait is set to True then
  // the maximum measurement time will be used. See data sheet pages 2, 5
  // and 7 for more details.
  unsigned long currentTimestamp = millis();
  if (currentTimestamp - lastReadTimestamp >= delaytime) {
    return true;
  } else
    return false;
}

/**
 * Read light level from sensor
 * The return value range differs if the MTreg value is changed. The global
 * maximum value is noted in the square brackets.
 * @return Light level in lux (0.0 ~ 54612,5 [117758,203])
 * 	   -1 : no valid return value
 * 	   -2 : sensor not configured
 */
float MS5::readLightLevel() {

  if (MS5_MODE == UNCONFIGURED) {
    Serial.println(F("[MS5] Device is not configured!"));
    return -2.0;
  }

  // Measurement result will be stored here
  float level = -1.0;

  // Read two bytes from the sensor, which are low and high parts of the sensor
  // value
  if (2 == I2C->requestFrom((int)MS5_I2CADDR, (int)2)) {
    unsigned int tmp = 0;
    tmp = __wire_read();
    tmp <<= 8;
    tmp |= __wire_read();
    level = tmp;
  }
  lastReadTimestamp = millis();

  if (level != -1.0) {
// Print raw value if debug enabled
#ifdef MS5_DEBUG
    Serial.print(F("[MS5] Raw value: "));
    Serial.println(level);
#endif

    if (MS5_MTreg != MS5_DEFAULT_MTREG) {
      level *= (float)((byte)MS5_DEFAULT_MTREG / (float)MS5_MTreg);
// Print MTreg factor if debug enabled
#ifdef MS5_DEBUG
      Serial.print(F("[MS5] MTreg factor: "));
      Serial.println(
          String((float)((byte)MS5_DEFAULT_MTREG / (float)MS5_MTreg)));
#endif
    }
    if (MS5_MODE == MS5::ONE_TIME_HIGH_RES_MODE_2 ||
        MS5_MODE == MS5::CONTINUOUS_HIGH_RES_MODE_2) {
      level /= 2;
    }
    // Convert raw value to lux
    level /= MS5_CONV_FACTOR;

// Print converted value if debug enabled
#ifdef MS5_DEBUG
    Serial.print(F("[MS5] Converted float value: "));
    Serial.println(level);
#endif
  }

  return level;
}
