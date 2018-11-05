/*
 * dev_spiflash.c
 *
 *  Created on: 2016��9��26��
 *      Author: pli
 */
#include <string.h>
#include "bsp.h"
#include "dev_spiflash.h"
#include "main.h"
#include "dev_spi.h"

/*
 * extern
 */
extern SPI_HandleTypeDef hspi4;



/*
 * definitions
 */
#define sSpiHandle		hspi4

/* Select sFLASH: Chip Select pin low */
#define sFLASH_CS_LOW() 		HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_RESET)
/* Deselect sFLASH: Chip Select pin high */
#define sFLASH_CS_HIGH() 		HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_SET)



/*
 * static variable
 */

SemaphoreHandle_t spiLock = NULL;
static float baudRateFactor = 1.5f;




/**
 * static functions
 */
static void sFLASH_LowLevel_Init(void);
static uint16_t sFLASH_UnProtectGlobal(void);
static uint16_t sFLASH_SendByte(uint8_t tx, uint8_t *p_rx);
static void sFLASH_WriteEnable(void);
static void sFLASH_WaitForWriteEnd(void);


static uint32_t TimeOutByte(uint16_t byteNum)
{
	float val = baudRateFactor * byteNum ;
	if (val <= 2)
		val = 2;

	return (uint32_t) (val + 5);
}



/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */


