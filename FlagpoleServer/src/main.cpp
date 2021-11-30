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
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);
}

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
            Serial.println("Orient X: " + (String)rbuf[5]);
            Serial.println("Orient Y: " + (String)rbuf[6]);
            Serial.println("Orient Z: " + (String)rbuf[7]);
            Serial.print("RSSI: ");
            Serial.println(rf95.lastRssi(), DEC);
        }
        else
        {
            Serial.println("Receive failed");
        }
    }
}