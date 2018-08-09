/**
 ******************************************************************************
 * File Name          : USART.c
 * Description        : This file provides code for the configuration
 *                      of the USART instances.
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
#include "string.h"
#include "usart.h"

#include "gpio.h"
#include "dma.h"
#include "main.h"
#include "stm32f4xx_it.h"
#include "LB_Layer1_Uart.h"
/* USER CODE BEGIN 0 */
#define UART_BUFF_SIZE 		128
#define UART_IDX_MSK		(0x00007FU)
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if (uartHandle->Instance == USART1)
	{
		/* USER CODE BEGIN USART1_MspInit 0 */

		/* USER CODE END USART1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE()
		;

		/**USART1 GPIO Configuration
		 PA9     ------> USART1_TX
		 PA10     ------> USART1_RX
		 */
		//lint -e525
    	GPIO_InitStruct.Pin = UART1_TX_Pin|UART1_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		hdma_usart1_tx.Instance = DMA2_Stream7;
		hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_tx.Init.Mode = DMA_NORMAL;
		hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmatx, hdma_usart1_tx);

		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(USART1_IRQn, ISR_PRIORITY_UART,
				ISR_SUB_PRIORITY_UART + 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		/* USER CODE BEGIN USART1_MspInit 1 */

		/* USER CODE END USART1_MspInit 1 */
	}
	else if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE()
		;

		/**USART2 GPIO Configuration
		 PD5     ------> USART2_TX
		 PD6     ------> USART2_RX
		 */
    	GPIO_InitStruct.Pin = UART2_TX_Pin|UART2_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		hdma_usart2_tx.Instance = DMA1_Stream6;
		hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_tx.Init.Mode = DMA_NORMAL;
		hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmatx, hdma_usart2_tx);

		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(USART2_IRQn, ISR_PRIORITY_UART,
				ISR_SUB_PRIORITY_UART + 1);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		/* USER CODE BEGIN USART2_MspInit 1 */

		/* USER CODE END USART2_MspInit 1 */
	}
	else if (uartHandle->Instance == USART3)
	{
		/* USER CODE BEGIN USART3_MspInit 0 */

		/* USER CODE END USART3_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE()
		;

		/**USART3 GPIO Configuration
		 PB10     ------> USART3_TX
		 PB11     ------> USART3_RX
		 */
    	GPIO_InitStruct.Pin = RS485_TXD_Pin|RS485_RXD_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		hdma_usart3_tx.Instance = DMA1_Stream3;
		hdma_usart3_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart3_tx.Init.Mode = DMA_NORMAL;
		hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmatx, hdma_usart3_tx);
		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(USART3_IRQn, ISR_PRIORITY_UART,
				ISR_SUB_PRIORITY_UART + 2);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		/* USER CODE BEGIN USART3_MspInit 1 */

		/* USER CODE END USART3_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

	if (uartHandle->Instance == USART1)
	{
		/* USER CODE BEGIN USART1_MspDeInit 0 */

		/* USER CODE END USART1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
		 PA9     ------> USART1_TX
		 PA10     ------> USART1_RX
		 */
    	HAL_GPIO_DeInit(GPIOA, UART1_TX_Pin|UART1_RX_Pin);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(uartHandle->hdmatx);

		/* Peripheral interrupt Deinit*/
		HAL_NVIC_DisableIRQ(USART1_IRQn);

		/* USER CODE BEGIN USART1_MspDeInit 1 */

		/* USER CODE END USART1_MspDeInit 1 */
	}
	else if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration    
		 PD5     ------> USART2_TX
		 PD6     ------> USART2_RX
		 */
    	HAL_GPIO_DeInit(GPIOD, UART2_TX_Pin|UART2_RX_Pin);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(uartHandle->hdmatx);

		/* Peripheral interrupt Deinit*/
		HAL_NVIC_DisableIRQ(USART2_IRQn);

		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	}
	else if (uartHandle->Instance == USART3)
	{
		/* USER CODE BEGIN USART3_MspDeInit 0 */

		/* USER CODE END USART3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration
		 PB10     ------> USART3_TX
		 PB11     ------> USART3_RX
		 */
    	HAL_GPIO_DeInit(GPIOB, RS485_TXD_Pin|RS485_RXD_Pin);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(uartHandle->hdmatx);

		/* Peripheral interrupt Deinit*/
		HAL_NVIC_DisableIRQ(USART3_IRQn);

		/* USER CODE BEGIN USART3_MspDeInit 1 */

		/* USER CODE END USART3_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

uint16_t NewUartData(uint16_t type, uint8_t* ptrData)
{
	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint32_t uartShellIdx = 0;
	static uint32_t lastId = 0;
	uint16_t ret = OK;
	if (type == 0)
	{
		uartBuff[uartShellIdx++ & UART_IDX_MSK] = *ptrData;

	}
	else // not whole data
	{
		if (lastId < uartShellIdx)
		{
			*ptrData = uartBuff[lastId++ & UART_IDX_MSK];
			trace_putchar(*ptrData);
		}
		else
		{
			ret = FATAL_ERROR;
		}
	}
	return ret;
}


static uint8_t shellData1[UART_BUFF_SIZE];
static uint8_t shellData1Len = 0;
void Usart2RXHandle(void)
{
	uint8_t data = (uint8_t) (huart2.Instance->DR & (uint8_t) 0x00FFU);
#ifdef TEST_L_BUS
	LB_Layer1_Rx(data);
#else

	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint8_t uartShellIdx = 0;
	if ('\r' == data ||'\n' == data)
	{
		if(uartShellIdx != 0)
		{
			uartBuff[uartShellIdx++] = '\n';
			memset((void*)shellData1,0,sizeof(shellData1));
			memcpy((void*)shellData1,(void*)uartBuff,uartShellIdx);
			shellData1Len = uartShellIdx;
			MsgPush(SHELL_RX_ID, (uint32_t)&shellData1[0],0);
		}
		else if('\r' == data)
		{
			MsgPush(SHELL_RX_ID, 0,0);
		}
		uartShellIdx = 0;
	}
	else
	{
		if(data != 127)//backspace
			uartBuff[uartShellIdx++] = data;
		else if(uartShellIdx)
			uartShellIdx--;
		if(uartShellIdx >= UART_BUFF_SIZE)
			uartShellIdx = 0;

	}
#endif

}

void Usart3RXHandle(void)
{
	uint8_t data = (uint8_t) (huart3.Instance->DR & (uint8_t) 0x00FFU);
	LB_Layer1_Uart_Rx(data);
}
#if 0
void Usart1RXHandle(void)
{
	uint8_t data = (uint8_t) (huart1.Instance->DR & (uint8_t) 0x00FFU);
	LB_Layer1_Uart_Rx(data);
}
#endif
/* USER CODE END 1 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
