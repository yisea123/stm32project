/*
 * dev_ad5689.c
 *
 *  Created on: 2019��3��25��
 *      Author: pli
 */


#include "dev_ad5689.h"


/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define SOFT_SPI  1  // 1:ʹ�����ģ��SPI   0��ʹ��Ӳ��SPI����

/* ˽�б��� ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi_AD5689;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: GPIO��ʼ����SPI�ӿ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void MX_AD5689_SPI_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  AD5689_GPIO_CLK_ENABLE();

  AD5689_SYNC_HIGH();
  AD5689_RST_HIGH();
  AD5689_SCK_HIGH();
  AD5689_MOSI_HIGH();

#if SOFT_SPI==1
  GPIO_InitStruct.Pin = AD5689_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD5689_SCK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD5689_MOSI_Pin;
  HAL_GPIO_Init(AD5689_MOSI_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD5689_SYNC_Pin;
  HAL_GPIO_Init(AD5689_SYNC_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD5689_RST_Pin;
  HAL_GPIO_Init(AD5689_RST_GPIO_Port, &GPIO_InitStruct);
#else
  AD5689_SPIx_CLK_ENABLE();

  GPIO_InitStruct.Pin = AD5689_SYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD5689_SYNC_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD5689_RST_Pin;
  HAL_GPIO_Init(AD5689_RST_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD5689_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = AD5689_GPIO_AFx;
  HAL_GPIO_Init(AD5689_SCK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD5689_MOSI_Pin;
  HAL_GPIO_Init(AD5689_MOSI_GPIO_Port, &GPIO_InitStruct);

  hspi_AD5689.Instance = AD5689_SPIx;
  hspi_AD5689.Init.Mode = SPI_MODE_MASTER;
  hspi_AD5689.Init.Direction = SPI_DIRECTION_1LINE;
  hspi_AD5689.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi_AD5689.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi_AD5689.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi_AD5689.Init.NSS = SPI_NSS_SOFT;
  hspi_AD5689.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi_AD5689.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi_AD5689.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi_AD5689.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi_AD5689.Init.CRCPolynomial = 0;
  HAL_SPI_Init(&hspi_AD5689);
#endif
}

/**
  * ��������: �򵥵���ʱ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void delay(void)
{
  uint16_t i;
  for(i=0;i<50;i++)
  {
    __NOP();
  }
}

#if SOFT_SPI==1

/**
  * ��������: ����AD5689�Ĵ���ֵ
  * �������: command��ָ��
  *           channel��ͨ��
  *           data   ������
  * �� �� ֵ: ��
  * ˵    ��: ���ģ��SPI
  */
void AD5689_SetRegisterValue(uint8_t command,DACHANNEL channel,uint16_t data)
{
  uint32_t reg=0;
  uint8_t i;

  reg=(command<<20)|(channel<<16);
  reg |=data;

  AD5689_SYNC_LOW();
  printf(":%08X\n",reg);
  delay();

	for(i=0;i<24;i++)
	{
		if (reg&0x800000)
		{
			AD5689_MOSI_HIGH();
		}
		else
		{
			AD5689_MOSI_LOW();
		}
		AD5689_SCK_HIGH();
		reg <<= 1;
    delay();
		AD5689_SCK_LOW();
    delay();
	}
	delay();
	AD5689_SYNC_HIGH();
  delay();

}
#else

/**
  * ��������: ����AD5689�Ĵ���ֵ
  * �������: command��ָ��
  *           channel��ͨ��
  *           data   ������
  * �� �� ֵ: ��
  * ˵    ��: Ӳ��SPI
  */
void AD5689_SetRegisterValue(uint8_t command,DACHANNEL channel,uint16_t data)
{
  uint8_t reg[3]={0};

  reg[0]=(command<<4)|channel;
  reg[1]=(data>>8)&0xFF;
  reg[2]=data&0xFF;

  AD5689_SYNC_LOW();
  delay();
  HAL_SPI_Transmit(&hspi_AD5689,reg,3,0xF);
  delay();
  AD5689_SYNC_HIGH();
}

