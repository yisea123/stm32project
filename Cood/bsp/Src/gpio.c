/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "main.h"
#include "tsk_measure.h"
#include "stm32f4xx_it.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PC8   ------> SDIO_D0
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, SPI4_CS_Pin|MOTOR1_MODE0_Pin|MOTOR1_EN_N_Pin
                          |MOTOR1_DIR_Pin|MOTOR1_RST_N_Pin, GPIO_PIN_RESET);
  WDI_SET(GPIO_PIN_SET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, LED_WORK1_Pin|LED_WORK2_Pin|MOTOR2_MODE0_Pin|LED_660_L_Pin 
                          |LED_880_L_Pin|SH_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOJ, MOTOR1_CURR_Pin|MOTOR3_DIR_Pin|MOTOR3_RST_N_Pin|MOTOR3_CURR_Pin 
                          |MOTOR4_MODE0_Pin|MOTOR4_EN_N_Pin|GAIN_L_REF2_Pin|GAIN_L_REF1_Pin 
                          |GAIN_S_MEA3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MOTOR2_EN_N_GPIO_Port, MOTOR2_EN_N_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, MOTOR2_RST_N_Pin|MOTOR2_CURR_Pin|MOTOR3_MODE0_Pin|MOTOR3_EN_N_Pin 
                          |MOTOR2_DIR_Pin|GAIN_S_REF3_Pin|GAIN_S_REF2_Pin|GAIN_S_REF1_Pin 
                          |GAIN_L_REF3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOK, MOTOR4_DIR_Pin|MOTOR4_RST_N_Pin|SPI6_CS_Pin|GAIN_L_MEA3_Pin 
                          |GAIN_L_MEA2_Pin|GAIN_L_MEA1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, MOTOR4_CURR_Pin|NCG3_Pin|NCG4_Pin|NCG5_Pin 
                          |NCG6_Pin|LED_660_L_IC_Pin|LED_880_L_IC_Pin|GAIN_S_MEA2_Pin 
                          |GAIN_S_MEA1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_660_S_IC_Pin|LED_880_S_IC_Pin|LED_880_S_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_660_S_GPIO_Port, LED_660_S_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, SYNC_N_Pin|SPI3_CS_Pin|WDI_Pin|FAN_CTR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin 
                           PEPin PEPin */
  GPIO_InitStruct.Pin = SPI4_CS_Pin|WDI_EN_Pin|MOTOR1_MODE0_Pin|MOTOR1_EN_N_Pin 
                          |MOTOR1_DIR_Pin|MOTOR1_RST_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin PIPin 
                           PIPin PIPin PIPin PIPin 
                           PIPin PIPin PIPin PIPin */
  GPIO_InitStruct.Pin = NC_Pin|NCI9_Pin|NCI10_Pin|NCI11_Pin 
                          |NCI12_Pin|NCI13_Pin|NCI14_Pin|NCI15_Pin 
                          |NCI0_Pin|NCI2_Pin|SW_SNS1_Pin|SW_SNS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                           PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = NCC13_Pin|NCC0_Pin|NCC1_Pin|NCC2_Pin 
                          |NCC3_Pin|NCC4_Pin|NCC5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin PFPin PFPin 
                           PFPin PFPin PFPin */
  GPIO_InitStruct.Pin = NCF2_Pin|NCF10_Pin|NCF11_Pin|NCF12_Pin 
                          |NCF13_Pin|NCF14_Pin|NCF15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI5_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI5_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin PHPin 
                           PHPin */
  GPIO_InitStruct.Pin = NCH2_Pin|NCH3_Pin|NCH7_Pin|NCH8_Pin 
                          |NCH9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin PHPin 
                           PHPin PHPin */
  GPIO_InitStruct.Pin = LED_WORK1_Pin|LED_WORK2_Pin|MOTOR2_MODE0_Pin|LED_660_L_Pin 
                          |LED_880_L_Pin|SH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = NCA4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NCA4_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RS485_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PBPin PBPin PBPin 
                           PBPin */
  GPIO_InitStruct.Pin = GPIO_PIN_2|MOTOR2_FAULT_N_Pin|NCB8_Pin
                          |NCB9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PJPin PJPin PJPin PJPin 
                           PJPin PJPin PJPin */
  GPIO_InitStruct.Pin = NCJ0_Pin|NCJ1_Pin|NCJ2_Pin|NCJ3_Pin 
                          |NCJ4_Pin|MOTOR3_FAULT_N_Pin|NCJ14_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = NCG0_Pin|NCG1_Pin|NCG15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LOW_VOL_DEC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LOW_VOL_DEC1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LOW_VOL_DEC2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LOW_VOL_DEC2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin 
                           PEPin */
  GPIO_InitStruct.Pin = NCE9_Pin|NCE10_Pin|MOTOR1_FAULT_N_Pin|NCE0_Pin 
                          |NCE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PJPin PJPin PJPin PJPin 
                           PJPin PJPin PJPin PJPin 
                           PJPin */
  GPIO_InitStruct.Pin = MOTOR1_CURR_Pin|MOTOR3_DIR_Pin|MOTOR3_RST_N_Pin|MOTOR3_CURR_Pin 
                          |MOTOR4_MODE0_Pin|MOTOR4_EN_N_Pin|GAIN_L_REF2_Pin|GAIN_L_REF1_Pin 
                          |GAIN_S_MEA3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MOTOR2_EN_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MOTOR2_EN_N_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin PDPin 
                           PDPin */
  GPIO_InitStruct.Pin = MOTOR2_RST_N_Pin|MOTOR2_CURR_Pin|MOTOR3_MODE0_Pin|MOTOR3_EN_N_Pin 
                          |MOTOR2_DIR_Pin|GAIN_S_REF3_Pin|GAIN_S_REF2_Pin|GAIN_S_REF1_Pin 
                          |GAIN_L_REF3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = NCD14_Pin|NCD15_Pin|NCD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PKPin PKPin PKPin PKPin 
                           PKPin PKPin */
  GPIO_InitStruct.Pin = MOTOR4_DIR_Pin|MOTOR4_RST_N_Pin|SPI6_CS_Pin|GAIN_L_MEA3_Pin 
                          |GAIN_L_MEA2_Pin|GAIN_L_MEA1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pins : PKPin PKPin */
  GPIO_InitStruct.Pin = MOTOR4_FAULT_N_Pin|NCK4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin 
                           PGPin PGPin PGPin PGPin 
                           PGPin */
  GPIO_InitStruct.Pin = MOTOR4_CURR_Pin|NCG3_Pin|NCG4_Pin|NCG5_Pin 
                          |NCG6_Pin|LED_660_L_IC_Pin|LED_880_L_IC_Pin|GAIN_S_MEA2_Pin 
                          |GAIN_S_MEA1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = LED_660_S_IC_Pin|LED_880_S_IC_Pin|LED_880_S_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED_660_S_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_660_S_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin PIPin */
  GPIO_InitStruct.Pin = SYNC_N_Pin|SPI3_CS_Pin|WDI_Pin|FAN_CTR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI3_MISO_EXTI0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI3_MISO_EXTI0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI6_MISO_EXTI11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI6_MISO_EXTI11_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  GPIO_InitStruct.Pin = LED_WORK1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_WORK1_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = LED_WORK2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_WORK2_GPIO_Port, &GPIO_InitStruct);

