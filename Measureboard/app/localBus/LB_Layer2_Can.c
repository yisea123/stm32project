/*
 * LB_Layer2_Can.c
 *
 *  Created on: 2017骞�4鏈�14鏃�
 *      Author: pli
 */

#include <string.h>
#include "main.h"
#include "can.h"
#include "LB_Layer_data.h"
#include "LB_Layer1_Can.h"
#include "LB_Layer2_Can.h"
static FrameHandler		layer2Rx = NULL;

#define CMDTP_BIT_N		26
#define FMTP_BIT_N		24
#define EXTID_BIT_N		16
#define SRC_BIT_N		8
#define DST_BIT_N		0

static uint16_t ConvertToHead(CanHead* ptrHead, CanRxMsgTypeDef* rxMsg)
{
	uint16_t ret= FATAL_ERROR;
	assert(ptrHead);
	assert(rxMsg);

	if(rxMsg->IDE == CAN_ID_STD)
	{
		TraceDBG(TSK_ID_LOCAL_BUS,"Can Rx Message ID error: %x,%x: len: %d; %x %x %x %x, %x %x %x %x\n",\
									rxMsg->ExtId, rxMsg->StdId,rxMsg->DLC,rxMsg->Data[0],rxMsg->Data[1],
									rxMsg->Data[2],rxMsg->Data[3],rxMsg->Data[4],rxMsg->Data[5],\
									rxMsg->Data[6],rxMsg->Data[7]);
		ret = FATAL_ERROR;
	}
	else
	{
		uint32_t head = rxMsg->ExtId;
		ptrHead->dst = (uint8_t)(head>>DST_BIT_N);
		ptrHead->src = (uint8_t)(head>>SRC_BIT_N);
		ptrHead->exId = (uint8_t)(head>>EXTID_BIT_N);
		ptrHead->frameType =(uint8_t) ((head>>FMTP_BIT_N) & 0x03);
		ret = OK;
	}
	return ret;
}

static uint16_t ConvertToTxHead(CanTxMsgTypeDef* txMsg, Layer2Frame* data, uint16_t id, uint16_t frameType)
{
	uint16_t ret= FATAL_ERROR;
	assert(data);
	assert(txMsg);
	id = id & 0xFF;
	frameType &= 0x03;

	txMsg->IDE = CAN_ID_EXT;
	txMsg->ExtId =  (uint32_t)((data->St.dst<<DST_BIT_N) | \
					(data->St.src<<SRC_BIT_N) | \
					(frameType<<FMTP_BIT_N) | \
					(id<<EXTID_BIT_N));
	return ret;
}

uint16_t LB_Layer2_Can_Init(FrameHandler callBack)
{
	uint16_t ret = OK;

	layer2Rx = callBack;
	return ret;
}


static uint16_t HandleRxFrames(CanHead* ptrHead, CanRxMsgTypeDef* rxMsg )
{
	MsgFrame* msgFframe = NULL;
	uint16_t ret = OK;

	TracePrint(TSK_ID_LOCAL_BUS,"Can Rx Message: %x: len: %d; %x %x %x %x, %x %x %x %x\n",\
					rxMsg->ExtId, rxMsg->DLC,rxMsg->Data[0],rxMsg->Data[1],
					rxMsg->Data[2],rxMsg->Data[3],rxMsg->Data[4],rxMsg->Data[5],\
					rxMsg->Data[6],rxMsg->Data[7]);

	if(ptrHead->frameType == MULTI_NO)
	{
		msgFframe = GetRightFrameBuff(ptrHead);
		if(msgFframe)
			memcpy(msgFframe->frame.data,rxMsg->Data,rxMsg->DLC);
		ret = OK;
	}
	else
	{
		ret = WARNING;
		msgFframe = GetRightFrameBuff(ptrHead);
		if(msgFframe)
		{
			memcpy(&msgFframe->frame.data[ptrHead->exId*8],rxMsg->Data,rxMsg->DLC);
		}
		if(ptrHead->frameType == MULTI_END)
		{
			ret = OK;
		}
	}
	if((ret==OK) && msgFframe)
	{
		msgFframe->typeMsg = FROM_CAN;
		if(layer2Rx)
			(*layer2Rx)(msgFframe);
	}
	return ret;
}


uint16_t LB_Layer2_Can_Rx(CanRxMsgTypeDef* rxMsg)
{
	uint16_t ret = OK;

	CanHead curCanHead;
	ret = ConvertToHead(&curCanHead, rxMsg);
	if(ret == OK)
	{
		ret = HandleRxFrames(&curCanHead,rxMsg);
	}
	return ret;
}

