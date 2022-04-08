#ifndef ZXCT1107_h
#define ZXCT1107_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class ZXCT1107
{
public:
    ZXCT1107(uint8_t pin);

    bool begin();

    float read_voltage();
    float read_salinity();

protected:
    static const int volt_avg_len = 32767;

private:
    uint8_t pin;
};

#endif