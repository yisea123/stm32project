/*
 * dev_ad7190.c
 *
 *  Created on: 2019年3月25日
 *      Author: pli
 */


#include "dev_ad7190.h"
#include "shell_io.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define AIN1P_AIN2N

#define ZERO_MODE               0 // 1：零点电压测试模式   0：正常电压采集模式
                                  // 零点电压测试用于获取得到0V（即短路）输入时的偏置电压
                                  // AD转换值。根据电路设计把运放输出端电压偏置0.2V左右
                                  // （即模拟量经过运放后实现了“放大倍数+电压偏置”功能）
                                  // 这样可以解决运放零点漂移问题。

/* 私有变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi_AD7190;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: SPI初始化
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
*/
void AD7190_SPI_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  AD7190_SPIx_CLK_ENABLE();
  AD7190_GPIO_CLK_ENABLE();
  /**SPI1 GPIO Configuration
  PA5     ------> SPI1_SCK
  PB4     ------> SPI1_MISO
  PB5     ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = AD7190_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(AD7190_SCK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD7190_MISO_Pin|AD7190_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(AD7190_MISO_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD7190_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AD7190_CS_GPIO_Port, &GPIO_InitStruct);


  hspi_AD7190.Instance = AD7190_SPIx;
  hspi_AD7190.Init.Mode = SPI_MODE_MASTER;
  hspi_AD7190.Init.Direction = SPI_DIRECTION_2LINES;
  hspi_AD7190.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi_AD7190.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi_AD7190.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi_AD7190.Init.NSS = SPI_NSS_SOFT;
  hspi_AD7190.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi_AD7190.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi_AD7190.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi_AD7190.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi_AD7190.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&hspi_AD7190);

}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 * @param registerValue - Data value to write.
 * @param bytesNumber - Number of bytes to be written.
 *
 * @return none.
*******************************************************************************/
void AD7190_SetRegisterValue(uint8_t registerAddress,
                             uint32_t  registerValue,
                             uint8_t bytesNumber)
{
    uint8_t writeCommand[5] = {0, 0, 0, 0, 0};
    uint8_t* dataPointer    = (uint8_t*)&registerValue;
    uint8_t bytesNr         = bytesNumber;

    writeCommand[0] = AD7190_COMM_WRITE | AD7190_COMM_ADDR(registerAddress);
    while(bytesNr > 0)
    {
        writeCommand[bytesNr] = *dataPointer;
        dataPointer ++;
        bytesNr --;
    }
    HAL_SPI_Transmit(&hspi_AD7190,writeCommand, bytesNumber+1,0xFFFFFF);
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 * @param bytesNumber - Number of bytes that will be read.
 *
 * @return buffer - Value of the register.
*******************************************************************************/
uint32_t AD7190_GetRegisterValue(uint8_t registerAddress,
                                      uint8_t bytesNumber)
{
    uint8_t registerWord[4] = {0, 0, 0, 0};
    uint8_t address         = 0;
    uint32_t  buffer          = 0x0;
    uint8_t i               = 0;

    address = AD7190_COMM_READ | AD7190_COMM_ADDR(registerAddress);

    HAL_SPI_Transmit(&hspi_AD7190,&address, 1,0xFFFFFF);
    HAL_SPI_Receive(&hspi_AD7190,registerWord,bytesNumber,0xFFFFFF);
    for(i = 0; i < bytesNumber; i++)
    {
      buffer = (buffer << 8) + registerWord[i];
    }
    return buffer;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return none.
*******************************************************************************/
void AD7190_Reset(void)
{
    uint8_t registerWord[7];

    registerWord[0] = 0x01;
    registerWord[1] = 0xFF;
    registerWord[2] = 0xFF;
    registerWord[3] = 0xFF;
    registerWord[4] = 0xFF;
    registerWord[5] = 0xFF;
    registerWord[6] = 0xFF;
    HAL_SPI_Transmit(&hspi_AD7190,registerWord, 7,0xFFFFFF);
}
/***************************************************************************//**
 * @brief Checks if the AD7190 part is present.
 *
 * @return status - Indicates if the part is present or not.
*******************************************************************************/
uint8_t AD7190_Init(void)
{
    uint8_t status = 1;
    uint32_t regVal = 0;

    AD7190_SPI_Init();
    AD7190_Reset();

    /* Allow at least 500 us before accessing any of the on-chip registers. */
    HAL_Delay(1);
    regVal = AD7190_GetRegisterValue(AD7190_REG_ID, 1);
//    TraceUser("ad7190:0x%X\n",regVal);
    if( (regVal & AD7190_ID_MASK) != ID_AD7190)
    {
        status = 0;
    }
    return status ;
}


/***************************************************************************//**
 * @brief Set device to idle or power-down.
 *
 * @param pwrMode - Selects idle mode or power-down mode.
 *                  Example: 0 - power-down
 *                           1 - idle
 *
 * @return none.
*******************************************************************************/
void AD7190_SetPower(uint8_t pwrMode)
{
     uint32_t oldPwrMode = 0x0;
     uint32_t newPwrMode = 0x0;

     oldPwrMode = AD7190_GetRegisterValue(AD7190_REG_MODE, 3);
     oldPwrMode &= ~(AD7190_MODE_SEL(0x7));
     newPwrMode = oldPwrMode | AD7190_MODE_SEL((pwrMode * (AD7190_MODE_IDLE)) | (!pwrMode * (AD7190_MODE_PWRDN)));
     AD7190_SetRegisterValue(AD7190_REG_MODE, newPwrMode, 3);
}

/***************************************************************************//**
 * @brief Waits for RDY pin to go low.
 *
 * @return none.
*******************************************************************************/
void AD7190_WaitRdyGoLow(void)
{
    uint32_t timeOutCnt = 0xFFFFF;

    while(AD7190_RDY_STATE && timeOutCnt--)
    {
        ;
    }
}

/***************************************************************************//**
 * @brief Selects the channel to be enabled.
 *
 * @param channel - Selects a channel.
 *
 * @return none.
*******************************************************************************/
void AD7190_ChannelSelect(uint16_t channel)
{
    uint32_t oldRegValue = 0x0;
    uint32_t newRegValue = 0x0;

    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3);
    oldRegValue &= ~(AD7190_CONF_CHAN(0xFF));
    newRegValue = oldRegValue | AD7190_CONF_CHAN(1 << channel);
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3);
}
/***************************************************************************//**
 * @brief setting continuous read data enable or disable
 *
 * @param cread - continuous read data
 *                 Example: 0 - Disable
 *                          1 - enable
 *
 * @return none.
*******************************************************************************/
void AD7190_Continuous_ReadData(unsigned char cread)
{
  unsigned char registerWord=0;
  
  if(cread==1)
  {
    registerWord=0x5C;
  }
  else
  {
    AD7190_WaitRdyGoLow();
    registerWord=0x5e;
  }
  HAL_SPI_Transmit(&hspi_AD7190,&registerWord,1,0xFFFFFF);
}
/***************************************************************************//**
 * @brief Performs the given calibration to the specified channel.
 *
 * @param mode - Calibration type.
 * @param channel - Channel to be calibrated.
 *
 * @return none.
*******************************************************************************/
void AD7190_Calibrate(uint8_t mode, uint8_t channel)
{
    uint32_t oldRegValue = 0x0;
    uint32_t newRegValue = 0x0;

    AD7190_ChannelSelect(channel);
    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_MODE, 3);
    oldRegValue &= ~AD7190_MODE_SEL(0x7);
    newRegValue = oldRegValue | AD7190_MODE_SEL(mode);
