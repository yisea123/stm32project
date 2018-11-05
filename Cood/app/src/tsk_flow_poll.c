/*
 * tsk_flow_poll.c
 *
 *  Created on: 2016��12��1��
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
	POLL_IDLE, POLL_INIT, POLL_RUN, POLL_RUN_DELAY,POLL_DELAY,
	POLL_FINISH,

} POLL_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(POLL_IDLE),
	TO_STR(POLL_INIT),
	TO_STR(POLL_RUN),
	TO_STR(POLL_RUN_DELAY),
	TO_STR(POLL_DELAY),
	TO_STR(POLL_FINISH),
};

static 	TSK_MSG localMsg;
static void TskCallFin_Poll(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC( POLL_ID, &localMsg);
}

void StartPollTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	uint32_t timeDelay = 0;
	osEvent event;
	POLL_STATE tskState = POLL_IDLE;

	const uint8_t taskID = TSK_ID_FLOW_POLL;
	PollSch pollLoc = {0,0,0};
	localMsg.callBackFinish = NULL;
	TracePrint(taskID,"started\n");
	tickOut = osWaitForever;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(POLL_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout, %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			if(POLL_DELAY== tskState)
			{
				tskState = POLL_RUN;
				SendTskMsgLOC(POLL_ID, &localMsg);
				TraceMsg(taskID, "step_poll finished once!\n");
			}
			else if (POLL_RUN_DELAY == tskState)
			{
				tskState = POLL_RUN;
				SendTskMsgLOC(POLL_ID, &localMsg);
				TraceMsg(taskID, "one step_poll is finished error\n");
			}
			else
			{
				tskState = POLL_IDLE;
			}

		}
		else
		{
			TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = POLL_FINISH;
				SendTskMsg(MB_MAINSTEP, TSK_FORCE_BREAK, 0, NULL, NULL);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				SendTskMsgLOC(POLL_ID, &localMsg);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = POLL_FINISH;
				SendTskMsg(MB_MAINSTEP, TSK_RESETIO, 0, NULL, NULL);
				//MsgPush ( FLOW_MOTOR_CTL_ID, (uint32_t)&exMsg, 0);
				//MsgPush ( FLOW_MIX_CTL_ID, (uint32_t)&exMsg, 0);
				//MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&exMsg, 0);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				SendTskMsgLOC(POLL_ID, &localMsg);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if (tskState != POLL_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is,%d, %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{

					tskState = POLL_INIT;
					localMsg = *(TSK_MSG_CONVERT(event.value.p));


					pollLoc = *(PollSch*)(localMsg.val.p);
					SendTskMsgLOC(POLL_ID, &localMsg);
					//change to init
					//send msg to be running;
				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case POLL_IDLE:
						//idle
						break;
					case POLL_INIT:
						tskState = POLL_RUN;

						SendTskMsgLOC(POLL_ID, &localMsg);
						break;
					case POLL_RUN:
						if(!pollLoc.cycle)
						{
							tskState = POLL_FINISH;
							SendTskMsgLOC(POLL_ID, &localMsg);
						}
						else
						{
							timeDelay = (uint32_t)(GetStepsMS_WithDelay(pollLoc.subStep));
							pollLoc.cycle--;
							TraceMsg(taskID, "step_poll start once, step: %d cycle:%d time: %d\n",pollLoc.subStep, pollLoc.cycle, timeDelay);
							SendTskMsg(MB_MAINSTEP, TSK_INIT, pollLoc.subStep, TskCallFin_Poll, NULL);
							tskState = POLL_RUN_DELAY;
						}
						break;
					case POLL_RUN_DELAY:
						tickOut = pollLoc.delayTime *TIME_UNIT;
						tskState = POLL_DELAY;
						break;

					case POLL_FINISH:
						TraceMsg( taskID, "step_poll cycle finish\n");
						tskState = POLL_IDLE;
						TSK_FINISH_ACT(&localMsg,taskID,OK,OK);
						break;
					default:
						TraceMsg( taskID, "step_poll fatal error\n");
						tskState = POLL_IDLE;
						TSK_FINISH_ACT(&localMsg,taskID,OK,OK);
						break;
				}
			}
			else
			{}
		}
	}
}


