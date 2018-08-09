/*
 * dev_can.c
 *
 *  Created on: 2016��8��17��
 *      Author: pli
 */
#include "main.h"
#include "shell_io.h"

#include "can.h"
#include <string.h>


#include "dev_can.h"
#include "unit_parameter.h"
#include "tim.h"
uint32_t txCanCount1 = 0;
uint32_t txCanCount2 = 0;

extern TIM_HandleTypeDef htim6;

#define TXMSG_QUEUE_RETRY		(TXMSG_QUEUE_SIZE+10)

static volatile SemaphoreHandle_t lockMsg = NULL;
static SemaphoreHandle_t canLock = NULL;






static uint32_t errorCountCan = 0;


static void InitCanMsgData(void)
{
	if (!lockMsg)
	{
		lockMsg = OS_CreateSemaphore();
	}
	if (!canLock)
	{
		canLock = OS_CreateSemaphore();
	}
}

static CanRxMsgTypeDef* GetNewCanRxMsgID(uint8_t type)
{
#define MSG_QUEUE_SIZE				32
#define MSG_ID_MSK					(0x1F)


	static uint32_t canMsgId = 0;
	static uint32_t lastId = 0;
	static CanRxMsgTypeDef CanMsgBuffer[MSG_QUEUE_SIZE];
	CanRxMsgTypeDef* ptrMsg = NULL;
	//todo
	//portENTER_CRITICAL();
	if(0 ==type)
	{
		ptrMsg = &CanMsgBuffer[(canMsgId++)&MSG_ID_MSK];
	}
	else
	{
		if(lastId != canMsgId)
		{
			ptrMsg = &CanMsgBuffer[(lastId++)&MSG_ID_MSK];
		}
	}
	//todo
	//portEXIT_CRITICAL();
	return ptrMsg;
}

static CanTxMsgTypeDef* GetNewCanTxMsgID(uint8_t type)
{
#define MSG_QUEUE_SIZE				16
#define MSG_ID_MSK					(0x0F)


	static uint32_t canMsgId = 0;
	static uint32_t lastId = 0;
	static CanTxMsgTypeDef CanMsgBuffer[MSG_QUEUE_SIZE];
	CanTxMsgTypeDef* ptrMsg = NULL;
//	OS_Use(lockMsg);
	if(0 ==type)
	{
		ptrMsg = &CanMsgBuffer[(canMsgId++)&MSG_ID_MSK];
	}
	else
	{
		if(lastId != canMsgId)
		{
			ptrMsg = &CanMsgBuffer[(lastId++)&MSG_ID_MSK];
		}
	}
//	OS_Unuse(lockMsg);
	return ptrMsg;
}



static void PrintCanMsg(CanTxMsgTypeDef* pTxMsg)
{
	uint8_t buf[30] = {0,0,0,0,0,0,0,0,0,};
	uint8_t len = 0;
	if(pTxMsg->RTR)
	{
		len = 3;
		memcpy(&buf[0], "RTR", 3);
	}
	for(uint8_t idx = 0; idx <pTxMsg->DLC; idx++)
	{
		/*lint -e586*/
		len += snprintf((char*)&buf[len], 4, "%02x ", pTxMsg->Data[idx]);
	}
#ifdef MASTER
	shell_AddRTC("master send:\t%04x,\t%s\n",  pTxMsg->StdId, buf);
#endif
#ifdef SLAVE
// SLAVE
	shell_AddRTC("slave send:\t%04x,\t%s\n",  pTxMsg->StdId,  buf);
#endif
}

static void SendCanMsg_BSP(CAN_HandleTypeDef* hcan, uint32_t transmitmailbox)
 {
	hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
	if (hcan->pTxMsg->IDE == CAN_ID_STD)
	{
	//	assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));
		hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId
				<< 21U) | hcan->pTxMsg->RTR);
	}
	else
	{
	//	assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
		hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId
				<< 3U) | hcan->pTxMsg->IDE | hcan->pTxMsg->RTR);
	}

	/* Set up the DLC */
	hcan->pTxMsg->DLC &= (uint8_t) 0x0000000FU;
	hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t) 0xFFFFFFF0U;
	hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

	/* Set up the data field */
	hcan->Instance->sTxMailBox[transmitmailbox].TDLR =
			(((uint32_t) hcan->pTxMsg->Data[3U] << 24U)
					| ((uint32_t) hcan->pTxMsg->Data[2U] << 16U)
					| ((uint32_t) hcan->pTxMsg->Data[1U] << 8U)
					| ((uint32_t) hcan->pTxMsg->Data[0U]));
	hcan->Instance->sTxMailBox[transmitmailbox].TDHR =
			(((uint32_t) hcan->pTxMsg->Data[7U] << 24U)
					| ((uint32_t) hcan->pTxMsg->Data[6U] << 16U)
					| ((uint32_t) hcan->pTxMsg->Data[5U] << 8U)
					| ((uint32_t) hcan->pTxMsg->Data[4U]));
	/* Request transmission */
	hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;
}