#endif

/**
  * ��������: ���AD5689
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void AD5689_Reset(void)
{
  AD5689_SetRegisterValue(COMMAND_RESET,DAC_ALL,0);
}

/**
  * ��������: ��������
  * �������: powerdown_channel1��ͨ��1�ĵ���ģʽ
  *           powerdown_channel2��ͨ��2�ĵ���ģʽ
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void AD5689_PowerDown(AD5689PowerdownType powerdown_channel1,AD5689PowerdownType powerdown_channel2)
{
  uint8_t data=0x3C;
  data |=((powerdown_channel2<<6)|powerdown_channel1);
  AD5689_SetRegisterValue(COMMAND_POWER_DOWN,DAC_ALL,data);
}

/**
  * ��������: LDAC���μĴ�������
  * �������: ldac1��ͨ��1��LDAC����ѡ��
  *           ldac2��ͨ��2��LDAC����ѡ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void AD5689_LDAC_MASK(AD5689LDACMASKType ldac1,AD5689LDACMASKType ldac2)
{
  uint8_t data=ldac1;
  data |=(ldac2<<3);
  AD5689_SetRegisterValue(COMMAND_LDAC_MASK,DAC_ALL,data);
}

/**
  * ��������: �ڲ��ο���ѹ����
  * �������: ref���ڲ��ο���ѹѡ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void AD5689_Internal_Reference(AD5689IntRefType ref)
{
  AD5689_SetRegisterValue(COMMAND_INT_REF,DAC_ALL,ref);
}

/**
  * ��������: д������Ĵ���n(ȡ����LDAC)
  * �������: channel��ͨ��
  *           data����ֵ
  * �� �� ֵ: ��
  * ˵    ��: ����0001�����û����д�����DAC��ר������Ĵ�����
  *           �� LDACΪ�͵�ƽʱ������Ĵ�����͸���� (�������
  *           LDAC���μĴ�������)��
  */
void AD5689_Write_InputREG(DACHANNEL channel,uint16_t data)
{
  AD5689_SetRegisterValue(COMMAND_WRITEn,channel,data);
}

/**
  * ��������: ������Ĵ���n�����ݸ���DAC�Ĵ���n
  * �������: channel��ͨ��
  *           data����ֵ
  * �� �� ֵ: ��
  * ˵    ��: ����0010����DAC�Ĵ���/����м���ѡ������Ĵ�������
  *           �ݲ�ֱ�Ӹ���DAC���
  */
void AD5689_InputREG_toUpdate_DACREG(DACHANNEL channel,uint16_t data)
{
  AD5689_SetRegisterValue(COMMAND_WRITEn_UPDATEn,channel,data);
}

/**
  * ��������: д��͸���DACͨ��n(��LDAC�޹�)
  * �������: channel��ͨ��
  *           data����ֵ
  * �� �� ֵ: ��
  * ˵    ��: ����0011�����û�д��DAC�Ĵ�����ֱ�Ӹ���DAC���
  */
void AD5689_WriteUpdate_DACREG(DACHANNEL channel,uint16_t data)
{
  AD5689_SetRegisterValue(COMMAND_WRITE_UPDATEn,channel,data);
}

/**
  * ��������: LDAC���μĴ�������
  * �������: decn���ջ�������
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void AD5689_DAISYCHAIN_OPERATION(AD5689DECNType decn)
{
  AD5689_SetRegisterValue(COMMAND_DCEN_REG,DAC_ALL,decn);
}

/***************************************************************************//**
 * @brief Checks if the AD5689 part is present.
 *
 * @return status - Indicates if the part is present or not.
*******************************************************************************/
void AD5689_Init(void)
{
  MX_AD5689_SPI_Init();
  AD5689_RST_LOW();
  HAL_Delay(5);
  AD5689_RST_HIGH();
  HAL_Delay(5);
  AD5689_Reset();
  AD5689_LDAC_MASK(LDAC_Disable,LDAC_Disable);
}