//    AD7190CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, newRegValue, 3); // CS is not modified.
    AD7190_WaitRdyGoLow();
//    AD7190CS_DISABLE();
}

/***************************************************************************//**
 * @brief Setting chop enable or disable
 *
 * @param chop - chop setting
 *               Example: 0 - Disable
 *                        1 - enable
 *  
 * @return none.
*******************************************************************************/
void AD7190_ChopSetting(unsigned char chop)
{
  unsigned int oldRegValue = 0x0;
  unsigned int newRegValue = 0x0;   
   
  oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3);
  if(chop==1)
  {
    newRegValue = oldRegValue | AD7190_CONF_CHOP;
  }
  else
  {
    newRegValue = oldRegValue & (~AD7190_CONF_CHOP); 
  }
  
  AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3);
}



void AD7190_MultiChannelSelect(uint16_t channel)
{
	uint16_t oldRegValue = 0x0;
	uint16_t newRegValue = 0x0;

	oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3);
	oldRegValue &= ~(AD7190_CONF_CHAN(0xFF));
	newRegValue = oldRegValue | AD7190_CONF_CHAN(channel);
	AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3);
}

/***************************************************************************//**
 * @brief Selects the polarity of the conversion and the ADC input range.
 *
 * @param polarity - Polarity select bit.
                     Example: 0 - bipolar operation is selected.
                              1 - unipolar operation is selected.
* @param range - Gain select bits. These bits are written by the user to select
                 the ADC input range.
 *
 * @return none.
*******************************************************************************/
void AD7190_RangeSetup(uint8_t polarity, uint8_t range)
{
    uint32_t oldRegValue = 0x0;
    uint32_t newRegValue = 0x0;

    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF,3);
    oldRegValue &= ~(AD7190_CONF_UNIPOLAR | AD7190_CONF_GAIN(0x7));
    newRegValue = oldRegValue | (polarity * AD7190_CONF_UNIPOLAR) | AD7190_CONF_GAIN(range) | AD7190_CONF_BUF;
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3);
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion.
 *
 * @return regData - Result of a single analog-to-digital conversion.
