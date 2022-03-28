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
#define MILLIS_MIN 60000
#define MILLIS_30_SEC 30000
#define MILLIS_10_SEC 10000
#define MILLIS_SEC 1000
#define POLLING_FREQ MILLIS_10_SEC

#define RF95_FREQ 915.0
#define MS5_I2C_ADDRESS 0x76
#define BH1750_I2C_ADDRESS 0x23
#define BH1750_MODE ONE_TIME_HIGH_RES_MODE

uint8_t state;
int32_t basePressure;
unsigned long lastMeasure;

RH_RF95 rf95(RF95_CS, RF95_INT);
Adafruit_MPU6050 mpu6050;
BH1750 bh1750(BH1750_I2C_ADDRESS);
ZXCT1107 zxct1107 = ZXCT1107(ZXCT1107_PIN);
Gravity_DO gravitydo = Gravity_DO(GRAVITYDO_PIN);
MS5 ms5(MS5_I2C_ADDRESS);

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

    Serial.print("GRAVITYDO\tFull saturation voltage calibrated to: ");
    Serial.print(gravitydo.cal());
    Serial.println("");
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
    while (!ms5.begin())
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
    digitalWrite(SD_CS, HIGH);
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
    File root = SD.open("/");
    root.rewindDirectory();

    //loop through files
    while (true)
    {
        File dataStorage = root.openNextFile();
        if (!dataStorage)
        {
            // no more files
            Serial.println("**nomorefiles**");
            break;
        }

        Serial.print(dataStorage.name());
        Serial.println("");

        StaticJsonDocument<251> doc;
        deserializeJson(doc, dataStorage);
        uint8_t output[RH_RF95_MAX_MESSAGE_LEN];
        serializeJson(doc, output);
        dataStorage.close();

        digitalWrite(SD_CS, HIGH);

        if (rf95.send(output, sizeof(output)))
        {
            rf95.waitPacketSent();
        }
        else
        {
            Serial.println("RF95\tTransmission Failed!");
        }
    }
    rf95.sleep();
    root.close();
}

// sd card code
void sdInit()
{
    digitalWrite(RF95_CS, HIGH);
    Serial.println("SD\tInitializing");
    while (!SD.begin(SD_CS))
        Serial.println("SD\tInitialization failed!");
    Serial.println("SD\tInitialization success");
}

void ms5ManTest()
{
    Wire.begin(0x76);
    Wire.write(0x1E);
    delay(100);
    Wire.write(0xA2);
    Serial.print(Wire.requestFrom(0x76, 1));
    Serial.println("");
    Serial.print(Wire.available());
    Serial.println("");
    int calib = Wire.read();
    // calib <<= 8;
    // calib |= Wire.read();
    Serial.print(calib);
    Serial.print(" calib ");
    Serial.println("");
    delay(20);
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Initializing");
    Wire.begin();
    SPI.begin();
    pinMode(SD_CS, OUTPUT);
    pinMode(RF95_CS, OUTPUT);
    delay(5000);

    rf95Init();
    mpu6050Init();
    //ms5Init();
    bh1750Init();
    zxct1107Init();
    gravitydoInit();
    sdInit();
    Serial.println("Initialization Complete");
    lastMeasure = millis();
}

void loop()
{
    // //need to poll pressure for state change
    // int32_t pressure = ms5.readPressure();
    // if (pressure <= basePressure * 1.25)
    // {
    //     state = STATE_SUBMERGE;
    //     //state = STATE_SURFACE;
    // }
    // else if (pressure > basePressure * 1.25)
    // {
    //     // disable radio
    //     digitalWrite(RF95_CS, HIGH);
    //     state = STATE_SUBMERGE;
    // }

    state = STATE_SUBMERGE;
    switch (state)
    {
    case STATE_SURFACE: //when surfaced transmit every 10 seconds
        rf95Loop();
        delay(MILLIS_10_SEC);
        break;
    case STATE_SUBMERGE:
        //poll oncer per hour while submerged
        if (millis() >= lastMeasure + POLLING_FREQ)
        {
            StaticJsonDocument<251> packet;
            lastMeasure = millis();

            packet["timeStamp"] = lastMeasure;
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
            //packet["pressure"] = ms5Loop();
            //serializeJsonPretty(packet, Serial);

            //write to sd
            String fileName = String(lastMeasure) + ".txt";
            char fileNameBuf[fileName.length()];
            fileName.toCharArray(fileNameBuf, fileName.length());
            File dataStorage = SD.open(fileNameBuf, FILE_WRITE);
            serializeJson(packet, dataStorage);
            dataStorage.close();

            rf95Loop();
        }
        break;
    default:
        rf95Loop();
        delay(MILLIS_10_SEC);
        break;
    }
}