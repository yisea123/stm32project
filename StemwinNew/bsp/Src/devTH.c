/*
 * devTH.c
 *
 *  Created on: 2019年5月8日
 *      Author: pli
 */




#include "string.h"
#include "usart.h"

#include "bsp.h"
#include "stm32f4xx_it.h"
#include "pcf8574.h"


#include "usart.h"
#include "delay.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/* USER CODE BEGIN 0 */
#define UART_BUFF_SIZE 		20
#define UART_IDX_MSK		(0x00007FU)
/* USER CODE END 0 */
enum
{
	NONE_ACT,
	READ_RELAY,
	WRITE_RELAY,
};

int16_t tempTh[2] = {9999,9999};
uint16_t uart3Idx = 0;
uint16_t uart2Idx = 0;
uint16_t uart2cnt = 0;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
uint16_t uart3State = 0;
uint16_t relayInput[4] = {0,0,0,0};
uint16_t relayOutput[4] = {0,0,0,0};
uint16_t uart3cnt = 0;
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
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

void UpdateTH(void)
{
//	static const uint8_t sendData[9] = {0x01,0x04,0x00,0x00,0x00,0x02,0x71,0xcb};
	static const uint8_t sendData[9] = {0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0b};
	PCF8574_WriteBit(6,1);
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart2, (uint8_t*)sendData, (uint16_t)8, 100);
	if(ret != HAL_OK)
	{
		//retry?

	}
	uart2Idx = 0;
	PCF8574_WriteBit(6,0);
	//osDelay(200);

}

void ReadRelay(void)
{
	static const uint8_t sendData[18] =
	{0x48,0x3a,0x01,0x52,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd5,0x45,0x44,};
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart3, (uint8_t*)sendData, (uint16_t)15, 100);
	if(ret != HAL_OK)
	{
		//retry?

	}
	uart3Idx = 0;
	uart3State = READ_RELAY;
}

void WriteRelay(void)
{
	static uint8_t sendData[18] =
	{0x48,0x3a,0x01,0x57,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xdC,0x45,0x44,};
	for(uint16_t i =0; i<4;i++)
	{
		sendData[4+i] = relayOutput[i];
	}
	sendData[12] = 0;
	for(uint16_t i=0; i<12;i++)
	{
		sendData[12] = sendData[12] + sendData[i];
	}
	HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart3, (uint8_t*)sendData, (uint16_t)15, 100);
	if(ret != HAL_OK)
	{
		//retry?

	}
	uart3Idx = 0;
	uart3State = WRITE_RELAY;
}


void MX_USART3_UART_Init(void)
{

	huart3.Instance = USART3;
	huart3.Init.BaudRate = 9600;
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
	if(uartHandle->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟

		GPIO_InitStruct.Pin=GPIO_PIN_9;			//PA9
		GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_InitStruct.Pull=GPIO_PULLUP;			//上拉
		GPIO_InitStruct.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_InitStruct.Alternate=GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);	   	//初始化PA9

		GPIO_InitStruct.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);	   	//初始化PA10

#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);		//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,7,0);	//抢占优先级7，子优先级0
#endif
	}
		else if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspInit 0 */
			__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
			__HAL_RCC_GPIOD_CLK_ENABLE();			//使能GPIOA时钟

		/* USER CODE END USART2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE()
		;

		/**USART2 GPIO Configuration
		 */
    	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		__HAL_UART_ENABLE_IT(uartHandle, USART_IT_RXNE);
		//USART_ITConfig(USART2_IRQn, USART_IT_RXNE, ENABLE);
		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(USART2_IRQn, ISR_PRIORITY_UART,
				1);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		/* USER CODE BEGIN USART2_MspInit 1 */

		/* USER CODE END USART2_MspInit 1 */
	}
	else if (uartHandle->Instance == USART3)
	{
		/* USER CODE BEGIN USART3_MspInit 0 */
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟

		/* USER CODE END USART3_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE()
		;

		/**USART3 GPIO Configuration
		 PB10     ------> USART3_TX
		 PB11     ------> USART3_RX
		 */
    	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


		/* Peripheral interrupt init */
		__HAL_UART_ENABLE_IT(uartHandle, USART_IT_RXNE);
		HAL_NVIC_SetPriority(USART3_IRQn, ISR_PRIORITY_UART,
				2);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		/* USER CODE BEGIN USART3_MspInit 1 */

		/* USER CODE END USART3_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

	if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		 */
    	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

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
    	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

		/* Peripheral interrupt Deinit*/
		HAL_NVIC_DisableIRQ(USART3_IRQn);

		/* USER CODE BEGIN USART3_MspDeInit 1 */

		/* USER CODE END USART3_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

void Usart2RXHandle(void)
{
	uint8_t data = (uint8_t) (huart2.Instance->DR & (uint8_t) 0x00FFU);

	static uint8_t uartBuff[20];
	uartBuff[uart2Idx++] = data;
	if(uart2Idx >= 9)
	{
		uart2cnt++;
		tempTh[0] = (uint16_t)(uartBuff[3]*256+uartBuff[4]);
		tempTh[1] = (uint16_t)(uartBuff[5]*256+uartBuff[6]);
		uart2Idx = 0;
	}
}

void Usart3RXHandle(void)
{
	uint8_t data = (uint8_t) (huart3.Instance->DR & (uint8_t) 0x00FFU);
	//LB_Layer1_Uart_Rx(data);
	static uint8_t uartBuff[30];
	uartBuff[uart3Idx++] = data;
	if(uart3Idx >= 15)
	{
		if(uart3State == WRITE_RELAY)
		{
			//write
			uart3cnt += 0x100;
		}
		else
		{
			relayInput[0] = uartBuff[4];
			relayInput[1] = uartBuff[5];
			relayInput[2] = uartBuff[6];
			relayInput[3] = uartBuff[7];

			uart3cnt += 1;
		}
	}

}
