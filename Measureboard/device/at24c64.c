/*
 * I2c_Simu.c
 *
 *  Created on: 2016锟斤拷10锟斤拷14锟斤拷
 *      Author: pli
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"

#include "dev_eep.h"
#include <string.h>
#include "at24c64.h"

#define 	NO_BUSY 	0x01
////////////////////////////////////////////////////////////////////////////////////////////

#define 	WP4

#define 	TW_WRITE 	0
#define 	TW_READ  	1

#define 	TW_ACK 		1
#define 	TW_NOACK 	0
#define 	IIC_USE_DMA

//static uint16_t useGpioSimu = 0;


#ifdef IIC_GPIO_SIMU

void At24cxxWaitBusy(void);
extern void At24WriteStr(uint16_t addr,uint8_t *dat,uint8_t length);
extern void At24ReadStr(uint16_t addr,uint8_t *dat,uint8_t len);




#define SDA_H		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET)
#define SDA_L		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET)

#define SCL_H		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET)
#define SCL_L		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET)

#define SDA			 HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5)

#define ADDR_AT24C64	0XA0



static inline void I2cDelayus(uint16_t tick) //0.1us
{
	while(tick--)
	{
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	}
}

//IIC锟斤拷锟斤拷锟�/////////////////////////////////////////////////////////////////
//IIC锟斤拷锟斤拷锟�/////////////////////////////////////////////////////////////////
//IIC锟斤拷锟斤拷锟�/////////////////////////////////////////////////////////////////
//锟斤拷始锟斤拷锟斤拷模锟斤拷

void Twi_ack(uint8_t ack)
/*
 锟斤拷锟斤拷:锟斤拷锟酵凤拷锟斤拷位.
 锟斤拷锟斤拷:ack=1,锟斤拷锟斤拷应锟斤拷位;ack=0,锟斤拷锟酵凤拷应锟斤拷位.
 */
{

	if (!ack)  //锟斤拷应锟斤拷
		SDA_H;
	else
		//应锟斤拷
		SDA_L;
	I2cDelayus(50);
	SCL_H;
	I2cDelayus(50);
	SCL_L;
	I2cDelayus(50);

}

void TwiStart(void)
//锟斤拷锟斤拷:锟斤拷锟斤拷IIC锟斤拷始锟脚猴拷.
{

	SDA_H;
	I2cDelayus(10);
	SCL_H;
	I2cDelayus(50);
	SDA_L;
	I2cDelayus(50);
	SCL_L;
	I2cDelayus(10);

}

void TwiStop(void)
//锟斤拷锟斤拷:锟斤拷锟斤拷IIC停止锟脚猴拷.
{

	SDA_L;
	I2cDelayus(10);
	SCL_H;
	I2cDelayus(50);
	SDA_H;
	I2cDelayus(50);

}

uint8_t TwiWriteByte(uint8_t c)
/*
 锟斤拷锟斤拷:锟斤拷锟斤拷锟斤拷写一锟街斤拷,锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷应锟斤拷
 锟斤拷锟斤拷:0--失锟斤拷,1--锟缴癸拷
 */
{

	uint8_t i, ack;

	for (i = 0; i < 8; i++)
	{

		if ((c << i) & 0x80)
		{
			SDA_H;
		}
		else
		{
			SDA_L;
		}
		I2cDelayus(50);
		SCL_H;
		I2cDelayus(60);
		SCL_L;
	}

	I2cDelayus(10);
	SDA_H;
	I2cDelayus(10);
	SCL_H;
	I2cDelayus(50);
	if (SDA)
		ack = 0;     //失锟斤拷
	else
		ack = 1;
	SCL_L;
	I2cDelayus(50);
	return ack;

}

