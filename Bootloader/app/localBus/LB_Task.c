/*
 * LB_Task.c
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */

#include <string.h>
#include "main.h"
#include "LB_Layer_data.h"
#include "LB_Layer2.h"
#include "LB_Layer7.h"
#include "LB_App.h"
#include "usart.h"
typedef enum
{
	LB_IDLE,
	LB_RSP_TRIGGER,
	LB_BURST_TRIGGER,
	LB_FINISH,
}LBState;

static uint16_t SendResponse(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t ret = OK;
	uint8_t dst = frame->St.dst;
	frame->St.dst = frame->St.src;
	frame->St.src = dst;
	frame->St.function |= RESPONSE_BIT;
	NotifyLB(ptrMsgFrame, TX_HANDLE);

//	MsgPush(SCH_LB_ID, (uint32_t)ptrMsgFrame, 100);
	return ret;
}

static uint16_t RxTrigger(MsgFrame* frames)
{
	uint16_t ret = OK;
	NotifyLB(frames, RX_HANDLE);
//	MsgPush(SCH_LB_ID, (uint32_t)frames, 0);
	return ret;
}


void LB_Init()
{
	LB_APP_Init(RxTrigger, SendResponse);
}

#ifdef LB_TEST

static uint8_t sendBuf[1024];

static uint8_t hexChar(uint8_t i)
{
	const uint8_t str[] = "0123456789ABCDEF";
	if(i<16)
		return str[i];
	else
		return '0';
}

#endif

#if 0

void StartLBTask(void const * argument)
{
	(void) argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	LBState tskState = LB_IDLE;
	TSK_MSG locMsg;
	uint16_t ret = OK;
	MsgFrame* localFrames;
	const uint8_t taskID = TSK_ID_LOCAL_BUS;

	locMsg.callBack = NULL;
	TracePrint(taskID, "started  \n");

	ret = LB_APP_Init(RxTrigger, SendResponse);
	if (ret != OK)
	{
		TraceDBG(taskID, "LB init failed!\n");
	}

	uint32_t txCnt = 0;
	while (TASK_LOOP_ST)
	{
		event = osMessageGet(SCH_LB_ID, tickOut);
		TracePrint(taskID, "called %d  \n", tskState);
		tickOut = osWaitForever;
		if (event.status != osEventMessage)
		{
			//handle burst message
			TracePrint(taskID, "timeout %d  \n",
					tskState);
			tskState = LB_IDLE;
		//	HAL_UART_Transmit(&huart3,comTest, strlen(comTest), 15 );
		}
		else
		{
			if ( event.value.v)
			{
				localFrames = (MsgFrame*)(event.value.p);
				if(localFrames)
				{
					uint8_t status = (localFrames->frame.St.function & RESPONSE_BIT);
					if(status != 0)
					{
						txCnt++;
						//shell_Add_Rawdata(sendBuf, strlen(sendBuf));
						ret = LB_Layer2_Tx(localFrames);
					}
					else
					{
						ret = LB_Layer7_Decode(localFrames);
					}
#ifdef LB_TEST
					memset(sendBuf,0, sizeof(sendBuf));
					int i = 0;
					sendBuf[0] = '\n';
					for(i=0;i<localFrames->St.length;i++)
					{
						sendBuf[i*3+1] = hexChar( localFrames->St.dataBuff[i]/16 );
						sendBuf[i*3 + 2] = hexChar( localFrames->St.dataBuff[i]%16 );
						sendBuf[i*3 + 3] = ' ';
					}
					sendBuf[i*3+1] = '\n';
					TraceMsg(taskID, "%s Msg: func: %x,%d,%s\n", (status? "TX":"RX"),localFrames->St.function,txCnt,sendBuf);
#else
					TraceMsg(taskID, "%s Msg: func: %x , %d\n", (status? "TX":"RX"), localFrames->frame.St.function,txCnt);
#endif
					if (ret != OK)
					{
						//error handling
						TraceDBG(taskID, "LB failed: %d, ret: %d\n",status,ret );
					}
					else
					{
						TracePrint(taskID, "LB OK! \n");
					}
				}



				tskState = LB_IDLE;
				if (locMsg.callBack)
				{
					locMsg.callBack(ret);
				}

				//change to init
				//send msg to be running;
			}
		}

	}
}


#endif
