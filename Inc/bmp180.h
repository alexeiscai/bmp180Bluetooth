/*
 * bmp18.h
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
//#define BMP_UT_REQUEST_VAL  (0x2E)

// Pressure measurment :

#define ATM_PRESSURE 	    		(101325.0f) // pressure in Pa
#define PRESS_CONST_COEFICIENT 	    (44330.0f)

// #define ATM_PRESSURE (101325)

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
}BmpCallibrationData;

typedef struct
{
	float temp;
	int32_t press;
	float altitude;
}MeasuredData;

typedef struct
{
	BmpCallibrationData calib;
	MeasuredData uncomp;
	MeasuredData data;
}BmpData;

void bmpInit(BmpData* bmpData);

bool isBmpConnected(void);

float getAltitude(BmpData bmpData, int oss);

void readCallibrationData(BmpData* bmpData);

uint16_t getUTemp(void);

float getTemp(BmpData* bmpData);

float getPressure(BmpData bmpData, int oss);

uint32_t getUpress(int oss);

#endif /* INC_BMP180_H_ */
