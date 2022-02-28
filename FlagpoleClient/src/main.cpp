// PUCKFish Client Code
// Alex Necakov 2022

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
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
#define POLLING_FREQ 50
#define RF95_FREQ 915.0
#define BH1750_I2C_ADDRESS 0x23
#define BH1750_MODE ONE_TIME_HIGH_RES_MODE

RH_RF95 rf95(RF95_CS, RF95_INT);
Adafruit_MPU6050 mpu6050;
BH1750 bh1750(BH1750_I2C_ADDRESS);
ZXCT1107 zxct1107 = ZXCT1107(ZXCT1107_PIN);
Gravity_DO gravitydo = Gravity_DO(GRAVITYDO_PIN);

float packetnum = 0;
sensors_event_t aEvent, gEvent, tEvent;

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

void rf95Loop()
{
}

//mpu6050 accel/gyro/temp sensor code
int mpu6050Init()
{
    while (!mpu.begin())
        Serial.println("MPU6050\tInit failed");
    Serial.println("MPU6050\tInit success");

    return 0;
}

void mpu6050Loop()
{
    mpu6050.getEvent(&aEvent, &gEvent, &tEvent);
}

//bh1750 light sensor code
int bh1750Init()
{
    while (!lightMeter.begin(lightMeter.begin(BH1750::BH1750_MODE)))
        Serial.println("BH1750\tInit failed");
    Serial.println("BH1750\tInit success");

    return 0;
}

//time per measurement - 120ms
float bh1750Loop()
{
    while (!lightMeter.measurementReady(true))
    {
        yield();
    }
    float lux = lightMeter.readLightLevel();
    lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
    return lux;
}

//zxct1107 salinity sensor code
int zxct1107Init()
{
    while (!zxct1107.begin())
        Serial.println("ZXCT1107\tInit failed");
    Serial.println("ZXCT1107\tInit success");

    return 0;
}

float zxct1107Loop()
{
    return zxct1107.read_voltage();
}

//gravity dissolved oxygen sensor code
int gravitydoInit()
{
    while (!gravitydo.begin())
        Serial.println("GRAVITYDO\tInit failed");
    Serial.println("GRAVITYDO\tInit success");

    gravitydo.cal();
    Serial.println("GRAVITYDO\tCalibrated");

    return 0;
}

float gravitydoLoop()
{
    return gravitydo.read_do_percentage();
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    rf95Init();
    mpu6050Init();
    bh1750Init();
    zxct1107Init();
    gravitydoInit();
}

void loop()
{
    //get readings
    float luxReading = mpu6050Loop();
    bh1750Loop();
    float salinity = zxct1107Loop();
    float doxReading = gravitydoLoop();

    rf95Loop();
}