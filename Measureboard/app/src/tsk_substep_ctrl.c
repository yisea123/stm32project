/*
 * tsk_substep_ctrl.c
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */


#include <string.h>
#include "main.h"
#include "tsk_flow_all.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_temp_cfg.h"
#include "dev_temp.h"
#include "tsk_substep.h"
#include "tsk_measure.h"
#include "unit_flow_act.h"
#include "unit_meas_cfg.h"
#include "unit_meas_data.h"
#define SUB_STEP_SIG		0x02
#define VALVE_MAX_DELAY		(300+100+20)//(300)


extern uint16_t measRawValueTimes;


typedef enum
{
	SUBSTEP_IDLE,
	SUBSTEP_INIT,//1
	SUBSTEP_VALVE,//2
	SUBSTEP_VALVE_DELAY,//3
	SUBSTEP_MIX,
	SUBSTEP_MOTOR,//5
	SUBSTEP_MOTOR_DELAY,//6
	SUBSTEP_TIME,//7
	SUBSTEP_TIME_DELAY,
	SUBSTEP_MEAS,//9
	SUBSTEP_MEAS_DELAY,
	SUBSTEP_HEAT,//11
	SUBSTEP_HEAT_DELAY,
	SUBSTEP_ACT,//13
	SUBSTEP_ACT_DELAY,
	SUBSTEP_MEAS_BLANK,
	SUBSTEP_TEST_REF,
	SUBSTEP_TEST_REF_DELAY,
	SUBSTEP_FINISH,
}SUBSTEP_STATE;


static const char* taskStateDsp[] =
{
	TO_STR(SUBSTEP_IDLE),
	TO_STR(SUBSTEP_INIT),
	TO_STR(SUBSTEP_VALVE),
	TO_STR(SUBSTEP_VALVE_DELAY),
	TO_STR(SUBSTEP_MIX),
	TO_STR(SUBSTEP_MOTOR),
	TO_STR(SUBSTEP_MOTOR_DELAY),
	TO_STR(SUBSTEP_TIME),
	TO_STR(SUBSTEP_TIME_DELAY),
	TO_STR(SUBSTEP_MEAS),
	TO_STR(SUBSTEP_MEAS_DELAY),
	TO_STR(SUBSTEP_HEAT),
	TO_STR(SUBSTEP_HEAT_DELAY),
	TO_STR(SUBSTEP_ACT),
	TO_STR(SUBSTEP_ACT_DELAY),
	TO_STR(SUBSTEP_MEAS_BLANK),
	TO_STR(SUBSTEP_TEST_REF),
	TO_STR(SUBSTEP_TEST_REF_DELAY),
	TO_STR(SUBSTEP_FINISH),
};

extern __IO uint16_t blankMeasure;

void StartSubStepTask(void const * argument);


static void TskCallBackLoc(uint32_t idx)
{
	(void)idx;
	TracePrint(TSK_ID_SUBSTEP_CTRL,"substep callback %d  \n",idx);
	SendTskMsg(FLOW_SUBSTEP_ID, TSK_SUBSTEP, 0, NULL);
}



void StartSubStepTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	SUBSTEP_STATE tskState = SUBSTEP_IDLE;

	uint32_t subStepId = 0;
	uint32_t timeDelay = 0;
	TSK_MSG localMsg;
	const uint8_t taskID = TSK_ID_SUBSTEP_CTRL;
	localMsg.tskState = TSK_SUBSTEP;
	TracePrint(taskID,"started  \n");
	while(TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_SUBSTEP_ID, tickOut );

	//	tickOut = osWaitForever;
		if(event.status != osEventMessage )
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{

			case SUBSTEP_TEST_REF_DELAY:
				CheckLedError();
				UpdateGainSetting(0);
				tskState = SUBSTEP_FINISH;
				MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
				break;

			case SUBSTEP_TIME_DELAY:
			case SUBSTEP_ACT_DELAY:
			case SUBSTEP_HEAT_DELAY:
				tskState = SUBSTEP_FINISH;
				MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
				break;
			case SUBSTEP_MEAS_DELAY:

				SubStep_CalcAndStoreRawMeasAbs();
				measResultPrint &= MEASURE_DATA_INVALID_MSK;
				measRawValueTimes = 0;

				tskState = SUBSTEP_FINISH;
				MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
				break;
			default:
				TraceDBG(taskID,"bugs about time out, original state is:%d->%s!\n",tskState,taskStateDsp[tskState]);
				tskState = SUBSTEP_IDLE;
				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if(mainTskState == TSK_FORCE_BREAK)
			{
				tskState = SUBSTEP_FINISH;
				measResultPrint &= MEASURE_DATA_INVALID_MSK;
				SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_FORCE_BREAK, 0, NULL);
				SendTskMsg(FLOW_MIX_CTL_ID, TSK_FORCE_BREAK, 0, NULL);
				SendTskMsg(FLOW_VALVE_CTL_ID, TSK_FORCE_BREAK, 0, NULL);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
				blankMeasure = BLANK_NONE;
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
				tskState = SUBSTEP_FINISH;
				measResultPrint &= MEASURE_DATA_INVALID_MSK;
				SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_RESETIO, 0, NULL);
				SendTskMsg(FLOW_MIX_CTL_ID, TSK_RESETIO, 0, NULL);
				SendTskMsg(FLOW_VALVE_CTL_ID, TSK_RESETIO, 0, NULL);

				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
				blankMeasure = BLANK_NONE;
			}
			else if(mainTskState == TSK_INIT)
			{

				if(tskState != SUBSTEP_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is:%d->%s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{
					subStepId = TSK_MSG_CONVERT(event.value.p)->val.value;

					tskState = SUBSTEP_INIT;
					localMsg = *(TSK_MSG_CONVERT(event.value.p));

					localMsg.tskState = TSK_SUBSTEP;
					MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					//change to init
					//send msg to be running;
				}
				measResultPrint &= MEASURE_DATA_INVALID_MSK;
				flowStepRun[2].step = subStepId;
				flowStepRun[2].duringTime = (CalcDuringTimeMsStep_WithDelay((uint16_t)subStepId)+500)/1000;
				flowStepRun[2].startTime = GetCurrentST();
			}
			else if(mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
				case SUBSTEP_IDLE:
					break;
				case SUBSTEP_INIT:

					TraceMsg(taskID,"%03d ** %s ** is called, During Time: %d s\n", subStepId, GetStepName((uint16_t)subStepId), flowStepRun[2].duringTime);


					if(IN_RANGE(subStepId,STEP_0, STEP_MAX))
					{
						tskState = SUBSTEP_VALVE;
//						exMsg.tskState = TSK_INIT;
//						exMsg.val.p = (void*) &allStepsConfig[subStepId-STEP_0];
//						exMsg.callBack = TskCallBackLoc;
					}
					else if(IN_RANGE(subStepId,TIME_0, TIME_MAX))
					{
						timeDelay = timeConfig[subStepId-TIME_0];
						if(timeConfig[subStepId-TIME_0])
						{
							tskState = SUBSTEP_TIME;
						}
						else
						{
							tskState = SUBSTEP_FINISH;
						}
					}
					else if(IN_RANGE(subStepId,SPECIAL_IDX_0, SPECIAL_IDX_MAX))
					{
						timeDelay = specialConfig[subStepId-SPECIAL_IDX_0];
						switch(subStepId)
						{
						case TEST_LED_REF:
							tskState = SUBSTEP_TEST_REF;
							break;
						case BLANK_MEAS:
							tskState = SUBSTEP_MEAS_BLANK;
							break;
						case MEASUREMENT:
							tskState = SUBSTEP_MEAS;
							break;
						case PREHEATING:
							SendTskMsg(TEMPERATURE_ID,TSK_INIT, TEMP_PREHEAT_CMD, NULL);
							tskState = SUBSTEP_HEAT;
							break;
						case HEATING_CTRL:
							tskState = SUBSTEP_HEAT;
							SendTskMsg(TEMPERATURE_ID,TSK_INIT, TEMP_CTRL_CMD, NULL);
							break;
						case HEATING_STOP:
							tskState = SUBSTEP_HEAT;
							SendTskMsg(TEMPERATURE_ID,TSK_INIT, TEMP_STOP_CMD, NULL);
							break;
						case PUMP_RESET:
							SendTskMsg(FLOW_MIX_CTL_ID,TSK_RESETIO, 0, NULL);
							SendTskMsg(FLOW_MOTOR_CTL_ID,TSK_RESETIO, 0, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case VALVE_STOP:
							SendTskMsg(FLOW_VALVE_CTL_ID,TSK_FORCE_BREAK, 0, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case PUMP_ENABLE:
							SendTskMsg(FLOW_VALVE_CTL_ID,TSK_FORCE_ENABLE, 0, NULL);
							SendTskMsg(FLOW_MOTOR_CTL_ID,TSK_FORCE_ENABLE, 0, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case PUMP_DISABLE:
							SendTskMsg(FLOW_MOTOR_CTL_ID,TSK_FORCE_DISABLE, 0, NULL);
							SendTskMsg(FLOW_VALVE_CTL_ID,TSK_FORCE_DISABLE, 0, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case PUMP_STOP:
							SendTskMsg(FLOW_MOTOR_CTL_ID,TSK_FORCE_BREAK, 0, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case MIXTURE_STOP:
							SendTskMsg(FLOW_MIX_CTL_ID,TSK_FORCE_BREAK, 0, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case MIXTURE_RUN:
							SendTskMsg(FLOW_MIX_CTL_ID,TSK_INIT, MIX_RUN, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case MIXTURE_CYC:
							SendTskMsg(FLOW_MIX_CTL_ID,TSK_INIT, MIX_CYCL_RUN, NULL);
							tskState = SUBSTEP_ACT;
							break;
						case BLANK_CALC_START:
							blankMeasure = BLANK_ENABLE1;
							blankSettingUsed = blankSetting;
							tskState = SUBSTEP_ACT;
							break;
						case BLANK_HOLD:
							blankMeasure = BLANK_DIABLE;
							tskState = SUBSTEP_ACT;
							break;
						case BLANK_CONTINUE:
							blankMeasure = BLANK_ENABLE2;
							tskState = SUBSTEP_ACT;
							break;
						case BLANK_CALC_END:
							blankMeasure = BLANK_END;
							tskState = SUBSTEP_ACT;
							break;
						}
						//todo
					}
					else
					{
						tskState = SUBSTEP_FINISH;
						//todo
					}
					MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					break;

				case SUBSTEP_VALVE:
					{
						SendTskMsg(FLOW_VALVE_CTL_ID,TSK_INIT, (uint32_t) &allStepsConfig[subStepId-STEP_0], TskCallBackLoc);
						tskState = SUBSTEP_VALVE_DELAY;
						tickOut = VALVE_DURING_TIME;
						//wait time out
					}
					break;
				case SUBSTEP_VALVE_DELAY:
					tskState = SUBSTEP_MOTOR;
					MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					break;
				case SUBSTEP_MOTOR:
				{
					SendTskMsg(FLOW_MOTOR_CTL_ID,TSK_INIT, (uint32_t) &allStepsConfig[subStepId-STEP_0], TskCallBackLoc);

					tskState = SUBSTEP_MOTOR_DELAY;
					//lint -e662 -e661
					tickOut = CalcDuringTimeMsStep_WithDelay((uint16_t)subStepId);
				}
				break;
				case SUBSTEP_MOTOR_DELAY:
					tskState = SUBSTEP_FINISH;
					MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					break;
				case SUBSTEP_ACT_DELAY:
				case SUBSTEP_HEAT_DELAY:
				case SUBSTEP_TIME_DELAY:
				case SUBSTEP_TEST_REF_DELAY:
					TraceDBG(taskID,"task have bugs! %d \n", tskState);
					tskState = SUBSTEP_FINISH;
					MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					break;
				case SUBSTEP_MEAS_DELAY:
					measRawValueTimes = 0;
					tskState = SUBSTEP_FINISH;
					MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					break;
				case SUBSTEP_TIME:
					tickOut = timeDelay*TIME_UNIT;
					tskState = SUBSTEP_TIME_DELAY;
					//wait time out
					break;
				case SUBSTEP_HEAT:
					tickOut = timeDelay*TIME_UNIT;
					tskState = SUBSTEP_HEAT_DELAY;
					//wait time out
					break;
				case SUBSTEP_ACT:
					tickOut = timeDelay*TIME_UNIT;
					tskState = SUBSTEP_ACT_DELAY;
					//wait time out
					break;
				case SUBSTEP_TEST_REF:
					UpdateGainSetting(1);
					tickOut = timeDelay*TIME_UNIT;
					tskState = SUBSTEP_TEST_REF_DELAY;
					break;
//				case SUBSTEP_MEAS_BLANK:
//					tickOut = timeDelay*TIME_UNIT;
//					tskState = SUBSTEP_MEAS_DELAY;
//					blankMeasure = 1;
//					measResultPrint |= 0xF0;
					//wait time out
					break;
				case SUBSTEP_MEAS:
					tickOut = timeDelay*TIME_UNIT;
					measRawValueTimes = (uint16_t)(timeDelay/10);//unit is 100ms
					tskState = SUBSTEP_MEAS_DELAY;
					measResultPrint |= MEASURE_DATA_VALID_MSK;
//					blankMeasure = 0;
					//wait time out
					break;
				case SUBSTEP_FINISH:
					measResultPrint &= MEASURE_DATA_INVALID_MSK;
					flowStepRun[2].step = 0;
					flowStepRun[2].duringTime = 0;
					flowStepRun[2].remainTime = 0;
					subStepId = 0;
					tskState = SUBSTEP_IDLE;
					if(localMsg.callBack)
					{
						localMsg.callBack(taskID);
					}
					//MsgPush (FLOW_SUBSTEP_ID, (uint32_t)&localMsg, 0);
					break;
				default:
					break;

				}
				//when finish->call back;
			}
			else
			{
				//todo;
			}
		}

	}
}