static uint16_t sFLASH_SendByte(uint8_t tx, uint8_t *p_rx)
{
	uint16_t ret = OK;
	HAL_StatusTypeDef ret1 = HAL_OK;
	if (p_rx == NULL)
	{
		ret1 = HAL_SPI_Transmit(&sSpiHandle, &tx, 1, TimeOutByte(1));
		if (ret1 != HAL_OK)
		{
			ret = FATAL_ERROR;
		}
	}
	else
	{
		ret1 = HAL_SPI_TransmitReceive(&sSpiHandle, &tx, p_rx, 1,
				TimeOutByte(1));
		if (ret1 != HAL_OK)
		{
			ret = FATAL_ERROR;
		}
	}
	return ret;
}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
static void sFLASH_WriteEnable()
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "Write Enable" instruction */
	sFLASH_SendByte(sFLASH_CMD_WREN, NULL);

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
static void sFLASH_WaitForWriteEnd()
{
  uint8_t flashstatus = 0;
  uint32_t countRetry = 0;
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sFLASH_SendByte(sFLASH_CMD_RDSR, NULL);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
      /*!< Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
      sFLASH_SendByte(sFLASH_DUMMY_BYTE, &flashstatus);
  }//review, not stuck in while loop
  while ((countRetry++ < 100000)&&(flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
static void sFLASH_LowLevel_Init(void)
{
//	MX_SPI2_Init();
	baudRateFactor = 0.05f;
	/** de-select the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	//set clk to be 1????
	//todo
	//HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
}


/**
  * @brief  Global protect or un-protect.
  * @param  need_protect : SET means protect, RESET means un-protect.
  * @retval None
  */
static uint16_t sFLASH_UnProtectGlobal()
{
	uint16_t ret = 0;
    uint8_t cmd[2] = { sFLASH_CMD_WRSR, 0x00 };


	sFLASH_WriteEnable();

	//sFLASH_WaitForWriteEnd(cs_pin);

	sFLASH_CS_LOW();

	ret = HAL_SPI_Transmit(&sSpiHandle, cmd, 2, TimeOutByte(2));

	sFLASH_CS_HIGH();
	sFLASH_WriteEnable();
	sFLASH_WaitForWriteEnd();

	return ret;
}


/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
static uint16_t Dev_SpiFlash_ReadID( uint32_t *p_id)
{
    uint8_t Temp0 = 0, Temp1 = 0, Temp2 = 0;
    uint16_t ret = OK;

    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "RDID " instruction */
    ret = sFLASH_SendByte(0x9F, NULL);

    /*!< Read a byte from the FLASH */
    ret |= sFLASH_SendByte(sFLASH_DUMMY_BYTE, &Temp0);

    /*!< Read a byte from the FLASH */
    ret |= sFLASH_SendByte(sFLASH_DUMMY_BYTE, &Temp1);

    /*!< Read a byte from the FLASH */
    ret |= sFLASH_SendByte(sFLASH_DUMMY_BYTE, &Temp2);

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();

    *p_id = (uint32_t)((Temp0 << 16) | (Temp1 << 8) | Temp2);

    return ret;
}

static uint16_t SendFlashCmd(uint8_t* data, uint8_t len, uint8_t writeEnable)
{
	if(writeEnable)
		sFLASH_WriteEnable();
	sFLASH_CS_LOW();
	uint16_t ret = HAL_SPI_Transmit(&sSpiHandle, data, len, TimeOutByte(len));
	sFLASH_CS_HIGH();
	osDelay(2);
	sFLASH_WaitForWriteEnd();

	return ret;
}

/*
 * Below drivers have reused the drivers from HeavyMetal's project.
 *
 *
 *
 *
 *
 *
 *
 *
 */



static void Unlock(uint32_t addr)
{
	uint8_t cmd[5];
	cmd[0] = FLASH_CMD_FREE_LOCK;
	cmd[1] = 0x55;
	cmd[2] = 0xaa;
	cmd[3] = 0x40;
	cmd[4] = 0xd0;
	SendFlashCmd(cmd, 5, 1);

	cmd[0] = sFLASH_CMD_UNSE;
	cmd[1] = ((addr & 0xFF0000) >> 16);
	cmd[2] = ((addr & 0x00FF00) >> 8);
	cmd[3] = (addr & 0x0000FF);
	SendFlashCmd(cmd, 4, 1);

}
/*****************************************************************
** Function name     : Dev_SpiFlash_EraseBlk
** Descriptions      : Block erase
** Input parameters  : addr -- block number 0,1,2.....
** Output parameters : None
** Returned value    : The operation result. 0 -- sucess, 1 -- false
******************************************************************/
uint16_t Dev_SpiFlash_EraseBlk (uint32_t lb)
{
	uint32_t addr = 0;
    uint8_t cmd[5] = {0};
    uint16_t ret = OK;
    uint16_t retryCount = 3;
	if(lb>=FLASH_BLOCK_COUNT)
	{
		//xprintf("%s: Invalid para:block_num=%d \n", __FUNCTION__,lb);
		ret = FATAL_ERROR;
	}

	if(lb < FLASH_BLOCK_COUNT)
	{
		addr = lb * FLASH_BLOCK_SIZE;
	}
	else
	{
		ret = FATAL_ERROR;
	}
	OS_Use(spiLock);
	while(retryCount)
	{

		cmd[0] = FLASH_CMD_FREE_LOCK;
		cmd[1] = 0x55;
		cmd[2] = 0xaa;
		cmd[3] = 0x40;
		cmd[4] = 0xd0;
		SendFlashCmd(cmd, 5, 1);

		cmd[0] = sFLASH_CMD_UNSE;
		cmd[1] = ((addr & 0xFF0000) >> 16);
		cmd[2] = ((addr & 0x00FF00) >> 8);
		cmd[3] = (addr & 0x0000FF);
		ret = SendFlashCmd(cmd, 4, 1);
		/*send address command.*/
		cmd[0] = FLASH_CMD_BLOCK_ERASE_4K;
		cmd[1] = ((addr & 0xFF0000) >> 16);
		cmd[2] = ((addr & 0x00FF00) >> 8);
		cmd[3] = (addr & 0x0000FF);
		ret |= SendFlashCmd(cmd, 4, 1);
		if(ret == OK)
		{
			retryCount = 0;
		}
		else
		{
			retryCount--;
			TraceMsg(TSK_ID_DATA_LOG,"erase retry:%d\n",retryCount);
		}
	}
    OS_Unuse(spiLock);
    return ret;
}

uint16_t Dev_SpiFlash_EraseAll(void)
{
	uint16_t ret = OK;
	uint16_t i =0;
	for (i=0; i< FLASH_BLOCK_COUNT; i++)
	{
		ret |= Dev_SpiFlash_EraseBlk(i);
	}
	return ret;
}


/**
 * @brief  Initializes the peripherals used by the SPI FLASH driver.
 * @param  None
 * @retval None
 */
uint16_t Dev_SpiFlash_Init(void)
{
	uint16_t ret = OK;
	uint32_t fid;
	if(spiLock == NULL)
		spiLock = OS_CreateSemaphore();
	/** spi controller init */
	OS_Use(spiLock);
	sFLASH_LowLevel_Init();
	osDelay(100);

	/** globally un-protect the spi flash */
	ret |= sFLASH_UnProtectGlobal();

	Dev_SpiFlash_ReadID(&fid);
	if (sFLASH_AT25DF641_ID == fid)
	{
	}
	else
	{
		TraceDBG(TSK_ID_DATA_LOG, "wrong flash chip detected!\n");
		//Fail_Handler(SPI_FLASH_ERROR);
	}
	OS_Unuse(spiLock);
	return ret;
}


#define DMA_USED
/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
uint16_t Dev_SpiFlash_ReadWrite(const uint8_t *txBuf, uint8_t *rxBuf,
        uint32_t addr, uint16_t rwBytes)
{
    uint16_t ret = OK;
    uint32_t countRetry = 0;
    uint8_t *pBuffer = NULL;
    uint8_t cmd[4] = {
            sFLASH_CMD_READ,
            (addr & 0xFF0000) >> 16,
            (addr& 0xFF00) >> 8,
            (addr & 0xFF)
    };

    if ((txBuf == NULL && rxBuf == NULL) || rwBytes == 0) return ret;
    OS_Use(spiLock);

    if (txBuf == NULL)  // this is a read command
    {
        pBuffer = rxBuf;

        /*!< Select the FLASH: Chip Select low */
        sFLASH_CS_LOW();

        HAL_SPI_Transmit(&sSpiHandle, cmd, 4, TimeOutByte(4));
        (void)memset(pBuffer, sFLASH_DUMMY_BYTE, rwBytes);
#ifdef DMA_USED
        SPI4Status = START_SPI;
        countRetry = 0;
        HAL_SPI_Receive_DMA(&sSpiHandle, pBuffer, rwBytes);
        while(SPI4Status == START_SPI && countRetry++ < 100000);
#else
        ret = HAL_SPI_Receive(&sSpiHandle, pBuffer, rwBytes, TimeOutByte(rwBytes));
#endif
        sFLASH_CS_HIGH();
    }
    else    // this is a write command
    {
        pBuffer = (uint8_t *) txBuf;
        Unlock(addr);
        sFLASH_WriteEnable();
        /*!< Select the FLASH: Chip Select low */
        sFLASH_CS_LOW();
        /*!< Send "Write to Memory " instruction */
		cmd[0] = sFLASH_CMD_WRITE;
        ret = HAL_SPI_Transmit(&sSpiHandle, cmd, 4, TimeOutByte(4));


        if(ret == OK)
        {
#ifdef DMA_USED
        	SPI4Status = START_SPI;
        	countRetry = 0;
			HAL_SPI_Transmit_DMA(&sSpiHandle, pBuffer, rwBytes);
			while(SPI4Status == START_SPI && countRetry++ < 100000);
#else
        	ret = HAL_SPI_Transmit(&sSpiHandle, pBuffer, rwBytes, TimeOutByte(rwBytes));
#endif
        }


        /*!< Deselect the FLASH: Chip Select high */
        sFLASH_CS_HIGH();

        /*!< Wait the end of Flash writing */
        sFLASH_WaitForWriteEnd();

    }
    OS_Unuse(spiLock);
    if(ret != OK)
	{
		TraceDBG(TSK_ID_DATA_LOG, "\t\t\t RW error: adr:0x%x, len%d!\n",addr, rwBytes);
	}
    return ret;
}



// End of file
