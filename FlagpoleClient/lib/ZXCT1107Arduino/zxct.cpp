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

float ZXCT1107::read_current()
{
    float current_muA = 0;

    // Your code here
    current_muA += analogRead(this->pin);

    
    return current_muA;
}
float ZXCT1107:: read_voltage()
{
    float voltage = 3.3;

    
}


float ZXCT1107::read_salinity() {
    float salinity;
    float conductivity = read_current/voltage;

    salinity = (pow(conductivity, 1.0878)) * 0.4665;
    return salinity;

}