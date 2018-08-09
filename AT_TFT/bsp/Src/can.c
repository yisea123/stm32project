/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
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
#include "can.h"

#include "gpio.h"
#include "main.h"
#include "dev_can.h"
#include "stm32f4xx_it.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;


/* CAN1 init function */
void MX_CAN1_Init(void)
{
	CAN_FilterConfTypeDef sFilterConfig;
	static CanTxMsgTypeDef txMsg_Can1;
	static CanRxMsgTypeDef rxMsg_Can1;
// can clock is 42M hz
	//1m hz
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 24;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_9TQ;
  hcan1.Init.BS2 = CAN_BS2_4TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = ENABLE;//出错自动恢复
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
  hcan1.pTxMsg = &txMsg_Can1;
  hcan1.pRxMsg = &rxMsg_Can1;

  hcan1.pTxMsg->StdId = 0x321;
  hcan1.pTxMsg->ExtId = 0x01;
  hcan1.pTxMsg->RTR = CAN_RTR_DATA;
  hcan1.pTxMsg->IDE = CAN_ID_STD;
  hcan1.pTxMsg->DLC = 2;

  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler_Can(&hcan1);
  }
  /*##-2- Configure the CAN Filter ###########################################*/
   sFilterConfig.FilterNumber = 1;
   sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
   sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
   sFilterConfig.FilterIdHigh = 0x0000;
   sFilterConfig.FilterIdLow = 0x0000;
   sFilterConfig.FilterMaskIdHigh = 0x0000;
   sFilterConfig.FilterMaskIdLow = 0x0000;
   sFilterConfig.FilterFIFOAssignment = 0;
   sFilterConfig.FilterActivation = ENABLE;
   sFilterConfig.BankNumber = 14;
   if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
   {
     /* Filter configuration Error */
     Error_Handler_Can(&hcan1);
   }
//   uint32_t count = 300;
//   uint16_t ret = OK;
//   while(count--)
   {
//	   ret = HAL_CAN_Transmit(&hcan1, 10);
   }
  // HAL_CAN_Transmit(&hcan1,0);
}
//#define CAN_TEST


void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
#ifndef CAN_TEST
    /**CAN1 GPIO Configuration    
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX 
    */


    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif

	__CAN1_FORCE_RESET();
	__CAN1_RELEASE_RESET();
    /* Peripheral interrupt init */
//   HAL_NVIC_SetPriority(CAN1_TX_IRQn, ISR_PRIORITY_CAN, ISR_SUB_PRIORITY_CAN1_TX);
//   HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, ISR_PRIORITY_CAN, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
//    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, ISR_PRIORITY_CAN, ISR_SUB_PRIORITY_CAN1_RX);
//    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
//    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, ISR_PRIORITY_CAN, ISR_SUB_PRIORITY_CAN1_SCE);
//    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{
	__HAL_CAN_DISABLE_IT(canHandle, 0xFFFFFFFF);
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN1 GPIO Configuration    
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* Peripheral interrupt Deinit*/
 //   HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
//    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);

//    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);

  }
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
void Error_Handler_Can(CAN_HandleTypeDef* hcan)
{
	if(hcan == &hcan1)
	{
		SigPush(errCanTaskHandle, ERR_CAN_1);
	}
}

void StartCanRx(CAN_HandleTypeDef* hcan)
{
	// MX_CAN1_Init();
	if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */
		Error_Handler_Can(hcan);
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
