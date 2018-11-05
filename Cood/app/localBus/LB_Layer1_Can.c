/*
 * LB_Layer1_Can.c
 *
 *  Created on: 2016��8��17��
 *      Author: pli
 */

#include "main.h"

#include "can.h"
#include <string.h>
#include "LB_Layer_data.h"
#include "LB_Layer1_Can.h"
#include "LB_Layer2_Can.h"



static __IO SemaphoreHandle_t lockMsg = NULL;
static SemaphoreHandle_t canLock = NULL;



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
#define MSG_QUEUE_SIZE				16
#define MSG_ID_MSK					(16)
#define MSG_MINUS_NUM				480//(MSG_QUEUE_SIZE*30)

	static __IO uint32_t canMsgId = 0;
	static __IO uint32_t lastId = 0;
	static CanRxMsgTypeDef CanMsgBuffer[MSG_QUEUE_SIZE];

	CanRxMsgTypeDef* ptrMsg = NULL;
	//todo
//	portENTER_CRITICAL();
	if(GET_NEW ==type)
	{
		ptrMsg = &CanMsgBuffer[canMsgId%MSG_ID_MSK];
		canMsgId++;
	}
	else
	{
		if(lastId != canMsgId)
		{
			ptrMsg = &CanMsgBuffer[lastId%MSG_ID_MSK];
			lastId++;
		}
	}
	//todo
//	portEXIT_CRITICAL();
	return ptrMsg;
}

CanTxMsgTypeDef* GetNewCanTxMsgID(uint8_t type)
{
#define MSG_QUEUE_SIZE_TX				(LB_FRAME_BUF_LEN/8+1)

	static __IO uint32_t canMsgId = 0;
	static __IO uint32_t lastId = 0;

	static CanTxMsgTypeDef CanMsgBuffer[MSG_QUEUE_SIZE_TX];

	CanTxMsgTypeDef* ptrMsg = NULL;
	OS_Use(lockMsg);
	if(GET_NEW ==type)
	{
		ptrMsg = &CanMsgBuffer[canMsgId%MSG_QUEUE_SIZE_TX];
		canMsgId++;
	}
	else
	{
		if(lastId != canMsgId)
		{
			ptrMsg = &CanMsgBuffer[lastId%MSG_QUEUE_SIZE_TX];
			lastId++;
		}
	}
	OS_Unuse(lockMsg);
	return ptrMsg;
}



static void PrintCanMsg(const CanTxMsgTypeDef* pTxMsg)
{
	uint8_t buf[30] = {0,0,0,0,0,0,0,0,0,};
	uint32_t len = 0;
	if(pTxMsg->RTR)
	{
		len = 3;
		memcpy(&buf[0], "RTR", 3);
	}
	for(uint8_t idx = 0; idx <pTxMsg->DLC; idx++)
	{
		len += (uint32_t)snprintf((char*)&buf[len], 4, "%02x ", (int)pTxMsg->Data[idx]);
	}

	TracePrint(TSK_ID_LOCAL_BUS, "Can send:\t%x,\t%s\n",  pTxMsg->ExtId, buf);

}






static uint16_t CanTXMsg(CAN_HandleTypeDef* hcan, const CanTxMsgTypeDef *msg)
{
#define TIMEOUT_CAN_TX		10
	uint16_t ret = OK;
	OS_Use(canLock);
	*(hcan->pTxMsg) = *msg;
//	ret = SendCanMsg(hcan, TIMEOUT_CAN_TX);
	ret = HAL_CAN_Transmit(hcan, TIMEOUT_CAN_TX);
//	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
//	trace_printf("send one msg %d\n", ret) ;
	OS_Unuse(canLock);
	PrintCanMsg(msg);
//	shell_AddRTC("can send: %d,\n", HAL_GetTick());
	return ret;

}



static uint32_t SendAvailableCanMsg(void)
{
#define CAN_RETRY_TIME 3
	uint32_t tickOut = osWaitForever;
	static CanTxMsgTypeDef* msg = NULL;
	for(;;)
	{
		if(msg == NULL)
			msg = GetNewCanTxMsgID(GET_EXIST);
		if (msg != NULL)
		{
			uint16_t ret = CanTXMsg(ptrCanDevice, msg);
			if (ret == OK)
			{
				msg = NULL;
			}
			else
			{
				tickOut = CAN_RETRY_TIME;
				break;
			}
		}
		else
		{
			break;
		}
	}
	return tickOut;
}


//handle can;
void StartCanTask(void const * argument)
{
	uint32_t tickOut = osWaitForever;
	uint32_t evtSig;
//	CanTxMsgTypeDef* msg;
	CanRxMsgTypeDef* ptrRxMsg;
//	uint16_t ret = OK;
	(void)argument; // pc lint
	osEvent evt;
	const uint16_t taskID = TSK_ID_CAN_TSK;

	InitCanMsgData();
	//master init
	MX_CAN2_Init();


	while (1)
	{
		freeRtosTskTick[taskID]++;
		evt = osSignalWait(CAN_SIG_INFO, tickOut);
		tickOut = osWaitForever;
		if (evt.status == osEventSignal)
		{
			evtSig = evt.value.v;

			if (evtSig & RX_CAN_BUF2)
			{
				for (;;)
				{
					ptrRxMsg = GetNewCanRxMsgID(GET_EXIST);
					if (ptrRxMsg != NULL)
					{
						(void)LB_Layer2_Can_Rx(ptrRxMsg);
						TracePrint(TSK_ID_LOCAL_BUS,"Can RX Message: %x\n",ptrRxMsg->ExtId);
					}
					else
					{
						break;
					}
				}
			}
			if (evtSig & TX_CAN_BUF2)
			{
				tickOut = SendAvailableCanMsg();
			}
			if(evtSig & ERR_CAN_2)
			{
				uint16_t ret = OK;

				//uint16_t ret = HAL_CAN_DeInit(&hcan2);

				TracePrint(TSK_ID_LOCAL_BUS,"Can error before: %x; MCR:%x;MSR:%x;TSR:%X,IER:%X;ESR:%X;BTR:%X\n",ret,
						hcan2.Instance->MCR,hcan2.Instance->MSR,
						hcan2.Instance->TSR,hcan2.Instance->IER,
						hcan2.Instance->ESR,hcan2.Instance->BTR);
				Error_Handler_Can(&hcan2);
				osDelay(100);
				MX_CAN2_Init();

				TracePrint(TSK_ID_LOCAL_BUS,"Can error reinit: %x; MCR:%x;MSR:%x;TSR:%X,IER:%X;ESR:%X;BTR:%X\n",ret,
						hcan2.Instance->MCR,hcan2.Instance->MSR,
						hcan2.Instance->TSR,hcan2.Instance->IER,
						hcan2.Instance->ESR,hcan2.Instance->BTR);


			}
		}
		//can time out --> handle the TX function
		else
		{
			tickOut = SendAvailableCanMsg();
		}
	}

}



void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_DeInit(hcan);
	if(hcan == &hcan2)
	{
		//HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
		SigPush(tskCan, ERR_CAN_2);
	}
}




void HW_CanRXMsg(CAN_HandleTypeDef* hcan, uint16_t FIFONumber)
{
	CanRxMsgTypeDef* pRxMsg = GetNewCanRxMsgID(GET_NEW);
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
	SigPush(tskCan, RX_CAN_BUF2);
}

