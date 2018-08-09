/*
 * tsk_sch_io.c
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */




#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_meas_cfg.h"

#include "dev_eep.h"
#include "tsk_sch.h"
#include "unit_rtc_cfg.h"
#include "unit_sch_cfg.h"
#include "unit_flow_act.h"
#include "unit_io_cfg.h"




enum
{
	RO_POLL_IDX,
	IO_POLL_IDX,
};



static uint32_t lastTick[2] = {0,0};
static uint16_t roPollStatus = RO_POLL_CLR_TIMEOUT;
uint8_t doFuncValue[DO_FUNC_MAX] = {0,0,0,0,};


static const char* taskStateDsp[] =
{
		TO_STR(IO_IDLE),
		TO_STR(RO_PRETREATMENT_SET),//1
		TO_STR(RO_PRETREATMENT_CLR),
		TO_STR(RO_ALARMLOW_SET),//3
		TO_STR(RO_ALARMHIGH_SET),
		TO_STR(RO_ALARMLOW_CLR),//5
		TO_STR(RO_ALARMHIGH_CLR),
		TO_STR(RO_EVENT_SET),//7
		TO_STR(RO_EVENT_CLR),
		TO_STR(RO_POLL_SET_TIMEOUT),//9
		TO_STR(RO_POLL_CLR_TIMEOUT),
		TO_STR(IO_POLL_CHECK_TIMEOUT),//11
		TO_STR(AO_OUTPUT),
		TO_STR(REFRESH_OUTPUT),//13
		TO_STR(IO_FINISH),
		TO_STR(IO_POLL_INIT),
};






static void Reset_IO(void)
{
	//todo
	memset(doFuncValue, RO_CLR, DO_FUNC_MAX);
	for(uint16_t idx =0; idx<MAX_IO_BOARD_NUM;idx++)
	{
		doValue[idx*2] = RO_CLR;
		doValue[idx*2+1] = RO_CLR;
	}
	for(uint16_t idx =0; idx<MAX_IO_BOARD_NUM;idx++)
	{
		aoValue[idx*2] = AO_DEFAULT_OUT;
		aoValue[idx*2+1] = AO_DEFAULT_OUT;
	}
}


static void Break_RO(void)
{
	//todo
	memset(doFuncValue, RO_CLR, DO_FUNC_MAX);
	for(uint16_t idx =0; idx<MAX_IO_BOARD_NUM;idx++)
	{
		doValue[idx*2] = RO_CLR;
		doValue[idx*2+1] = RO_CLR;
	}
	for(uint16_t idx =0; idx<MAX_IO_BOARD_NUM;idx++)
	{
		aoValue[idx*2] = AO_DEFAULT_OUT;
		aoValue[idx*2+1] = AO_DEFAULT_OUT;
	}
}


static void RO_Output(IO_State tskState)
{
	//todo

	switch(tskState)
	{
		case RO_PRETREATMENT_SET:
			doFuncValue[DO_PRETREAT] = RO_SET;
			break;
		case RO_PRETREATMENT_CLR:
			doFuncValue[DO_PRETREAT] = RO_CLR;
			break;
		case RO_ALARMLOW_SET:
			doFuncValue[DO_ALARM_LOW] = RO_SET;
			break;
		case RO_ALARMLOW_CLR:
			doFuncValue[DO_ALARM_LOW] = RO_CLR;
			break;
		case RO_ALARMHIGH_SET:
			doFuncValue[DO_ALARM_HIGH] = RO_SET;
			break;
		case RO_ALARMHIGH_CLR:
			doFuncValue[DO_ALARM_HIGH] = RO_CLR;
			break;

		case RO_EVENT_SET:
			doFuncValue[DO_EVENT] = RO_SET;
			break;
		case RO_EVENT_CLR:
			doFuncValue[DO_EVENT] = RO_CLR;
			break;
		case RO_POLL_SET_TIMEOUT:
			doFuncValue[DO_SWITCH] = RO_SET;
			break;
		case RO_POLL_CLR_TIMEOUT:
			doFuncValue[DO_SWITCH] = RO_CLR;
			break;
		default:
			break;
	}
	UpdateDOValue(doFuncValue);
}



