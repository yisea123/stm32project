/*
 * tsk_mix_ctrl.c
 *
 *  Created on: 2017Äê10ÔÂ24ÈÕ
 *      Author: pli
 */
#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "tsk_substep.h"
#include "tsk_flow_all.h"
#include "tim.h"
#include "unit_sys_diagnosis.h"
#include "unit_statistics_data.h"
#include "tsk_motor_ctrl.h"

static uint16_t currMixPps = 0;
static const char* taskStateDsp[] =
{
	TO_STR(MIX_IDLE),
	TO_STR(MIX_INIT),
	TO_STR(MIX_CYCL_RUN),
	TO_STR(MIX_CYCL_RUN_RDY),
	TO_STR(MIX_CYCL_RUN_DELAY),
	TO_STR(MIX_CYCL_STOP),
	TO_STR(MIX_CYCL_STOP_RDY),
	TO_STR(MIX_CYCL_STOP_DELAY),
	TO_STR(MIX_ACC),
	TO_STR(MIX_ACC_DELAY),
	TO_STR(MIX_RUN),
	TO_STR(MIX_STOP),
	TO_STR(MIX_FINISH),
};



void MixAction(const uint16_t pps)
{
	static uint16_t oldpps = 0;
	static uint16_t mode = FTP;
	static uint32_t tickCount = 0;
	const uint16_t idx = IDX_MIXING;
	if (pps != 0)
	{
		SetMotorSpeed(idx, mixConfig.mixPps_Mode);
		SetMotorIdle(idx,MOTOR_WORK);

		UpdatePWMRegChn1(motorConfig[idx].ptrTim, CALC_PWM_REG(pps));

		HAL_TIM_PWM_Start(motorConfig[idx].ptrTim, motorConfig[idx].channel);
	}
	else
	{
		HAL_TIM_PWM_Stop(motorConfig[idx].ptrTim, motorConfig[idx].channel);
		SetMotorIdle(idx,MOTOR_STOP);
	}
	if(oldpps != 0)
	{
		tickCount = HAL_GetTick() - tickCount;
		if(mode == FTP)
			Sta_AddMixTime(tickCount/1000, oldpps);
		else
			Sta_AddMixTime(tickCount/1000, oldpps/2);
	}
	motorPPS[idx] = pps;
	oldpps = pps;
	currMixPps = pps;
	mode = mixConfig.mixPps_Mode;
	tickCount = HAL_GetTick();
	TracePrint(TSK_ID_MIX_CTRL, "Mix Pps: %d,\n",currMixPps);
}



