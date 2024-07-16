/*
 * bmp180.h
 *
 *  Created on: Jul 3, 2024
 *      Author: AScai
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#include <stdbool.h>
#include <math.h>

#include "stm32f1xx_hal.h"

#define BMP_CHIP_ID_REG		(0xD0)
#define BMP_I2C_TIMEOUT	    (50U)
#define BMP_CHIP_ID			(0x55)
#define BMP_READ_ADDR		(0xEF)
#define BMP_WRITE_ADDR		(0xEE)
#define BMP_CALIB_ADDR  	(0xAA)

#define ATM_PRESSURE 	    		(101325.0f) // pressure in Pa
#define PRESS_CONST_COEFICIENT 	    (44330.0f)

typedef struct
{
	short AC1;
	short AC2;
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short B1;
	short B2;
	short MB;
	short MC;
	short MD;
}BmpCalibrationData;

typedef struct
{
	float temp;
	int32_t press;
	float altitude;
}MeasuredData;

typedef struct
{
	BmpCalibrationData calib;
	MeasuredData uncomp;
	MeasuredData data;
}BmpData;


/**
 * @brief Initializes the BMP180 sensor.
 * @param bmpData: Pointer to BmpData structure to store calibration data.
 */
void bmpInit(BmpData* bmpData);


/**
 * @brief Checks if the BMP180 sensor is connected.
 * @return true if connected, false otherwise.
 */
bool isBmpConnected(void);


/**
 * @brief Calculates altitude based on the measured pressure and temperature.
 * @param bmpData: BmpData structure containing the measured data.
 * @param oss: Oversampling setting (0 to 3) for pressure measurement.
 * @return Calculated altitude in meters.
 */
float getAltitude(BmpData bmpData, int oss);


/**
 * @brief Reads calibration data from the BMP180 sensor and stores it in bmpData.
 * @param bmpData: Pointer to BmpData structure to store calibration data.
 */
void readCalibrationData(BmpData* bmpData);

/**
 * @brief Reads the uncompensated temperature value from the BMP180 sensor.
 * @return The uncompensated temperature value as a 16-bit unsigned integer.
 */
uint16_t getUTemp(void);

/**
 * @brief Calculates the actual temperature in degrees Celsius.
 * @param bmpData Pointer to a BmpData structure containing calibration and measured data.
 * @return The actual temperature in degrees Celsius.
 */
float getTemp(BmpData* bmpData);

/**
 * @brief Calculates the actual pressure in Pascals.
 * @param bmpData BmpData structure containing calibration and measured data.
 * @param oss Oversampling setting (0 to 3) for pressure measurement.
 * @return The actual pressure in Pascals.
 */
float getPressure(BmpData bmpData, int oss);

/**
 * @brief Reads the uncompensated pressure value from the BMP180 sensor.
 * @param oss Oversampling setting (0 to 3) for pressure measurement.
 * @return The uncompensated pressure value as a 32-bit unsigned integer.
 */
uint32_t getUpress(int oss);

#endif /* INC_BMP180_H_ */
