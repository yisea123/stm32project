/*
 * dev_ad5689.h
 *
 *  Created on: 2019年3月25日
 *      Author: pli
 */

#ifndef DEV_AD5689_H_
#define DEV_AD5689_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef enum
{
  DAC_A=0x1,
  DAC_B=0x8,
  DAC_ALL=0x9,
}DACHANNEL;

typedef enum
{
  NormalOperation=0,
  R1K2GND=1,
  R100K2GND=2,
  ThreeState=3,
}AD5689PowerdownType;
typedef enum
{
  LDAC_Enable=0,
  LDAC_Disable=1,
}AD5689LDACMASKType;

typedef enum
{
  IntRef_Enable=0,
  IntRef_Disable=1,
}AD5689IntRefType;

typedef enum
{
  Standalone_mode =0,
  DCEN_mode=1,
}AD5689DECNType;

/* 宏定义 --------------------------------------------------------------------*/
#define AD5689_SPIx                SPI1
#define AD5689_SPIx_CLK_ENABLE()   __HAL_RCC_SPI1_CLK_ENABLE()
#define AD5689_GPIO_CLK_ENABLE()   {__HAL_RCC_GPIOA_CLK_ENABLE();\
                                    __HAL_RCC_GPIOB_CLK_ENABLE();\
                                    __HAL_RCC_GPIOC_CLK_ENABLE();}
#define AD5689_GPIO_AFx            GPIO_AF5_SPI1

#define AD5689_SYNC_Pin            GPIO_PIN_13
#define AD5689_SYNC_GPIO_Port      GPIOC
#define AD5689_SCK_Pin             GPIO_PIN_5
#define AD5689_SCK_GPIO_Port       GPIOA
#define AD5689_RST_Pin             GPIO_PIN_4
#define AD5689_RST_GPIO_Port       GPIOB
#define AD5689_MOSI_Pin            GPIO_PIN_5
#define AD5689_MOSI_GPIO_Port      GPIOB

#define AD5689_SCK_HIGH()          AD5689_SCK_GPIO_Port->BSRR = AD5689_SCK_Pin
#define AD5689_SCK_LOW()           AD5689_SCK_GPIO_Port->BSRR = (uint32_t)AD5689_SCK_Pin<<16
#define AD5689_MOSI_HIGH()         AD5689_MOSI_GPIO_Port->BSRR = AD5689_MOSI_Pin
#define AD5689_MOSI_LOW()          AD5689_MOSI_GPIO_Port->BSRR = (uint32_t)AD5689_MOSI_Pin<<16
#define AD5689_SYNC_HIGH()         AD5689_SYNC_GPIO_Port->BSRR = AD5689_SYNC_Pin
#define AD5689_SYNC_LOW()          AD5689_SYNC_GPIO_Port->BSRR = (uint32_t)AD5689_SYNC_Pin<<16
#define AD5689_RST_HIGH()          AD5689_RST_GPIO_Port->BSRR = AD5689_RST_Pin
#define AD5689_RST_LOW()           AD5689_RST_GPIO_Port->BSRR = (uint32_t)AD5689_RST_Pin<<16

#define COMMAND_WRITEn             1
#define COMMAND_WRITEn_UPDATEn     2
#define COMMAND_WRITE_UPDATEn      3
#define COMMAND_POWER_DOWN         4
#define COMMAND_LDAC_MASK          5
#define COMMAND_RESET              6
#define COMMAND_INT_REF            7
#define COMMAND_DCEN_REG           8
#define COMMAND_READ_BACK          9

/* 扩展变量 ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi_AD5689;

/* 函数声明 ------------------------------------------------------------------*/
void AD5689_Init(void);
void AD5689_SetRegisterValue(uint8_t command,DACHANNEL channel,uint16_t data);
void AD5689_Reset(void);
void AD5689_PowerDown(AD5689PowerdownType powerdown_channel1,AD5689PowerdownType powerdown_channel2);
void AD5689_Internal_Reference(AD5689IntRefType ref);
void AD5689_Write_InputREG(DACHANNEL channel,uint16_t data);
void AD5689_InputREG_toUpdate_DACREG(DACHANNEL channel,uint16_t data);
void AD5689_WriteUpdate_DACREG(DACHANNEL channel,uint16_t data);
void AD5689_DAISYCHAIN_OPERATION(AD5689DECNType decn);

#ifdef __cplusplus
}
#endif



#endif /* DEV_AD5689_H_ */
