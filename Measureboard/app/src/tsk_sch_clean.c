/*
 * tsk_sch_clean.c
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
#include "unit_sch_cfg.h"
#include "dev_eep.h"
#include "tsk_sch.h"
#include "tsk_measure.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "dev_log_sp.h"
#include "dev_logdata.h"
#include "dev_temp.h"


typedef enum
{

	CLEAN_IDLE,
	CLEAN_TRIGGER,
	CLEAN_DELAY,
	CLEAN_FINISH,
}CleanState;

static const char* taskStateDsp[] =
{
	TO_STR(CLEAN_IDLE),
	TO_STR(CLEAN_TRIGGER),
	TO_STR(CLEAN_DELAY),
	TO_STR(CLEAN_FINISH),
};
static void CleanFinished(uint32_t val)
{
	(void)val;
	SendTskMsg(SCH_CLEAN_ID, TSK_SUBSTEP, 0, NULL);
}


uint32_t GetCleanDuringTime_Ms(uint32_t rangeIdx)
{
	rangeIdx = (MSK_RANGE_SEL&rangeIdx);
	assert(MEAS_RANGE_MAX > rangeIdx);
	uint32_t val = CalcDuringTimeMsStep_WithDelay(cleanSchSteps[rangeIdx]);
	return val;

}

void StartCleanTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	CleanState tskState = CLEAN_IDLE;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_SCH_CLEAN;
	locMsg.callBack = NULL;
	TracePrint(taskID,"clean task started  \n");
	uint32_t schCleanRangeSel = 0xFFFFFFFF;

	uint32_t cleanType = 0;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_CLEAN_ID, tickOut);
		//TracePrint(taskID,"clean task called %d  \n", tskState);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			TraceDBG(taskID,"bugs about time out, original state is:%d-%s!\n",tskState,taskStateDsp[tskState]);
			switch (tskState)
			{
			case CLEAN_IDLE:
				break;
			case CLEAN_DELAY:
				tskState = CLEAN_FINISH;
				MsgPush(SCH_CLEAN_ID, (uint32_t) &locMsg, 0);
				break;
			default:
				tskState = CLEAN_IDLE;
				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = CLEAN_IDLE;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_CLEAN_ID, (uint32_t ) &locMsg, 0);
			//	SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, NULL);
				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = CLEAN_IDLE;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_CLEAN_ID, (uint32_t ) &locMsg, 0);

			//	SendTskMsg(FLOW_TSK_ID, TSK_RESETIO, 0, NULL);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if (tskState != CLEAN_IDLE)
				{
					if( (TSK_MSG_CONVERT(event.value.p))->callBack )
					{
						(TSK_MSG_CONVERT(event.value.p))->callBack(DEVICE_BUSY_STATE);
					}
					//do nothing;
					//todo may not be right
					TraceDBG(taskID, "the schedule of clean task have bugs!\n")
;					//not finish;
				}
				else
				{
					locMsg = *(TSK_MSG_CONVERT(event.value.p));
					cleanType = (locMsg.val.value & MSK_MEAS_CALI_TYPE);
					schCleanRangeSel = (locMsg.val.value & MSK_RANGE_SEL);
					mainActionDetail = (uint16_t)((schCleanRangeSel<<8) | 0xFF);
					actionRuningDetail = locMsg.val.value;
					//idle
					uint32_t stTime = GetCurrentST();
					if(cleanType == MSK_CLEAN_SCH)
					{
						schInfo.lastCleanTime = stTime;
						actionExecuteTime_ST[ Sch_Clean] = stTime;
					}
					else
					{
						actionExecuteTime_ST[ Trigger_Clean] = stTime;
						schInfo.lastTrigClean = stTime;
						if(schCleanRangeSel == measSch.measCaliIndex)
							schInfo.lastCleanTime = schInfo.lastTrigClean;

					}


					Trigger_EEPSave((void*)&schInfo, sizeof(schInfo),SYNC_CYCLE);
					tskState = CLEAN_TRIGGER;

					locMsg.tskState = TSK_SUBSTEP;
					MsgPush(SCH_CLEAN_ID, (uint32_t) &locMsg, 0);
					uint8_t evData[6];
					evData[0] = (uint8_t)schCleanRangeSel;
					NewEventLog(EV_CLEANING, evData);
					//change to init
					//send msg to be running;
				}
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
					case CLEAN_IDLE:
					break;
					case CLEAN_TRIGGER:
					{
						TraceMsg(taskID,"CLEAN task execute: CLEAN_TRIGGER\n");
						tskState = CLEAN_DELAY;
						assert(MEAS_RANGE_MAX > schCleanRangeSel);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, cleanSchSteps[schCleanRangeSel], CleanFinished);
						tickOut = CalcDuringTimeMsStep_WithDelay(cleanSchSteps[schCleanRangeSel]);
					}
					break;
					case CLEAN_DELAY:
						tskState = CLEAN_FINISH;
						MsgPush(SCH_CLEAN_ID, (uint32_t) &locMsg, 0);
					break;

					case CLEAN_FINISH:
					{
						uint32_t stTime = GetCurrentST();
						tskState = CLEAN_IDLE;
						if(cleanType == MSK_CLEAN_SCH)
							schInfo.lastCleanFinish = stTime;
						else
						{
							if(schCleanRangeSel == measSch.measCaliIndex)
								schInfo.lastCleanFinish = stTime;
						}
						Trigger_EEPSave((void*)&schInfo, sizeof(schInfo),SYNC_CYCLE);
						if (locMsg.callBack)
						{
							locMsg.callBack(OK);
						}
					}
					break;
					default:
						if( locMsg.callBack )
						{
							locMsg.callBack(OK);
						}
						locMsg.callBack = NULL;
						TraceDBG(taskID,"sch_clean has bugs: %s: %d,\t%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);
						tskState = CLEAN_IDLE;

						break;
				}
				//when finish->call back;
			}
			else
			{
				//todo
			}
		}

	}
}