static uint32_t RO_GetIOTime(IO_State* ptrState)
{
#define IO_POLL_TIME		100
	static uint16_t	runTime = 0;
	uint32_t tickOut = IO_POLL_TIME;

	uint32_t currTick = HAL_GetTick();
	uint32_t tickDelay = GetTickDeviation(lastTick[IO_POLL_IDX],currTick);


	if (tickDelay >= (IO_POLL_TIME))
	{
		tickOut = 0;
	}
	else
	{
		tickOut = IO_POLL_TIME - tickDelay;
	}

	*ptrState = IO_POLL_CHECK_TIMEOUT;

	uint32_t currTime = 0;
	uint32_t stTime = CalcTime_ST(&RO_StartTime);
	(void)RTC_Get(IDX_RTC_ST,0,(void*)&currTime);

	if(currTime >= stTime)
	{
		if( (lastTick[RO_POLL_IDX] == 0x0) && (runTime == 0) )
		{
			tickOut = 0;
			*ptrState = RO_POLL_SET_TIMEOUT;
		}
		else
		{
			tickDelay = GetTickDeviation(lastTick[RO_POLL_IDX],currTick);
			if(roPollStatus == RO_POLL_SET_TIMEOUT)
			{
				//change to 0;
				if(tickDelay >= (uint32_t)RO_PWM_Cfg[1]*TIME_UNIT)
				{
					tickOut = 0;
					*ptrState = RO_POLL_CLR_TIMEOUT;
				}
				else
				{
					tickDelay = (uint32_t)RO_PWM_Cfg[1]*TIME_UNIT - tickDelay;
					if(tickDelay <= tickOut)
					{
						*ptrState = RO_POLL_CLR_TIMEOUT;
						tickOut = tickDelay;
					}
				}
			}
			else
			{
				//change to 1;
				if(tickDelay >= (uint32_t)RO_PWM_Cfg[0]*TIME_UNIT)
				{
					tickOut = 0;
					*ptrState = RO_POLL_SET_TIMEOUT;
				}
				else
				{
					tickDelay = (uint32_t)RO_PWM_Cfg[0]*TIME_UNIT - tickDelay;
					if(tickDelay <= tickOut)
					{
						*ptrState = RO_POLL_SET_TIMEOUT;
						tickOut = tickDelay;
					}
				}
			}

		}
		runTime = 1;
	}

	return tickOut;
}

void StartIOTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	IO_State tskState = IO_IDLE;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_SCH_IO;
	locMsg.callBack = NULL;
	locMsg.tskState = TSK_SUBSTEP;
	locMsg.val.value = IO_POLL_INIT;
	TracePrint(taskID,"task started  \n");
	tskState = IO_POLL_INIT;
	MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);//start by it self
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_IO_ID, tickOut);
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case IO_IDLE:
				break;
			case RO_POLL_SET_TIMEOUT:
			case RO_POLL_CLR_TIMEOUT:
				lastTick[RO_POLL_IDX] = HAL_GetTick();
				roPollStatus = tskState;
				RO_Output(tskState);
				tskState = IO_FINISH;
				MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
				break;
			case IO_POLL_CHECK_TIMEOUT:
				lastTick[IO_POLL_IDX] = HAL_GetTick();
				UpdateDIValue();
				UpdateAOValue();
				RO_Output(0);
				tskState = IO_FINISH;
				MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
				break;
			default:
				tskState = IO_FINISH;
				MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if(TSK_RENEW_STATUS == mainTskState)
			{
				tskState = IO_POLL_CHECK_TIMEOUT;
				tickOut = 0;
			}
			else if ( mainTskState == TSK_FORCE_BREAK)
			{
				Break_RO();
				tskState = IO_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_IO_ID, (uint32_t ) &locMsg, 0);
				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = IO_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_IO_ID, (uint32_t ) &locMsg, 0);

				Reset_IO();
			}
			else if ( mainTskState == TSK_INIT)
			{
				//idle
				//this function could be trigger always by other task;

				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				if(locMsg.val.value != 0)
				{
					IO_State tskState1 = (IO_State)locMsg.val.value;

					switch(tskState1)
					{
						case AO_OUTPUT:
							UpdateAOValue();
							TraceMsg(taskID,"AO_Event %d \n", tskState1);
							break;
						case RO_PRETREATMENT_SET:
						case RO_PRETREATMENT_CLR:
						case RO_ALARMLOW_SET:
						case RO_ALARMLOW_CLR:
						case RO_ALARMHIGH_SET:
						case RO_ALARMHIGH_CLR:
						case RO_EVENT_SET:
						case RO_EVENT_CLR:
							TraceMsg(taskID,"RO_Event %d \n", tskState1);
							RO_Output(tskState1);
							break;
						case REFRESH_OUTPUT:
							UpdateAOValue();
							RO_Output(0);
							break;
						default:
							break;
					}

				}
				tskState = IO_FINISH;
				MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
					case IO_IDLE:
					break;
					case IO_POLL_INIT:
						tskState = IO_POLL_INIT;
						tickOut = RO_GetIOTime(&tskState);
					break;

					case IO_FINISH:
						if (locMsg.callBack)
						{
							locMsg.callBack(OK);
							locMsg.callBack = NULL;
						}
						tskState = IO_POLL_INIT;

						MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
					break;
					case RO_POLL_SET_TIMEOUT:
					case RO_POLL_CLR_TIMEOUT:
					case IO_POLL_CHECK_TIMEOUT:
						tskState = IO_FINISH;
						MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
						break;
					default:
						tskState = IO_FINISH;
						MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);
						break;
						TraceDBG(taskID,"IO Poll wrong state: %d %s\n", tskState, taskStateDsp[tskState]);
						break;

				}
				//when finish->call back;
			}
			else //do nothing
			{
				//todo
				tskState = IO_FINISH;
				locMsg.callBack = NULL;
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_IO_ID, (uint32_t) &locMsg, 0);

			}
		}
		//call when IO task is executed;

	}
}



