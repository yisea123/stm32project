/*
 * dev_ad7172.c
 *
 *  Created on: 2016��7��25��
 *      Author: pli
 */





#include "tsk_measure.h"
#include "ad717x.h"
#include "main.h"


#include "ad7172_2_regs.h"

#include "string.h"
#include "dev_ad7172.h"
#include "unit_flow_act.h"
// Private structures for the two AD7172

AD717X_DEVICE 			AD717x[DEV_7172_MAX];





static void InitChnSet_AD7172(AD717X_DEVICE* p_dev)
{
	ad717x_st_reg* p_reg = NULL;

	p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP0_REG);
	p_reg->value &= 0xFE00;
//	p_reg->value |= /*AD717X_CHMAP_REG_CH_EN | */AD717X_CHMAP_REG_AINPOS(0) | AD717X_CHMAP_REG_AINNEG(1);
	p_reg->value |= /*AD717X_CHMAP_REG_CH_EN | */AD717X_CHMAP_REG_AINPOS(0) | AD717X_CHMAP_REG_AINNEG(1);
	AD717X_WriteRegister(p_dev, AD717X_CHMAP0_REG);
	p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP1_REG);
	p_reg->value &= 0xFE00;
//	p_reg->value |= /*AD717X_CHMAP_REG_CH_EN | */AD717X_CHMAP_REG_AINPOS(2) | AD717X_CHMAP_REG_AINNEG(3);
	p_reg->value |= /*AD717X_CHMAP_REG_CH_EN | */AD717X_CHMAP_REG_AINPOS(2) | AD717X_CHMAP_REG_AINNEG(3);
	AD717X_WriteRegister(p_dev, AD717X_CHMAP1_REG);
}

extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi6;


// To initialize the two ADCs, must be called before any adc operation
void DevAD_Init(void)
{
	int ret = 0;

	// Disable DRDY interrupts
//	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
//	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

	// Set CSs and SYNC to default high
	SPI3_CS_H;
	SPI3_CLK_H;
	SPI6_CS_H;
	SPI6_CLK_H;
	SYNC_H;
	SH_CIRCUIT_DIS;
	ret = AD717X_Setup(&AD717x[AD7172_MEA_ID], &hspi3, (ad717x_st_reg *)ad7172_2_regs_Mea, sizeof(ad7172_2_regs_Mea)/sizeof(ad717x_st_reg));

	if (0 > ret)
		TraceDBG(TSK_ID_AD_DETECT,"Error when setup ad7172 mea. ret = %d\n", ret);

	ret = AD717X_Setup(&AD717x[AD7172_REF_ID], &hspi6, (ad717x_st_reg *)ad7172_2_regs_Ref, sizeof(ad7172_2_regs_Ref)/sizeof(ad717x_st_reg));

	if (0 > ret)
		TraceDBG(TSK_ID_AD_DETECT,"Error when setup ad7172 ref. ret = %d\n", ret);

	// Following init steps have to resend spi commands in order to keep org
	// register definition in ad7172_2_regs.h. Direct access ad7172_2_regs[]
	// array is creepy because indexs are not consistent with register addr.
	// So use AD717X_GetReg() instead for safer way.

	// Set pin mappings for conversion channels
	InitChnSet_AD7172(&AD717x[AD7172_MEA_ID]);
	InitChnSet_AD7172(&AD717x[AD7172_REF_ID]);
	// Wait for AD7172 to be stable
	osDelay(100);
}




uint16_t DevAD_ReadReg(uint8_t device, uint8_t addr, uint32_t* value)
{
	uint32_t count = 8;
	if(device < DEV_7172_MAX)
	{
		AD717X_DEVICE* device7172 = &AD717x[device];
		ad717x_st_reg * p_reg;
		uint16_t ret = FATAL_ERROR;
		do
		{
			ret = AD717X_ReadRegister(device7172, (uint8_t)addr);
			osDelay(5);
			//review, not stuck in while loop
		}while(ret != OK && (--count));

		p_reg = AD717X_GetReg(device7172, (uint8_t)addr);
		*value = p_reg->value;
		return OK;
	}
	return ILLEGAL_ATTRIB_IDX;

}


uint16_t DevAD_WriteReg(uint8_t device, uint8_t addr,const uint32_t value)
{
	if(device < DEV_7172_MAX)
	{
		AD717X_DEVICE* device7172 = &AD717x[device];
		ad717x_st_reg * p_reg;
		p_reg = AD717X_GetReg(device7172, (uint8_t)addr);
		p_reg->value = value;
		AD717X_WriteRegister(device7172, (uint8_t)addr);
		return OK;
	}
	return ILLEGAL_ATTRIB_IDX;

}

uint16_t DevAD_ReadData(uint8_t device, int32_t* data)
{
	if(device < DEV_7172_MAX)
	{
		return AD717X_ReadDataSimple(&AD717x[device], (int32_t*)data);
	}
	return ILLEGAL_ATTRIB_IDX;

}

// To select ad7172 convert channel: 0, 1
uint16_t DevAD_SelectChn(uint8_t slave_id, uint8_t chnl)
{
	ad717x_st_reg* p_reg = NULL;
	uint16_t ret = OK;

	if(slave_id > DEV_7172_MAX)
		return ILLEGAL_ATTRIB_IDX;

	AD717X_DEVICE* p_dev = &AD717x[slave_id];
	// Check if the channel already enabled. Disable current channel before switch to another
	switch (chnl)
	{
		case AD_CHN_LONG:
			p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP0_REG);
			if (AD717X_CHMAP_REG_CH_EN == (p_reg->value & AD717X_CHMAP_REG_CH_EN))
			{
				// Already enabled, nothing to do
			}
			else
			{
				// Disable current channel
				p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP1_REG);
				p_reg->value = (uint32_t)((int32_t)p_reg->value & ~(AD717X_CHMAP_REG_CH_EN));

				AD717X_WriteRegister(p_dev, AD717X_CHMAP1_REG);
				// Enable channel
				p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP0_REG);
				p_reg->value |= AD717X_CHMAP_REG_CH_EN;
				AD717X_WriteRegister(p_dev, AD717X_CHMAP0_REG);

			}
			break;
		case AD_CHN_SHORT:
			p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP1_REG);
			if (AD717X_CHMAP_REG_CH_EN == (p_reg->value & AD717X_CHMAP_REG_CH_EN))
			{
				// Already enabled, nothing to do
			}
			else
			{
				// Disable current channel
				p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP0_REG);
				p_reg->value = (uint32_t)((int32_t)p_reg->value & ~(AD717X_CHMAP_REG_CH_EN));
				AD717X_WriteRegister(p_dev, AD717X_CHMAP0_REG);
				// Enable channel
				p_reg = AD717X_GetReg(p_dev, AD717X_CHMAP1_REG);
				p_reg->value |= AD717X_CHMAP_REG_CH_EN;
				AD717X_WriteRegister(p_dev, AD717X_CHMAP1_REG);

			}
			break;
		default:
			ret = ILLEGAL_ATTRIB_IDX;
			break;
	}


	return ret; // TODO error check when migrating to product verison
}