*******************************************************************************/
uint32_t AD7190_SingleConversion(void)
{
    uint32_t command = 0x0;
    uint32_t regData = 0x0;

    command = AD7190_MODE_SEL(AD7190_MODE_SINGLE) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) | AD7190_MODE_RATE(0x060);
//    AD7190CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3); // CS is not modified.
    AD7190_WaitRdyGoLow();
    regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
//    AD7190CS_DISABLE();

    return regData;
}

/***************************************************************************//**
 * @brief Returns the average of several conversion results.
 *
 * @return samplesAverage - The average of the conversion results.
*******************************************************************************/
uint32_t AD7190_ContinuousReadAvg(uint8_t sampleNumber)
{
    uint32_t samplesAverage = 0x0;
    uint8_t count = 0x0;
    uint32_t command = 0x0;

    command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) | AD7190_MODE_RATE(0x060);
//    AD7190CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
    for(count = 0;count < sampleNumber;count ++)
    {
        AD7190_WaitRdyGoLow();
        samplesAverage += AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
    }
//    AD7190CS_DISABLE();
    samplesAverage = samplesAverage / sampleNumber;

    return samplesAverage ;
}

/***************************************************************************//**
 * @brief Read data from temperature sensor and converts it to Celsius degrees.
 *
 * @return temperature - Celsius degrees.
*******************************************************************************/
uint32_t AD7190_TemperatureRead(void)
{
    uint8_t temperature = 0x0;
    uint32_t dataReg = 0x0;

    AD7190_RangeSetup(0, AD7190_CONF_GAIN_1);
    AD7190_ChannelSelect(AD7190_CH_TEMP_SENSOR);
    dataReg = AD7190_SingleConversion();
    dataReg -= 0x800000;
    dataReg /= 2815;   // Kelvin Temperature
    dataReg -= 273;    //Celsius Temperature
    temperature = (uint32_t) dataReg;

    return temperature;
}

