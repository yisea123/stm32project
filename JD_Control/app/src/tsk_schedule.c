/*
 * tsk_schedule.c
 *
 *  Created on: 2019Äê4ÔÂ12ÈÕ
 *      Author: pli
 */

#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_encoder.h"



typedef enum
{
	ST_SCH_IDLE,
	ST_SCH_CALI,
	ST_SCH_CALI_DELAY,
	ST_SCH_WELD,
	ST_SCH_WELD_DELAY,
	ST_SCH_MANUAL,
	ST_SCH_MANUAL_DELAY,
	ST_SCH_FINISH,
}ST_SCH_STATE;




static const char* taskStateDsp[] =
{
	TO_STR(ST_SCH_IDLE),
	TO_STR(ST_SCH_CALI),
	TO_STR(ST_SCH_CALI_DELAY),
	TO_STR(ST_SCH_WELD),
	TO_STR(ST_SCH_WELD_DELAY),
	TO_STR(ST_SCH_MANUAL),
	TO_STR(ST_SCH_MANUAL_DELAY),
	TO_STR(ST_SCH_FINISH),
};

ST_SCH_STATE CheckWeldInput()
{
	ST_SCH_STATE state = 0;
	if(uiBtn_Weld != 0)
	{
		uiBtn_Weld = 0;
		state = ST_SCH_WELD;
	}
	else if(digitInputWeld == BTN_PUSHDOWN)
	{
		digitInputWeld |= BTN_CHKED;
		state = ST_SCH_WELD;
	}

	return state;
}


ST_SCH_STATE CheckAllInput()
{
	ST_SCH_STATE state = 0;
	if(devLock == 0)
	{
		if(weldState == ST_WELD_IDLE)
		{
			if(caliAllReq)
			{
				if(currCaliReq == 1)
				{
					//todo
					currCaliReq = 2;
					SendTskMsg(CURR_CALI,TSK_INIT, 0, NULL, NULL);
				}
				else if(currCaliReq == 0)
				{
					//todo
					SendTskMsg(CURR_CALI,TSK_FORCE_STOP, 0, NULL, NULL);
				}

				if(voltCaliReq == 1)
				{
					OutPutPins_Call(CHN_OUT_AD_CUT, 1);
				}
				else
				{
					OutPutPins_Call(CHN_OUT_AD_CUT, 0);
				}

				if(speedCaliReq)
				{
					SetSpeedOutVolt(speedCaliOutput);
					//todo
				}
				else
				{
					//todo
					SetSpeedOutVolt(0);
				}
			}
			else
			{
				//Accept UI
				if((uiBtn_JogP) || \
						((digitInput&(1<<CHN_IN_JOG_DIR))&&(digitInput & (1<<CHN_IN_JOG_ACT))))
				{
					uiBtn_JogP = 0;
					SendTskMsg(MOTOR_CTRL,TSK_INIT, ST_MOTOR_JOGP, NULL, NULL);
					//
				}
				else if((uiBtn_JogN) || \
						((0 == digitInput&(1<<CHN_IN_JOG_DIR))&&(digitInput & (1<<CHN_IN_JOG_ACT))))
				{
					uiBtn_JogN = 0;
					SendTskMsg(MOTOR_CTRL,TSK_INIT, ST_MOTOR_JOGN, NULL, NULL);
					//
				}
				if(digitInput&(1<<CHN_IN_GAS_ON))
				{
					OutPutPins_Call(CHN_OUT_GAS, 1);
				}
				else
				{
					OutPutPins_Call(CHN_OUT_GAS, 0);
				}
			}
		}
		else
		{
			//Accept Weld action only
			state = CheckWeldInput();
		}
	}

	return ST_SCH_IDLE;
}


static TSK_MSG locMsg;

void TskFinish(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC(WELD_CTRL, &locMsg);
}


#define SCH_DELAY_TIME		50
void StartSchTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	ST_SCH_STATE tskState = ST_SCH_IDLE;

	const uint8_t taskID = TSK_ID_SCH;
	InitTaskMsg(&locMsg);
	TracePrint(taskID,"task started  \n");
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_CTRL, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case ST_SCH_IDLE:
				tskState = CheckAllInput();
				if(tskState == ST_SCH_IDLE)
				{
					tickOut = SCH_DELAY_TIME;
				}
				else
				{
					SendTskMsgLOC(SCH_CTRL, &locMsg);
				}
				break;

			default:
				tskState = ST_SCH_IDLE;

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
				tskState = ST_SCH_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(SCH_CTRL, &locMsg);
				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = ST_SCH_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(SCH_CTRL, &locMsg);
			}
			else if ( mainTskState == TSK_INIT)
			{
				//idle
				//this function could be trigger always by other task;

				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				tskState = GetStateRequest(tskState);
				weldDir = MOTOR_DIR_CW;
				UpdateWeldFInishPos();
				SendTskMsgLOC(SCH_CTRL, &locMsg);
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
				case ST_SCH_IDLE:
					break;

				case ST_SCH_WELD:
					tickOut = 1800*1000;
					SendTskMsg(WELD_CTRL, TSK_INIT, 0, TskFinish, NULL);
					tskState = ST_SCH_WELD_DELAY;

					break;

				case ST_SCH_WELD_DELAY:
					tskState = ST_SCH_IDLE;

					break;
				case ST_SCH_FINISH:
					tskState = ST_SCH_IDLE;
					TSK_FINISH_ACT(&locMsg,taskID,OK,OK);
					break;
				default:
					tskState = ST_SCH_IDLE;
					break;
				}
			}

		}
		//call when IO task is executed;
		VoltMonitorChk();
	}
}


