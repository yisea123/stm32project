/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#include "tim.h"
#include "gen_define.h"
#include "shell_io.h"
extern BoradType boardType;
uint32_t devType = 0;

uint8_t devAdr = 0;
char deviceName[32] = "Not defined";
/* USER CODE END 0 */


uint8_t doState1 = 0;
uint8_t doState2 = 0;

uint16_t aoValue1 = 0;
uint16_t aoValue2 = 0;

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Pinout Configuration
*/
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	devAdr = GetDevAdr();
	boardType = GetDevType();


	if(boardType == TYPE_DI_BOARD)
	{
		devAdr += DI_ADR;
	//	devAdr = 0x20;
		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		memcpy(deviceName,"Hach DI Board", sizeof("Hach DI Board"));
	}
	else if(boardType == TYPE_DO_BOARD)
	{
		devAdr += DO_ADR;
		//devAdr = 0x02;
		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		memcpy(deviceName,"Hach DO Board", sizeof("Hach DO Board"));
	}
	else if(boardType == TYPE_AO_BOARD)
	{
		devAdr += AO_ADR;
	//	devAdr = 0x20;

		memcpy(deviceName,"Hach AO Board", sizeof("Hach AO Board"));
		MX_TIM2_Init();
	}
	else
	{
//		memcpy(deviceName,"Not defined", sizeof("Not defined"));
	}
	devType = ((uint32_t)boardType<<4) + devAdr;

}

/* USER CODE BEGIN 2 */


BoradType GetDevType(void)
{
	GPIO_PinState st1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	GPIO_PinState st2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	uint16_t type = ((st1== GPIO_PIN_RESET)? 0x0:0x01);
	type |= ((st2== GPIO_PIN_RESET)? 0x0:0x02);

	return type;

}

uint8_t simuDO1 = 0x0;
uint8_t simuDO2 = 0x0;
uint16_t simuAO1 = 0x0;
uint16_t simuAO2 = 0x0;
uint8_t diState1 = 0x0;
uint8_t diState2 = 0x0;

uint16_t readBackAO2 = 0x0;
uint16_t readBackAO1 = 0x0;




static uint8_t 	doValue[2] = {0,0};
static uint16_t aoValue[2] = {0,0};


void UpdateOutput(void)
{
	if(boardType == TYPE_DO_BOARD)
	{
		OutputDO(1, doValue[0]);
		OutputDO(2, doValue[1]);
	}
	else if(boardType == TYPE_AO_BOARD)
	{
		OutputAO(1, aoValue[0]);
		OutputAO(2, aoValue[1]);
	}
}

void OutputDO(uint8_t chn, uint8_t val)
{
	if(chn == 1)
	{
		doValue[0] = val;
		uint8_t st = simuDO1 & 0x80;
		if(st)
		{
			val = simuDO1&0x7F;
		}
		doState1 = val;
		if(val)
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
		shell_Add("DO:P_%d=>%d\n",	chn, val);
	}
	else if(chn == 2)
	{
		doValue[1] = val;
		uint8_t st = simuDO2 & 0x80;
		if(st)
		{
			val = simuDO2&0x7F;
		}
		doState2 = val;
		if(val)
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
		shell_Add("DO:P_%d=>%d\n",	chn, val);
	}
	else
	{}
}



uint8_t InputDI(uint8_t chn)
{
	uint8_t state = 0x0;
	if(chn == 1)
	{
		GPIO_PinState st1 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);
		state = ((st1== GPIO_PIN_RESET)? 0x0:0x01);
		diState1 = state;
	//	("DI:P_%d=>%d\n",	chn, state);
	}
	else if(chn == 2)
	{
		GPIO_PinState st1 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2);
		state = ((st1== GPIO_PIN_RESET)? 0x0:0x01);
		diState2 = state;
	//	("DI:P_%d=>%d\n",	chn, state);
	}
	else
	{}

	return state;

}


void OutputAO(uint8_t chn, uint16_t val)
{
	if(chn == 1)
	{
		aoValue[0] = val;

		//uint16_t st = simuAO1 & 0x8000;
		if(simuAO1)
		{
			val = simuAO1;
		}

		aoValue1 = val;
		OutputAOWithClibration(val, 0);
	//	GPIO_PinState st1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	//	state = ((st1== GPIO_PIN_RESET)? 0x0:0x01);
		shell_Add("AO:P_%d=>%d\n",	chn, val);
	}
	else if(chn == 2)
	{
		aoValue[1] = val;
		//uint16_t st = simuAO2 & 0x8000;
		if(simuAO2)
		{
			val = simuAO2;
		}
		aoValue2 = val;
		OutputAOWithClibration(val, 1);
	//	GPIO_PinState st1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
	//	state = ((st1== GPIO_PIN_RESET)? 0x0:0x01);
		shell_Add("AO:P_%d=>%d\n",	chn, val);
	}
	else
	{}

	return ;

}

uint8_t GetDevAdr(void)
{
	GPIO_PinState st1 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3);
	GPIO_PinState st2 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4);
	GPIO_PinState st3 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5);
	GPIO_PinState st4 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6);
	uint8_t adr = ((st1== GPIO_PIN_RESET)? 0x0:0x01);
	adr |= ((st2== GPIO_PIN_RESET)? 0x0:0x02);
	adr |= ((st3== GPIO_PIN_RESET)? 0x0:0x04);
	adr |= ((st4== GPIO_PIN_RESET)? 0x0:0x08);
	return adr;

}

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
