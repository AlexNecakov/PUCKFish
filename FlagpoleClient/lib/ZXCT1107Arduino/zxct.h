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

    void read_voltage();
    void read_current();
    uint16_t read_salinity();

protected:
private:
    uint8_t pin;
    uint16_t voltage;
    uint16_t current;
};

#endif