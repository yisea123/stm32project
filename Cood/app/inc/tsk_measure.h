
#ifndef _TSK_MEASURE_H_
#define _TSK_MEASURE_H_

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#define SPI3_CS_H HAL_GPIO_WritePin(SPI3_CS_GPIO_Port,SPI3_CS_Pin,GPIO_PIN_SET)
#define SPI6_CS_H HAL_GPIO_WritePin(SPI6_CS_GPIO_Port,SPI6_CS_Pin,GPIO_PIN_SET)
#define SYNC_H 	HAL_GPIO_WritePin(SYNC_N_GPIO_Port,SYNC_N_Pin,GPIO_PIN_SET)
#define SPI3_CLK_H HAL_GPIO_WritePin(SPI3_SCLK_GPIO_Port,SPI3_SCLK_Pin,GPIO_PIN_SET)
#define SPI6_CLK_H HAL_GPIO_WritePin(SPI6_SCK_GPIO_Port,SPI6_SCK_Pin,GPIO_PIN_SET)
#define SPI3_CS_L HAL_GPIO_WritePin(SPI3_CS_GPIO_Port,SPI3_CS_Pin,GPIO_PIN_RESET)
#define SPI6_CS_L HAL_GPIO_WritePin(SPI6_CS_GPIO_Port,SPI6_CS_Pin,GPIO_PIN_RESET)
#define SYNC_L 	HAL_GPIO_WritePin(SYNC_N_GPIO_Port,SYNC_N_Pin,GPIO_PIN_RESET)
#define SPI3_CLK_L HAL_GPIO_WritePin(SPI3_CLK_GPIO_Port,SPI3_CLK_Pin,GPIO_PIN_RESET)
#define SPI6_CLK_L HAL_GPIO_WritePin(SPI6_CLK_GPIO_Port,SPI6_CLK_Pin,GPIO_PIN_RESET)
#define SPI3_DRDY HAL_GPIO_ReadPin(SPI3_MISO_EXTI0_GPIO_Port,SPI3_MISO_EXTI0_Pin)
#define SPI6_DRDY HAL_GPIO_ReadPin(SPI6_MISO_EXTI11_GPIO_Port,SPI6_MISO_EXTI11_Pin)

typedef enum _ENUM_AD_DET_STAGE {AD_DET_660_SHORT_D0 = 0, 
				AD_DET_660_SHORT_D1, 
				AD_DET_660_SHORT_D2, 
				AD_DET_660_SHORT_D3,
				AD_DET_900_SHORT_D0, 
				AD_DET_900_SHORT_D1, 
				AD_DET_900_SHORT_D2, 
				AD_DET_900_SHORT_D3,
				AD_DET_660_LONG_D0, 
				AD_DET_660_LONG_D1, 
				AD_DET_660_LONG_D2, 
				AD_DET_660_LONG_D3,
				AD_DET_900_LONG_D0, 
				AD_DET_900_LONG_D1, 
				AD_DET_900_LONG_D2, 
				AD_DET_900_LONG_D3, 
				AD_DET_FIN
} ENUM_AD_DET_STAGE;




#define AD_SAMPLING_D0				0x00000001
#define AD_SAMPLING_D1				0x00000002
#define AD_SAMPLING_D2				0x00000004
#define AD_SAMPLING_D3				0x00000008

#define AD_SAMPLING_CHNL_SHORT		0x00000010
#define AD_SAMPLING_CHNL_LONG		0x00000020

#define AD_SAMPLING_RDY				0x00000040
#define AD_SAMPLING_ALL (AD_SAMPLING_RDY|AD_SAMPLING_D0|AD_SAMPLING_D1|AD_SAMPLING_D2|AD_SAMPLING_D3|AD_SAMPLING_CHNL_SHORT|AD_SAMPLING_CHNL_LONG)


#define MEASURE_CYCLE_DONE_SIG	0x00000001



#define SH_CIRCUIT_EN		HAL_GPIO_WritePin(SH_GPIO_Port,SH_Pin,GPIO_PIN_SET)
#define SH_CIRCUIT_DIS		HAL_GPIO_WritePin(SH_GPIO_Port,SH_Pin,GPIO_PIN_RESET)


typedef struct
{
	int32_t* meas;
	int32_t* ref;
	int32_t len;
}MEAS_DATA;

void Init_Tim9_Value(void);
void ADDetectStart(void);
void ADDetectStop(void);

void Calc_BlankRawABS(void);
uint16_t GetNewRawMeasData(float* val, uint16_t len);
void SubStep_CalcAndStoreRawMeasAbs(void);
void StartAdTask(void const* argument);
void StartMeasCalcTask(void const* argument);
void StartAdc(void);
void GetADC_Data(void);

#endif // #ifndef _TSK_MEASURE_H_
