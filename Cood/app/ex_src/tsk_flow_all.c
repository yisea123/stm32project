/*
 * tsk_flow_all.c
 *
 *  Created on: 2016锟斤拷12锟斤拷1锟斤拷
 *      Author: pli
 */



#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "tsk_substep.h"
#include "tsk_flow_all.h"
#include "t_data_obj_flowstep.h"
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "unit_rtc_cfg.h"


typedef struct{
	uint16_t 	mainProcess;
	uint16_t 	subStepIdx;
	uint16_t 	subStep;
} FlowProcess;

typedef enum
{
	FLOW_IDLE,
	FLOW_INIT,
	FLOW_SUBSTEP,
	FLOW_SUBSTEP_DELAY,
	//	FLOW_MAINSTEP_INIT,
	FLOW_FINISH,
}FLOW_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(FLOW_IDLE),
	TO_STR(FLOW_INIT),
	TO_STR(FLOW_SUBSTEP),
	TO_STR(FLOW_SUBSTEP_DELAY),
	TO_STR(FLOW_FINISH),
};


void StartFlowTask(void const * argument);


static uint16_t UpdateSubStep(FlowProcess* flowProcess)
{
	uint16_t ret = FATAL_ERROR;
	//todo
	return ret;
}


static void TriggerTskFinish(uint32_t idx)
{
	if(idx != TSK_MIX_IDX)
	{
		SendTskMsg(FLOW_TSK_ID,TSK_SUBSTEP,0, NULL, NULL);
	}
}



/* StartFlowTask function */
void StartFlowTask(void const * argument)
{
	(void)argument; // pc lint
	/* USER CODE BEGIN StartFlowTask */
	uint32_t tickOut = osWaitForever;
	uint16_t signal;
	osEvent event;
	FLOW_STATE tskState = FLOW_IDLE;
	TSK_MSG localMsg;
	//lint -e550
	const uint8_t taskID = TSK_ID_FLOW_CTRL;
	FlowProcess gFlowProcess;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	localMsg.val.value = 0;
	gFlowProcess.mainProcess = 0;
	gFlowProcess.subStep = 0;
	gFlowProcess.subStepIdx = 0;

	TracePrint(taskID,"started  \n");


	while(TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_TSK_ID, tickOut );

		if( event.status != osEventMessage )
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
				case FLOW_SUBSTEP_DELAY:
					tskState = FLOW_SUBSTEP;
					MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
					break;
				default:
					tskState = FLOW_IDLE;
					MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
					break;
			}
		}
		else
		{
			tickOut = osWaitForever;
			TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			if( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = FLOW_FINISH;
				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
				SendTskMsg(FLOW_SUBSTEP_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
				tskState = FLOW_FINISH;


				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
				SendTskMsg(FLOW_SUBSTEP_ID, TSK_RESETIO, 0, NULL, NULL);
			}
			else if( mainTskState == TSK_INIT)
			{
				if(tskState != FLOW_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{

					tskState = FLOW_INIT;
					localMsg = *(TSK_MSG_CONVERT(event.value.p));

					localMsg.tskState = TSK_SUBSTEP;
					MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case FLOW_IDLE:
						//idle
						break;
					case FLOW_INIT:
						signal = (uint16_t)(localMsg.val.value);
						flowStepRun[1].step = signal;
						flowStepRun[1].duringTime = (uint32_t)(CalcDuringTimeMsStep_WithDelay((uint16_t)flowStepRun[1].step) + 500)/1000;
						flowStepRun[1].startTime = GetCurrentST();
						if(IN_RANGE(signal,A_0, A_MAX))
						{

							//main step
							gFlowProcess.mainProcess = (uint16_t)signal;
							gFlowProcess.subStepIdx = 0;
							TraceMsg(taskID,"main Process %d  [ %s ]is called During time: %d S\n", gFlowProcess.mainProcess, GetStepName(gFlowProcess.mainProcess), flowStepRun[1].duringTime);
							tskState = FLOW_SUBSTEP;
						}
						else if(IN_RANGE(signal,SUB_STEP, SUB_STEP_MAX) )
						{
							gFlowProcess.mainProcess = 0;
							gFlowProcess.subStepIdx = 0;
							gFlowProcess.subStep = signal;
							tskState = FLOW_SUBSTEP;
						}
						else if(IN_RANGE(signal,SPECIAL_IDX_0, SPECIAL_IDX_MAX) )
						{
							gFlowProcess.mainProcess = 0;
							gFlowProcess.subStepIdx = 0;
							gFlowProcess.subStep = signal;
							tskState = FLOW_SUBSTEP;
						}
						else
						{
							tskState = FLOW_FINISH;
						}
						MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
						break;
					case FLOW_SUBSTEP:

						if(UpdateSubStep(&gFlowProcess)==OK)
						{
							SendTskMsg(FLOW_SUBSTEP_ID, TSK_INIT, gFlowProcess.subStep, TriggerTskFinish, NULL);
							tskState = FLOW_SUBSTEP_DELAY;
							tickOut = CalcDuringTimeMsStep_WithDelay(gFlowProcess.subStep);
						}
						else
						{
							gFlowProcess.mainProcess=0;
							gFlowProcess.subStep = 0;
							gFlowProcess.subStepIdx = 0;
							tskState = FLOW_FINISH;
							MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
						}
						break;
					case FLOW_SUBSTEP_DELAY:
						tskState = FLOW_SUBSTEP;
						MsgPush ( FLOW_TSK_ID, (uint32_t)&localMsg, 0);
						break;
					case FLOW_FINISH:
						flowStepRun[1].duringTime = 0;
						flowStepRun[1].remainTime = 0;
						tskState = FLOW_IDLE;
						TraceMsg(taskID,"flow finish %d ! \n",gFlowProcess.mainProcess);
						if(localMsg.callBackFinish)
						{
							localMsg.callBackFinish(OK, 0);
						}
						break;
				}
			}
		}

	}
	/* USER CODE END StartFlowTask */
}



