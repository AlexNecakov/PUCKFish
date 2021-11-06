//  ███████╗███████╗ █████╗ ████████╗██╗  ██╗███████╗██████╗     ██╗      ██████╗ ██████╗  █████╗
//  ██╔════╝██╔════╝██╔══██╗╚══██╔══╝██║  ██║██╔════╝██╔══██╗    ██║     ██╔═══██╗██╔══██╗██╔══██╗
//  █████╗  █████╗  ███████║   ██║   ███████║█████╗  ██████╔╝    ██║     ██║   ██║██████╔╝███████║
//  ██╔══╝  ██╔══╝  ██╔══██║   ██║   ██╔══██║██╔══╝  ██╔══██╗    ██║     ██║   ██║██╔══██╗██╔══██║
//  ██║     ███████╗██║  ██║   ██║   ██║  ██║███████╗██║  ██║    ███████╗╚██████╔╝██║  ██║██║  ██║
//  ╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝

#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>

// for feather m0 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match TX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    Serial.begin(9600);
    delay(5000);

    Serial.println("LoRa System Initializing");

    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init())
    {
        Serial.println("LoRa radio init failed");
        while (1)
            ;
    }
    Serial.println("LoRa radio init OK!");

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: %f",RF95_FREQ);

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);
}

float packetnum = 0;

void loop()
{
    if (rf95.available())
    {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {
            float *rbuf = (float *)buf;

            Serial.print("Got: ");
            Serial.println("Packet #: " + (String)rbuf[0]);
            Serial.println("Seconds: " + (String)rbuf[1]);
            Serial.println("Accel X: " + (String)rbuf[2]);
            Serial.println("Accel Y: " + (String)rbuf[3]);
            Serial.println("Accel Z: " + (String)rbuf[4]);
            Serial.print("RSSI: ");
            Serial.println(rf95.lastRssi(), DEC);
        }
        else
        {
            Serial.println("Receive failed");
        }
    }
}