/*
 * tsk_main_step.c
 *
 *  Created on: 2018Äê3ÔÂ27ÈÕ
 *      Author: pli
 */



#include "includes.h"

typedef enum
{
	MAIN_STEP_IDLE,
	MAIN_STEP_INIT,
	MAIN_STEP_MAINSTEP,
	MAIN_STEP_MAINSTEP_DELAY,
	//	MAIN_STEP_MAINSTEP_INIT,
	MAIN_STEP_FINISH,
	MAIN_STEP_FINISH_ER,
}MAIN_STEP_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(MAIN_STEP_IDLE),
	TO_STR(MAIN_STEP_INIT),
	TO_STR(MAIN_STEP_MAINSTEP),
	TO_STR(MAIN_STEP_MAINSTEP_DELAY),
	TO_STR(MAIN_STEP_FINISH),
	TO_STR(MAIN_STEP_FINISH_ER),

};

static TSK_MSG  localMsg;

static uint16_t UpdateMainStep(FlowStepInfo* flowStep)
{
	uint16_t ret = FATAL_ERROR;
	if(IN_RANGE(flowStep->mainStep, e_main_step_start, e_main_step_end))
	{
		//lint -e685 -e568
		flowStep->preMain = mainSteps[flowStep->mainStep - e_main_step_start][flowStep->mainStepIdx];
		flowStep->mainStepIdx++;
	}
	else
	{
        if(flowStep->mainStepIdx == 0)
        {
            flowStep->mainStepIdx++;
            flowStep->preMain = flowStep->mainStep;
        }
        else
        {
            flowStep->preMain = 0;
        }
        
	}

	if(IN_RANGE(flowStep->preMain, e_valve_start, e_pre_main_end))
	{
		ret = OK;
	}
    else if(flowStep->preMain > e_pre_main_end)
    {
        TraceDBG(TSK_ID_MAINSTEP,"main step call premain error,%d > %d! \n",flowStep->preMain, e_pre_main_end);
    }
    else
    {}
	return ret;
}

static void TskCallFin_Main(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC(MB_MAINSTEP, &localMsg);
}


static void TskUpdate_Main(uint16_t ret, uint16_t val)
{
	SendTskMsg(MB_MAINSTEP, TSK_RENEW_STATUS , (uint32_t)0, NULL,NULL);
}




/* StartMainStepTask function */
void StartMainStepTask(void const * argument)
{
	(void)argument;
	uint32_t tickOut = osWaitForever;
	MAIN_STEP_STATE tskState = MAIN_STEP_IDLE;
	//lint -e550
	osEvent event;
	const uint8_t taskID = TSK_ID_MAINSTEP;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	localMsg.val.value = 0;
//	TracePrint(taskID,"started  \n");
	uint16_t result = OK;;

	while(1)
	{
		event = osMessageGet(MB_MAINSTEP, tickOut );
		if( event.status != osEventMessage )
		{
			TracePrint(taskID, "Timeout, %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
				case MAIN_STEP_MAINSTEP_DELAY:
					tskState = MAIN_STEP_MAINSTEP;
					SendTskMsgLOC(MB_MAINSTEP, &localMsg);
					break;
				default:
					tskState = MAIN_STEP_IDLE;
					SendTskMsgLOC(MB_MAINSTEP, &localMsg);
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
                memset((void*)&gFlowStep,0,sizeof(gFlowStep));
				tskState = MAIN_STEP_FINISH;
                SendTskMsgLOC(MB_MAINSTEP, &localMsg);
				SendTskMsg(MB_PREMAIN, TSK_FORCE_BREAK,(uint32_t)0, NULL,NULL);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{   
                memset((void*)&gFlowStep,0,sizeof(gFlowStep));
                tskState = MAIN_STEP_FINISH;
                SendTskMsgLOC(MB_MAINSTEP, &localMsg);
				SendTskMsg(MB_PREMAIN, TSK_RESETIO,(uint32_t)0, NULL,NULL);
			}
			else if( mainTskState == TSK_INIT)
			{
				if(tskState != MAIN_STEP_IDLE)
				{
					//do nothing;
					//todo may not be right
					result = FATAL_ERROR;
					TraceDBG(taskID,"the schedule has bugs, original state is,%d, %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				else
				{
					InitCaliIndexValue();
					result = OK;
				}
				//this function could be trigger always by other task;
				//idle
				{
                    
                    memset((void*)&gFlowStep,0,sizeof(gFlowStep));
					tskState = MAIN_STEP_INIT;
                    SendTskMsgLOC(MB_MAINSTEP, &localMsg);
					
				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case MAIN_STEP_IDLE:
						//idle
						break;
					case MAIN_STEP_INIT:
                        gFlowStep.mainStepIdx = 0;
						gFlowStep.mainStep = (uint16_t)(localMsg.val.value);
						flowStepRun[ID_MAIN_STEP].duringTime = (uint32_t)(CalcDuringSecondsStep_WithDelay(gFlowStep.mainStep));
						flowStepRun[ID_MAIN_STEP].startTime = GetCurrentST();
                        
						if(IN_RANGE(gFlowStep.mainStep,e_valve_start, e_main_step_end))
						{
							//main step
							TraceMsg(taskID,"%3d, %s is called During time, %d S\n", gFlowStep.mainStep, GetStepName(gFlowStep.mainStep), flowStepRun[ID_MAIN_STEP].duringTime);
							tskState = MAIN_STEP_MAINSTEP;
						}						
						else
						{
							tskState = MAIN_STEP_FINISH;
						}
						SendTskMsgLOC(MB_MAINSTEP, &localMsg);
						break;
					case MAIN_STEP_MAINSTEP:

						if(UpdateMainStep(&gFlowStep)==OK)
						{
							SendTskMsg(MB_PREMAIN, TSK_INIT, (uint32_t)gFlowStep.preMain, TskCallFin_Main,NULL);
							tskState = MAIN_STEP_MAINSTEP_DELAY;
							tickOut = 1000 * CalcDuringSecondsStep_WithDelay(gFlowStep.preMain);
						}
						else
						{
							tskState = MAIN_STEP_FINISH;
							SendTskMsgLOC(MB_MAINSTEP, &localMsg);
						}
						break;
					case MAIN_STEP_MAINSTEP_DELAY:
						tskState = MAIN_STEP_MAINSTEP;
						SendTskMsgLOC(MB_MAINSTEP, &localMsg);
						break;
					case MAIN_STEP_FINISH_ER:
						TSK_FINISH_ACT(&localMsg,taskID,FATAL_ERROR,result);
						TraceDBG(taskID,"%3d, finished Error,%d! \n",gFlowStep.mainStep, result);
						tskState = MAIN_STEP_IDLE;
						break;

					case MAIN_STEP_FINISH:

						tskState = MAIN_STEP_IDLE;
						TraceMsg(taskID,"%3d, finished result, %d! \n",gFlowStep.mainStep, result);
						TSK_FINISH_ACT(&localMsg,taskID,OK,result);
						memset((void*)&gFlowStep,0,sizeof(gFlowStep));
						break;
				}
			}
		}

	}
	/* USER CODE END StartMainStepTask */
}