void StartMixTask(void const * argument)
{
#define QUEUE_SIZE		4
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	TSK_MSG locMixMsg;
	uint16_t mixPps = 800;
	uint16_t ppsAcc = 100;
	uint16_t ppsStart = 300;
	uint16_t ppsAccDelay = 100;
	uint16_t mixMode = 0;
	MIX_STATE tskState = MIX_IDLE;
	MIX_STATE nextState = MIX_IDLE;
	const uint8_t taskID = TSK_ID_MIX_CTRL;


	locMixMsg.callBackFinish = NULL;
	InitMotorPins(IDX_MIXING,MOTOR_WORK);
	TracePrint(taskID,"started  \n");
	MixAction(0);

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_MIX_CTL_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case MIX_ACC_DELAY:
				tskState = nextState;
				if(mixConfig.mixPps_AccMode)
				{
					if(currMixPps < mixPps)
					{
						if(ppsStart > currMixPps)
						{
							tskState = MIX_ACC_DELAY;
							MixAction( ppsStart);
							tickOut = ppsAccDelay;
						}
						else if(mixPps - currMixPps >= ppsAcc)
						{
							MixAction( (uint16_t)(currMixPps + ppsAcc));
							tskState = MIX_ACC_DELAY;
							tickOut = ppsAccDelay;
						}
					}
					//acc to lower speed
					else
					{
						if(ppsStart < currMixPps)
						{
							if(currMixPps - mixPps >= ppsAcc)
							{
								MixAction( (uint16_t)(currMixPps - ppsAcc));
								tskState = MIX_ACC_DELAY;
								tickOut = ppsAccDelay;
							}
							else
							{
								MixAction( (uint16_t)(mixPps));
							}
						}
						else
						{
							MixAction( (uint16_t)(mixPps));
							tickOut = ppsAccDelay;
						}
					}
				}
				if(tskState == nextState)
				{
					MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &locMixMsg, 0);
				}
				break;
			case MIX_CYCL_RUN_DELAY:
				tskState = MIX_CYCL_STOP;
				MsgPush(FLOW_MIX_CTL_ID, (uint32_t ) &locMixMsg, 0);
				break;
			case MIX_CYCL_STOP_DELAY:
				tskState = MIX_CYCL_RUN;
				MsgPush(FLOW_MIX_CTL_ID, (uint32_t ) &locMixMsg, 0);
				break;
			default:
				tskState = MIX_IDLE;
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
				tskState = MIX_FINISH;
				MixAction(0);
				locMixMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMixMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_MIX_CTL_ID, (uint32_t ) &locMixMsg, 0);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = MIX_FINISH;
				MixAction(0);
				ResetStepMotor(IDX_MIXING);
				locMixMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMixMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_MIX_CTL_ID, (uint32_t ) &locMixMsg, 0);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if (tskState != MIX_IDLE)
				{
					//do nothing;
					//todo may not be right
				//	TraceDBG( taskID,"the schedule of MIX task have bugs!\n");
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{
					locMixMsg = *(TSK_MSG_CONVERT(event.value.p));
					if(locMixMsg.val.value == MIX_CYCL_RUN)
					{
						mixMode = MIX_CYCL_RUN;
						mixPps = mixConfig.mixPps_Idle;
					}
					else
					{
						mixMode = 0;
						mixPps = mixConfig.mixPps_Run;
					}
					ppsStart = mixConfig.mixPps_AccStartPps;
					ppsAcc = mixConfig.mixPps_AccPps;
					ppsAccDelay = mixConfig.mixPps_AccTime;
					if(ppsAcc <= 10)
						ppsAcc = 10;
					if(ppsAccDelay <= 5)
						ppsAccDelay = 5;
					tskState = MIX_INIT;
					locMixMsg.tskState = TSK_SUBSTEP;
					MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &locMixMsg, 0);
					//change to init
					//send msg to be running;
				}
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
					case MIX_IDLE:
					break;
					case MIX_INIT:
						TraceMsg( taskID,"mix pps: %d\n", mixPps);
						tskState = MIX_ACC;
						nextState = MIX_RUN;
						MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &locMixMsg, 0);
					break;
					case MIX_ACC:
						tskState = MIX_ACC_DELAY;
						tickOut = 0;
						break;
					case MIX_RUN:
					{
						MixAction(mixPps);
						if(MIX_CYCL_RUN == mixMode)
							tskState = MIX_CYCL_RUN;
						else
							tskState = MIX_FINISH;
						MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &locMixMsg, 0);
					}
					break;
					case MIX_STOP:
						mixPps = 0;
						MixAction(mixPps);
						nextState = MIX_FINISH;
						tskState = MIX_ACC;
						tickOut = 0;
					break;
					case MIX_FINISH:

					tskState = MIX_IDLE;
					if (locMixMsg.callBackFinish)
					{
						locMixMsg.callBackFinish(OK, 0);
					}

					//MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &mixMsg, 0);
					break;
					case MIX_CYCL_RUN:
						tskState = MIX_ACC;
						nextState = MIX_CYCL_RUN_RDY;
						mixPps = mixConfig.mixPps_Idle;
						MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &locMixMsg, 0);
					break;
					case MIX_CYCL_RUN_RDY:
						MixAction(mixPps);
						tskState = MIX_CYCL_RUN_DELAY;
						tickOut = (uint32_t)(mixConfig.mixIdle_RunTime* TIME_UNIT);
						break;
					case MIX_CYCL_STOP:
						mixPps = 0x0;
						tskState = MIX_ACC;
						nextState = MIX_CYCL_STOP_RDY;
						MsgPush(FLOW_MIX_CTL_ID, (uint32_t) &locMixMsg, 0);
					break;
					case MIX_CYCL_STOP_RDY:
						tskState = MIX_CYCL_STOP_DELAY;
						tickOut = (uint32_t)(mixConfig.mixIdle_IdleTime* TIME_UNIT);
					break;
					//shall not be reached for below cases!
					case MIX_CYCL_RUN_DELAY:
						tskState = MIX_CYCL_STOP;
						MsgPush(FLOW_MIX_CTL_ID, (uint32_t ) &locMixMsg, 0);
						break;
					case MIX_CYCL_STOP_DELAY:
						//tskState = MIX_CYCL_RUN;
						//need to acc;;
						tskState = MIX_INIT;
						MsgPush(FLOW_MIX_CTL_ID, (uint32_t ) &locMixMsg, 0);
						break;
					default:
						//MixAction(0x0);
						tskState = MIX_IDLE;
						break;

				}
				//when finish->call back;
			}
			else
			{
				//todo
			}
		}

	}
}

