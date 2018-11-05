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
#include "unit_flow_cfg.h"

uint16_t      	devicePollStart ;//		__attribute__ ((section (".ram_noinit")));
PollConfig		pollConfig_ram 	;//		__attribute__ ((section (".ram_noinit")));

typedef enum
{
	POLL_SCH_IDLE, POLL_SCH_INIT, POLL_SCH_RUN, POLL_SCH_DELAY,
	//	FLOW_MAINSTEP_INIT,
	POLL_SCH_FINISH,

} POLL_SCH_STATE;


static const char* taskStateDsp[] =
{
	TO_STR(POLL_SCH_IDLE),
	TO_STR(POLL_SCH_INIT),
	TO_STR(POLL_SCH_RUN),
	TO_STR(POLL_SCH_DELAY),
	TO_STR(POLL_SCH_FINISH),
};


static TSK_MSG  localMsg;

static void TskCallFin_PollSch(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC( POLL_SCH_ID, &localMsg);
}


static void TskUpdate_PollSch(uint16_t ret, uint16_t val)
{
	SendTskMsg(POLL_SCH_ID, TSK_RENEW_STATUS , (uint32_t)0, NULL,NULL);
}

/* StartPollSchTask function */
void StartPollSchTask(void const * argument)
{
	(void)argument;

	osEvent event;
	uint32_t tickOut = osWaitForever;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	localMsg.val.value = 0;
    
    PollConfig pollConfigLoc = {0,0,0};
//	TracePrint(taskID,"started  \n");
	uint16_t result = OK;
    POLL_SCH_STATE tskState = POLL_SCH_IDLE;
	uint32_t pollSchIdx = 0;	
	const uint8_t taskID = TSK_ID_POLLSCH;
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
			switch (tskState)
			{
				case POLL_SCH_DELAY:

					SendTskMsgLOC( POLL_SCH_ID, &localMsg);
					break;
				default:
					if (POLL_SCH_IDLE != tskState)
					{
						tskState = POLL_SCH_IDLE;
						TraceDBG(taskID,"the schedule has bugs, original state is,%d, %s!\n",tskState, taskStateDsp[tskState]);
					}
					break;
			}
		}
		else
		{
			tickOut = osWaitForever;
			localMsg = *TSK_MSG_CONVERT(event.value.p);
			const TSK_STATE mainTskState = localMsg.tskState;
			TSK_MSG_RELASE;
			if( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = POLL_SCH_FINISH;
                SendTskMsgLOC( POLL_SCH_ID, &localMsg);
				SendTskMsg(POLL_ID, TSK_FORCE_BREAK,(uint32_t)0, NULL,NULL);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{   
                tskState = POLL_SCH_FINISH;
                SendTskMsgLOC( POLL_SCH_ID, &localMsg);
				SendTskMsg(POLL_ID, TSK_RESETIO,(uint32_t)0, NULL,NULL);
			}
			else if( mainTskState == TSK_INIT)
			{
				if (tskState != POLL_SCH_IDLE)
				{
					//do nothing;
					//todo may not be right
					result = FATAL_ERROR;
					TraceDBG(taskID,"the schedule has bugs, original state is,%d, %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				else
				{
					result = OK;
				}
				//this function could be trigger always by other task;
				//idle
				{
					tskState = POLL_SCH_INIT;
					pollConfigLoc = *(PollConfig*)(localMsg.val.p);
					localMsg.tskState = TSK_SUBSTEP;
					if(pollConfigLoc.stepIdx == ALL_STEPS_RUN)
					{
						pollSchIdx = 0;
					}
					else
					{
						pollSchIdx = pollConfigLoc.stepIdx;
					}
					SendTskMsgLOC(POLL_SCH_ID, &localMsg);
					//change to init
					//send msg to be running;
				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch (tskState)
				{
					case POLL_SCH_IDLE:
						//idle
						break;
					case POLL_SCH_INIT:
                        tskState = POLL_SCH_RUN;
						SendTskMsgLOC( POLL_SCH_ID, &localMsg);
						break;
					case POLL_SCH_RUN:
						if(!pollConfigLoc.start)
						{
							tskState = POLL_SCH_FINISH;
							SendTskMsgLOC( POLL_SCH_ID, &localMsg);
						}
						else if(pollConfigLoc.cycle == 0)
						{
							tskState = POLL_SCH_FINISH;
							SendTskMsgLOC( POLL_SCH_ID, &localMsg);
						}
						else
						{
							if (pollSchIdx >= POLL_SCH_SIZE)
							{
								tskState = POLL_SCH_FINISH;
								SendTskMsgLOC( POLL_SCH_ID, &localMsg);
							}
							else
							{
								tickOut =
										100000
										+ (uint32_t) pollSchCfg[pollSchIdx].cycle \
										* (pollSchCfg[pollSchIdx].delayTime*TIME_UNIT + \
												(uint32_t)(GetStepsMS_WithDelay(pollSchCfg[pollSchIdx].subStep)) );

								tskState = POLL_SCH_DELAY;
								SendTskMsg(POLL_ID, TSK_INIT, (uint32_t)&pollSchCfg[pollSchIdx], TskCallFin_PollSch, NULL);

								TraceMsg(taskID,
										"poll_sch start once, idx, %d cycle,%d, time, %d\n",
										pollSchIdx, pollConfigLoc.cycle, tickOut);
							}
						}
						break;
					case POLL_SCH_DELAY:
					//	tickOut = 0;
						if(pollConfigLoc.stepIdx == ALL_STEPS_RUN)
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
						TraceMsg(taskID,
										"poll_sch finished once,idx, %d, the cycle remained,%d\n",
										pollSchIdx, pollConfigLoc.cycle);
						SendTskMsgLOC( POLL_SCH_ID, &localMsg);
						break;

					case POLL_SCH_FINISH:
						tskState = POLL_SCH_IDLE;
						TraceMsg(taskID,"finished result, %d, \n", result);
						TSK_FINISH_ACT(&localMsg,taskID,OK,result);
						break;
				}
			}
			else
			{
			}
		}
	}
	/* USER CODE END StartPollSchTask */
}