uint8_t TwiReadByte(uint8_t *c, uint8_t ack)
//锟斤拷一锟街斤拷 ack: 1时应锟斤拷0时锟斤拷应锟斤拷
{

	uint8_t i, ret;

	ret = 0;
	SDA_H;
	for (i = 0; i < 8; i++)
	{
		I2cDelayus(5);
		SCL_L;
		I2cDelayus(60);
		SCL_H;
		I2cDelayus(10);
		ret <<= 1;
		if (SDA)
			ret++;
		I2cDelayus(40);
	}
	SCL_L;
	I2cDelayus(10);
	Twi_ack(ack);
	*c = ret;
	return (ret);

}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void At24cxxWaitBusy(void)
/*
 锟斤拷锟斤拷忙锟斤拷锟�,原锟斤拷:锟斤拷锟斤拷忙时锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫达拷锟斤拷锟接︼拷锟�,
 忙锟斤拷锟接口猴拷锟斤拷,只锟斤拷一锟斤拷锟斤拷锟斤拷锟斤拷锟揭拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷,
 锟斤拷:锟斤拷锟斤拷写锟斤拷锟�,要锟斤拷锟斤拷锟斤拷时,锟斤拷锟斤拷锟斤拷锟侥讹拷锟斤拷锟斤拷写锟斤拷锟斤拷之锟戒不锟斤拷要锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�.
 */
{

	uint8_t i;
	while (1)
	{

		TwiStart();
		i = TwiWriteByte(ADDR_AT24C64);
		TwiStop();
		if (i == NO_BUSY)
			break;

	}
	return;

}

void Iic_LowInit(void)
{
	if(useGpioSimu)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
		GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
	}
}


void At24WriteStr(uint16_t addr, uint8_t *dat, uint8_t length)
/*
 锟斤拷锟斤拷:写AT24C64锟斤拷锟斤拷
 锟斤拷锟斤拷:addr-->锟斤拷锟捷匡拷锟斤拷始锟斤拷址,dat-->锟斤拷要写锟斤拷锟斤拷锟捷伙拷锟斤拷锟斤拷锟斤拷址,锟斤拷要写锟斤拷锟斤拷锟捷筹拷锟斤拷.
 注锟斤拷:锟斤拷锟斤拷at24c64锟斤拷每一页为32锟街斤拷,锟斤拷锟斤拷每锟斤拷写锟斤拷锟斤拷锟捷诧拷锟缴筹拷锟斤拷锟斤拷一页
 锟侥斤拷:锟斤拷臃锟斤拷锟斤拷欠锟叫达拷锟缴癸拷.
 */
{

	uint8_t i;

//	At24cxxWaitBusy();
//	EEPROM_WRITE_ENABLE;
	TwiStart();
	TwiWriteByte(ADDR_AT24C64);
	TwiWriteByte((uint8_t) (addr > 8));     //写锟斤拷址锟斤拷锟街斤拷
	TwiWriteByte((uint8_t) addr);     //写锟斤拷址锟斤拷锟街斤拷
	for (i = 0; i < length; i++)
		TwiWriteByte(dat[i]);     //写锟斤拷锟斤拷锟街斤拷
	TwiStop();
//	EEPROM_WRITE_DISABLE;

}

void At24ReadStr(uint16_t addr, uint8_t *dat, uint8_t len)
/*
 锟斤拷锟斤拷:锟斤拷AT24C64锟斤拷锟斤拷
 锟斤拷锟斤拷:addr-->锟斤拷锟捷匡拷锟斤拷始锟斤拷址,dat-->锟斤拷锟斤拷锟斤拷锟斤拷锟捷伙拷锟斤拷锟斤拷锟斤拷址,锟斤拷锟斤拷锟斤拷锟斤拷锟捷筹拷锟斤拷.
 */
{
	uint8_t i;

//	At24cxxWaitBusy();
	TwiStart();
	TwiWriteByte(ADDR_AT24C64);
	TwiWriteByte((uint8_t) (addr >> 8));
	TwiWriteByte((uint8_t) addr);
	TwiStart();
	TwiWriteByte(ADDR_AT24C64 | TW_READ);
	for (i = 0; i < len - 1; i++)
	{

		TwiReadByte(dat++, TW_ACK);

	}
	TwiReadByte(dat, TW_NOACK);     //NO ACK
	TwiStop();

}

#endif


