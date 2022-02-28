#include "Arduino.h"

#include "zxct.h"

ZXCT1107::ZXCT1107(uint8_t pin)
{
    this->pin = pin;
}

bool ZXCT1107::begin()
{
    return true;
}

float ZXCT1107::read_voltage()
{
    float voltage_mV = 0;

    // Your code here
    voltage_mV += analogRead(this->pin);

    
    return voltage_mV;
}
