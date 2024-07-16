# BMP180 Sensor Library

This library provides functions to interface with the BMP180 sensor using an STM32 microcontroller. The BMP180 is a barometric pressure sensor that can measure temperature and pressure, and calculate altitude based on pressure readings. Additionally, in the main.c sensor data is transmited via Bluetooth using HAL_UART_Transmit(), an UART communication is configured for HC-05 module.

## Table of Contents
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)

## Features

- Initialize the BMP180 sensor
- Read uncompensated temperature and pressure values
- Calculate actual temperature and pressure
- Calculate altitude based on pressure readings
- Check sensor connection

## Prerequisites

- STM32 HAL library
- I2C configured for communication with BMP180 sensor

## Installation

1. Copy the `bmp180.h` and `bmp180.c` files to your project directory.
2. Include the `bmp180.h` header file in your main code or wherever you need to use the BMP180 functions.

## Usage

### Initializing the Sensor

Before using the sensor, initialize it by reading the calibration data:

```c
#include "bmp180.h"

// Define the I2C handle (extern if defined elsewhere)
extern I2C_HandleTypeDef hi2c1;

BmpData bmpData;

int main(void)
{
    // HAL initialization code

    // Initialize BMP180
    bmpInit(&bmpData);

  float temperature = getTemp(&bmpData);

  int oss = 0; // Oversampling setting (0 to 3)
  float pressure = getPressure(bmpData, oss);
  float altitude = getAltitude(bmpData, oss);

}