void ad7190_unipolar_multichannel_conf(void)
{
  unsigned int command = 0x0;
  
  // chop enable
  AD7190_ChopSetting(1);
  
  /* Calibrates channel AIN1(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN1P_AINCOM);
  /* Calibrates channel AIN2(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN2P_AINCOM);
  /* Calibrates channel AIN3(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN3P_AINCOM);
  /* Calibrates channel AIN4(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN4P_AINCOM);
  
  /* Selects unipolar operation and ADC's input range to +-Vref/1. */
  AD7190_RangeSetup(1, AD7190_CONF_GAIN_1);  
  /* Calibrates channel AIN1(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL, AD7190_CH_AIN1P_AINCOM);
  /* Calibrates channel AIN2(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL, AD7190_CH_AIN2P_AINCOM);
  /* Calibrates channel AIN3(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL, AD7190_CH_AIN3P_AINCOM);
  /* Calibrates channel AIN4(+) - AINCOM(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL, AD7190_CH_AIN4P_AINCOM);
     
  // Ê¹ÄÜ£ºAD7190_CH_AIN1P_AINCOM
  //       AD7190_CH_AIN2P_AINCOM
  //       AD7190_CH_AIN3P_AINCOM
  //       AD7190_CH_AIN4P_AINCOM
  AD7190_MultiChannelSelect(0xF0);
  
  /* Performs a conversion. */ 
  command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_DAT_STA| \
                            AD7190_MODE_CLKSRC(AD7190_CLK_EXT_MCLK1_2) | AD7190_MODE_RATE(10);
  AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);  
  
  AD7190_Continuous_ReadData(1);

}


void AD7190_conf(void)
{
  uint32_t command = 0x0;
#ifdef AIN1P_AIN2N
  /* Calibrates channel AIN1(+) - AIN2(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN1P_AIN2M);
  /* Selects unipolar operation and ADC's input range to +-Vref/1. */
  AD7190_RangeSetup(0, AD7190_CONF_GAIN_128);
  AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL,AD7190_CH_AIN1P_AIN2M);
  /* Performs a single conversion. */
  AD7190_ChannelSelect(AD7190_CH_AIN1P_AIN2M);
  command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) |\
            AD7190_MODE_RATE(1023)|AD7190_MODE_SINC3;
  AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
  AD7190_WaitRdyGoLow();
  AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
  AD7190_WaitRdyGoLow();
  AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
#else
  /* Calibrates channel AIN3(+) - AIN4(-). */
  AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN3P_AIN4M);
  /* Selects unipolar operation and ADC's input range to +-Vref/1. */
  AD7190_RangeSetup(0, AD7190_CONF_GAIN_128);
  AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL,AD7190_CH_AIN3P_AIN4M);
  /* Performs a single conversion. */
  AD7190_ChannelSelect(AD7190_CH_AIN3P_AIN4M);
  command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) |\
            AD7190_MODE_RATE(384)|AD7190_MODE_SINC3;
  AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
  AD7190_WaitRdyGoLow();
  AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
  AD7190_WaitRdyGoLow();
  AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
#endif
}

uint32_t AD7190_ReadAvg(uint8_t sampleNumber)
{
#if 0
    uint32_t samplesAverage = 0x0;
    uint8_t count = 0x0;

    for(count = 0;count < sampleNumber;count ++)
    {
      AD7190_WaitRdyGoLow();
      samplesAverage += (AD7190_GetRegisterValue(AD7190_REG_DATA, 3)>>4);
    }
    samplesAverage = samplesAverage / sampleNumber;

    return samplesAverage ;
#else
    uint32_t samplesValue = 0x0;
    uint32_t tick = HAL_GetTick();
    AD7190_WaitRdyGoLow();
    samplesValue = (AD7190_GetRegisterValue(AD7190_REG_DATA, 3)>>4);
    tick = HAL_GetTick() - tick;
    TraceUser("time used,%d\n",tick);
    return samplesValue;
#endif
}

