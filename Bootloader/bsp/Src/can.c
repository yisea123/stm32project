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
#include "mxconstants.h"
#include "stm32f4xx_it.h"
/* USER CODE BEGIN 0 */
#include "main.h"

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void)
{
	CAN_FilterConfTypeDef sFilterConfig;
	static CanTxMsgTypeDef txMsg_Can1;
	static CanRxMsgTypeDef rxMsg_Can1;
	//1m hz
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 12;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_10TQ;
  hcan1.Init.BS2 = CAN_BS2_4TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = ENABLE;
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
  hcan1.pTxMsg = &txMsg_Can1;
  hcan1.pRxMsg = &rxMsg_Can1;

  hcan1.pTxMsg->StdId = 0x321;
  hcan1.pTxMsg->ExtId = 0x00;
  hcan1.pTxMsg->RTR = CAN_RTR_DATA;
  hcan1.pTxMsg->IDE = CAN_ID_STD;
  hcan1.pTxMsg->DLC = 2;

  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler_Can(&hcan1);
  }
  /*##-2- Configure the CAN Filter ###########################################*/
  //lint -e539
   sFilterConfig.FilterNumber = 0;
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

}

/* CAN2 init function */
void MX_CAN2_Init(void)
{
	static CanTxMsgTypeDef txMsg_Can2;
	static CanRxMsgTypeDef rxMsg_Can2;
	CAN_FilterConfTypeDef sFilterConfig;
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 3;//24
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SJW = CAN_SJW_1TQ;
  hcan2.Init.BS1 = CAN_BS1_10TQ;
  hcan2.Init.BS2 = CAN_BS2_4TQ;


  hcan2.Init.TTCM = DISABLE;
  hcan2.Init.ABOM = ENABLE;
  hcan2.Init.AWUM = DISABLE;
  hcan2.Init.NART = DISABLE;
  hcan2.Init.RFLM = DISABLE;
  hcan2.Init.TXFP = DISABLE;
  hcan2.pTxMsg = &txMsg_Can2;
  hcan2.pRxMsg = &rxMsg_Can2;
  hcan2.pTxMsg->StdId = 0x321;
  hcan2.pTxMsg->ExtId = 0x01;
  hcan2.pTxMsg->RTR = CAN_RTR_DATA;
  hcan2.pTxMsg->IDE = CAN_ID_STD;
  hcan2.pTxMsg->DLC = 4;
  hcan2.pTxMsg->Data[0] = 0xFF;
  hcan2.pTxMsg->Data[1] = 0xFF;

  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler_Can(&hcan2);
  }
  /*##-2- Configure the CAN Filter ###########################################*/
  //lint -e539
  //trick point about filter 14 which is used by can2 but occupied by can1;
  sFilterConfig.FilterNumber = 14;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
//note Can1
   if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
   {
     /* Filter configuration Error */
     Error_Handler_Can(&hcan2);
   }
   StartCanRx(&hcan2);
//#define TEST_CAN2
#ifdef TEST_CAN2
   uint16_t ret = 0;
   uint16_t count = 2;
   while(count--)
   {
	   ret = HAL_CAN_Transmit(&hcan2, 0x05);
   }
 //  StartCanRx(&hcan2);
#endif

}

static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;


void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* Peripheral clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }
  
    /**CAN1 GPIO Configuration    

    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX 
    */
    GPIO_InitStruct.Pin = CAN1_RX_Pin|CAN1_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, ISR_PRIORITY_CAN, ISR_SUB_PRIORITY_CAN1_TX);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, ISR_PRIORITY_CAN, ISR_SUB_PRIORITY_CAN1_RX);
   /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }
  
    /**CAN2 GPIO Configuration    
    PB13     ------> CAN2_TX
    PB5     ------> CAN2_RX 
    */
    GPIO_InitStruct.Pin = CAN2_TX_Pin|CAN2_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__CAN2_FORCE_RESET();
	__CAN2_RELEASE_RESET();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, ISR_PRIORITY_CAN2, ISR_SUB_PRIORITY_CAN2_RX);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN2_SCE_IRQn, ISR_PRIORITY_CAN2, ISR_SUB_PRIORITY_CAN2_SCE);
    HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
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
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }
  
    /**CAN1 GPIO Configuration    
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, CAN1_RX_Pin|CAN1_TX_Pin);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }
  
    /**CAN2 GPIO Configuration    
    PB13     ------> CAN2_TX
    PB5     ------> CAN2_RX 
    */
    HAL_GPIO_DeInit(GPIOB, CAN2_TX_Pin|CAN2_RX_Pin);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);

    HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);

  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
} 
/* USER CODE BEGIN 1 */
void Error_Handler_Can(CAN_HandleTypeDef* hcan)
{
	if(hcan == &hcan1)
	{
		__CAN1_FORCE_RESET();
		__CAN1_RELEASE_RESET();
	}
	else
	{
		__CAN2_FORCE_RESET();
		__CAN2_RELEASE_RESET();
		//SigPush(tskCan, ERR_CAN_2);
	}
	TraceDBG(0xFFFF,"Can interface Error: %x \n", hcan);
}

void StartCanRx(CAN_HandleTypeDef* hcan)
{
	if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */

		Error_Handler_Can(hcan);
	}
	HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
}



/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
