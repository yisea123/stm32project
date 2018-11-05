/***************************************************************************//**
 *   @file    AD717X.c
 *   @brief   AD717X implementation file.
 *   @devices AD7172-2, AD7172-4, AD7173-8, AD7175-2, AD7175-8, AD7176-2
 *            AD7177-2
 *   @author  acozma (andrei.cozma@analog.com)
 *            dnechita (dan.nechita@analog.com)
 *
 ********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "stm32f4xx_hal.h"

#include "main.h"
#include "AD717x.h"
#include "dev_spi.h"
/***************************************************************************//**
 * @brief  Searches through the list of registers of the driver instance and
 *         retrieves a pointer to the register that matches the given address.
 * @param device - The handler of the instance of the driver.
 * @param reg_address - The address to be used to find the register.
 *
 * @return A pointer to the register if found or 0.
 *******************************************************************************/
ad717x_st_reg *AD717X_GetReg(AD717X_DEVICE *device, uint8_t reg_address)
{
	uint8_t i;
	ad717x_st_reg *reg = 0;

	if (!device || !device->regs)
		return 0;

	for (i = 0; i < device->num_regs; i++)
	{
		if (device->regs[i].addr == reg_address)
		{
			reg = &device->regs[i];
			break;
		}
	}

	return reg;
}