//not called
static uint16_t SendCanMsg(CAN_HandleTypeDef* hcan, uint32_t timeOut)
{
	uint16_t ret = FATAL_ERROR;
	uint16_t reTry = 0;
	PrintCanMsg(hcan->pTxMsg);
	while(ret != OK)
	{
		 ret = OK;
		 if((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
		 {
			 SendCanMsg_BSP(hcan, 0);
		 }
		 else if((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
		 {
			 SendCanMsg_BSP(hcan, 1);
		 }
		 else if((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
		 {
			 SendCanMsg_BSP(hcan, 2);
		 }
		 else
		 {
			 ret = FATAL_ERROR;
		 }
		 if( ret != OK)
		 {
			 osDelay(2);
			 reTry++;

			 if(reTry > 10)
			 {
				 txCanCount2++;
				 break;
			 }
		 }
	}
	return ret;
}


//not called
static uint16_t CanTXMsg(CAN_HandleTypeDef* hcan, const CanTxMsgTypeDef *msg)
{
#define TIMEOUT_CAN_TX		10
	uint16_t ret = OK;
	OS_Use(canLock);
	*(hcan->pTxMsg) = *msg;
	txCanCount1++;
	ret = SendCanMsg(hcan, TIMEOUT_CAN_TX);
	OS_Unuse(canLock);
//	shell_AddRTC("can send: %d,\n", HAL_GetTick());
	return ret;

}


#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Waggregate-return"

uint32_t rxHandleCount = 0;
//handle the new received msg;
void StartCanRXTask(void const * argument)
{
	osEvent event;
	CanRxMsgTypeDef* msg;
	uint32_t lastId = 0;
	uint32_t newId = 0;
	uint32_t tickOut = osWaitForever;

	InitCanMsgData();
	//master init
	StartCanRx(&hcan1);
	//for arclink
	InitCan_GValue();
	CheckSimuAction();
	while (1)
	{
		event = osSignalWait(RX_CAN_BUF1, tickOut);
		for(;;)
		{

			msg = GetNewCanRxMsgID(1);
			if(msg != NULL)
			{
				rxHandleCount++;
				CanProcess(msg);
			}
			else
			{
				break;
			}
		}
	}

}

static uint16_t HW_SendCanMsg(CAN_HandleTypeDef* hcan, uint32_t timeOut)
{
	uint16_t ret = FATAL_ERROR;
	uint16_t reTry = 0;
	PrintCanMsg(hcan->pTxMsg);
	while(ret != OK)
	{
		 ret = OK;
		 if((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
		 {
			 SendCanMsg_BSP(hcan, 0);
		 }
		 else if((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
		 {
			 SendCanMsg_BSP(hcan, 1);
		 }
		 else if((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
		 {
			 SendCanMsg_BSP(hcan, 2);
		 }
		 else
		 {
			 ret = FATAL_ERROR;
		 }
		 if( ret != OK)
		 {
			 osDelay(2);
			 reTry++;

			 if(reTry > 10)
			 {
				 ret = OK;
				 txCanCount2++;
				 break;
			 }
		 }
	}
	return ret;
}


static uint16_t HW_CanTXMsg(CAN_HandleTypeDef* hcan, const CanTxMsgTypeDef *msg)
{
#define TIMEOUT_CAN_TX		10
	uint16_t ret = OK;
//	OS_Use(canLock);
	*(hcan->pTxMsg) = *msg;
	txCanCount1++;
	ret = HW_SendCanMsg(hcan, TIMEOUT_CAN_TX);
//	OS_Unuse(canLock);
//	shell_AddRTC("can send: %d,\n", HAL_GetTick());
	return ret;

}


//handle the async tx msg;
void StartCanTXTask(void const * argument)
{
	uint32_t 			tickOut = osWaitForever;
	CanTxMsgTypeDef* 	msg;
	uint16_t 			ret = OK;
	CanTxMsgTypeDef 	txmsg;
	txmsg.DLC = 8;
	txmsg.RTR=CAN_RTR_DATA;
	txmsg.IDE= CAN_ID_EXT;
	txmsg.ExtId = 0x1234;
	#define CAN_RETRY_TIME		5
	while (1)
	{
		//tickOut = 10;
		osSignalWait(TX_CAN_BUF1, tickOut);
		//tickOut = 1000;

		//for(;;)
		{
			//no matter is new message or is timeout;
			if(ret == OK)
				msg = GetNewCanTxMsgID(1);
			msg = &txmsg;

			if (msg == NULL)
			{
				ret = OK;
				break;
			}
			else
			{
				hcan1.pTxMsg = &txmsg;
				//ret = HAL_CAN_Transmit(&hcan1, 10);
				ret = HW_CanTXMsg(&hcan1, msg);
				if(ret == OK)
				{
				}
				else
				{
					tickOut = CAN_RETRY_TIME;
					break;
				}
			}
		}
	}

}




void StartCanErrTask(void const * argument)
{
	osEvent event;
	uint32_t tickOut = osWaitForever;

	while (1)
	{

		event = osSignalWait(CAN_ERR_ALL, tickOut);
		if (event.status == osEventSignal)
		{
			if ((event.value.signals & ERR_CAN_1) == ERR_CAN_1)
			{


				HAL_CAN_DeInit(&hcan1);
				MX_CAN1_Init();
				osDelay(10);
				StartCanRx(&hcan1);
				osDelay(100);
				errorCountCan++;
			}
		}
	}

}

#pragma GCC diagnostic pop


void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_DeInit(hcan);
	if (hcan == &hcan1)
	{
		HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
		HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
		HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
		SigPush(errCanTaskHandle, ERR_CAN_1);
	}
	else
	{
		HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
		HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
		HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
		SigPush(errCanTaskHandle, ERR_CAN_2);
	}

}
//sw interface

void TSK_CAN_Transmit(const CanTxMsgTypeDef* sendmsg)
{
//	CanTxMsgTypeDef* msg = GetNewCanTxMsgID(0);
//	*msg = *sendmsg;
//	SignalPush(txCan, TX_CAN_BUF1);
}

void TSK_CAN_Transmit1(const CanTxMsgTypeDef* sendmsg)
{
	CanTxMsgTypeDef* msg = GetNewCanTxMsgID(0);
	*msg = *sendmsg;
	SignalPush(txCan, TX_CAN_BUF1);
}


static void SimuData(CanRxMsgTypeDef* pRxMsg)
{
	static uint32_t count = 0;
	static uint32_t rxCount = 0;
	static uint32_t delayCount = 0;
	if(parameterDynamic.simulateSave[0])
	{
		if(delayCount==0)
		{
			count++;

			if(count%4 == 1)
			{
				pRxMsg->ExtId = 0x1F203140;
				pRxMsg->IDE = CAN_ID_EXT;
				pRxMsg->RTR = CAN_RTR_DATA;
				pRxMsg->DLC = 8;
				//02 00 00 07 08 F0 01 FE
				pRxMsg->Data[0] = 0x02;
				pRxMsg->Data[1] = 0x00;
				pRxMsg->Data[2] = 0x00;
				pRxMsg->Data[3] = 0x07;
				pRxMsg->Data[4] = 0x02;
				pRxMsg->Data[5] = (count%200)&0xFF;
				pRxMsg->Data[6] = 0x01;
				pRxMsg->Data[7] = 0xFE;
			}
			else if(count%4 == 3)
			{
				pRxMsg->ExtId = 0x1F2031C8;//0x1F203140;
				pRxMsg->IDE = CAN_ID_EXT;
				pRxMsg->RTR = CAN_RTR_DATA;
				pRxMsg->DLC = 4;
				//38 93 1F FE
				pRxMsg->Data[0] = ((count+100)%200)&0xFF;
				pRxMsg->Data[1] = 0x93;
				pRxMsg->Data[2] = 0x1F;
				pRxMsg->Data[3] = 0xFE;
				rxCount++;
			}
			else
			{}
			if((rxCount / 100)>=parameterDynamic.simulateSave[1])
			{
				rxCount = 0;
				parameterDynamic.simulateSave[0]--;
				delayCount = 0x600;
				if(parameterDynamic.simulateSave[0] == 0)
				{
					HAL_TIM_Base_Stop_IT(&htim6);
				}
			}
		}
		else
		{
			delayCount--;
		}
	}
}




void SimuCan(void)
{
	HW_CanRXMsg(&hcan1, 0);

}



//low level interface;

uint32_t rxCanCount = 0;


void HW_CanRXMsg(CAN_HandleTypeDef* hcan, uint16_t FIFONumber)
{
	CanRxMsgTypeDef* pRxMsg = GetNewCanRxMsgID(0);
	/* USER CODE END CAN1_RX0_IRQn 0 */
	pRxMsg->IDE = (uint8_t)0x04U & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
	if (pRxMsg->IDE == CAN_ID_STD)
	{
		pRxMsg->StdId = (uint32_t)0x000007FFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 21U);
	}
	else
	{
		pRxMsg->ExtId = (uint32_t)0x1FFFFFFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 3U);
	}

	pRxMsg->RTR = (uint8_t)0x02U & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
	/* Get the DLC */
	pRxMsg->DLC = (uint8_t)0x0FU & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR;
	/* Get the FMI */
	pRxMsg->FMI = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDTR >> 8U);
	/* Get the data field */
	pRxMsg->Data[0U] = (uint8_t)0xFFU & hcan->Instance->sFIFOMailBox[FIFONumber].RDLR;
	pRxMsg->Data[1U] = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 8U);
	pRxMsg->Data[2U] = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 16U);
	pRxMsg->Data[3U] = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 24U);

	pRxMsg->Data[4U] = (uint8_t)0xFFU & hcan->Instance->sFIFOMailBox[FIFONumber].RDHR;
	pRxMsg->Data[5U] = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 8U);
	pRxMsg->Data[6U] = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 16U);
	pRxMsg->Data[7U] = (uint8_t)0xFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 24U);
	__HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO0);
	//CanProcess(pRxMsg);
	SimuData(pRxMsg);
	SignalPush(rxCan, RX_CAN_BUF1);
	rxCanCount++;
}



