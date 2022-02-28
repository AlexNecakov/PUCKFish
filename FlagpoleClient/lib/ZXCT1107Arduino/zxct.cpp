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
    for (int i = 0; i < volt_avg_len; ++i)
    {
        voltage_mV += analogRead(this->pin) / 1024.0 * 5000.0;
    }
    voltage_mV /= volt_avg_len;
    return voltage_mV;
}
