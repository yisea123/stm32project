/*
 * dev_spi.h
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */
/******************************************************************************/
#ifndef _DEV_SPI_H_
#define _DEV_SPI_H_


enum
{
	START_SPI=0,
	FINISH_SPI,
	ERROR_SPI,
};


typedef enum
{
	SPI_ID_5 = 0,
	SPI_ID_MAX,
}SPI_ID;

typedef enum
{
	SPI_DISABLE=0,
	SPI_ENABLE,
}SPI_STATE;

extern volatile uint16_t spiDone[SPI_ID_MAX];

#define SPI4Status  spiDone[SPI_ID_4]
#define SPI3Status  spiDone[SPI_ID_3]
#define SPI6Status  spiDone[SPI_ID_6]
#define SPI5Status  spiDone[SPI_ID_5]

typedef struct
{
	GPIO_TypeDef* 	port;
	uint32_t		pin;
//	volatile uint16_t* status;
}CS_PIN;

void SetSpiState(const SPI_ID id, SPI_STATE status);
void SPI_SemInit(void);
/*! Reads data from SPI. */
uint16_t SPI_Read_SP(SPI_HandleTypeDef *hspi,
                       uint8_t* sendData,
					   uint8_t* revData,
					   uint16_t bytesNumber);
uint16_t SPI_Read_DMA(SPI_HandleTypeDef *hspi,
                       uint8_t* sendData,
					   uint8_t* readData,
					   uint16_t bytesNumber);
/*! Writes data to SPI. */
uint16_t SPI_Write_SP(SPI_HandleTypeDef*  hspi,
		uint8_t* data,
		uint16_t bytesNumber);


uint16_t SPI_Write_DMA(SPI_HandleTypeDef*  hspi,
		uint8_t* data,
		uint16_t bytesNumber);
#endif /* _DEV_SPI_H_ */