/***************************************************************************//**
 * @brief Reads the value of the specified register.
 *
 * @param device - The handler of the instance of the driver.
 * @addr - The address of the register to be read. The value will be stored
 *         inside the register structure that holds info about this register.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_ReadRegister(AD717X_DEVICE *device, uint8_t addr)
{
	uint16_t ret = OK;
	//todo: risk about dma transfer with static data;
	//could recover automatically
	static uint8_t buffer[8] =
	{ 0, 0, 0, 0, 0, 0, 0, 0 };
	static uint8_t sendBuffer[8];
	uint8_t i = 0;
	uint8_t check8 = 0;
	uint8_t msgBuf[8] =
	{ 0, 0, 0, 0, 0, 0, 0, 0 };
	ad717x_st_reg *pReg;

	if (!device)
		return FATAL_ERROR; //INVALID_VAL;

	pReg = AD717X_GetReg(device, addr);
	if (!pReg)
		return FATAL_ERROR; //INVALID_VAL;

	/* Build the Command word */
	sendBuffer[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD
			| AD717X_COMM_REG_RA(pReg->addr);

	/* Read data from the device */
	ret = SPI_Read_DMA(device->spi, sendBuffer,buffer,
			((device->useCRC != AD717X_DISABLE) ?
					(uint8_t) (pReg->size + 2) : (uint8_t) (pReg->size + 1)));
	if (ret != OK)
		return ret;

	/* Check the CRC */
	if (device->useCRC == AD717X_USE_CRC)
	{
		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD
				| AD717X_COMM_REG_RA(pReg->addr);
		for (i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD717X_ComputeCRC8(msgBuf, (uint8_t) (pReg->size + 2));
	}
	if (device->useCRC == AD717X_USE_XOR)
	{
		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD
				| AD717X_COMM_REG_RA(pReg->addr);
		for (i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD717X_ComputeXOR8(msgBuf, (uint8_t) (pReg->size + 2));
	}

	if (check8 != 0)
	{
		/* ReadRegister checksum failed. */
		return ERROR_SPI_DATA; //COMM_ERR;
	}

	/* Build the result */
	pReg->value = 0;
	for (i = 1; i < pReg->size + 1; i++)
	{
		pReg->value <<= 8;
		pReg->value += buffer[i];
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes the value of the specified register.
 *
 * @param device - The handler of the instance of the driver.
 * @param reg - Register structure holding info about the register to be written
 * @param addr - The address of the register to be written with the value stored
 *               inside the register structure that holds info about this
 *               register.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_WriteRegister(AD717X_DEVICE *device, uint8_t addr)
{
	uint16_t ret = OK;
	int32_t regValue = 0;
	uint8_t wrBuf[8] =
	{ 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t i = 0;
	uint8_t crc8 = 0;
	ad717x_st_reg *preg;

	if (!device)
		return FATAL_ERROR; //INVALID_VAL;

	preg = AD717X_GetReg(device, addr);
	if (!preg)
		return FATAL_ERROR; //INVALID_VAL;

	/* Build the Command word */
	wrBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_WR
			| AD717X_COMM_REG_RA(preg->addr);

	/* Fill the write buffer */
	regValue = (int32_t) preg->value;
	for (i = 0; i < preg->size; i++)
	{
		wrBuf[preg->size - i] = (uint8_t) (regValue & 0xFF);
		regValue >>= 8;
	}

	/* Compute the CRC */
	if (device->useCRC != AD717X_DISABLE)
	{
		crc8 = AD717X_ComputeCRC8(wrBuf, (uint8_t) (preg->size + 1));
		wrBuf[preg->size + 1] = crc8;
	}

	/* Write data to the device */
	ret = SPI_Write_SP(device->spi, wrBuf,
			(device->useCRC != AD717X_DISABLE) ?
					(uint8_t) (preg->size + 2) : (uint8_t) (preg->size + 1));

	return ret;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param device - The handler of the instance of the driver.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_Reset(AD717X_DEVICE *device)
{
	uint16_t ret = 0;
	uint8_t wrBuf[8] =
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	if (!device)
		return FATAL_ERROR;

	ret = SPI_Write_SP(device->spi, wrBuf, 8);

	osDelay(1); // Wait at least 500 us until reset done (717x data sheet p.40), given 1000

	return ret;
}

/***************************************************************************//**
 * @brief Waits until a new conversion result is available.
 *
 * @param device - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns if no new data is available.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_WaitForReady(AD717X_DEVICE *device, uint32_t timeout)
{
	ad717x_st_reg *statusReg;
	uint16_t ret;
	int8_t ready = 0;

	if (!device || !device->regs)
		return FATAL_ERROR; //INVALID_VAL;

	statusReg = AD717X_GetReg(device, AD717X_STATUS_REG);
	if (!statusReg)
		return FATAL_ERROR; //INVALID_VAL;
	//review, not stuck in while loop
	while (!ready && --timeout)
	{
		/* Read the value of the Status Register */
		ret = AD717X_ReadRegister(device, AD717X_STATUS_REG);
		if (ret != OK)
			return ret;

		/* Check the RDY bit in the Status Register */
		ready = (statusReg->value & AD717X_STATUS_REG_RDY) != 0;
	}

	return timeout ? OK : FATAL_ERROR;	//;
}

/***************************************************************************//**
 * @brief Reads the conversion result from the device.
 *
 * @param device - The handler of the instance of the driver.
 * @param pData - Pointer to store the read data.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_ReadData(AD717X_DEVICE *device, int32_t* pData)
{
	ad717x_st_reg *dataReg = NULL;
	uint16_t ret = FATAL_ERROR;
	uint16_t retryCount = 3;

	if (!device || !device->regs)
		return FATAL_ERROR;	//INVALID_VAL;

	dataReg = AD717X_GetReg(device, AD717X_DATA_REG);
	if (!dataReg)
		return FATAL_ERROR;	//INVALID_VAL;

	//review, not stuck in while loop
	while (ret != OK)
	{
		/* Read the value of the Status Register */
		ret = AD717X_ReadRegister(device, AD717X_DATA_REG);
		retryCount--;
		if (retryCount == 0)
			break;
	}

	/* Get the read result */
	*pData = (int32_t) dataReg->value;

	if (ret)
	{
		TraceDBG(TSK_ID_AD_DETECT, "SPI error Status: %d,err %d, %d,%d,%d,%d\n",
				device->spi->Instance->SR, device->spi->ErrorCode,
				device->spi->TxXferCount, device->spi->TxXferSize,
				device->spi->RxXferCount, device->spi->RxXferSize\
);
	}
	return ret;
}

/***************************************************************************//**
 * @brief Reads the conversion result from the device.
 *
 * @param device - The handler of the instance of the driver.
 * @param pData - Pointer to store the read data.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_ReadDataSimple(AD717X_DEVICE *device, int32_t* pData)
{
#define CMD_GETVALUE_7172		0x44
#define CMD_BYTES_NUM			5
	//todo: risk about dma transfer with static data;
	//could recover automatically

	static uint8_t sendBuffer[8] =
	{ CMD_GETVALUE_7172, 0, 0, 0, 0, 0, 0, 0 };
	static uint8_t buffer[8] =
		{ CMD_GETVALUE_7172, 0, 0, 0, 0, 0, 0, 0 };

	uint16_t ret = FATAL_ERROR;
	uint16_t retryCount = 3;

	//review, not stuck in while loop
	while (retryCount)
	{
		sendBuffer[0] = CMD_GETVALUE_7172;
		(void) SPI_Read_DMA(device->spi, sendBuffer, buffer,
				CMD_BYTES_NUM);
		buffer[0] = CMD_GETVALUE_7172;
		if (0 != AD717X_ComputeCRC8(buffer, CMD_BYTES_NUM))
		{
			retryCount--;
		}
		else
		{
			*pData = 0;
			for (uint8_t i = 1; i < 4; i++)
			{
				*pData <<= 8;
				*pData += buffer[i];
			}
			ret = OK;
			break;
		}

	}

	if (ret)
	{
		TraceDBG(TSK_ID_AD_DETECT, "SPI error Status: %d,err %d, %d,%d,%d,%d\n",
				device->spi->Instance->SR, device->spi->ErrorCode,
				device->spi->TxXferCount, device->spi->TxXferSize,
				device->spi->RxXferCount, device->spi->RxXferSize\
);
	}
	return ret;
}

/***************************************************************************//**
 * @brief Computes the CRC checksum for a data buffer.
 *
 * @param pBuf - Data buffer
 * @param bufSize - Data buffer size in bytes
 *
 * @return Returns the computed CRC checksum.
 *******************************************************************************/
uint8_t AD717X_ComputeCRC8(uint8_t * pBuf, uint8_t bufSize)
{
	uint8_t i = 0;
	uint8_t crc = 0;
	//review, not stuck in while loop
	while (bufSize)
	{
		for (i = 0x80; i != 0; i >>= 1)
		{
			if (((crc & 0x80) != 0) != ((*pBuf & i) != 0)) /* MSB of CRC register XOR input Bit from Data */
			{
				crc = (uint8_t) (crc << 1);
				crc ^= AD717X_CRC8_POLYNOMIAL_REPRESENTATION;
			}
			else
			{
				crc = (uint8_t) (crc << 1);
			}
		}
		pBuf++;
		bufSize--;
	}
	return crc;
}

/***************************************************************************//**
 * @brief Computes the XOR checksum for a data buffer.
 *
 * @param pBuf - Data buffer
 * @param bufSize - Data buffer size in bytes
 *
 * @return Returns the computed XOR checksum.
 *******************************************************************************/
uint8_t AD717X_ComputeXOR8(uint8_t * pBuf, uint8_t bufSize)
{
	uint8_t xor = 0;
	//review, not stuck in while loop
	while (bufSize)
	{
		xor ^= *pBuf;
		pBuf++;
		bufSize--;
	}
	return xor;
}

/***************************************************************************//**
 * @brief Updates the CRC settings.
 *
 * @param device - The handler of the instance of the driver.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_UpdateCRCSetting(AD717X_DEVICE *device)
{
	ad717x_st_reg *interfaceReg;

	if (!device || !device->regs)
		return FATAL_ERROR;	//INVALID_VAL;

	interfaceReg = AD717X_GetReg(device, AD717X_IFMODE_REG);
	if (!interfaceReg)
		return FATAL_ERROR;	//INVALID_VAL;

	/* Get CRC State. */
	if (AD717X_IFMODE_REG_CRC_STAT(interfaceReg->value))
	{
		device->useCRC = AD717X_USE_CRC;
	}
	else if (AD717X_IFMODE_REG_XOR_STAT(interfaceReg->value))
	{
		device->useCRC = AD717X_USE_XOR;
	}
	else
	{
		device->useCRC = AD717X_DISABLE;
	}

	return OK;
}

/***************************************************************************//**
 * @brief Initializes the AD717X.
 *
 * @param device - The handler of the instance of the driver.
 * @param dev_type - The type of the device that the driver should be
 *                   instantiated with.
 * @param slave_select - The Slave Chip Select Id to be passed to the SPI calls.
 * @param regs - The list of registers of the device (initialized or not) to be
 *               added to the instance of the driver.
 *
 * @return Returns 0 for success or negative error code.
 *******************************************************************************/
uint16_t AD717X_Setup(AD717X_DEVICE *device, SPI_HandleTypeDef* spi,
		ad717x_st_reg *regs, uint8_t num_regs)
{
	uint16_t ret;
	ad717x_st_reg *preg;

	if (!device || !regs)
		return FATAL_ERROR;	//INVALID_VAL;

	device->regs = regs;
	device->spi = spi;
	device->num_regs = num_regs;

	/*  Reset the device interface.*/
	ret = AD717X_Reset(device);
	if (ret != OK)
		return ret;

	/* Initialize ADC mode register. */
	ret = AD717X_WriteRegister(device, AD717X_ADCMODE_REG);
	if (ret != OK)
		return ret;

	/* Initialize Interface mode register. */
	ret = AD717X_WriteRegister(device, AD717X_IFMODE_REG);
	if (ret != OK)
		return ret;

	/* Get CRC State */
	ret = AD717X_UpdateCRCSetting(device);
	if (ret != OK)
		return ret;

	/* Initialize registers AD717X_GPIOCON_REG through AD717X_OFFSET0_REG */
	preg = AD717X_GetReg(device, AD717X_GPIOCON_REG);
	if (!preg)
		return FATAL_ERROR;	//INVALID_VAL;

	ret = AD717X_ReadRegister(device, AD717X_ID_REG);
	//review, not stuck in while loop
	while (preg && preg->addr != AD717X_OFFSET0_REG)
	{
		if (preg->addr == AD717X_ID_REG)
		{
			preg++;
			continue;
		}

		ret = AD717X_WriteRegister(device, (uint8_t) (preg->addr));
		if (ret != OK)
			break;
		preg++;
	}

	return ret;
}


