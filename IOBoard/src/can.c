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

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

#if 0
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
  hcan1.Init.ABOM = DISABLE;
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

  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler_Can(&hcan);
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
   if(HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
   {
     /* Filter configuration Error */
     Error_Handler_Can(&hcan);
   }
#endif

	MX_CAN_Open_Init(125);
}



void MX_CAN_Open_Init(uint16_t CANbitRate)
{
	static CanRxMsgTypeDef rxMsg;
	static CanTxMsgTypeDef txMsg;
		CAN_FilterConfTypeDef sFilterConfig;
		hcan.Instance = CAN;
	txMsg.DLC = 8;
	txMsg.StdId = 0x321;
	txMsg.IDE = CAN_ID_STD;
	hcan.pTxMsg = &txMsg;
	hcan.pRxMsg = &rxMsg;	
		hcan.Init.Prescaler = 48;
	switch (CANbitRate)
	{
		case 1000:
			hcan.Init.Prescaler = 6;
			break;
		case 500:
			hcan.Init.Prescaler = 12;
			break;
		default:
		case 250:
			hcan.Init.Prescaler = 24;
			break;
		case 125:
			hcan.Init.Prescaler = 48;
			break;
		case 100:
			hcan.Init.Prescaler = 60;
			break;
		case 50:
			hcan.Init.Prescaler = 120;
			break;
		case 20:
			hcan.Init.Prescaler = 300;
			break;
		case 10:
			hcan.Init.Prescaler = 600;
			break;
	}
	//hcan.Init.Prescaler = 48;
	hcan.Init.Mode = CAN_MODE_NORMAL;

	hcan.Init.SJW = CAN_SJW_1TQ;
	hcan.Init.BS1 = CAN_BS1_3TQ;
	hcan.Init.BS2 = CAN_BS2_4TQ;
	hcan.Init.TTCM = DISABLE;
	hcan.Init.ABOM = ENABLE;
	hcan.Init.AWUM = DISABLE;
	hcan.Init.NART = DISABLE;
	hcan.Init.RFLM = DISABLE;
	hcan.Init.TXFP = DISABLE;
	//45M
	if (HAL_CAN_Init(&hcan) != HAL_OK)
	{
		Error_Handler_Can(&hcan);
	}

	/*##-2- Configure the CAN Filter ###########################################*/
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
	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		Error_Handler_Can(&hcan);
	}


	StartCanRx(&hcan);


}

void SendData_Can(uint8_t* data, uint16_t len)
{
	for(uint16_t i=0;i< len; i+=8)
	{
		hcan.pTxMsg->StdId = 0;
		hcan.pTxMsg->ExtId = 0;
		hcan.pTxMsg->RTR = CAN_RTR_DATA;
		hcan.pTxMsg->IDE = CAN_ID_STD;
		hcan.pTxMsg->DLC = 8;
		memcpy(hcan.pTxMsg->Data, &data[i], 8);
		HAL_CAN_Transmit(&hcan, 10);
		osDelay(3);
	}
}
void Test()
{
	   uint32_t count = 100;
	   uint16_t ret = 0;
	   while(count--)
	   {
		   ret = HAL_CAN_Transmit(&hcan, 0x15);
	   }
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspInit 0 */

  /* USER CODE END CAN_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(CEC_CAN_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
		/* USER CODE BEGIN CAN_MspInit 1 */

		/* USER CODE END CAN_MspInit 1 */
	}
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

	if (canHandle->Instance == CAN)
	{
		/* USER CODE BEGIN CAN_MspDeInit 0 */

  /* USER CODE END CAN_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);

  }
  /* USER CODE BEGIN CAN_MspDeInit 1 */

  /* USER CODE END CAN_MspDeInit 1 */
} 

void StartCanRx(CAN_HandleTypeDef* hcan1)
{
	if (HAL_CAN_Receive_IT(hcan1, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */

		Error_Handler_Can(hcan1);
	}
}

void Error_Handler_Can(CAN_HandleTypeDef* hcan)
{
		__CAN_FORCE_RESET();
		__CAN_RELEASE_RESET();
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
