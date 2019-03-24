/*
 * LB_Layer2_Can.c
 *
 *  Created on: 2017骞�5鏈�19鏃�
 *      Author: pli
 */

#include <string.h>
#include "main.h"
#include "usart.h"

#include "LB_Layer_data.h"


uint16_t GetFrameLen(Layer2Frame* ptrFrame)
{
	uint16_t len = (uint16_t)(ptrFrame->St.length+EX_DATA_LEN+CRC_DATA_LEN);
//	if(len >= LB_FRAME_BUF_LEN) //
//	assert(len < LB_FRAME_BUF_LEN);
	return len;
}

#define	FRAME_NUM_MAX 0x04

static MsgFrame frames[FRAME_NUM_MAX];
#define NULL_ADR 0xFFFF
static uint16_t currFrameIdx = 0;
#if 0

MsgFrame* GetRightFrameBuff(CanHead* ptrHead)
{
	uint8_t getNewOne = 0;
	MsgFrame* ptrFrame = NULL;

	if(ptrHead->dst != SLAVE_ADR)
	{
		TraceMsg(TSK_ID_LOCAL_BUS,"Error address\n");
		return ptrFrame;
	}
	if(ptrHead->frameType == MULTI_NO)
	{
		getNewOne = 1;
	}
	else if(ptrHead->frameType == MULTI_ST)
	{
		if(ptrHead->exId == 0)
		{
			getNewOne = 1;
		}
		else
		{
			getNewOne = 0xFF;
		}
	}
	else
	{

	}
	switch(getNewOne)
	{
		case 0:
			for(uint8_t i=0;i<FRAME_NUM_MAX;i++)
			{
				if(frames[i].usedSrc == ptrHead->src)
				{
					ptrFrame = &frames[i];
					ptrFrame->typeMsg = FROM_CAN;
					break;
				}
			}
			break;
		case 1:
			for(uint8_t i=0;i<FRAME_NUM_MAX;i++)
			{
				//todo add semaphore
				if(frames[i].usedSrc == NULL_ADR)
				{
					ptrFrame = &frames[i];
					ptrFrame->typeMsg = FROM_CAN;
					frames[i].usedSrc = ptrHead->src;
					currFrameIdx = i;
					break;
				}
			}
			break;

		default:
			break;
	}

	return frames;
}
#endif

void LB_Layer_Data_Init()
{
	for(uint8_t i=0;i<FRAME_NUM_MAX;i++)
	{
		frames[i].usedSrc = NULL_ADR;
	}
}
void FreeFrameBuff(MsgFrame* frame)
{
	frame->usedSrc = NULL_ADR;
}

MsgFrame* LB_Layer2_GetNewBuff(void)
{
	MsgFrame* ptrFrame = &frames[currFrameIdx++ % FRAME_NUM_MAX];
	ptrFrame->usedSrc = 0x1FF;//reserved for uart;
	ptrFrame->usedSrc = FROM_UART;
	//todo add semaphore
	return ptrFrame;

}
