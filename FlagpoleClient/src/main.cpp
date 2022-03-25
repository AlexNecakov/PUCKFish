// PUCKFish Client Code
// Alex Necakov 2022

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <RH_RF95.h>
#include <Adafruit_MPU6050.h>
#include <BH1750.h>
#include <zxct.h>
#include <do_grav.h>
#include <ms5.h>

// State Defines
#define STATE_SURFACE 0
#define STATE_SUBMERGE 1

// Pin Defines
#define RF95_INT 3
#define RF95_RST 4
#define RF95_CS 8
#define SD_CS 10

#define ZXCT1107_PIN A0
#define GRAVITYDO_PIN A1
#define VBAT_PIN A7

// Value Defines
#define MILLIS_HR 3600000
#define MILLIS_30_SEC 30000
#define MILLIS_10_SEC 10000
#define MILLIS_SEC 1000
#define POLLING_FREQ MILLIS_10_SEC

#define RF95_FREQ 915.0
#define MS5_I2C_ADDRESS 0x76
#define BH1750_I2C_ADDRESS 0x23
#define BH1750_MODE ONE_TIME_HIGH_RES_MODE

uint8_t state;
uint8_t packetNum;
int32_t basePressure;
unsigned long lastMeasure;

RH_RF95 rf95(RF95_CS, RF95_INT);
Adafruit_MPU6050 mpu6050;
BH1750 bh1750(BH1750_I2C_ADDRESS);
ZXCT1107 zxct1107 = ZXCT1107(ZXCT1107_PIN);
Gravity_DO gravitydo = Gravity_DO(GRAVITYDO_PIN);
MS5 ms5(MS5_I2C_ADDRESS);
File dataStorage;

//mpu6050 accel/gyro/temp sensor code
void mpu6050Init()
{
    Serial.println("MPU6050\tInitializing");
    while (!mpu6050.begin())
        Serial.println("MPU6050\tInit failed");
    Serial.println("MPU6050\tInit success");
}

//bh1750 light sensor code
void bh1750Init()
{
    Serial.println("BH1750\tInitializing");
    while (!bh1750.begin(BH1750::BH1750_MODE))
        Serial.println("BH1750\tInit failed");
    Serial.println("BH1750\tInit success");
}

float bh1750Loop()
{
    float lux = bh1750.readLightLevel();

    bh1750.configure(BH1750::ONE_TIME_HIGH_RES_MODE);

    return lux;
}

//zxct1107 salinity sensor code
void zxct1107Init()
{
    Serial.println("ZXCT1107\tInitializing");
    while (!zxct1107.begin())
        Serial.println("ZXCT1107\tInit failed");
    Serial.println("ZXCT1107\tInit success");
}

int16_t zxct1107Loop()
{
    int16_t salinity = zxct1107.read_salinity();
    return salinity;
}

//gravity dissolved oxygen sensor code
void gravitydoInit()
{
    Serial.println("GRAVITYDO\tInitializing");
    while (!gravitydo.begin())
        Serial.println("GRAVITYDO\tInit failed");
    Serial.println("GRAVITYDO\tInit success");

    //gravitydo.cal();
    Serial.println("GRAVITYDO\tCalibrated");
}

float gravitydoLoop()
{
    float dissolvedOxygen = gravitydo.read_do_percentage();
    return dissolvedOxygen;
}

//pressure sensor code
void ms5Init()
{
    Serial.println("MS5\tInitializing");
    while (!ms5.begin(MS5_I2C_ADDRESS))
        Serial.println("MS5\tInit failed");
    basePressure = ms5.readPressure();

    Serial.println("MS5\tInit success");
}

int32_t ms5Loop()
{
    int32_t pressure = ms5.readPressure();

    return pressure;
}

//rfm95 radio code
void rf95Init()
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
}

void rf95Loop()
{
    dataStorage = SD.open("storage.json", FILE_READ);
    StaticJsonDocument<192000> doc;
    deserializeJson(doc, dataStorage);
    uint8_t output[sizeof(doc)];
    serializeJson(doc, output);
    rf95.send(output, sizeof(output));
    rf95.waitPacketSent();
    dataStorage.close();
}

// sd card code
void sdInit()
{
    Serial.println("SD\tInitializing");
    while (!SD.begin(SD_CS))
        Serial.println("SD\tInitialization failed!");

    // create storage file
    dataStorage = SD.open("storage.json", FILE_WRITE);
    dataStorage.close();
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    delay(5000);

    rf95Init();
    mpu6050Init();
    ms5Init();
    bh1750Init();
    zxct1107Init();
    gravitydoInit();
    lastMeasure = millis();
    //sdInit();
}

void loop()
{
    //need to poll pressure for state change
    int32_t pressure = ms5.readPressure();
    if (pressure <= basePressure * 1.25)
        state = STATE_SUBMERGE;
        //state = STATE_SURFACE;
    else if (pressure > basePressure * 1.25)
        state = STATE_SUBMERGE;

    switch (state)
    {
    case STATE_SURFACE: //when surfaced transmit every 10 seconds
        rf95Loop();
        break;
    case STATE_SUBMERGE:
        //poll oncer per hour while submerged
        if (millis() >= lastMeasure + POLLING_FREQ)
        {
            lastMeasure = millis();

            StaticJsonDocument<256> packet;

            packet["timeStamp"] = millis();
            sensors_event_t aEvent, gEvent, tEvent;
            mpu6050.getEvent(&aEvent, &gEvent, &tEvent);
            packet["acceleration"][0] = aEvent.acceleration.x;
            packet["acceleration"][1] = aEvent.acceleration.y;
            packet["acceleration"][2] = aEvent.acceleration.z;
            packet["orientation"][0] = gEvent.orientation.x;
            packet["orientation"][1] = gEvent.orientation.y;
            packet["orientation"][2] = gEvent.orientation.z;
            packet["temperature"] = tEvent.temperature;
            packet["ambientLight"] = bh1750Loop();
            packet["salinity"] = zxct1107Loop();
            packet["dissolvedOxygen"] = gravitydoLoop();
            packet["pressure"] = ms5Loop();
            serializeJsonPretty(packet, Serial);

            // write to sd
            // dataStorage = SD.open("storage.json", FILE_WRITE);
            // serializeJson(packet, dataStorage);
            // dataStorage.close();
        }
        break;
    default:
        rf95Loop();
        break;
    }

    delay(10000);
}