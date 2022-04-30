## Welcome to PUCKFish!
Design Team: Will Aracri, Peter Ha, Ammar Hussain, Alex Necakov, Victoria Thomas
### Please view our [wiki](https://github.com/AlexNecakov/PUCKFish/wiki) for more information, including the hardware and software READMEs!

## Introduction
PUCKFish is an underwater array of sensors meant to characterize underwater environments by sensing important information, designed for the 2021-2022 senior design project at Boston University. Originally designed to track Lobsters, this device measures pressure, temperature, dissolved oxygen levels, ambient light, salinity and current speed as well as direction. The system is fully waterproof and meant to survive pressures found at a depth of around 1000 feet and survive for 10 days without a recharge. Data collection begins automatically after submersion and then transmits the data collected to a base station via radio. PUCKFish can be built on a budget of around $150. This document will walk you through the PUCKFish design project, tips, and lessons learned. This document will also point you to other areas on the Github for more information about the relevant subsystems.

## Design Process
The architecture of PUCKFish optimized for two priorities, Cost and Survivability. From these priorities, three major challenges were identified.

1. PCB and Electronics Design: Any electronics used must use low-current draw. The electronics must also be recharged without introducing a risk of water-intrusion. Any sensors used must be low-cost.
2. Sensing Current Speed and Direction: Any method used to measure current speed and direction must be robust and simple.
3. Enclosure Design: An enclosure must be able to protect against high-pressure environments as well as protect electronics from saltwater intrusion.
Any design change should carefully consider these challenges and decide on the appropriate trades to achieve survivability and a low cost.

## PCB and Electronics Design
PUCKFish must read 6 different metrics from onboard sensors, store the data locally, transmit the data to a shipboard, and be rechargeable. Our software is written in C using the Arduino Framework and can be easily installed, modified, built and uploaded following instructions found on the [Software Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Software-Overview) page of our wiki. Our main software can be found in the [/FlagpoleClient](https://github.com/AlexNecakov/PUCKFish/tree/master/FlagpoleClient) directory. Our electronics hardware consists of several sensors, a LiPo battery, SD card, and Adafruit Feather M0 with LoRa radio to control the system/broadcast data over the LoRa 900 mHz frequency. All of these components were put together on a custom PCB designed in Eagle. The PCB currently lacks a location for the SD card and Dissolved Oxygen sensor to be includeded, and should be added in future versions of this project. Information about our hardware, including sensor vendor links and specifications, data sheets, and renders of our PCB layouts can be found on the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview) wiki page. PCB project information can be found in the [/PCB/Eagle Project](https://github.com/AlexNecakov/PUCKFish/tree/master/PCB/Eagle%20Project) directory.

PUCKFish uses Qi wireless charging in order to replenish the battery. Using wireless charging eliminates the possibility of water-intrusion or corroded charging leads. 

PUCKFish uses several types of sensors to read back information to the PCB. Information on the sensors may be found in the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview) page and the software mounted on them may be found in the [Software Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Software-Overview) page. These sensors were selected due to price, power draw, supply voltage, measurement range, and waterproof capabilities.

PUCKFish uses a LoRa capable radio to transmit data over the 900 mHz frequency in JSON formatted packets. This radio is mounted to the Feather Board and begins sending the data once the PUCKFish has surfaced from the ocean. For more information on the radio, see the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview) page for the specifications of the hardware and the [Software Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Software-Overview) page for the software that is used to collect/transmit the data.

Several sensors require access to the water to measure their respective characteristics. The pressure sensor uses a -08 O-ring to attach a vinyl tube which then leads to the exterior while pouring the epoxy. The salinity sensor uses two wire leads that sense the current produced between them, which then calculates the salinity. These wire leads must be exposed when pouring the epoxy. The dissolved oxygen sensor contains a galvanic probe. This probe must be prepped with a chemical provided from the sensor manufacturer. The probe must also be exposed while pouring the epoxy. The silicone mold as discussed below has a hole that these sensors can be routed through to achieve exposure to the water and must be routed through this hole before pouring the epoxy. 

## Sensing Current Speed and Direction
PUCKFish uses an IMU to measure current speed and direction. Because PUCKFish is shaped as a sphere, how it leans in the water determines which direction and how fast the water is moving. The decision as to how PUCKFish measures current speed and direction is an important factor as to how the device is shaped overall. Other methods of measuring current, like weather vanes or flow tubes, are too delicate and are unable to operate for long periods underwater with any degree of reliability. The design of PUCKFish’s enclosure can be found below. Information about the IMU implementation can be found in the wiki in both the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview) and [Software Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Software-Overview) pages.

## Enclosure Design
PUCKFish uses a 3d Printed skeleton to hold electronic components and is encased within an epoxy shell. Sensors that need access to the water protrude from the epoxy shell in order to successfully measure the required components.

The 3d Printed Skeleton is the product of several steps of iteration to fit the PCB and accommodate the Qi wireless charging receiver. By using the newest version of the skeleton and printing on a 3D printer of correct size, this element may be printed with no extra manufacturing steps. Inserting the PCB is simple. For more information on the 3D printed skeleton, view relevant documentation in the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview) page.

The epoxy shell was developed specifically with survivability and cost in mind. Metallic or plastic structures containing the electronics introduce the problem of seams connecting the devices. In each of these cases, expensive manufacturing techniques as well as complex sealing surfaces between the seams of segments are damaging to both cost as well as survivability. Manufacturing the epoxy shell requires a series of two molds to form, one mold that forms the final silicone form, and the final silicone form that creates the epoxy shell. It is essential that the silicone mold is used to form the epoxy casing, as 3D printed molds are too porous for the epoxy to form in and other machined elements are too costly to make. To find the mold’s as well as the mold making process, go to the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview) page or find the relevant CAD models [/Structure Cad](https://github.com/AlexNecakov/PUCKFish/tree/master/Structure%20CAD) directory.

## Manufacturing PUCKFish
For a general description of how to manufacture PUCKFish follow the instructions below.
1. First, populate the PCB with the required components as shown in the [Hardware Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Hardware-Overview)
2. Build and upload the FlagpoleClient program to the Feather Board as described in the [Software Overview](https://github.com/AlexNecakov/PUCKFish/wiki/Software-Overview)
3. 3D print the PCB skeleton for the electronics.
4. 3D print the Silicone mold molds.
5. Pour the silicone molds for epoxy. 
6. Install the populated PCB into the 3D printed skeleton
7. Install the aluminum block into the top of the 3D printed skeleton with epoxy
8. Screw in the eyebolt to the aluminum block.
9. Install the 3D printed skeleton with populated PCB into the silicone mold, with the eyebolt pointed through the mold.
10. Pour epoxy into the silicone mold in three parts. DO NOT POUR EPOXY IN ONE BATCH (The heat for the epoxy may damage electrical components)
11. Wait three days in between epoxy cures. The device is ready to operate after charging after the last cure period.

## Operation
See our [wiki](https://github.com/AlexNecakov/PUCKFish/wiki) or [Users Manual](https://github.com/AlexNecakov/PUCKFish/blob/master/Reports/Users%20Manual.docx) for additional information on how to operate PUCKFish.
