/*
 * dev_ad7190.c
 *
 *  Created on: 2019年3月25日
 *      Author: pli
 */


#include "dev_ad7190.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define AIN1P_AIN2N

/* 私有变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi_weight;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: SPI初始化
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
*/
void MX_WEIGHT_SPI_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  WEIGHT_SPIx_CLK_ENABLE();
  WEIGHT_GPIO_CLK_ENABLE();
  /**SPI1 GPIO Configuration
  PA5     ------> SPI1_SCK
  PB4     ------> SPI1_MISO
  PB5     ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = WEIGHT_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(WEIGHT_SCK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = WEIGHT_MISO_Pin|WEIGHT_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(WEIGHT_MISO_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = WEIGHT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WEIGHT_CS_GPIO_Port, &GPIO_InitStruct);


  hspi_weight.Instance = WEIGHT_SPIx;
  hspi_weight.Init.Mode = SPI_MODE_MASTER;
  hspi_weight.Init.Direction = SPI_DIRECTION_2LINES;
  hspi_weight.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi_weight.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi_weight.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi_weight.Init.NSS = SPI_NSS_SOFT;
  hspi_weight.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi_weight.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi_weight.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi_weight.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi_weight.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&hspi_weight);

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
    HAL_SPI_Transmit(&hspi_weight,writeCommand, bytesNumber+1,0xFFFFFF);
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

    HAL_SPI_Transmit(&hspi_weight,&address, 1,0xFFFFFF);
    HAL_SPI_Receive(&hspi_weight,registerWord,bytesNumber,0xFFFFFF);
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
    HAL_SPI_Transmit(&hspi_weight,registerWord, 7,0xFFFFFF);
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

    MX_WEIGHT_SPI_Init();
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
//    WEIGHT_CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, newRegValue, 3); // CS is not modified.
    AD7190_WaitRdyGoLow();
//    WEIGHT_CS_DISABLE();
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
//    WEIGHT_CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3); // CS is not modified.
    AD7190_WaitRdyGoLow();
    regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
//    WEIGHT_CS_DISABLE();

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
//    WEIGHT_CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
    for(count = 0;count < sampleNumber;count ++)
    {
        AD7190_WaitRdyGoLow();
        samplesAverage += AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
    }
//    WEIGHT_CS_DISABLE();
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

void weight_ad7190_conf(void)
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

uint32_t weight_ad7190_ReadAvg(uint8_t sampleNumber)
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

    AD7190_WaitRdyGoLow();
    samplesValue = (AD7190_GetRegisterValue(AD7190_REG_DATA, 3)>>4);

    return samplesValue;
#endif
}

