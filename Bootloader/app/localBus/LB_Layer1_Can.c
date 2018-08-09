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


#if 0
static volatile SemaphoreHandle_t lockMsg = NULL;
static SemaphoreHandle_t canLock = NULL;

#endif

static uint32_t errorCountCan = 0;


static void InitCanMsgData(void)
{
#if 0
	if (!lockMsg)
	{
		lockMsg = OS_CreateSemaphore();
	}
	if (!canLock)
	{
		canLock = OS_CreateSemaphore();
	}

#endif

}

static CanRxMsgTypeDef* GetNewCanRxMsgID(uint8_t type)
{
#define MSG_QUEUE_SIZE				16
#define MSG_ID_MSK					(16)
#define MSG_MINUS_NUM				480//(MSG_QUEUE_SIZE*30)

	static volatile uint32_t canMsgId = 0;
	static volatile uint32_t lastId = 0;
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

	static volatile uint32_t canMsgId = 0;
	static volatile uint32_t lastId = 0;

	static CanTxMsgTypeDef CanMsgBuffer[MSG_QUEUE_SIZE_TX];

	CanTxMsgTypeDef* ptrMsg = NULL;
#if 0
	OS_Use(lockMsg);

#endif
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
#if 0
	OS_Unuse(lockMsg);

#endif

	return ptrMsg;
}



static void PrintCanMsg(CanTxMsgTypeDef* pTxMsg)
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
static uint16_t CanTXMsg_HW(CAN_HandleTypeDef* hcan, const CanTxMsgTypeDef *msg)
{
	uint8_t transmitmailbox = 12;

	if ((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
    {
      transmitmailbox = 0U;
    }
    else if ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
    {
      transmitmailbox = 1U;
    }
    else if ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME2)
    {
      transmitmailbox = 2U;
    }
	if(transmitmailbox <= 2)
	{
		*(hcan->pTxMsg) = *msg;
		/* Set up the Id */
		hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
		if (hcan->pTxMsg->IDE == CAN_ID_STD)
		{
		  assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));
		  hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21U) | \
													  hcan->pTxMsg->RTR);
		}
		else
		{
		  assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
		  hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3U) | \
													  hcan->pTxMsg->IDE | \
													  hcan->pTxMsg->RTR);
		}

		/* Set up the DLC */
		hcan->pTxMsg->DLC &= (uint8_t)0x0000000FU;
		hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0U;
		hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

		/* Set up the data field */
		hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3U] << 24U) |
												 ((uint32_t)hcan->pTxMsg->Data[2U] << 16U) |
												 ((uint32_t)hcan->pTxMsg->Data[1U] << 8U) |
												 ((uint32_t)hcan->pTxMsg->Data[0U]));
		hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7U] << 24U) |
												 ((uint32_t)hcan->pTxMsg->Data[6U] << 16U) |
												 ((uint32_t)hcan->pTxMsg->Data[5U] << 8U) |
												 ((uint32_t)hcan->pTxMsg->Data[4U]));
		/* Request transmission */
		hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;
    }

	return transmitmailbox;
}






void SendAvailCanMsgTick(void)
{
//	for(uint8_t idx=0;idx<2;idx++)
	{
		CanTxMsgTypeDef* msg = GetNewCanTxMsgID(GET_EXIST);
		if (msg != NULL)
		{
			CanTXMsg_HW(ptrCanDevice, msg);
		}
		else
		{
			communicationState &= ~TX_STATE;
		}
	}
}





uint32_t communicationState = 1;
__IO uint16_t resetRequest = 0;
MsgFrame* ptrHandleRxMsg = NULL;
MsgFrame* ptrHandleTxMsg = NULL;
//handle can;
void HandleCanMsg_Bootloader(uint32_t classState)
{

	if(classState == RX_HANDLE)
	{
		uint32_t state = communicationState & RX_HANDLE;


		if(state)
		{
			LB_Layer7_Decode(ptrHandleRxMsg);
			communicationState &= ~RX_HANDLE;
		//	trace_printf("RX_HANDLE; one: %d\n", HAL_GetTick());
		}
	}
	else
	{
		uint32_t state = communicationState & TX_HANDLE;
		if(state)
		{
			LB_Layer2_Tx(ptrHandleTxMsg);
			communicationState &= ~TX_HANDLE;
			trace_printf("TX_HANDLE; one: %d\n", HAL_GetTick());

		}


	}
	if(resetRequest >= 0x55)
	{
		__disable_irq();
		NVIC_SystemReset();
	}


}

void NotifyLB(MsgFrame* frames, uint32_t state)
{
	if(state == TX_HANDLE)
	{
		ptrHandleTxMsg = frames;
		communicationState |= TX_HANDLE;
	}
	else
	{
		ptrHandleRxMsg = frames;
		communicationState |= RX_HANDLE;
	}
	StartTimer();
}

#if 0

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


	InitCanMsgData();
	//master init
	MX_CAN2_Init();


	while (1)
	{
		evt = osSignalWait(CAN_SIG_INFO, tickOut);
		tickOut = osWaitForever;
		if (evt.status == osEventSignal)
		{
			evtSig = evt.value.v;

			if (evtSig & RX_CAN_BUF1)
			{
				for (;;)
				{
					ptrRxMsg = GetNewCanRxMsgID(GET_EXIST);
					if (ptrRxMsg != NULL)
					{
						//todo
						//handle the rx msg
						(void)LB_Layer2_Can_Rx(ptrRxMsg);
						TracePrint(TSK_ID_LOCAL_BUS,"Can RX Message: %x\n",ptrRxMsg->ExtId);
					}
					else
					{
						break;
					}
				}
			}
			if (evtSig & TX_CAN_BUF1)
			{
				tickOut = SendAvailableCanMsg();
			}
			if(evtSig & ERR_CAN_1)
			{
				uint16_t ret = OK;

				//uint16_t ret = HAL_CAN_DeInit(&hcan2);

				//TracePrint(TSK_ID_LOCAL_BUS,"Can error before: %x; MCR:%x;MSR:%x;TSR:%X,IER:%X;ESR:%X;BTR:%X\n",ret,
				//		hcan2.Instance->MCR,hcan2.Instance->MSR,
				//		hcan2.Instance->TSR,hcan2.Instance->IER,
				//		hcan2.Instance->ESR,hcan2.Instance->BTR);
				Error_Handler_Can(&hcan2);
				MX_CAN2_Init();

				//TracePrint(TSK_ID_LOCAL_BUS,"Can error reinit: %x; MCR:%x;MSR:%x;TSR:%X,IER:%X;ESR:%X;BTR:%X\n",ret,
				//		hcan2.Instance->MCR,hcan2.Instance->MSR,
				//		hcan2.Instance->TSR,hcan2.Instance->IER,
				//		hcan2.Instance->ESR,hcan2.Instance->BTR);


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

	//HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
	SigPush(tskCan, ERR_CAN_1);
}


#endif



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
//	communicationState |= RX_STATE;
	(void)LB_Layer2_Can_Rx(pRxMsg);
	TracePrint(TSK_ID_LOCAL_BUS,"Can RX Message: %x\n",pRxMsg->ExtId);
//	SigPush(tskCan, RX_CAN_BUF1);
}

