# PUCKFish Software Documentation

## Contents
- [Overview](#overview)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Development](#development)
- [Building/Uploading](#building/uploading)
- [Use/Debugging](#use/debugging)

## Overview
This is the software documentation for the 2021-22 PUCKFish Senior Design Project for our client, Fathom Fishing. This program runs on an [Adafruit Feather M0 with built-in LoRa RFM95 radio](https://www.adafruit.com/product/3178) connected to our complete PUCKFish electronics board and sensor suite. Within this documentation you will find information about how to install, build, and flash the software onto new boards, dependencies and uses of existing functions, and how to make future changes to the software.

## Dependencies
PUCKFish Software contains the following as software dependencies:
- PlatformIO (either IDE or CLI)
  - [PlatformIO IDE](https://platformio.org/platformio-ide)
  - [PlatformIO CLI](https://platformio.org/install/cli)
- Sensor Drivers
  - [Radiohead RFM95 Driver](https://www.airspayce.com/mikem/arduino/RadioHead/)
  - [BlueRobotics MS5837 Driver](https://github.com/bluerobotics/BlueRobotics_MS5837_Library)
  - [Atlas Scientific Gravity Analog DO Sensor Driver](https://files.atlas-scientific.com/gravity-DO-ardunio-code.pdf)
  - [Adafruit MPU6050 Driver](https://github.com/adafruit/Adafruit_MPU6050)
  - [HP BH1750 Driver](https://github.com/Starmbi/hp_BH1750)
  - Arduino ZXCT1107 Driver (included with install)
  - [Adafruit SD Card Driver](https://github.com/adafruit/SD)
- [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)

## Installation