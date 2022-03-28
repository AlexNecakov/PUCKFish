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
        voltage_mV += analogRead(this->pin) / 1024.0 * 3300.0;
    }
    voltage_mV /= volt_avg_len;
    return voltage_mV;
}

float ZXCT1107::read_salinity()
{
    float voltageOut = read_voltage();
    float voltageSense = 3300.0 - voltageOut;
    float currentOut = 0.004 * voltageSense;

    float conductivity = currentOut / voltageOut;
    float salinity = conductivity * 0.64;
    return salinity;
}