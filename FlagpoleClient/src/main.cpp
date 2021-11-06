//  ███████╗███████╗ █████╗ ████████╗██╗  ██╗███████╗██████╗     ██╗      ██████╗ ██████╗  █████╗
//  ██╔════╝██╔════╝██╔══██╗╚══██╔══╝██║  ██║██╔════╝██╔══██╗    ██║     ██╔═══██╗██╔══██╗██╔══██╗
//  █████╗  █████╗  ███████║   ██║   ███████║█████╗  ██████╔╝    ██║     ██║   ██║██████╔╝███████║
//  ██╔══╝  ██╔══╝  ██╔══██║   ██║   ██╔══██║██╔══╝  ██╔══██╗    ██║     ██║   ██║██╔══██╗██╔══██║
//  ██║     ███████╗██║  ██║   ██║   ██║  ██║███████╗██║  ██║    ███████╗╚██████╔╝██║  ██║██║  ██║
//  ╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝

#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_FXOS8700.h>

// for feather m0 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Change to 915.0 or other frequency, must match TX's freq!
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);

float packetnum = 0;

void setup()
{
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
    Serial.print("Set Freq to: %f", RF95_FREQ);
    
    rf95.setTxPower(23, false);

    if (!accelmag.begin(ACCEL_RANGE_4G))
    {
        Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
        while (1)
            ;
    }
}

void loop()
{
    delay(1000); // Wait 1 second between transmits

    sensors_event_t aevent, mevent;
    accelmag.getEvent(&aevent, &mevent);

    //Display the accel results (acceleration is measured in m/s^2)
    Serial.print("A ");
    Serial.print("X: ");
    Serial.print(aevent.acceleration.x, 4);
    Serial.print("  ");
    Serial.print("Y: ");
    Serial.print(aevent.acceleration.y, 4);
    Serial.print("  ");
    Serial.print("Z: ");
    Serial.print(aevent.acceleration.z, 4);
    Serial.print("  ");
    Serial.println("m/s^2");

    float radiopacket[20] = {packetnum++, (float)millis() / 1000, aevent.acceleration.x, aevent.acceleration.y, aevent.acceleration.z};

    Serial.println("Sending...");
    delay(10);
    rf95.send((uint8_t *)radiopacket, sizeof(radiopacket) * 4);
}