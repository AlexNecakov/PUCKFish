#include "Arduino.h"
#include <cmath>
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
    voltage_mV += analogRead(this->pin);

    
    return voltage_mV;
}

float ZXCT1107::read_salinity() {
    float salinity = 0;
    // float conductivity = read_current/voltage_mV;

    // salinity = (pow(conductivity, 1.0878)) * 0.4665;
    return salinity;

}