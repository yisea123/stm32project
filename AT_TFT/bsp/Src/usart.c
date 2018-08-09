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
#include "usart.h"

#include "gpio.h"
#include "dma.h"
#include "main.h"
#include "stm32f4xx_it.h"
#include "shell_io.h"
/* USER CODE BEGIN 0 */
#define UART_BUFF_SIZE 		128
#define UART_IDX_MSK		(0x00007FU)
/* USER CODE END 0 */

UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_tx;

/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
 // while(1)
  {
	//  char sendBuf[] = "test.\r\n";
	//  shellSend(sendBuf,sizeof(sendBuf));
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();
  
    /**USART6 GPIO Configuration    
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart6_tx.Instance = DMA2_Stream7;
    hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
    hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart6_tx.Init.Mode = DMA_NORMAL;
    hdma_usart6_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart6_tx);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART6_IRQn, ISR_PRIORITY_UART, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();
  
    /**USART6 GPIO Configuration    
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART6_IRQn);

  }
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */


uint16_t NewUartData(uint16_t type,uint8_t* ptrData)
{


	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint32_t uartShellIdx = 0;
	static uint32_t lastId = 0;
	uint16_t ret = OK;
	if(type == 0)
	{
		uartBuff[uartShellIdx&UART_IDX_MSK] = *ptrData;
		uartShellIdx++;
	}
	else // not whole data
	{
		if(lastId < uartShellIdx)
		{
			*ptrData = uartBuff[lastId&UART_IDX_MSK];
			lastId++;
		}
		else
		{
			ret = FATAL_ERROR;
		}
	}
	return ret;
}
static uint8_t shellData1[UART_BUFF_SIZE];

void Usart6RXHandle(void)
{
	uint8_t data = (uint8_t)(huart6.Instance->DR & (uint8_t)0x00FFU);
	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint8_t uartShellIdx = 0;
	if(shellStatus)
	{
		if ('\r' == data ||'\n' == data)
		{
			if(uartShellIdx != 0)
			{
				uartBuff[uartShellIdx++] = '\n';
				memset((void*)shellData1,0,sizeof(shellData1));
				memcpy((void*)shellData1,(void*)uartBuff,uartShellIdx);
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
			uartBuff[uartShellIdx++] = data;
			if(uartShellIdx >= UART_BUFF_SIZE)
				uartShellIdx = 0;
		}
	}
	else
	{
		HandleUartRx(data);
	}
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
