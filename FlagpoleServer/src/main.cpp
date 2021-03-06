// PUCKFish Base Station Code
// Alex Necakov 2022

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>

// Pin Defines
#define RF95_CS 8
#define RF95_RST 4
#define RF95_INT 3

// Value Defines
#define RF95_FREQ 915.0

RH_RF95 rf95(RF95_CS, RF95_INT);

int rf95Init()
{
    // Serial.println("RF95\tInitializing");
    while (!rf95.init())
        Serial.println("RF95\tInit failed");
    // Serial.println("RF95\tInit success");

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    while (!rf95.setFrequency(RF95_FREQ))
        Serial.println("RF95\tSet frequency failed");
    // Serial.print("RF95\tSet frequency to: ");
    // Serial.println(RF95_FREQ);

    return 0;
}

void rf95Loop()
{
    if (rf95.available())
    {
        // Serial.println("RF95\tAvailable");
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {
            StaticJsonDocument<251> packet;
            // Serial.println("RF95\tReceive success");
            deserializeJson(packet, buf);
            serializeJson(packet, Serial);
            Serial.println("");
        }
        else
        {
            // Serial.println("RF95\tReceive failed");
        }
    }
}

void setup()
{
    Serial.begin(9600);
    delay(5000);
    Serial.println("");
    rf95Init();
}

void loop()
{
    rf95Loop();
}