/*
 * tsk_poll_sch.c
 *
 *  Created on: 2017骞�1鏈�20鏃�
 *      Author: pli
 */

#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "tsk_substep.h"
#include "tsk_flow_all.h"
#include "unit_meas_cfg.h"
#include "unit_flow_act.h"


typedef enum
{
	POLL_SCH_IDLE, POLL_SCH_INIT, POLL_SCH_RUN, POLL_SCH_RUN_DELAY,
	//	FLOW_MAINSTEP_INIT,
	POLL_SCH_FINISH,

} POLL_SCH_STATE;


static const char* taskStateDsp[] =
{
	TO_STR(POLL_SCH_IDLE),
	TO_STR(POLL_SCH_INIT),
	TO_STR(POLL_SCH_RUN),
	TO_STR(POLL_SCH_RUN_DELAY),
	TO_STR(POLL_SCH_FINISH),
};
#define ALL_STEPS_RUN		0xFFFFFFFF

static void PollSchCallBack(uint32_t val)
{
	(void)val;
	SendTskMsg(POLL_SCH_ID, TSK_SUBSTEP, 0, NULL);
}

void StartPollSchTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	POLL_SCH_STATE tskState = POLL_SCH_IDLE;
	TSK_MSG localMsg;
	uint32_t pollSchIdx = 0;
	PollConfig pollConfigLoc = {0,0,0,0,0};
	const uint8_t taskID = TSK_ID_POLL_SCH_POLL;
	localMsg.callBack = NULL;
	TracePrint(taskID,"started\n");
	tickOut = osWaitForever;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(POLL_SCH_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			if (POLL_SCH_IDLE != tskState)
			{
				tskState = POLL_SCH_IDLE;
				TraceDBG(taskID,"the schedule has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
			}

		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = POLL_SCH_FINISH;
				SendTskMsg(POLL_ID, TSK_FORCE_BREAK, 0, NULL);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = POLL_SCH_FINISH;

				SendTskMsg(POLL_ID, TSK_RESETIO, 0, NULL);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);
			}
			else if (mainTskState == TSK_INIT)
			{

				if (tskState != POLL_SCH_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{
					tskState = POLL_SCH_INIT;
					localMsg = *(TSK_MSG_CONVERT(event.value.p));
					pollConfigLoc = *(PollConfig*)(localMsg.val.p);
					localMsg.tskState = TSK_SUBSTEP;
					if(pollConfigLoc.steps == ALL_STEPS_RUN)
					{
						pollSchIdx = 0;
					}
					else
					{
						pollSchIdx = pollConfigLoc.steps;
					}

					MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);
					//change to init
					//send msg to be running;
				}
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				switch (tskState)
				{
					case POLL_SCH_IDLE:
						//idle
						break;
					case POLL_SCH_INIT:
						tskState = POLL_SCH_RUN;
						MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);
						break;
					case POLL_SCH_RUN:
						if(!pollConfigLoc.start)
						{
							tskState = POLL_SCH_FINISH;
							MsgPush ( POLL_SCH_ID, (uint32_t)&localMsg, 0);
						}
						else if(pollConfigLoc.cycle == 0)
						{
							tskState = POLL_SCH_FINISH;
							MsgPush ( POLL_SCH_ID, (uint32_t)&localMsg, 0);
						}
						else
						{
							if (pollSchIdx >= POLL_SCH_SIZE)
							{
								tskState = POLL_SCH_RUN_DELAY;
								MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);
							}
							else
							{
								uint8_t idx = pollSchTrig[pollSchIdx];
								if(pollConfigLoc.steps != ALL_STEPS_RUN)
								{
									idx = (uint8_t)pollSchIdx;
								}
								if ((idx == 0) || (idx > POLL_SCH_SIZE))
								{
									//stop
									tskState = POLL_SCH_RUN_DELAY;
									MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);
								}
								else
								{
									tickOut =
											10000
											+ ((uint32_t) pollSchCfg[idx - 1].cycle
											* (uint32_t) pollSchCfg[idx - 1].duringTime)
											* TIME_UNIT;


									tskState = POLL_SCH_RUN_DELAY;
									SendTskMsg(POLL_ID, TSK_INIT, (uint32_t)&pollSchCfg[idx - 1], PollSchCallBack);
								}
								TraceMsg(taskID,
										"poll_sch start once, idx: %d cycle:%d time %d\n",
										idx, pollConfigLoc.cycle, tickOut);
							}
						}
						break;
					case POLL_SCH_RUN_DELAY:
						if(pollConfigLoc.steps == ALL_STEPS_RUN)
						{
							pollSchIdx++;
							if(pollSchIdx > POLL_SCH_SIZE)
							{
								pollSchIdx = 0;
								if(pollConfigLoc.cycle)
									pollConfigLoc.cycle--;
							}
						}
						else
						{
							if(pollConfigLoc.cycle)
								pollConfigLoc.cycle--;
						}
						if(pollConfigLoc.cycle)
							tskState = POLL_SCH_RUN;
						else
							tskState = POLL_SCH_FINISH;

						MsgPush(POLL_SCH_ID, (uint32_t )&localMsg, 0);
						TraceMsg(taskID,
									"poll_sch finished once, the cycle remained:%d\n",
									pollConfigLoc.cycle);
						break;
					case POLL_SCH_FINISH:
						tskState = POLL_SCH_IDLE;
						if(localMsg.callBack)
						{
							localMsg.callBack(taskID);
						}
						break;
				}
			}
			else
			{
			}
		}
	}


}


