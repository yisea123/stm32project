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
	POLL_IDLE, POLL_INIT, POLL_RUN, POLL_RUN_DELAY,
	POLL_FINISH,

} POLL_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(POLL_IDLE),
	TO_STR(POLL_INIT),
	TO_STR(POLL_RUN),
	TO_STR(POLL_RUN_DELAY),
	TO_STR(POLL_FINISH),
};

void StartPollTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	uint32_t timeDelay = 0;
	osEvent event;
	POLL_STATE tskState = POLL_IDLE;
	TSK_MSG localMsg;
	const uint8_t taskID = TSK_ID_FLOW_POLL;
	PollSch pollLoc = {0,0,0};
	localMsg.callBack = NULL;
	TracePrint(taskID,"started\n");
	tickOut = osWaitForever;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(POLL_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			if (POLL_RUN_DELAY == tskState)
			{
				tskState = POLL_RUN;
				MsgPush(POLL_ID, (uint32_t )&localMsg, 0);
				TraceMsg(taskID, "one step_poll is finished\n");
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
				SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, NULL);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(POLL_ID, (uint32_t )&localMsg, 0);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = POLL_FINISH;
				SendTskMsg(FLOW_TSK_ID, TSK_RESETIO, 0, NULL);
				//MsgPush ( FLOW_MOTOR_CTL_ID, (uint32_t)&exMsg, 0);
				//MsgPush ( FLOW_MIX_CTL_ID, (uint32_t)&exMsg, 0);
				//MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&exMsg, 0);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(POLL_ID, (uint32_t )&localMsg, 0);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if (tskState != POLL_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{

					tskState = POLL_INIT;
					localMsg = *(TSK_MSG_CONVERT(event.value.p));

					localMsg.tskState = TSK_SUBSTEP;
					pollLoc = *(PollSch*)(localMsg.val.p);
					MsgPush(POLL_ID, (uint32_t)&localMsg, 0);
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
						timeDelay = pollLoc.duringTime;
						MsgPush ( POLL_ID, (uint32_t)&localMsg, 0);
						break;
					case POLL_RUN:
						if(!pollLoc.cycle)
						{
							tskState = POLL_FINISH;
							MsgPush ( POLL_ID, (uint32_t)&localMsg, 0);
						}
						else
						{
							timeDelay = pollLoc.duringTime;
							tickOut = timeDelay * TIME_UNIT;
							pollLoc.cycle--;
							TraceMsg(taskID, "step_poll start once, step: %d cycle:%d time: %d\n",pollLoc.subStep, pollLoc.cycle, timeDelay);
							SendTskMsg(FLOW_TSK_ID, TSK_INIT, pollLoc.subStep, NULL);
							tskState = POLL_RUN_DELAY;
						}
						break;
					case POLL_RUN_DELAY:
						tskState = POLL_RUN;
						MsgPush(POLL_ID, (uint32_t )&localMsg, 0);
						TraceMsg(taskID, "step_poll finished once!\n");
						break;
					case POLL_FINISH:
						TraceMsg( taskID, "step_poll cycle finish\n");
						tskState = POLL_IDLE;
						if(localMsg.callBack)
						{
							localMsg.callBack(taskID);
						}
						break;
				}
			}
			else
			{}
		}
	}
}


