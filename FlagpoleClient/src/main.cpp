// PPPPPPPPPPPPPPPPP   UUUUUUUU     UUUUUUUU       CCCCCCCCCCCCCKKKKKKKKK    KKKKKKKFFFFFFFFFFFFFFFFFFFFFF  iiii                  hhhhhhh
// P::::::::::::::::P  U::::::U     U::::::U    CCC::::::::::::CK:::::::K    K:::::KF::::::::::::::::::::F i::::i                 h:::::h
// P::::::PPPPPP:::::P U::::::U     U::::::U  CC:::::::::::::::CK:::::::K    K:::::KF::::::::::::::::::::F  iiii                  h:::::h
// PP:::::P     P:::::PUU:::::U     U:::::UU C:::::CCCCCCCC::::CK:::::::K   K::::::KFF::::::FFFFFFFFF::::F                        h:::::h
//   P::::P     P:::::P U:::::U     U:::::U C:::::C       CCCCCCKK::::::K  K:::::KK   F:::::F       FFFFFFiiiiiii     ssssssssss   h::::h hhhhh
//   P::::P     P:::::P U:::::D     D:::::UC:::::C                K:::::K K:::::K     F:::::F             i:::::i   ss::::::::::s  h::::hh:::::hhh
//   P::::PPPPPP:::::P  U:::::D     D:::::UC:::::C                K::::::K:::::K      F::::::FFFFFFFFFF    i::::i ss:::::::::::::s h::::::::::::::hh
//   P:::::::::::::PP   U:::::D     D:::::UC:::::C                K:::::::::::K       F:::::::::::::::F    i::::i s::::::ssss:::::sh:::::::hhh::::::h
//   P::::PPPPPPPPP     U:::::D     D:::::UC:::::C                K:::::::::::K       F:::::::::::::::F    i::::i  s:::::s  ssssss h::::::h   h::::::h
//   P::::P             U:::::D     D:::::UC:::::C                K::::::K:::::K      F::::::FFFFFFFFFF    i::::i    s::::::s      h:::::h     h:::::h
//   P::::P             U:::::D     D:::::UC:::::C                K:::::K K:::::K     F:::::F              i::::i       s::::::s   h:::::h     h:::::h
//   P::::P             U::::::U   U::::::U C:::::C       CCCCCCKK::::::K  K:::::KK   F:::::F              i::::i ssssss   s:::::s h:::::h     h:::::h
// PP::::::PP           U:::::::UUU:::::::U  C:::::CCCCCCCC::::CK:::::::K   K::::::KFF:::::::FF           i::::::is:::::ssss::::::sh:::::h     h:::::h
// P::::::::P            UU:::::::::::::UU    CC:::::::::::::::CK:::::::K    K:::::KF::::::::FF           i::::::is::::::::::::::s h:::::h     h:::::h
// P::::::::P              UU:::::::::UU        CCC::::::::::::CK:::::::K    K:::::KF::::::::FF           i::::::i s:::::::::::ss  h:::::h     h:::::h
// PPPPPPPPPP                UUUUUUUUU             CCCCCCCCCCCCCKKKKKKKKK    KKKKKKKFFFFFFFFFFF           iiiiiiii  sssssssssss    hhhhhhh     hhhhhhh

#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <RH_RF95.h>
#include <Adafruit_MPU6050.h>
#include <BH1750.h>

// for feather m0 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define VBATPIN A7

// Change to 915.0 or other frequency, must match TX's freq!
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);
Adafruit_MPU6050 mpu;
BH1750 lightMeter(0x23);

float packetnum = 0;

void setup()
{
    Serial.begin(9600);
    Wire.begin();
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
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    rf95.setTxPower(23, false);

    if (!mpu.begin())
    {
        Serial.println("Sensor init failed");
        while (1)
            yield();
    }
    Serial.println("Found a MPU-6050 sensor");

    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
    {
        Serial.println(F("BH1750 Advanced begin"));
    }
    else
    {
        Serial.println(F("Error initialising BH1750"));
    }
}

void loop()
{
    delay(1000); // Wait 1 second between transmits

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    //Display the accel results (acceleration is measured in m/s^2)
    Serial.print("Accelerometer ");
    Serial.print("X: ");
    Serial.print(a.acceleration.x, 1);
    Serial.print(" m/s^2, ");
    Serial.print("Y: ");
    Serial.print(a.acceleration.y, 1);
    Serial.print(" m/s^2, ");
    Serial.print("Z: ");
    Serial.print(a.acceleration.z, 1);
    Serial.println(" m/s^2");

    if (lightMeter.measurementReady())
    {
        float lux = lightMeter.readLightLevel();
        Serial.print("Light: ");
        Serial.print(lux);
        Serial.println(" lx");
    }

    float radiopacket[40] = {packetnum++, (float)millis() / 1000, aevent.acceleration.x, aevent.acceleration.y, aevent.acceleration.z, event.gyro.x, event.gyro.y, event.gyro.z};

    Serial.println("Sending...");
    Serial.println();
    delay(10);
    rf95.send((uint8_t *)radiopacket, sizeof(radiopacket) * 4);

    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    Serial.print("VBat: ");
    Serial.println(measuredvbat);

    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
}