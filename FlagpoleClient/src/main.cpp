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
#include <MS5837.h>

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

#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0

#define RF95_FREQ 915.0
#define MS5_I2C_ADDRESS 0x76
#define BH1750_I2C_ADDRESS 0x23
#define BH1750_MODE ONE_TIME_HIGH_RES_MODE

#define SEA_LEVEL_MBAR 1013.25

uint8_t state;
unsigned long lastMeasure;

RH_RF95 rf95(RF95_CS, RF95_INT);
Adafruit_MPU6050 mpu6050;
BH1750 bh1750(BH1750_I2C_ADDRESS);
ZXCT1107 zxct1107 = ZXCT1107(ZXCT1107_PIN);
Gravity_DO gravitydo = Gravity_DO(GRAVITYDO_PIN);
MS5837 ms5;

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
    bh1750.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
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

float zxct1107Loop()
{
    float salinity = zxct1107.read_salinity();

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

    ms5.setModel(MS5837::MS5837_30BA);

    ms5.setFluidDensity(1029); // kg/m^3 (freshwater, 1029 for seawater)

    Serial.println("MS5\tInit success");
}

float ms5Loop()
{
    ms5.read();
    float depth = ms5.depth();

    return depth;
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
            //Serial.println("**nomorefiles**");
            break;
        }

        //Serial.print(dataStorage.name());
        //Serial.println("");

        StaticJsonDocument<251> doc;
        deserializeJson(doc, dataStorage);

        // serializeJson(doc, Serial);
        uint8_t output[RH_RF95_MAX_MESSAGE_LEN];
        serializeJson(doc, output);
        // Serial.print(output);
        // Serial.println("");
        dataStorage.close();

        digitalWrite(SD_CS, HIGH);

        if (rf95.send(output, sizeof(output)))
        {
            rf95.waitPacketSent();
            Serial.print("Sent ");
            Serial.print(sizeof(output));
            Serial.println(" bytes");
        }
        else
        {
            // Serial.println("RF95\tTransmission Failed!");
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

    Serial.println("SD\tClearing storage");
    File root = SD.open("/");
    root.rewindDirectory();

    //loop through files to delete
    while (true)
    {
        File dataStorage = root.openNextFile();
        if (!dataStorage)
        {
            // no more files
            //Serial.println("**nomorefiles**");
            break;
        }

        SD.remove(dataStorage.name());
        dataStorage.close();
    }
    root.close();
    //Serial.println("SD\tStorage cleared");
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
    ms5Init();
    bh1750Init();
    zxct1107Init();
    gravitydoInit();
    sdInit();
    Serial.println("Initialization Complete");
    lastMeasure = millis();
}

void loop()
{
    // //need to poll pressure for state 
    // //Serial.println("Reading begin");
    // ms5.read();
    // //Serial.println("Reading end");
    // float pressure = ms5.pressure();
    // //Serial.print(pressure);
    // //Serial.println("");
    // if (pressure <= SEA_LEVEL_MBAR * 1.01)
    // {
    //     state = STATE_SURFACE;
    // }
    // else if (pressure > SEA_LEVEL_MBAR * 1.01)
    // {
    //     // disable radio
    //     digitalWrite(RF95_CS, HIGH);
    //     state = STATE_SUBMERGE;
    // }

    state = STATE_SUBMERGE;
    //Serial.print(state);
    //Serial.println("");

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

            packet["#"] = lastMeasure;
            sensors_event_t aEvent, gEvent, tEvent;
            mpu6050.getEvent(&aEvent, &gEvent, &tEvent);
            packet["a"][0] = aEvent.acceleration.x;
            packet["a"][1] = aEvent.acceleration.y;
            packet["a"][2] = aEvent.acceleration.z;
            packet["o"][0] = gEvent.orientation.x;
            packet["o"][1] = gEvent.orientation.y;
            packet["o"][2] = gEvent.orientation.z;
            packet["t"] = tEvent.temperature;
            packet["l"] = bh1750Loop();
            packet["s"] = zxct1107Loop();
            packet["d"] = gravitydoLoop();
            packet["p"] = ms5Loop();
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
        delay(MILLIS_10_SEC);
        break;
    default:
        rf95Loop();
        delay(MILLIS_10_SEC);
        break;
    }
}