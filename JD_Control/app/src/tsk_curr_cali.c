/*
 * tsk_curr_cali.c
 *
 *  Created on: 2019Äê4ÔÂ13ÈÕ
 *      Author: pli
 */
#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_encoder.h"

static TSK_MSG locMsg;


static const char* taskStateDsp[] =
{
	TO_STR(CURR_CALI_IDLE),
	TO_STR(CURR_CALI_START),
	TO_STR(CURR_CALI_PREGAS),
	TO_STR(CURR_CALI_PREGAS_DELAY),
	TO_STR(CURR_CALI_ARC),
	TO_STR(CURR_CALI_ARC_DELAY),
	TO_STR(CURR_CALI_POSTGAS),
	TO_STR(CURR_CALI_POSTGAS_DELAY),
	TO_STR(CURR_CALI_FINISH),
};





void StartCurrCaliTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	CURR_CALI_STATE tskState = CURR_CALI_IDLE;
	uint32_t caliCurrCnt = 0;
	double currReadBack = 0;
	double currReadBackCnt = 0;
	const uint8_t taskID = TSK_ID_CURR_CALI;
	InitTaskMsg(&locMsg);
	TracePrint(taskID,"task started  \n");
	uint32_t tickStartArc;
	float caliOutputValue = 0;

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(CURR_CALI, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case CURR_CALI_IDLE:
				break;
			case CURR_CALI_PREGAS_DELAY:
				tskState = CURR_CALI_ARC;

				SendTskMsgLOC(CURR_CALI, &locMsg);
				break;
			case CURR_CALI_ARC_DELAY:
			{
				uint32_t delay = GetTickDuring(tickStartArc);
				if(weldCurr_Read > CURR_DETECT_LIMIT)
				{
					tskState = CURR_CALI_CURRENT;
					SendTskMsgLOC(CURR_CALI, &locMsg);
				}
				else if(weldProcess.preDelay*TIME_UNIT <= delay)
				{
					tskState = CURR_CALI_POSTGAS;
					SendTskMsgLOC(CURR_CALI, &locMsg);
					TraceDBG(taskID,"No arc detect in time: %d %s\n", delay, taskStateDsp[tskState]);
				}
				else
				{}
				tickOut = 20;

			}
				break;
			case CURR_CALI_CURRENT_DELAY:
				caliCurrCnt++;
				tskState = CURR_CALI_CURRENT_DELAY;
				if(caliCurrCnt % 20 == 1)
				{
					if(caliOutputValue < 9.99f)
					{
						caliOutputValue += currCaliSetDiff;
						if(caliOutputValue > 9.99f)
						{
							caliOutputValue = 10.0f;
						}
						SetCurrOutVolt(caliOutputValue);
						currReadBack = 0.0f;
					}
					else
					{
						//finish the calibration;
						currCaliReq = 0xFFFF;
						tskState = CURR_CALI_POSTGAS;
						SendTskMsgLOC(CURR_CALI, &locMsg);
					}
				}
				else if(caliCurrCnt % 20 >= 4)
				{
					currReadBack += weldCurr_Read;
					currReadBackCnt +=1.0f;
				}
				else if(caliCurrCnt % 20 == 0)
				{
					CaliCurrent curr = {caliOutputValue,(float)(currReadBack/currReadBackCnt), 0x33};
					WeldPut(OBJ_IDX_CURR_CALI_NEW,0, &curr.outValue);
					WeldPut(OBJ_IDX_CURR_CALI_NEW,1, &curr.actCurrent);
					WeldPut(OBJ_IDX_CURR_CALI_NEW,2, &curr.caliFlag);
				}

				tickOut = 50;
				break;
			case CURR_CALI_POSTGAS_DELAY:
				tskState = CURR_CALI_FINISH;
				SendTskMsgLOC(CURR_CALI, &locMsg);
				break;
			default:
				tskState = CURR_CALI_IDLE;

				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_INIT)
			{
				//idle
				//this function could be trigger always by other task;

				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				tskState = CURR_CALI_START;
				SendTskMsgLOC(CURR_CALI, &locMsg);
				caliCurrCnt = 0;
				currReadBack = 0;
				currReadBackCnt = 0;
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
				case CURR_CALI_IDLE:
					break;

				case CURR_CALI_START:
					tskState = CURR_CALI_PREGAS;
					SendTskMsgLOC(CURR_CALI, &locMsg);
					break;
				case CURR_CALI_PREGAS:
					OutPutPins_Call(CHN_OUT_GAS, 1);
					OutPutPins_Call(CHN_OUT_AD_CUT,0);
					tskState = CURR_CALI_PREGAS_DELAY;
					tickOut = weldProcess.preGasTime*TIME_UNIT;
					break;
				case CURR_CALI_ARC:
					OutPutPins_Call(CHN_OUT_GAS, 1);
					OutPutPins_Call(CHN_OUT_AD_CUT,0);
					OutPutPins_Call(CHN_OUT_ARC_ON, 1);
					SetSpeedOutVolt(0);
					OutputCurrent(weldProcess.preCurr);

					tickStartArc = HAL_GetTick();
					tskState = CURR_CALI_ARC_DELAY;
					break;
				case CURR_CALI_CURRENT:
					{
						uint16_t val = 1;
						tskState = CURR_CALI_CURRENT_DELAY;
						WeldPut(OBJ_IDX_CURR_CALI_CLR,0, &val);
						caliCurrCnt = 0;
						currReadBack = 0;
						currReadBackCnt = 0;
						tickOut = 0;
					}
					break;
				case CURR_CALI_POSTGAS:
					SetCurrOutVolt(0);
					OutPutPins_Call(CHN_OUT_AD_CUT,	0);
					tskState = CURR_CALI_POSTGAS_DELAY;
					tickOut = currCaliPreGas*TIME_UNIT;
					break;
				case CURR_CALI_FINISH:
					SetCurrOutVolt(0);
					OutPutPins_Call(CHN_OUT_GAS, 0);
					OutPutPins_Call(CHN_OUT_AD_CUT,	0);
					tskState = CURR_CALI_IDLE;
					TSK_FINISH_ACT(&locMsg,taskID,OK,OK);

					break;
				default:
					tskState = CURR_CALI_IDLE;
					break;
				}
			}
			else
			{
				tskState = CURR_CALI_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(CURR_CALI, &locMsg);
			}

		}
	}
}
