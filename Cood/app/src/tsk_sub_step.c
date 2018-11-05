/*
 * tsk_sub_step.c
 *
 *  Created on: 2018Äê3ÔÂ27ÈÕ
 *      Author: pli
 */




#include "includes.h"


typedef enum
{
	SUB_STEP_IDLE,
	SUB_STEP_INIT,
	SUB_STEP_ACTION,
	SUB_STEP_ACTION_DELAY,
	//	SUB_STEP_ACTION_INIT,
	SUB_STEP_FINISH,
	SUB_STEP_FINISH_ER,
}SUB_STEP_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(SUB_STEP_IDLE),
	TO_STR(SUB_STEP_INIT),
	TO_STR(SUB_STEP_ACTION),
	TO_STR(SUB_STEP_ACTION_DELAY),
	TO_STR(SUB_STEP_FINISH),
	TO_STR(SUB_STEP_FINISH_ER),

};

static TSK_MSG  localMsg;

static uint16_t UpdateSubStep(FlowStepInfo* flowStep)
{
	uint16_t ret = FATAL_ERROR;
	if(IN_RANGE(flowStep->subStep, e_sub_step_start, e_sub_step_end))
	{
		//lint -e685 -e568
		flowStep->preSub = subSteps[flowStep->subStep - e_sub_step_start][flowStep->subStepIdx];
		flowStep->subStepIdx++;
	}
	else
	{		
        if(flowStep->subStepIdx == 0)
        {
            flowStep->subStepIdx++;
            flowStep->preSub = flowStep->subStep;
        }
        else
        {
            flowStep->preSub = 0;
        }
	}

	if(IN_RANGE(flowStep->preSub, e_valve_start, e_pre_sub_end))
	{
		ret = OK;
	}
    else if(flowStep->preSub > e_pre_sub_end)
    {
        TraceDBG(TSK_ID_SUBSTEP,"sub step call presub error,%d > %d! \n",flowStep->preSub, e_pre_sub_end);
    }
    else
    {}
	return ret;
}
static void TskCallFin_Sub(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC( MB_SUBSTEP, &localMsg);
}


static void TskUpdate_Sub(uint16_t ret, uint16_t val)
{
	SendTskMsg(MB_SUBSTEP, TSK_RENEW_STATUS , (uint32_t)0, NULL,NULL);
}




/* StartSubStepTask function */
void StartSubStepTask(void const * argument)
{
	osEvent event;
	uint32_t tickOut = osWaitForever;
	SUB_STEP_STATE tskState = SUB_STEP_IDLE;
	//lint -e550
	const uint8_t taskID = TSK_ID_SUBSTEP;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	localMsg.val.value = 0;
//	TracePrint(taskID,"started  \n");
	uint16_t result = OK;

	while(1)
	{
		event = osMessageGet(MB_SUBSTEP, tickOut);
		//		tickOut = osWaitForever;
		if (event.status != osEventMessage)//timeout
		{
			TracePrint(taskID, "Timeout, %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
				case SUB_STEP_ACTION_DELAY:
					tskState = SUB_STEP_ACTION;
					SendTskMsgLOC( MB_SUBSTEP, &localMsg);
					break;
				default:
					tskState = SUB_STEP_IDLE;
					SendTskMsgLOC( MB_SUBSTEP, &localMsg);
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
				tskState = SUB_STEP_FINISH;
                SendTskMsgLOC( MB_SUBSTEP, &localMsg);
				SendTskMsg(MB_PRESUB, TSK_FORCE_BREAK,(uint32_t)0, NULL,NULL);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
                tskState = SUB_STEP_FINISH;
                SendTskMsgLOC( MB_SUBSTEP, &localMsg);
				SendTskMsg(MB_PRESUB, TSK_RESETIO,(uint32_t)0, NULL,NULL);
			}
			else if( mainTskState == TSK_INIT)
			{
				if(tskState != SUB_STEP_IDLE)
				{
					//do nothing;
					//todo may not be right
					result = FATAL_ERROR;
					TraceDBG(taskID,"the schedule has bugs, original state is,%d- %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				else
				{
					result = OK;
				}
				//this function could be trigger always by other task;
				//idle
				{
					tskState = SUB_STEP_INIT;
                    SendTskMsgLOC( MB_SUBSTEP, &localMsg);
				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case SUB_STEP_IDLE:
						//idle
						break;
					case SUB_STEP_INIT:
                        gFlowStep.subStepIdx = 0;
						gFlowStep.subStep = (uint16_t)(localMsg.val.value);
						flowStepRun[ID_SUB_STEP].duringTime = (uint32_t)(CalcDuringSecondsStep_WithDelay(gFlowStep.subStep));
						flowStepRun[ID_SUB_STEP].startTime = GetCurrentST();

						if(IN_RANGE(gFlowStep.subStep,e_valve_start, e_sub_step_end))
						{
							TraceMsg(taskID,"%3d, %s is called During time, %d S\n", \
                                gFlowStep.subStep, GetStepName(gFlowStep.subStep), flowStepRun[ID_SUB_STEP].duringTime);
							tskState = SUB_STEP_ACTION;
						}
						else
						{
							tskState = SUB_STEP_FINISH_ER;
						}
						SendTskMsgLOC( MB_SUBSTEP, &localMsg);
						break;
					case SUB_STEP_ACTION:

						if(UpdateSubStep(&gFlowStep)==OK)
						{
							SendTskMsg(MB_PRESUB, TSK_INIT, (uint32_t)gFlowStep.preSub, TskCallFin_Sub,NULL);
							tskState = SUB_STEP_ACTION_DELAY;
							tickOut = 1000 * CalcDuringSecondsStep_WithDelay(gFlowStep.preSub);
						}
						else
						{
							tskState = SUB_STEP_FINISH;
							SendTskMsgLOC( MB_SUBSTEP, &localMsg);
						}
						break;
					case SUB_STEP_ACTION_DELAY:
						tskState = SUB_STEP_ACTION;
						SendTskMsgLOC( MB_SUBSTEP, &localMsg);
						break;
					case SUB_STEP_FINISH_ER:
						TSK_FINISH_ACT(&localMsg, taskID, FATAL_ERROR, result);

						TraceDBG(taskID,"%3d, finished Error£º%d! \n",gFlowStep.subStep, result);
						tskState = SUB_STEP_IDLE;
						break;

					case SUB_STEP_FINISH:

						tskState = SUB_STEP_IDLE;
						TracePrint(taskID,"%3d, finished result, %d! \n",gFlowStep.subStep, result);
						TSK_FINISH_ACT(&localMsg,taskID,OK, result);
						break;
				}
			}
		}

	}
	/* USER CODE END StartSubStepTask */
}



