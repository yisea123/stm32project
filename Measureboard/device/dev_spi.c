/*
 * dev_spi.c
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */
/******************************************************************************/

#include "stm32f4xx_hal.h"
#include "tsk_measure.h"

#include "string.h"
#include "main.h"
#include "dev_spi.h"

extern SPI_HandleTypeDef hspi4;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi5;
extern SPI_HandleTypeDef hspi6;
#define NOT_USE_OS
#define SPI_TIME_OUT		55//care about the tick interrupt is hold for more than one ms

//note: the dma needs the buffer to be defined in ram not ccram;

__IO uint16_t spiDone[SPI_ID_MAX] = {0,0,0,};
static SemaphoreHandle_t lock[SPI_ID_MAX] = {NULL,NULL,NULL,NULL} ;


typedef struct
{
	SPI_ID id;
	SPI_HandleTypeDef *hspi;
	CS_PIN csPin;
	__IO uint16_t* ptrState;
}SPI_Cfg;

static const SPI_Cfg spiCfg[SPI_ID_MAX] =
{
	{SPI_ID_3,&hspi3 ,{SPI3_CS_GPIO_Port, SPI3_CS_Pin},&spiDone[SPI_ID_3]},
	{SPI_ID_4,&hspi4 ,{SPI4_CS_GPIO_Port, SPI4_CS_Pin},&spiDone[SPI_ID_4]},
	{SPI_ID_5,&hspi5 ,{SPI5_CS_GPIO_Port, SPI5_CS_Pin},&spiDone[SPI_ID_5]},
	{SPI_ID_6,&hspi6 ,{SPI6_CS_GPIO_Port, SPI6_CS_Pin},&spiDone[SPI_ID_6]},
};



static inline SPI_ID GetSpiID(SPI_HandleTypeDef* hspi)
{
	SPI_ID id = SPI_ID_3;
	if (hspi == &hspi4)
		id = SPI_ID_4;
	else if (hspi == &hspi3)
		id = SPI_ID_3;
	else if (hspi == &hspi6)
		id = SPI_ID_6;
	else if (hspi == &hspi5)
		id = SPI_ID_5;
	else
	{
	}
	return id;
}

static inline void SPI_Done(SPI_HandleTypeDef* hspi, uint16_t status)
{
#ifdef NOT_USE_OS

	spiDone[GetSpiID(hspi)] = status;
#else

	MsgPush (SPI_RDY_ID, 0, 0);
#endif
}



void SetSpiState(const SPI_ID id, SPI_STATE status)
{
	if(status == SPI_DISABLE)
	{
		OS_Unuse(lock[id]);
		HAL_GPIO_WritePin(spiCfg[id].csPin.port, (uint16_t)spiCfg[id].csPin.pin, GPIO_PIN_SET);
//		*csPins[id].status = 1;
	}
	else
	{
		OS_Use(lock[id]);
		spiDone[id] = START_SPI;
		HAL_GPIO_WritePin(spiCfg[id].csPin.port, (uint16_t)spiCfg[id].csPin.pin, GPIO_PIN_RESET);

//		*csPins[id].status = 0;
	}
}




// Interrupt callback from spi write done
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI_Done(hspi, FINISH_SPI);
}
// Interrupt callback from spi write done
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI_Done(hspi, FINISH_SPI);
}
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI_Done(hspi, FINISH_SPI);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	SPI_Done(hspi, ERROR_SPI);
}

void SPI_SemInit(void)
{
	for(uint16_t i = 0; i < SPI_ID_MAX;i++)
	{
		if(lock[i] == NULL)
			lock[i] = OS_CreateSemaphore();
	}
}

/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param hspi - SPI_HandleTypeDef instance
 * @param data - Data represents the write buffer as an input parameter and the
 *               read buffer as an output parameter.
 * @param bytesNumber - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
uint16_t SPI_Read_SP(SPI_HandleTypeDef *hspi,
                       uint8_t* sendData,
					   uint8_t* revData,
					   uint16_t bytesNumber)
{
	SPI_ID id = GetSpiID(hspi);
	uint16_t ret = OK;
	uint16_t retry = 0;
	//review, not stuck in while loop
	while(1)
	{
		SetSpiState(id, SPI_ENABLE);

		ret = HAL_SPI_TransmitReceive(hspi, sendData, revData, bytesNumber,
					SPI_TIME_OUT);
		SetSpiState(id, SPI_DISABLE);
		if(ret == HAL_OK)
		{
			break;
		}
		else
		{
			retry++;
			if(retry > 0x03)
			{
				break;
			}
		}
	}
	return ret;
}


uint16_t SPI_Read_DMA(SPI_HandleTypeDef *hspi,
                       uint8_t* sendData,
					   uint8_t* readData,
					   uint16_t bytesNumber)
{
	SPI_ID id = GetSpiID(hspi);
	uint16_t ret = OK;
	//review, not stuck in while loop

	SetSpiState(id, SPI_ENABLE);

	ret = HAL_SPI_TransmitReceive_DMA(hspi, sendData, readData, bytesNumber);

#ifdef NOT_USE_OS
	uint32_t countRetry = (uint32_t)( 18000 * bytesNumber);

	//review, not stuck in while loop
	while ((countRetry--) && spiDone[id] == START_SPI)
		;

#else
	osEvent event = osMessageGet(SPI_RDY_ID, SPI_TIME_OUT );
	if( event.status == osEventMessage )
	{
		ret = HAL_OK;
	}
	else
	{
		ret = HAL_TIMEOUT;
	}
#endif
	SetSpiState(id, SPI_DISABLE);
	return ret;
}
/***************************************************************************//**
 * @brief Writes data to SPI.
 *
 * @param hspi - SPI_HandleTypeDef instance
 * @param data - Data represents the write buffer.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
uint16_t SPI_Write_SP(SPI_HandleTypeDef*  hspi,
		uint8_t* data,
		uint16_t bytesNumber)
{
	uint16_t ret = OK;
	SPI_ID id = GetSpiID(hspi);
	SetSpiState(id, SPI_ENABLE);

	ret = HAL_SPI_Transmit(hspi, data, bytesNumber, SPI_TIME_OUT);

	SetSpiState(id, SPI_DISABLE);
	return ret;
}


uint16_t SPI_Write_DMA(SPI_HandleTypeDef*  hspi,
		uint8_t* data,
		uint16_t bytesNumber)
{
	uint16_t ret = OK;

	SPI_ID id = GetSpiID(hspi);

	SetSpiState(id, SPI_ENABLE);


	ret = HAL_SPI_Transmit_DMA(hspi, data, bytesNumber);
	//review, not stuck in while loop
#ifdef NOT_USE_OS
	uint32_t countRetry = (uint32_t)(18000 * bytesNumber);
	//review, not stuck in while loop
	while ((countRetry--) && spiDone[id] == START_SPI)
		;

#else
		osEvent event = osMessageGet(SPI_RDY_ID, SPI_TIME_OUT );
		if( event.status == osEventMessage )
		{
			ret = HAL_OK;
		}
		else
		{
			ret = HAL_TIMEOUT;
		}
#endif

	SetSpiState(id, SPI_DISABLE);
	return ret;
}

