/*
 * devTH.c
 *
 *  Created on: 2019��5��8��
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


int16_t tempTh[2] = {9999,9999};

uint16_t uart2Idx = 0;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

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
	if(uartHandle->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��

		GPIO_InitStruct.Pin=GPIO_PIN_9;			//PA9
		GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_InitStruct.Pull=GPIO_PULLUP;			//����
		GPIO_InitStruct.Speed=GPIO_SPEED_FAST;		//����
		GPIO_InitStruct.Alternate=GPIO_AF7_USART1;	//����ΪUSART1
		HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);	   	//��ʼ��PA9

		GPIO_InitStruct.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);	   	//��ʼ��PA10

#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);		//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,7,0);	//��ռ���ȼ�7�������ȼ�0
#endif
	}
		else if (uartHandle->Instance == USART2)
	{
		/* USER CODE BEGIN USART2_MspInit 0 */

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
    	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


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

uint16_t NewUartData(uint16_t type, uint8_t* ptrData)
{
	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint32_t uartShellIdx = 0;
	static uint32_t lastId = 0;
	if (type == 0)
	{
		uartBuff[uartShellIdx++ & UART_IDX_MSK] = *ptrData;

	}
	return 0;
}

static uint8_t shellData2[UART_BUFF_SIZE];
static uint8_t shellData2Len = 0;
void Usart2RXHandle(void)
{
	uint8_t data = (uint8_t) (huart2.Instance->DR & (uint8_t) 0x00FFU);

	static uint8_t uartBuff[20];
	uartBuff[uart2Idx++] = data;
	if(uart2Idx >= 9)
	{
		tempTh[0] = uartBuff[3]*256+uartBuff[4];
		tempTh[1] = uartBuff[5]*256+uartBuff[6];
		uart2Idx = 0;
	}
}

void Usart3RXHandle(void)
{
	uint8_t data = (uint8_t) (huart3.Instance->DR & (uint8_t) 0x00FFU);
	//LB_Layer1_Uart_Rx(data);
}