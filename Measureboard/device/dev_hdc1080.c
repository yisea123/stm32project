/*
 * dev_hdc1080.c
 *
 *  Created on: 2017Äê7ÔÂ17ÈÕ
 *      Author: pli
 */

#include "main.h"
#include "i2c.h"
#include "dev_eep.h"
#include "gpio.h"
#include "unit_temp_cfg.h"
#include "dev_temp.h"
#include "dev_hdc1080.h"
#include <string.h>
#include <math.h>

#define HDC1000_ADDRESS			0x80	//I2C address for HDC1000

#define HDC1000_TEMP_ADDR		0x00	//Temperature measurement output register
#define	HDC1000_HUMIDITY_ADDR	0x01	//Relative humidity output register
#define HDC1000_CONFIG_ADDR		0x02	//HDC1000 configuration and status

#define HDC1000_MAN_ID_ADDR		0xFE	//Manufacturer ID register (default: 'TI')


/*
	CONFIGURATION REGISTER DESCRIPTION
	============================================
	Name	Registers	Description			Configuration
	RST		[15]		SW reset			0:  Normal operation
	Rsvd	[14]		Reserved			N/A
	HEAT	[13]		Heater				0:  Heater disabled
	MODE	[12]		Mode of acquisition	1:  Temp&Humidity in sequence
	BTST	[11]		Battery status		N/A
	TRES	[10]		Temp resolution		1:  11 bit
	HRES	[9:8]		Humidity resolution	01: 11 bit
	Rsvd	[7:0]		Reserved			N/A
*/
#define HDC1000_TEMP_RH_11BIT_MSB		0x15	//MSB of configuration
#define HDC1000_TEMP_RF_11BIT_LSB		0x00	//LSB of configuration


uint16_t Init_hdc1080(void)
{
	uint8_t txBuf[3];

	txBuf[0] = HDC1000_CONFIG_ADDR;
	txBuf[1] = HDC1000_TEMP_RH_11BIT_MSB;
	txBuf[2] = HDC1000_TEMP_RF_11BIT_LSB;

	//init to 11 bit resolution;
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1,HDC1000_ADDRESS,txBuf, 3, 10 );

	if(status == HAL_OK)
	{
		//
		Start_hdc1080();
	}
	return status;

}


uint16_t Start_hdc1080(void)
{

	uint8_t txBuf[3];

	txBuf[0] = HDC1000_TEMP_ADDR;

	//init to 11 bit resolution;
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1,HDC1000_ADDRESS,txBuf, 1, 10 );

	if(status == HAL_OK)
	{
		//
	}
	return status;

}



uint16_t Read_hdc1080(uint16_t* ptrHumidity, uint16_t* ptrTemp)
{

	uint8_t rxBuf[4];



	//init to 11 bit resolution;
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&hi2c1,HDC1000_ADDRESS, rxBuf, 4, 10 );

	if(status == HAL_OK)
	{
		//
		*ptrHumidity = (uint16_t)(rxBuf[0] * 256 + rxBuf[1]);
		*ptrTemp = (uint16_t)(rxBuf[2] * 256 + rxBuf[3]);
	}
	return status;

}

int16_t hadc1080_Val[2];
extern int16_t humidity;

uint16_t UpdateTH(void)
{
	static uint16_t val[2] = {0,0};

	uint16_t ret = Read_hdc1080(&val[0], &val[1]);
	if(ret == 0)
	{
		hadc1080_Val[0] = (int16_t)(val[0]*0.25177001953125 - 4000);
		humidity = hadc1080_Val[1] = (int16_t)(val[1]*0.152587890625);
	//	memcpy(hadc1080_Val,val, sizeof(val));
	}
	else
	{
		assert(1);
	}
	ret = Start_hdc1080();
	return ret;

}