uint16_t LB_Layer2_Can_TxRaw(const uint8_t* data, uint16_t len)
{
	assert(data);
	for(uint16_t idx = 0; idx < len; )
	{
		CanTxMsgTypeDef* txMsg = GetNewCanTxMsgID(GET_NEW);
		assert(txMsg);
		txMsg->IDE = CAN_ID_EXT;
		txMsg->ExtId = 0x1FFFFFFFU;

		txMsg->DLC = (uint16_t)(len-idx)%8;
		if(txMsg->DLC)
			memcpy(txMsg->Data, &data[idx], txMsg->DLC);

		idx= (uint16_t)(idx + 8);
	}
	SigPush(tskCan, TX_CAN_BUF2);
	return OK;
}


uint16_t LB_Layer2_Can_Tx(MsgFrame* ptrMsgFrame)
{
	uint16_t ret = OK;
	assert(ptrMsgFrame);



	Layer2Frame* ptrFrame = &ptrMsgFrame->frame;
	assert(ptrFrame);


	uint16_t len = GetFrameLen(ptrFrame);

	if( len <= 8)
	{

		CanTxMsgTypeDef* txMsg = GetNewCanTxMsgID(GET_NEW);
		assert(txMsg);

		ConvertToTxHead( txMsg,ptrFrame,0x0, MULTI_NO);
		txMsg->DLC = len;
		memcpy(txMsg->Data, ptrFrame->data, txMsg->DLC);
		TracePrint(TSK_ID_LOCAL_BUS,"Can Tx Message: %x: len: %d; %x %x %x %x, %x %x %x %x\n",\
				txMsg->ExtId, txMsg->DLC,txMsg->Data[0],txMsg->Data[1],
				txMsg->Data[2],txMsg->Data[3],txMsg->Data[4],txMsg->Data[5],\
				txMsg->Data[6],txMsg->Data[7]);
	}
	else
	{
		//multi frames:
		uint16_t idx = 0;
		uint16_t exId = 0;

		CanTxMsgTypeDef* txMsg = GetNewCanTxMsgID(GET_NEW);
		assert(txMsg);
		ConvertToTxHead( txMsg,ptrFrame,exId, MULTI_ST);
		txMsg->DLC = 8;
		memcpy(txMsg->Data, &ptrFrame->data[idx], txMsg->DLC);
		idx = (uint16_t)(idx + 8);
		exId++;

		TracePrint(TSK_ID_LOCAL_BUS,"Can Tx Message: %x: len: %d; %x %x %x %x, %x %x %x %x\n",\
						txMsg->ExtId, txMsg->DLC,txMsg->Data[0],txMsg->Data[1],
						txMsg->Data[2],txMsg->Data[3],txMsg->Data[4],txMsg->Data[5],\
						txMsg->Data[6],txMsg->Data[7]);

		while(idx < (len - 8))
		{
			txMsg = GetNewCanTxMsgID((uint8_t)GET_NEW);
			assert(txMsg);
			ConvertToTxHead( txMsg,ptrFrame,exId, MULTI_ING);
			txMsg->DLC = 8;
			memcpy(txMsg->Data, &ptrFrame->data[idx], txMsg->DLC);
			exId++;
			idx = (uint16_t)(idx + 8);

			TracePrint(TSK_ID_LOCAL_BUS,"Can Tx Message: %x: len: %d; %x %x %x %x, %x %x %x %x\n",\
							txMsg->ExtId, txMsg->DLC,txMsg->Data[0],txMsg->Data[1],
							txMsg->Data[2],txMsg->Data[3],txMsg->Data[4],txMsg->Data[5],\
							txMsg->Data[6],txMsg->Data[7]);
		}
		txMsg = GetNewCanTxMsgID((uint8_t)GET_NEW);
		assert(txMsg);
		txMsg->DLC =  (uint8_t)(len - idx);
		ConvertToTxHead( txMsg,ptrFrame,exId, MULTI_END);
		memcpy(txMsg->Data, &ptrFrame->data[idx], txMsg->DLC);

		TracePrint(TSK_ID_LOCAL_BUS,"Can Tx Message: %x: len: %d; %x %x %x %x, %x %x %x %x\n",\
						txMsg->ExtId, txMsg->DLC,txMsg->Data[0],txMsg->Data[1],
						txMsg->Data[2],txMsg->Data[3],txMsg->Data[4],txMsg->Data[5],\
						txMsg->Data[6],txMsg->Data[7]);
	}
	SigPush(tskCan, TX_CAN_BUF2);

	return ret;
}

