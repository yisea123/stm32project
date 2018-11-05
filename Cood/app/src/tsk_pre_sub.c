/*
 * tsk_pre_sub.c
 *
 *  Created on: 2018年3月29日
 *      Author: pli
 */

/*
 * tsk_sub_step.c
 *
 *  Created on: 2018年3月27日
 *      Author: pli
 */




#include "includes.h"


typedef enum
{
	PRE_SUB_IDLE,
	PRE_SUB_INIT,
	PRE_SUB_ACTION,
	PRE_SUB_ACTION_DELAY,
	//	PRE_SUB_ACTION_INIT,
	PRE_SUB_FINISH,
	PRE_SUB_FINISH_ER,
}PRE_SUB_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(PRE_SUB_IDLE),
	TO_STR(PRE_SUB_INIT),
	TO_STR(PRE_SUB_ACTION),
	TO_STR(PRE_SUB_ACTION_DELAY),
	TO_STR(PRE_SUB_FINISH),
	TO_STR(PRE_SUB_FINISH_ER),

};

static TSK_MSG  localMsg;

static uint16_t UpdatePreSubStep(FlowStepInfo* flowStep)
{
	uint16_t ret = FATAL_ERROR;
	if(IN_RANGE(flowStep->preSub, e_pre_sub_start, e_pre_sub_end))
	{
		//lint -e685 -e568
		flowStep->microStep = preSubSteps[flowStep->preSub - e_pre_sub_start][flowStep->preSubIdx];
		flowStep->preSubIdx++;
	}
	else
	{
        if(flowStep->preSubIdx == 0)
        {
            flowStep->preSubIdx++;
            flowStep->microStep = flowStep->preSub;
        }
        else
        {
            flowStep->microStep = 0;
        }
	}

	if(IN_RANGE(flowStep->microStep, e_valve_start, e_micro_step_end))
	{
		ret = OK;
	}
    else if(flowStep->microStep > e_micro_step_end)
    {
        TraceMsg(TSK_ID_PRESUB,"presub step call microstep error:%d > %d! \n",flowStep->microStep, e_micro_step_end);
    }
    else
    {}
	return ret;
}
static void TskCallFin_PreSub(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC( MB_PRESUB, &localMsg);
}


static void TskUpdate_PreSub(uint16_t ret, uint16_t val)
{
	SendTskMsg(MB_PRESUB, TSK_RENEW_STATUS , (uint32_t)0, NULL,NULL);
}




/* StartPreSubTask function */
void StartPreSubTask(void const * argument)
{
	(void)argument;
	osEvent event;
	uint32_t tickOut = osWaitForever;
	PRE_SUB_STATE tskState = PRE_SUB_IDLE;
	//lint -e550
	const uint8_t taskID = TSK_ID_PRESUB;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	localMsg.val.value = 0;
//	TracePrint(taskID,"started  \n");
	uint16_t result = OK;;

	while(1)
	{
		event = osMessageGet(MB_PRESUB, tickOut);
		//		tickOut = osWaitForever;
		if (event.status != osEventMessage)//timeout
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
				case PRE_SUB_ACTION_DELAY:
					tskState = PRE_SUB_ACTION;
					SendTskMsgLOC( MB_PRESUB, &localMsg);
					break;
				default:
					tskState = PRE_SUB_IDLE;
					SendTskMsgLOC( MB_PRESUB, &localMsg);
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
				tskState = PRE_SUB_FINISH;
                SendTskMsgLOC( MB_PRESUB, &localMsg);
				SendTskMsg(MB_MICROSTEP, TSK_FORCE_BREAK,(uint32_t)0, NULL,NULL);
			}
			else if(TSK_RESETIO == mainTskState)
			{
                tskState = PRE_SUB_FINISH;
                SendTskMsgLOC( MB_PRESUB, &localMsg);
				SendTskMsg(MB_MICROSTEP, TSK_RESETIO,(uint32_t)0, NULL,NULL);
			}
			else if( mainTskState == TSK_INIT)
			{
				if(tskState != PRE_SUB_IDLE)
				{
					//do nothing;
					//todo may not be right
					result = FATAL_ERROR;
					TraceDBG(taskID,"the schedule has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				else
				{
					result = OK;
				}
				//this function could be trigger always by other task;
				//idle
				{
					tskState = PRE_SUB_INIT;
                    SendTskMsgLOC( MB_PRESUB, &localMsg);

				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case PRE_SUB_IDLE:
						//idle
						break;
					case PRE_SUB_INIT:
                        
						gFlowStep.preSub = (uint16_t)(localMsg.val.value);
						flowStepRun[ID_PRE_SUB].duringTime = (uint32_t)(CalcDuringSecondsStep_WithDelay(gFlowStep.preSub));
						flowStepRun[ID_PRE_SUB].startTime = GetCurrentST();
                        gFlowStep.preSubIdx = 0;
                        
                        if((e_pump_initialize == gFlowStep.preSub) && (motorPos_Pps == 0) )
                        {
                        	//SendTskMsg(FLOW_VALVE_CTL_ID, TSK_FORCE_BREAK, (uint32_t )0, NULL, NULL);
                       // 	Hal_Delay(2000);
                            TraceMsg(taskID,"%3d, %s is called But no need to execute\n", \
                                gFlowStep.preSub, GetStepName(gFlowStep.preSub));
                            tskState = PRE_SUB_FINISH;
                        }
						else if(IN_RANGE(gFlowStep.preSub,e_valve_start, e_sub_step_end))
						{
                            
							TraceMsg(taskID,"%3d, %s is called During time, %d S\n", \
                                gFlowStep.preSub, GetStepName(gFlowStep.preSub), flowStepRun[ID_PRE_SUB].duringTime);
							tskState = PRE_SUB_ACTION;
						}
						else
						{
							tskState = PRE_SUB_FINISH_ER;
						}
						SendTskMsgLOC( MB_PRESUB, &localMsg);
						break;
					case PRE_SUB_ACTION:
                        
						if(UpdatePreSubStep(&gFlowStep)==OK)
						{
							SendTskMsg(MB_MICROSTEP, TSK_INIT, (uint32_t)gFlowStep.microStep, TskCallFin_PreSub,NULL);
							tskState = PRE_SUB_ACTION_DELAY;
							tickOut = 1000 * CalcDuringSecondsStep_WithDelay(gFlowStep.microStep);
						}
						else
						{
							tskState = PRE_SUB_FINISH;
							SendTskMsgLOC( MB_PRESUB, &localMsg);
						}
						break;
					case PRE_SUB_ACTION_DELAY:
						tskState = PRE_SUB_ACTION;
						SendTskMsgLOC( MB_PRESUB, &localMsg);
						break;
					case PRE_SUB_FINISH_ER:
						TSK_FINISH_ACT(&localMsg,taskID,FATAL_ERROR,result);
                        TraceMsg(taskID,"%3d, finished Error,%d! \n",gFlowStep.preSub, result);
						tskState = PRE_SUB_IDLE;
						break;

					case PRE_SUB_FINISH:

						tskState = PRE_SUB_IDLE;
						TraceMsg(taskID,"%3d, finished result, %d! \n",gFlowStep.preSub,result);
						TSK_FINISH_ACT(&localMsg,taskID,OK,result);
						break;
				}
			}
		}

	}
	/* USER CODE END StartPreSubTask */
}






