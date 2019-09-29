/*
 * tsk_weld.c
 *
 *  Created on: 2019��3��27��
 *      Author: pli
 */

#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_encoder.h"



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
	SigPush(outputTaskHandle, DO_OUT_REFRESH);
}
static TSK_MSG locMsg;
static void HomeFinish(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC(WELD_CTRL, &locMsg);
}

void OutPutPins_Call(uint16_t pinChn, uint16_t val)
{
	if(val)
	{
		digitOutput |= 1<<pinChn;
	}
	else
	{
		digitOutput &= ~(1<<pinChn);
	}
	SigPush(outputTaskHandle, DO_OUT_REFRESH);
}

static void VoltMonitorChk(void)
{

	if(weldState <= ST_WELD_PRE_GAS_DELAY)
	{
		OutPutPins_Call(CHN_OUT_AD_CUT,1);
	}
	else if(ST_WELD_ARC_ON == weldState  ||  weldState == ST_WELD_ARC_ON_DELAY)
	{
		OutPutPins_Call(CHN_OUT_AD_CUT,0);
	}
	else
	{
		OutPutPins_Call(CHN_OUT_AD_CUT,1);
	}
}
static ST_WELD_STATE GetStateRequest(ST_WELD_STATE tskState)
{

	if(weldState > ST_WELD_INITPARA)
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


void StartWeldTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	ST_WELD_STATE tskState = ST_WELD_IDLE;

	const uint8_t taskID = TSK_ID_WELD;
	uint32_t tickStartArc = 0;
	uint32_t tickStart;
	uint32_t tickPostGasStart = 0;
	InitTaskMsg(&locMsg);
	TracePrint(taskID,"task started  \n");
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		weldState = tskState;
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
			{
				uint32_t delay = GetTickDuring(tickStart);
				if(delay >=	weldProcess.preGasTime*TIME_UNIT)
				{
					tskState = ST_WELD_ARC_ON;
					SendTskMsgLOC(WELD_CTRL, &locMsg);
				}
				else
				{
					gasRemainTime = (weldProcess.preGasTime*TIME_UNIT - delay)/TIME_UNIT;
					tickOut = 200;
					tskState = ST_WELD_PRE_GAS_DELAY;
				}
			}
				break;
			case ST_WELD_ARC_ON_DELAY:
			{
				uint32_t delay = GetTickDuring(tickStartArc);
				if(simulateWeld == 0)
				{
					if(weldCurr_Read > CURR_DETECT_LIMIT)
					{
						tskState = ST_WELD_MOTION;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
					}
					else if(weldProcess.preDelay*TIME_UNIT <= delay)
					{
						tskState = ST_WELD_STOP;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						TraceDBG(taskID,"No arc detect in time: %d %s\n", delay, taskStateDsp[tskState]);
					}
					else
					{}
				}
				else
				{
					tskState = ST_WELD_MOTION;
					SendTskMsgLOC(WELD_CTRL, &locMsg);
				}
				tickOut = WELD_DELAY_TIME;

			}
				break;
			case ST_WELD_MOTION_CYC:
				ptrCurrWeldSeg = GetWeldSeg(motorPos_Read-motorPos_WeldStart);
				if(ptrCurrWeldSeg->state == 0)
				{
					tskState = ST_WELD_STOP;
					SendTskMsgLOC(WELD_CTRL, &locMsg);
				}
				tickOut = WELD_DELAY_TIME;
				break;
			case ST_WELD_POST_HOME_DELAY:
				TraceDBG(taskID,"wrong state: %d %s\n", tskState, taskStateDsp[tskState]);
				break;
			case ST_WELD_POST_GAS_DELAY:
			{
				uint32_t delay = GetTickDuring(tickPostGasStart);
				if(delay >=	weldProcess.postGasTime*TIME_UNIT)
				{
					tskState = ST_WELD_FINISH;
					SendTskMsgLOC(WELD_CTRL, &locMsg);
				}
				else
				{
					gasRemainTime = (weldProcess.postGasTime*TIME_UNIT - delay)/TIME_UNIT;
					tickOut = 200;
					tskState = ST_WELD_POST_GAS_DELAY;
				}
			}
				break;
			default:
				tskState = ST_WELD_IDLE;
				SendTskMsgLOC(WELD_CTRL, &locMsg);
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
				SendTskMsgLOC(WELD_CTRL, &locMsg);
				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = ST_WELD_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(WELD_CTRL, &locMsg);
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
						motorPos_WeldStart = motorPos_Read;
						if(weldDir == MOTOR_DIR_CW)
						{
							motorPos_WeldStart = motorPos_Read;
							motorPos_WeldFinish = GetWeldFinishPos(0xFFFF) + motorPos_WeldStart;
						}
						else
						{
							motorPos_WeldStart = motorPos_Read;
							motorPos_WeldFinish = motorPos_WeldStart-GetWeldFinishPos(0xFFFF);
						}
						tskState = ST_WELD_PRE_GAS;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;
					case ST_WELD_PRE_GAS:
						digitOutput |= (1<<CHN_OUT_GAS);
						daOutputRawDA[CHN_DA_CURR_OUT] = 0;
						daOutputRawDA[CHN_DA_SPEED_OUT] = 0;
						SigPush(outputTaskHandle, (DA_OUT_REFRESH_SPEED|DA_OUT_REFRESH_CURR|DO_OUT_REFRESH));
						tickStart = HAL_GetTick();
						tickOut = 200;
						tskState = ST_WELD_PRE_GAS_DELAY;
						break;
					case ST_WELD_PRE_GAS_DELAY:
						tskState = ST_WELD_ARC_ON;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;

					case ST_WELD_ARC_ON:
						SetSpeedOutVolt(0);
						OutputCurrent(weldProcess.preCurr);
						if(simulateWeld == 0)
							OutPutPins_Call(CHN_OUT_ARC_ON, 1);
						tskState = ST_WELD_ARC_ON_DELAY;
						tickStartArc = HAL_GetTick();
						tickOut = WELD_DELAY_TIME;
						break;
					case ST_WELD_ARC_ON_DELAY:
						tskState = ST_WELD_MOTION;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;
					case ST_WELD_MOTION:
					{
						ptrCurrWeldSeg = GetWeldSeg(0);
						SendTskMsg(PWM_CTRL,TSK_INIT, 0, NULL, NULL);
						SendTskMsg(MOTOR_CTRL,TSK_INIT, ST_MOTOR_WELD_MOTION_START, NULL, NULL);
						tickOut = WELD_DELAY_TIME;//pwm time
						tskState = ST_WELD_MOTION_CYC;
					}
						break;
					case ST_WELD_STOP:
						//stop weld;
						tickPostGasStart = HAL_GetTick();
						tskState = ST_WELD_POST_HOME;
						SendTskMsg(MOTOR_CTRL,TSK_INIT, ST_MOTOR_WELD_MOTION_FINISH, NULL, NULL);
						SendTskMsg(PWM_CTRL,TSK_RESETIO, 0, NULL, NULL);
						SendTskMsgLOC(WELD_CTRL, &locMsg);

						break;
					case ST_WELD_POST_HOME:
						//send to motor task to home
						SendTskMsg(MOTOR_CTRL,TSK_INIT, ST_MOTOR_HOME, HomeFinish, NULL);
						SendTskMsg(PWM_CTRL,TSK_RESETIO, 0, NULL, NULL);
						tickOut = HOME_DELAY_TIME;
						break;
					case ST_WELD_POST_HOME_DELAY:
						TraceDBG(taskID,"wrong state: %d %s\n", tskState, taskStateDsp[tskState]);
						tskState = ST_WELD_POST_GAS;
						SendTskMsgLOC(WELD_CTRL, &locMsg);
						break;
					case ST_WELD_POST_GAS:
						//output gas;
					{
						tickOut = GetTickDuring(tickPostGasStart);
						if(tickOut < weldProcess.postGasTime*TIME_UNIT)
							tickOut = weldProcess.postGasTime*TIME_UNIT - tickOut;
						else
							tickOut = 0;
						tskState = ST_WELD_POST_GAS_DELAY;
					}
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
			VoltMonitorChk();
		}
	}
}
