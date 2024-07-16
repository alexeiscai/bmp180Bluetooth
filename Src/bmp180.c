/*
 * bmp180.c
 *
 *  Created on: Jul 3, 2024
 *      Author: AScai
 */

#include <bmp180.h>

extern I2C_HandleTypeDef hi2c1;

bool isBmpConnected(void)
{
	uint8_t chipId = 0;
	HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_CHIP_ID_REG, 1, &chipId, 1, BMP_I2C_TIMEOUT);

	if ( BMP_CHIP_ID == chipId)
	{
		return true;
	}

	return false;
}

void readCalibrationData(BmpData* bmpData)
{
	uint8_t calibData[22] = {0};
	HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, BMP_CALIB_ADDR, 1, calibData, 22, HAL_MAX_DELAY);

	bmpData->calib.AC1 = ((calibData[0] << 8) | calibData[1]);
	bmpData->calib.AC2 = ((calibData[2] << 8) | calibData[3]);
	bmpData->calib.AC3 = ((calibData[4] << 8) | calibData[5]);
	bmpData->calib.AC4 = ((calibData[6] << 8) | calibData[7]);
	bmpData->calib.AC5 = ((calibData[8] << 8) | calibData[9]);
	bmpData->calib.AC6 = ((calibData[10] << 8) | calibData[11]);
	bmpData->calib.B1 = ((calibData[12] << 8) | calibData[13]);
	bmpData->calib.B2 = ((calibData[14] << 8) | calibData[15]);
	bmpData->calib.MB = ((calibData[16] << 8) | calibData[17]);
	bmpData->calib.MC = ((calibData[18] << 8) | calibData[19]);
	bmpData->calib.MD = ((calibData[20] << 8) | calibData[21]);
}

uint16_t getUTemp(void)
{
	uint8_t datatowrite = 0x2E;
	uint8_t rawTemp[2] = {0};
	HAL_I2C_Mem_Write(&hi2c1, BMP_WRITE_ADDR, 0xF4, 1, &datatowrite, 1, 1000);
	HAL_Delay (5);
	HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, 0xF6, 1, rawTemp, 2, 1000);
	return ((rawTemp[0]<<8) + rawTemp[1]);
}

float getTemp(BmpData* bmpData)
{
	float temp = 0;
	long X1 = 0;
	long X2 = 0;
	long B5 = 0;
	bmpData->uncomp.temp = getUTemp();

	X1 = ((bmpData->uncomp.temp - bmpData->calib.AC6) * (bmpData->calib.AC5 / (pow(2,15))));
	X2 = ((bmpData->calib.MC * (pow(2,11))) / ( X1 + bmpData->calib.MD));
	B5 = X1 + X2;
	temp = (B5 + 8) / (pow(2,4));
	return temp / 10;
}

uint32_t getUpress(int oss)
{
	uint8_t datatowrite = 0x34 + (oss<<6);
	uint8_t rawPressure[3] = {0};
	uint32_t uPressure = 0;
	HAL_I2C_Mem_Write(&hi2c1, BMP_WRITE_ADDR, 0xF4, 1, &datatowrite, 1, 1000);
	switch (oss)
	{
		case (0):
			HAL_Delay (5);
			break;
		case (1):
			HAL_Delay (8);
			break;
		case (2):
			HAL_Delay (14);
			break;
		case (3):
			HAL_Delay (26);
			break;
	}
	HAL_I2C_Mem_Read(&hi2c1, BMP_READ_ADDR, 0xF6, 1, rawPressure, 3, 1000);
	uPressure = (((rawPressure[0] << 16) + (rawPressure[1] << 8) + rawPressure[2]) >> (8 - oss));
	return uPressure;
}


float getPressure(BmpData bmpData, int oss)
{
	long X1, X2, X3, B3, B5, B6;
	unsigned long B4, B7;
	long pressure = 0;
	uint32_t UP = getUpress(oss);

	X1 = ((bmpData.uncomp.temp - bmpData.calib.AC6) * (bmpData.calib.AC5 / (pow(2,15))));
	X2 = ((bmpData.calib.MC * (pow(2,11))) / (X1 + bmpData.calib.MD));
	B5 = X1 + X2;
	B6 = B5 - 4000;
	X1 = (bmpData.calib.B2 * (B6 * B6 / (pow(2,12)))) / (pow(2,11));
	X2 = bmpData.calib.AC2 * B6 / (pow(2,11));
	X3 = X1 + X2;
	B3 = (((bmpData.calib.AC1 * 4 + X3) << oss) + 2) / 4;
	X1 = bmpData.calib.AC3 * B6 / pow(2,13);
	X2 = (bmpData.calib.B1 * (B6 * B6 / (pow(2,12)))) / (pow(2,16));
	X3 = ((X1 + X2) + 2) / pow(2,2);
	B4 = bmpData.calib.AC4 * (unsigned long)(X3 + 32768) / (pow(2,15));
	B7 = ((unsigned long)UP - B3) * (50000 >> oss);
	if (B7 < 0x80000000) { pressure = (B7 * 2) / B4; }
	else { pressure = (B7/B4)*2; }
	X1 = (pressure / (pow(2,8))) * (pressure / (pow(2,8)));
	X1 = (X1 * 3038) / (pow(2,16));
	X2 = (-7357 * pressure) / (pow(2,16));
	pressure = pressure + (X1+X2+3791) / (pow(2,4));

	return pressure;
}

float getAltitude(BmpData bmpData, int oss)
{
	float pressure = 0;

	pressure = getPressure(bmpData, oss);

	return PRESS_CONST_COEFICIENT * (1.0f - pow((pressure / ATM_PRESSURE), (1/5.255)));
}

void bmpInit(BmpData* bmpData)
{
	readCalibrationData(bmpData);
}