uint16_t EEP_READ_ADR( uint16_t adr,uint8_t* data, const uint16_t len)
{
	uint16_t ret = OK;
	uint16_t lenR = 0;
	uint16_t tempLen = 0;
	uint16_t adr_R = adr;
	uint16_t retryCount = 0;
	assert(adr%PAGE_LEN == 0);
	assert(len>0);

	while(len > lenR )
	{
		HAL_StatusTypeDef ret1;
		tempLen = (uint16_t)(len - lenR);
		if(tempLen > PAGE_LEN)
		{
			tempLen = PAGE_LEN;
		}
#ifdef IIC_GPIO_SIMU
		if(useGpioSimu)
		{
			At24ReadStr(adr_R, data, tempLen);
			lenR = (uint16_t)(lenR + tempLen);
			adr_R = (uint16_t)(adr_R + tempLen);
			data += tempLen;
			ret = OK;
		}
		else
#endif

		{
	#ifdef IIC_USE_DMA
			WDI_TRIGGER;
			ret1 = HAL_I2C_Mem_Read_DMA(&eepIIC, EEP_ADR, adr_R, I2C_MEMADD_SIZE_16BIT, data, (uint16_t)tempLen);
	#else
			ret1 = HAL_I2C_Mem_Read(&eepIIC, EEP_ADR, adr_R, I2C_MEMADD_SIZE_16BIT, data, (uint16_t)tempLen, eepTimeOut(tempLen));
	#endif
			if(ret1 != HAL_OK)
			{
				retryCount++;
				if (HAL_I2C_GetError(&eepIIC) != HAL_I2C_ERROR_AF)
				{
					I2C_Error_Handler();
				}
			}
			else
			{
	#ifdef IIC_USE_DMA
				uint16_t count = 40000;
				//review, not stuck in while loop
				while (count-- && HAL_I2C_GetState(&eepIIC) != HAL_I2C_STATE_READY)
				{
					WDI_TRIGGER;
				}
	#endif
				retryCount = 0;
				lenR = (uint16_t)(lenR + tempLen);
				adr_R = (uint16_t)(adr_R + tempLen);
				data += tempLen;
			}
			if(retryCount > MAX_RETRY )
			{
				ret = FATAL_ERROR;
			}
			else
			{
				ret = OK;
			}
		}
	}
	return ret;
}


uint16_t EEP_WRITE_ADR(uint16_t adr, uint8_t* data, const uint16_t len)
{
	uint16_t ret = OK;
	uint16_t lenW = 0;
	uint16_t tempLen = 0;
	uint16_t adr_W = adr;
	uint16_t retryCount = 0;
	assert(adr%PAGE_LEN == 0);
	assert(len>0);
	//review, not stuck in while loop
	while(len > lenW )
	{
		HAL_StatusTypeDef ret1;
		tempLen = (uint16_t)(len - lenW);
		if(tempLen > PAGE_LEN)
		{
			tempLen = PAGE_LEN;
		}
#ifdef IIC_GPIO_SIMU
		if(useGpioSimu)
		{
			At24WriteStr(adr_W, data, tempLen);
			lenW = (uint16_t)(lenW + tempLen);
			adr_W = (uint16_t)(adr_W + tempLen);
			data += tempLen;
			ret = OK;
		}
		else
#endif
		{
	#ifdef IIC_USE_DMA
			ret1 = HAL_I2C_Mem_Write_DMA(&eepIIC, EEP_ADR, adr_W, I2C_MEMADD_SIZE_16BIT, data, tempLen);
	#else
			ret1 = HAL_I2C_Mem_Write(&eepIIC, EEP_ADR, adr_W, I2C_MEMADD_SIZE_16BIT, data, tempLen, eepTimeOut(tempLen));
	#endif

			if(ret1 != HAL_OK)
			{
				retryCount++;
				if (HAL_I2C_GetError(&eepIIC) != HAL_I2C_ERROR_AF)
				{
					I2C_Error_Handler();
				}
			}
			else
			{
	#ifdef IIC_USE_DMA
				uint16_t count = 50000;
				//review, not stuck in while loop
				while (count-- && HAL_I2C_GetState(&eepIIC) != HAL_I2C_STATE_READY)
				{
				}
	#endif
				retryCount = 0;
				lenW = (uint16_t)(lenW + tempLen);
				adr_W = (uint16_t)(adr_W + tempLen);
				data += tempLen;

			}
			if(retryCount > MAX_RETRY )
			{
				ret = FATAL_ERROR;
			}
			else
			{
				ret = OK;
			}
		}
	}
	return ret;

}