//test Pin for temp
GPIO_InitStruct.Pin = GPIO_PIN_2;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}


static const PinInst testPins[] =
{
		{GPIOC,GPIO_PIN_2},
		{GPIOC,GPIO_PIN_2},
};

static const uint16_t PinSIze = sizeof(testPins)/sizeof(PinInst);

void TestPinSet(uint16_t chn, uint8_t state)
{
	assert(chn < PinSIze);

	if(state)
		HAL_GPIO_WritePin(testPins[chn].port,testPins[chn].pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(testPins[chn].port,testPins[chn].pin,GPIO_PIN_RESET);
}

void TestPinToggle(uint16_t chn)
{
	assert(chn < PinSIze);
	HAL_GPIO_TogglePin(testPins[chn].port,testPins[chn].pin);
}

/* USER CODE BEGIN 2 */
extern volatile uint16_t lowVoltTage;
extern volatile uint16_t highVoltTage;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// Switch off the interrupts and read out data
#if 0
	if(LOW_VOL_DEC1_Pin == GPIO_Pin || LOW_VOL_DEC2_Pin == GPIO_Pin)
	{
		//todo
		//handle voltage high and low:
		// default: 1 detect low voltage than 10.3
		GPIO_PinState state = HAL_GPIO_ReadPin(LOW_VOL_DEC1_GPIO_Port,LOW_VOL_DEC1_Pin);
		if(GPIO_PIN_RESET == state)
		{
			lowVoltTage = 1;
			//low voltage is detected;
		}
		else
		{
			lowVoltTage = 0;
		}
		state = HAL_GPIO_ReadPin(LOW_VOL_DEC2_GPIO_Port,LOW_VOL_DEC2_Pin);
		//default: 0 detect high voltage than 13.1
		if(GPIO_PIN_SET == state)
		{
			highVoltTage = 2;
		}
		else
		{
			highVoltTage = 0;

		}
	//	SendTskMsg(MCU_STATUS_ID,TSK_INIT,MCU_VOLT_REFRESH, NULL);

	}

#endif
}


/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
