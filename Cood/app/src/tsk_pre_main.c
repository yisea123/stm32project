/*
 * tsk_pre_main.c
 *
 *  Created on: 2018年3月29日
 *      Author: pli
 */



/*
 * tsk_main_step.c
 *
 *  Created on: 2018年3月27日
 *      Author: pli
 */



#include "includes.h"

typedef enum
{
	PRE_MAIN_IDLE,
	PRE_MAIN_INIT,
	PRE_MAIN_MAINSTEP,
	PRE_MAIN_MAINSTEP_DELAY,
	//	PRE_MAIN_MAINSTEP_INIT,
	PRE_MAIN_FINISH,
	PRE_MAIN_FINISH_ER,
}PRE_MAIN_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(PRE_MAIN_IDLE),
	TO_STR(PRE_MAIN_INIT),
	TO_STR(PRE_MAIN_MAINSTEP),
	TO_STR(PRE_MAIN_MAINSTEP_DELAY),
	TO_STR(PRE_MAIN_FINISH),
	TO_STR(PRE_MAIN_FINISH_ER),

};
static TSK_MSG  localMsg;


static uint16_t UpdatePreMainStep(FlowStepInfo* flowStep)
{
	uint16_t ret = FATAL_ERROR;
	if(IN_RANGE(flowStep->preMain, e_pre_main_start, e_pre_main_end))
	{
		//lint -e685 -e568
		flowStep->subStep = preMainSteps[flowStep->preMain - e_pre_main_start][flowStep->preMainIdx];
		flowStep->preMainIdx++;
	}
	else
	{
        if(flowStep->preMainIdx == 0)
        {
            flowStep->preMainIdx++;
            flowStep->subStep = flowStep->preMain;
        }
        else
        {
            flowStep->subStep = 0;
        }

	}

	if(IN_RANGE(flowStep->subStep, e_valve_start, e_sub_step_end))
	{
		ret = OK;
	}
    else if(flowStep->subStep > e_sub_step_end)
    {
        TraceDBG(TSK_ID_PREMAIN,"premain step call substep error, %d > %d! \n",flowStep->subStep, e_sub_step_end);
    }
    else
    {}
	return ret;
}
static void TskCallFin_PreMain(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC( MB_PREMAIN, &localMsg);
}


static void TskUpdate_PreMain(uint16_t ret, uint16_t val)
{
	SendTskMsg(MB_PREMAIN, TSK_RENEW_STATUS , (uint32_t)0, NULL,NULL);
}




/* StartPreMainTask function */
void StartPreMainTask(void const * argument)
{
	(void)argument;
	osEvent event;
	uint32_t tickOut = osWaitForever;
	PRE_MAIN_STATE tskState = PRE_MAIN_IDLE;
	//lint -e550
	const uint8_t taskID = TSK_ID_PREMAIN;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	localMsg.val.value = 0;
//	TracePrint(taskID,"started  \n");
	uint16_t result = OK;;

	while(1)
	{
		event = osMessageGet(MB_PREMAIN, tickOut);
		//		tickOut = osWaitForever;
		if (event.status != osEventMessage)//timeout
		{
			TracePrint(taskID, "Timeout, %d,%s, Time,%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
				case PRE_MAIN_MAINSTEP_DELAY:
					tskState = PRE_MAIN_MAINSTEP;
					SendTskMsgLOC( MB_PREMAIN, &localMsg);
					break;
				default:
					tskState = PRE_MAIN_IDLE;
					SendTskMsgLOC( MB_PREMAIN, &localMsg);
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
				tskState = PRE_MAIN_FINISH;
                SendTskMsgLOC( MB_PREMAIN, &localMsg);
				SendTskMsg(MB_SUBSTEP, TSK_FORCE_BREAK,(uint32_t)0, NULL,NULL);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
                tskState = PRE_MAIN_FINISH;
                SendTskMsgLOC( MB_PREMAIN, &localMsg);
				SendTskMsg(MB_SUBSTEP, TSK_RESETIO,(uint32_t)0, NULL,NULL);
			}
			else if( mainTskState == TSK_INIT)
			{
				if(tskState != PRE_MAIN_IDLE)
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
					tskState = PRE_MAIN_INIT;
                    SendTskMsgLOC( MB_PREMAIN, &localMsg);

				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case PRE_MAIN_IDLE:
						//idle
						break;
					case PRE_MAIN_INIT:

						gFlowStep.preMain = (uint16_t)(localMsg.val.value);
						flowStepRun[ID_PRE_MAIN].duringTime = (uint32_t)(CalcDuringSecondsStep_WithDelay(gFlowStep.preMain));
						flowStepRun[ID_PRE_MAIN].startTime = GetCurrentST();
                        gFlowStep.preMainIdx = 0;
						if(IN_RANGE(gFlowStep.preMain,e_valve_start, e_pre_main_end))
						{
							TraceMsg(taskID,"%d, %s is executed time, %d S\n", \
                                gFlowStep.preMain, GetStepName(gFlowStep.preMain), flowStepRun[ID_PRE_MAIN].duringTime);
							tskState = PRE_MAIN_MAINSTEP;
						}
						else
						{
							tskState = PRE_MAIN_FINISH;
						}
						SendTskMsgLOC( MB_PREMAIN, &localMsg);
						break;
					case PRE_MAIN_MAINSTEP:

						if(UpdatePreMainStep(&gFlowStep)==OK)
						{
							SendTskMsg(MB_SUBSTEP, TSK_INIT, (uint32_t)gFlowStep.subStep, TskCallFin_PreMain,NULL);
							tskState = PRE_MAIN_MAINSTEP_DELAY;
							tickOut = 1000 * CalcDuringSecondsStep_WithDelay(gFlowStep.subStep);
						}
						else
						{
							tskState = PRE_MAIN_FINISH;
							SendTskMsgLOC( MB_PREMAIN, &localMsg);
						}
						break;
					case PRE_MAIN_MAINSTEP_DELAY:
						tskState = PRE_MAIN_MAINSTEP;
						SendTskMsgLOC( MB_PREMAIN, &localMsg);
						break;
					case PRE_MAIN_FINISH_ER:
						TSK_FINISH_ACT(&localMsg,taskID,FATAL_ERROR,result);
						TraceDBG(taskID,"%3d, finished Error,%d! \n",gFlowStep.preMain, result);
						tskState = PRE_MAIN_IDLE;
						break;

					case PRE_MAIN_FINISH:

						tskState = PRE_MAIN_IDLE;
						TracePrint(taskID,"%3d, finished result, %d! \n",gFlowStep.preMain,result);
						TSK_FINISH_ACT(&localMsg,taskID,OK,result);
						break;
				}
			}
		}

	}
	/* USER CODE END StartPreMainTask */
}





