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
#include "LB_Task.h"
#include "usart.h"
#include "unit_statistics_data.h"
#include "unit_rtc_cfg.h"
#include "unit_sch_cfg.h"
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
	TracePrint(TSK_ID_LOCAL_BUS,"Send back: 0x%x ,Err: %d, 0x%08x\n", (frame->St.function&FUNCTION_MSK),  (frame->St.function&ERROR_BIT), ptrMsgFrame);
	MsgPush(SCH_LB_ID, (uint32_t)ptrMsgFrame, 10);
	return ret;
}

static uint32_t rxTick = 0;

static uint16_t RxTrigger(MsgFrame* frames)
{
	uint16_t ret = OK;
	rxTick = HAL_GetTick();
	MsgPush(SCH_LB_ID, (uint32_t)frames, 0);
	return ret;
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
	memset( (void*)&communicationStatus[0],0,sizeof(communicationStatus));

	communicationStatus[COM_MAX_TICK] = 0;
	communicationStatus[COM_MIN_TICK] = 0xFFFFFF;
	uint32_t tickSum = 0;
	uint32_t tickNum = 0;
	uint32_t tickCalc = 0;
	while (TASK_LOOP_ST)
	{
	//	tickOut = 1000;
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;//0
		event = osMessageGet(SCH_LB_ID, tickOut);
//		TracePrint(taskID, "called %d  \n", tskState);

		if (event.status != osEventMessage)
		{
			//handle burst message
			  hcan2.pTxMsg->StdId = 0x321;
			  hcan2.pTxMsg->ExtId = 0x01;
			  hcan2.pTxMsg->RTR = CAN_RTR_DATA;
			  hcan2.pTxMsg->IDE = CAN_ID_STD;
			  hcan2.pTxMsg->DLC = 2;
			ret = HAL_CAN_Transmit(&hcan2, 0x10);
			TracePrint(taskID, "timeout %d  ret = %d\n",
					tskState, ret);
			tskState = LB_IDLE;
			//to let mcu work


		//	HAL_UART_Transmit(&huart3,comTest, strlen(comTest), 15 );
		}
		else
		{
			if ( event.value.v)
			{
				tickOut = osWaitForever;
				localFrames = (MsgFrame*)(event.value.p);
				if(localFrames)
				{
					uint8_t func = (localFrames->frame.St.function & RESPONSE_BIT);
					uint32_t currST = GetCurrentST();
#if 0
					if(UI_Start == WAIT_FOR_UI)
					{
						uint16_t loc = UI_CONNECTED;
						SCH_Put(OBJ_IDX_UI_START,0,&loc);
					}
#endif

					if(func != 0)
					{
						uint32_t _tick = HAL_GetTick();

						communicationStatus[COM_OK_COUNT]++;
						communicationHappenST[COM_OK_COUNT] = currST;
						//TraceMsg(taskID,"Start TX: func: %x , %d\n", localFrames->frame.St.function,txCnt);
						freeRtosTskState[taskID] |= 0x20;
						//shell_Add_Rawdata(sendBuf, strlen(sendBuf));
						uint32_t tickNow = GetTickDeviation(rxTick,_tick);
						if(tickCalc++ > 100)
						{
							if(communicationStatus[COM_MIN_TICK] > tickNow)
							{
								communicationHappenST[COM_MIN_TICK] = currST;
								communicationStatus[COM_MIN_TICK] = tickNow;
							}
							else if(communicationStatus[COM_MAX_TICK] < tickNow)
							{
								communicationHappenST[COM_MAX_TICK] = currST;
								communicationStatus[COM_MAX_TICK] = tickNow;
							}
							else
							{}
							tickSum += tickNow;

							tickNum ++;
							communicationHappenST[COM_TICK_USED] = currST;
							communicationHappenST[COM_AVG_TICK] = currST;
							communicationStatus[COM_AVG_TICK] = tickSum/tickNum;
							communicationStatus[COM_TICK_USED] = tickSum;
							tickCalc = 1000;
						}
						ret = LB_Layer2_Tx(localFrames);
					}
					else
					{
					//	TraceMsg(taskID,"Start Decode: func: %x , %d\n", localFrames->frame.St.function,txCnt);
						freeRtosTskState[taskID] |= 0x02;
						ret = LB_Layer7_Decode(localFrames);
						if(ret != OK)
						{
							communicationStatus[COM_WARNING_COUNT]++;
							communicationHappenST[COM_WARNING_COUNT] = currST;
						}
						freeRtosTskState[taskID] |= 0x08;
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
//					TraceMsg(taskID, "%s Msg: func: %x , %d\n", (status? "TX":"RX"), localFrames->frame.St.function,txCnt);
#endif
					if (ret != OK)
					{
						//error handling
						int len = GetFrameLen(&localFrames->frame);
						uint8_t* data = localFrames->frame.St.dataBuff;

						TraceMsg(taskID,"Type: %d; Uart Message:len: %d; %x %x %x %x, %x %x %x %x,%x %x %x %x\n",\
									func,\
									len,localFrames->frame.St.dst,localFrames->frame.St.function,localFrames->frame.St.src,localFrames->frame.St.length, \
									data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
						TraceDBG(taskID, "LB failed:Message:0x%08x, %d->%d, ret: %d\n",localFrames, localFrames->frame.St.function, (localFrames->frame.St.function&0x3F),ret );
					}
					else
					{
						//if(func != 0)
						//	TraceMsg(taskID,"Finish: type: %d, func: 0x%x , 0x%02x\n", func, localFrames->frame.St.function,freeRtosTskState[taskID]);
			//			TracePrint(taskID, "LB OK! \n");
					}
				}



				locMsg.tskState = TSK_SUBSTEP;
				tskState = LB_IDLE;
				if (locMsg.callBack)
				{
					locMsg.callBack(ret);
				}

				//change to init
				//send msg to be running;
			}
		}
		//TraceMsg(taskID, "Message Finished\n");

	}
}
