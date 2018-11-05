/*
 * tsk_sch_autorange.c
 *
 *  Created on: 2018Äê7ÔÂ23ÈÕ
 *      Author: pli
 */






#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "tsk_substep.h"
#include "tsk_sch.h"
#include "tim.h"
#include "unit_sch_cfg.h"
#include "unit_meas_cfg.h"
#include "unit_meas_data.h"
#include "unit_cfg_ex.h"
#include "unit_rtc_cfg.h"

static const char* taskStateDsp[] =
{
	TO_STR(SCH_AR_IDLE),
	TO_STR(SCH_AR_PRE),
	TO_STR(SCH_AR_MEAS),
	TO_STR(SCH_AR_MEAS_DELAY),
	TO_STR(SCH_AR_CALI_CHK),
	TO_STR(SCH_AR_CALI_DELAY),
	TO_STR(SCH_AR_MEAS_RETRY_CHK),
	TO_STR(SCH_AR_FINISH),
	TO_STR(SCH_AR_BREAK),
};

static uint32_t validResult = OK;

static void ARFinished(uint32_t val)
{
	(void)val;
	SendTskMsg(SCH_AUTORANGE_ID, TSK_SUBSTEP, 0, NULL, NULL);
	validResult = val;
}



void StartAutoRangeTask(void const * argument)
{
#define QUEUE_SIZE		4
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	uint16_t retryTimes = 0;
	osEvent event;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_AUTO_RANGE;
	uint32_t schRangeSelection = 0;
	uint32_t measSchMode = 0;
	TracePrint(taskID,"started  \n");
	SCH_AUTO_RANGE_STATE tskState = SCH_AR_IDLE;

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_AUTORANGE_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case SCH_AR_IDLE:
				break;

			default:
				TraceDBG(taskID,"the schedule has bugs, original state is:%d - %s!\n",tskState, taskStateDsp[tskState]);
				tskState = SCH_AR_IDLE;
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
				tskState = SCH_AR_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_AUTORANGE_ID, (uint32_t ) &locMsg, 0);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = SCH_AR_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_AUTORANGE_ID, (uint32_t ) &locMsg, 0);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if (tskState != SCH_AR_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG( taskID,"the schedule of autorange task have bugs!\n");
					//not finish;
				}
				else
				{
					locMsg = *(TSK_MSG_CONVERT(event.value.p));
					retryTimes = 0;
					schRangeSelection = locMsg.val.value;
					measSchMode = ((MSK_MEAS_SCH | MSK_MEAS_TRIG)&schRangeSelection);
					tskState = SCH_AR_PRE;
					locMsg.tskState = TSK_SUBSTEP;
					MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
					//change to init
					//send msg to be running;
				}
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
					case SCH_AR_IDLE:
					break;
					case SCH_AR_PRE:
						retryTimes = 0;
						tskState = SCH_AR_MEAS;
						MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
					break;
					case SCH_AR_MEAS:
						validResult = OK;
						schRangeSelection = measSch.measCaliIndex;
						schRangeSelection &= MSK_RANGE_SEL;
						schRangeSelection |= measSchMode;
						SendTskMsg(SCH_MEAS_ID, TSK_INIT, schRangeSelection, ARFinished, NULL);
						tskState = SCH_AR_MEAS_DELAY;
						tickOut = GetMeasDuringTime_Ms(schRangeSelection);
						TraceMsg(taskID,"schedule task - SCH_MEASURE: T%d\n",tickOut);
						break;
					case SCH_AR_MEAS_DELAY:
						retryTimes++;
						if(validResult != OK)
						{

							TraceMsg(TSK_ID_SCH_MEAS, "Failed once, retry: %d \n",\
										retryTimes );
							if (retryTimes >= retryIimesMax)
							{
								measResultRealTime.measValue = failedMeasureVal;
								measResultRealTime.measFlag = (measResultRealTime.measFlag | failedMeasureFlag);
								IssueFinalResult();
								TraceMsg(TSK_ID_SCH_MEAS, "Failed once, retry: %d \n", retryTimes );
								tskState = SCH_AR_FINISH;
							}
							else
							{
								tskState = SCH_AR_CALI_CHK;
								measResultRealTime.measFlag = 0;
							}
						}
						else
						{
							tskState = SCH_AR_FINISH;
						}
						MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_AR_CALI_CHK:
					{
						measSch.measCaliIndex = _measSch.measCaliIndex;
						uint16_t newState = ChkCalibrationSCH();
						if(newState != OK)
						{
							//do nothing;
							tskState = SCH_AR_MEAS;
							MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
						}
						else
						{
							uint16_t caliNum = 0;
							validResult = OK;
							schRangeSelection = measSch.measCaliIndex;
							schRangeSelection &= MSK_RANGE_SEL;
							schRangeSelection |= MSK_CALI_AR;
							SendTskMsg(SCH_CALI_ID, TSK_INIT, schRangeSelection, ARFinished, NULL);
							tskState = SCH_AR_CALI_DELAY;
							tickOut = GetCaliDuringTime_Ms(schRangeSelection&MSK_RANGE_SEL , &caliNum);
							//todo

							TraceMsg(taskID,"schedule task - SCH_CALIBRATION T%d\n",tickOut);
						}
					}
						break;

					case SCH_AR_CALI_DELAY:
						if(validResult != OK)
						{
							TraceDBG(taskID,"Calibration with no valid result, auto range is stopped!\n");
							measResultRealTime.measValue = failedMeasureVal;
							measResultRealTime.measFlag = (measResultRealTime.measFlag | failedMeasureFlag | FLAG_CALI_STATUS_ER);
							IssueFinalResult();
							tskState = SCH_AR_FINISH;
						}
						else
						{
							tskState = SCH_AR_MEAS;
						}
						MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_AR_POST:
						//error for retry
						//measResultRealTime.measFlag = (measResultRealTime.measFlag | failedMeasureFlag);
						if( validResult != OK)
						{
							tskState = SCH_AR_POST_DELAY;
							schRangeSelection = measSch.measCaliIndex;
							schRangeSelection &= MSK_RANGE_SEL;
							//schRangeSelection |= MSK_CALI_AR;
							TraceMsg(taskID,"schedule task - post cleaning for measure\n");
							SendTskMsg(SCH_CLEAN_ID, TSK_INIT, schRangeSelection, ARFinished, NULL);
							tickOut = GetCleanDuringTime_Ms(schRangeSelection);

							flowStepRun[0].startTime = GetCurrentST();
							flowStepRun[0].duringTime = (tickOut+500)/1000;
							TraceMsg(taskID,"schedule task - SCH_CLEANING T%d\n",tickOut);

						}
						else
						{
							tskState = SCH_AR_FINISH;
							MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
						}
						break;
					case SCH_AR_POST_DELAY:
						tskState = SCH_AR_FINISH;
						MsgPush(SCH_AUTORANGE_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_AR_FINISH:
						tskState = SCH_AR_IDLE;
						if (locMsg.callBackFinish)
						{
							locMsg.callBackFinish(OK, 0);
						}
						break;
					default:
						tskState = SCH_AR_IDLE;
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
