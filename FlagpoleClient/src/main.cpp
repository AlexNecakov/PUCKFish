// PUCKFish Client Code
// Alex Necakov 2022

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <RH_RF95.h>
#include <Adafruit_MPU6050.h>
#include <BH1750.h>
#include <zxct.h>
#include <do_grav.h>

// Pin Defines
#define RF95_CS 8
#define RF95_RST 4
#define RF95_INT 3

#define ZXCT1107_PIN A0
#define GRAVITYDO_PIN A1
#define VBAT_PIN A7

// Value Defines
#define POLLING_FREQ 5
#define RF95_FREQ 915.0
#define BH1750_I2C_ADDRESS 0x23
#define BH1750_MODE ONE_TIME_HIGH_RES_MODE

bool isSurfaced;
int packNum = 0;

RH_RF95 rf95(RF95_CS, RF95_INT);
Adafruit_MPU6050 mpu6050;
BH1750 bh1750(BH1750_I2C_ADDRESS);
ZXCT1107 zxct1107 = ZXCT1107(ZXCT1107_PIN);
Gravity_DO gravitydo = Gravity_DO(GRAVITYDO_PIN);

// DynamicJsonDocument packet(200);
// JsonArray timeStamp = packet.createNestedArray("timeStamp");
// //JsonArray dissolvedOxygen = packet.createNestedArray("dissolvedOxygen");
// JsonArray ambientLight = packet.createNestedArray("ambientLight");
//JsonArray salinity = packet.createNestedArray("salinity");
// //JsonArray depth = packet.createNestedArray("depth");
// JsonArray temperature = packet.createNestedArray("temperature");
// JsonArray accelerationX = packet.createNestedArray("accelerationX");
// JsonArray accelerationY = packet.createNestedArray("accelerationY");
// JsonArray accelerationZ = packet.createNestedArray("accelerationZ");
// JsonArray orientationX = packet.createNestedArray("orientationX");
// JsonArray orientationY = packet.createNestedArray("orientationY");
// JsonArray orientationZ = packet.createNestedArray("orientationZ");

//mpu6050 accel/gyro/temp sensor code
int mpu6050Init()
{
    while (!mpu6050.begin())
        Serial.println("MPU6050\tInit failed");
    Serial.println("MPU6050\tInit success");
    return 0;
}

// void mpu6050Loop(StaticJsonDocument packet)
// {
//     sensors_event_t aEvent, gEvent, tEvent;
//     mpu6050.getEvent(&aEvent, &gEvent, &tEvent);
//     packet["accelerationX"] = (aEvent.acceleration.x);
//     packet["accelerationY"] = (aEvent.acceleration.y);
//     packet["accelerationZ"] = (aEvent.acceleration.z);
//     packet["orientationX"] = (gEvent.gyro.x);
//     packet["orientationY"] = (gEvent.gyro.y);
//     packet["orientationZ"] = (gEvent.gyro.z);
//     packet["temperature"] = (tEvent.temperature);
// }

//bh1750 light sensor code
int bh1750Init()
{
    while (!bh1750.begin(BH1750::BH1750_MODE))
        Serial.println("BH1750\tInit failed");
    Serial.println("BH1750\tInit success");

    return 0;
}

// void bh1750Loop(StaticJsonDocument packet)
// {
//     while (!bh1750.measurementReady(true))
//     {
//         yield();
//     }
//     packet["ambientLight"] = (bh1750.readLightLevel());
//     bh1750.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
// }

// //zxct1107 salinity sensor code
// int zxct1107Init()
// {
//     while (!zxct1107.begin())
//         Serial.println("ZXCT1107\tInit failed");
//     Serial.println("ZXCT1107\tInit success");

//     return 0;
// }

// void zxct1107Loop()
// {
//     salinity.add(zxct1107.read_voltage());
// }

// //gravity dissolved oxygen sensor code
// int gravitydoInit()
// {
//     while (!gravitydo.begin())
//         Serial.println("GRAVITYDO\tInit failed");
//     Serial.println("GRAVITYDO\tInit success");

//     // gravitydo.cal();
//     // Serial.println("GRAVITYDO\tCalibrated");

//     return 0;
// }

// void gravitydoLoop()
// {
//     dissolvedOxygen.add(gravitydo.read_do_percentage());
// }

//rfm95 radio code
int rf95Init()
{
    Serial.println("RF95\tInitializing");
    while (!rf95.init())
        Serial.println("RF95\tInit failed");
    Serial.println("RF95\tInit success");

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    while (!rf95.setFrequency(RF95_FREQ))
        Serial.println("RF95\tSet frequency failed");
    Serial.print("RF95\tSet frequency to: ");
    Serial.println(RF95_FREQ);

    rf95.setTxPower(23, false);

    return 0;
}

// void rf95Loop(StaticJsonDocument packet)
// {
//     uint8_t output[sizeof(packet)];
//     serializeJson(packet, output);
//     serializeJsonPretty(packet, Serial);
//     rf95.send(output, sizeof(output));
//     rf95.waitPacketSent();
// }

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    while (!Serial)
        delay(10);
    Serial.println("Setup begin");
    //implement when pressure sensor is here
    isSurfaced = false;

    rf95Init();
    mpu6050Init();
    bh1750Init();
    //zxct1107Init();
    ///gravitydoInit();
    Serial.println("Setup finished");
}

void loop()
{
    Serial.println("Looping");
    StaticJsonDocument<200> packet;
    //get readings
    if (!isSurfaced)
    {
        packet["timeStamp"] = (millis());
        sensors_event_t aEvent, gEvent, tEvent;
        mpu6050.getEvent(&aEvent, &gEvent, &tEvent);
        packet["accelerationX"] = (aEvent.acceleration.x);
        packet["accelerationY"] = (aEvent.acceleration.y);
        packet["accelerationZ"] = (aEvent.acceleration.z);
        packet["orientationX"] = (gEvent.gyro.x);
        packet["orientationY"] = (gEvent.gyro.y);
        packet["orientationZ"] = (gEvent.gyro.z);
        packet["temperature"] = (tEvent.temperature);

        while (!bh1750.measurementReady(true))
        {
            yield();
        }
        packet["ambientLight"] = (bh1750.readLightLevel());
        bh1750.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
        // mpu6050Loop(packet);
        // bh1750Loop(packet);
        //zxct1107Loop();
        //gravitydoLoop();
    }
    Serial.println("Finished readings");
    delay(1000);

    //send when surfaced (currently debug code)
    //if(packNum++%POLLING_FREQ == 0)
    // rf95Loop(packet);
    uint8_t output[sizeof(packet)];
    serializeJson(packet, output);
    serializeJsonPretty(packet, Serial);
    Serial.println("");
    rf95.send(output, sizeof(output));
    rf95.waitPacketSent();

    Serial.println("Packet Sent");
}