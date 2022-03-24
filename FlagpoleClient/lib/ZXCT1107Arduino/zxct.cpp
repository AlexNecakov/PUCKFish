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

void ZXCT1107::read_current()
{
    current = analogRead(pin);
}
void ZXCT1107::read_voltage()
{
    voltage = analogRead(pin);
}

uint16_t ZXCT1107::read_salinity()
{
    read_voltage();
    read_current();

    uint8_t salinity;
    uint8_t conductivity = current / voltage;

    salinity = (pow(conductivity, 1.0878)) * 0.4665;
    return salinity;
}