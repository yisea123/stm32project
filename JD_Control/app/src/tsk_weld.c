/*
 * tsk_weld.c
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"

static const char* taskStateDsp[] =
{
	TO_STR(ST_WELD_IDLE),
	TO_STR(ST_WELD_INITPARA),
	TO_STR(ST_WELD_PRE_GAS),
	TO_STR(ST_WELD_PRE_GAS_DELAY),
	TO_STR(ST_WELD_ARC_ON),
	TO_STR(ST_WELD_ARC_ON_DELAY),
	TO_STR(ST_WELD_UPSLOPE),
	TO_STR(ST_WELD_UPSLOPE_CYC),
	TO_STR(ST_WELD_MOTION),
	TO_STR(ST_WELD_MOTION_CYC),
	TO_STR(ST_WELD_STOP),
	TO_STR(ST_WELD_POST_HOME),
	TO_STR(ST_WELD_POST_HOME_DELAY),
	TO_STR(ST_WELD_POST_GAS),
	TO_STR(ST_WELD_POST_GAS_DELAY),
	TO_STR(STD_WELD_FINISH),
};

static void Break_RO()
{
	digitOutput = 0x0;
	SendTskMsg(OUTPUT_QID, TSK_INIT, DO_OUT_REFRESH, NULL, NULL);
}

ST_WELD_STATE GetStateRequest(ST_WELD_STATE tskState)
{

	if(weldStatus > ST_WELD_INITPARA)
	{
		if(tskState <= ST_WELD_PRE_GAS_DELAY)
		{
			tskState = ST_WELD_ARC_ON;
		}
		else if(tskState <= ST_WELD_STOP)
		{
			tskState = ST_WELD_STOP;
		}
		else if(tskState < ST_WELD_POST_GAS)
		{
			tskState = ST_WELD_POST_GAS;
		}
		else
		{
			tskState = ST_WELD_FINISH;
		}
	}
	else
	{
		tskState = ST_WELD_INITPARA;
	}
	return tskState;
}

#define WELD_MOTION_DELAY_TIME		50

void StartWeldTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	ST_WELD_STATE tskState = ST_WELD_IDLE;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_WELD;
	InitTaskMsg(&locMsg);
	TracePrint(taskID,"task started  \n");
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(WELD_CTRL, tickOut);
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case ST_WELD_IDLE:
				break;
			case ST_WELD_PRE_GAS_DELAY:
				tskState = ST_WELD_ARC_ON;
				SendTskMsgLOC(WELD_CTRL, &locMsg);
				break;
			case ST_WELD_ARC_ON_DELAY:
				TraceDBG(taskID,"No arc detect in time: %d %s\n", tskState, taskStateDsp[tskState]);
				tskState = ST_WELD_STOP;
				SendTskMsgLOC(WELD_CTRL, &locMsg);
				break;
			case ST_WELD_MOTION_CYC:
				//todo
				//check weld pos, change pwm
				//or close ;ST_WELD_STOP

				tickOut = WELD_MOTION_DELAY_TIME;
				break;
			case ST_WELD_POST_HOME_DELAY:
				TraceDBG(taskID,"wrong state: %d %s\n", tskState, taskStateDsp[tskState]);
				break;
			case ST_WELD_POST_GAS_DELAY:
				tskState = ST_WELD_FINISH;
				MsgPush(WELD_CTRL, (uint32_t) &locMsg, 0);
				break;
			default:
				tskState = ST_WELD_IDLE;
				MsgPush(WELD_CTRL, (uint32_t) &locMsg, 0);
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
				Break_RO();
				tskState = ST_WELD_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(WELD_CTRL, (uint32_t ) &locMsg, 0);
				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = ST_WELD_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(WELD_CTRL, (uint32_t ) &locMsg, 0);


			}
			else if ( mainTskState == TSK_INIT)
			{
				//idle
				//this function could be trigger always by other task;

				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				tskState = GetStateRequest(tskState);
				SendTskMsgLOC(WELD_CTRL, &locMsg);
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
				case ST_WELD_IDLE:
					break;
					case ST_WELD_INITPARA:
						//init timer set
						//init counter
						//init motor pos

						break;
					case ST_WELD_PRE_GAS:
						tickOut = weldProcess.preGasTime*TIME_UNIT;
						tskState = ST_WELD_PRE_GAS_DELAY;
						break;
					case ST_WELD_PRE_GAS_DELAY:
						tskState = ST_WELD_ARC_ON;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;

					case ST_WELD_ARC_ON:
						tskState = ST_WELD_ARC_ON_DELAY;
						tickOut = weldProcess.preDelay*TIME_UNIT;
						break;
					case ST_WELD_ARC_ON_DELAY:
						tskState = ST_WELD_MOTION;
						break;
					case ST_WELD_MOTION:
						//start motion
						//start timer
						//start pwm
						tickOut = WELD_MOTION_DELAY_TIME;//pwm time
						tskState = ST_WELD_MOTION_CYC;
						break;
					case ST_WELD_STOP:
						//stop weld;
						tskState = ST_WELD_POST_HOME;
						SendTskMsgLOC(WELD_CTRL, &locMsg);

						break;
					case ST_WELD_POST_HOME:
						//send to motor task to home
						break;
					case ST_WELD_POST_HOME_DELAY:
						TraceDBG(taskID,"wrong state: %d %s\n", tskState, taskStateDsp[tskState]);
						tskState = ST_WELD_POST_GAS;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;
					case ST_WELD_POST_GAS:
						//output gas;
						tickOut = weldProcess.postGasTime*TIME_UNIT;
						tskState = ST_WELD_POST_GAS_DELAY;
						break;
					case ST_WELD_POST_GAS_DELAY:
						tskState = ST_WELD_IDLE;
						TraceDBG(taskID,"wrong state: %d %s\n", tskState, taskStateDsp[tskState]);
						break;
					case ST_WELD_FINISH:
						tskState = ST_WELD_IDLE;
						TSK_FINISH_ACT(&locMsg,taskID,OK,OK);
						break;
					default:
						tskState = ST_WELD_IDLE;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;
				}
				//when finish->call back;
			}

		}
		//call when IO task is executed;

	}
}

