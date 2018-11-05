/*
 * tsk_sch_all.c
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */



#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_sch_cfg.h"
#include "dev_eep.h"
#include "tsk_sch.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_meas_cfg.h"
#include "unit_meas_data.h"
#include "unit_sys_info.h"
#include "unit_flow_act.h"
#include "dev_log_sp.h"
#include "unit_sys_diagnosis.h"
#include "unit_cfg_ex.h"

#define MAX_MEAS_RETRY_TIMES		0x02u


static uint32_t schRangeSelection = 0xFFFFFFFF;

extern uint16_t validCaliFlag;
extern uint16_t 	systemBreakState;
static const char* taskStateDsp[] =
{
	TO_STR(SCH_IDLE),
	TO_STR(SCH_MEASURE),
	TO_STR(SCH_MEASURE_DELAY),

	TO_STR(SCH_CALIBRATION),
	TO_STR(SCH_CALIBRATION_DELAY),

	TO_STR(SCH_CLEANING),
	TO_STR(SCH_CLEANING_DELAY),

	TO_STR(SCH_DRAIN),
	TO_STR(SCH_DRAIN_DELAY),

	TO_STR(SCH_FLUSH),
	TO_STR(SCH_FLUSH_DELAY),

	TO_STR(SCH_PRIME),
	TO_STR(SCH_PRIME_DELAY),
	TO_STR(SCH_FLOW),
	TO_STR(SCH_FLOW_DELAY),
	TO_STR(SCH_STOP),

	TO_STR(SCH_BREAK),
	TO_STR(SCH_RECOVERY),
	TO_STR(SCH_FINISH),
	TO_STR(SCH_FLOW_1),
	TO_STR(SCH_AR_MEASURE),
};

static SCH_STATE tskState = SCH_IDLE;
static void SchTskFinish(uint32_t idx)
{
	//(void)idx;
	if(idx)
	{
		TraceDBG(TSK_ID_SCH,"Schedule Get result Error: %d, sch_all: %s->%d\n",idx,taskStateDsp[tskState],tskState );
	}

	if(tskState == SCH_IDLE)
		SendTskMsg(SCH_ID, TSK_SUBSTEP, 0, NULL, NULL);
	else if(idx != DEVICE_BUSY_STATE)
		SendTskMsg(SCH_ID, TSK_SUBSTEP, 0, NULL, NULL);
	else
	{}
}

uint32_t GetSeconds_TimeCfg(TimeCfg* ptrStartTime)
{
	uint32_t days = CalcDays(ptrStartTime->year, ptrStartTime->month, ptrStartTime->date);
	uint32_t secondStart = CalcSeconds(ptrStartTime->hour, ptrStartTime->minute,ptrStartTime->second);

	return (days*86400 + secondStart);
}

void UpdateLedDiag(uint16_t ledLow, uint16_t val)
{

	if((schRangeSelection & 0x0F) >= 2)
	{
#define LED_LONG_ERROR_MSK 0xCC
#define LED_LONG_LOW_MSK 0x0C
		val = (val & LED_LONG_ERROR_MSK);
		ledLow = (ledLow & LED_LONG_LOW_MSK);
	}
	Dia_UpdateDiagnosis(LED_ERROR, val);
	Dia_UpdateDiagnosis(LED_OUTPUT_LOW, ledLow);
}

static uint16_t SchTskCheck(uint32_t* ptrIdleTime, uint16_t mode, TimeCfg* ptrStartTime, uint32_t eachTime, uint32_t lastExecTime)
{
	uint16_t ret = FATAL_ERROR;
	uint32_t lastCount = 0;
	*ptrIdleTime = osWaitForever;
	schRangeSelection = measSch.measCaliIndex;

	if((mode == PERIOD_MODE) && (Sch_Mode == PERIOD_MODE) && (systemBreakState == OK))
	{

		uint32_t secondStart = GetSeconds_TimeCfg(ptrStartTime);
		uint32_t secondsNow = GetCurrentSeconds();

		if(secondsNow >= secondStart)
		{
			if(lastExecTime >= secondStart)
			{
				uint32_t seondsDeviation = eachTime;
				if(lastExecTime > secondsNow)
				{
					TraceDBG(TSK_ID_SCH,"Sch Err;  now:%d S Each: %d lastExecTime: %d > secondsNow: %d;\n",\
																secondsNow, eachTime,lastExecTime,secondsNow);
					seondsDeviation = eachTime;
				}
				else
				{
					lastCount = (lastExecTime-secondStart)/eachTime;
					//assert(secondsNow >= seconds );
					seondsDeviation = (secondsNow - secondStart) - eachTime * lastCount;
				}

				if(seondsDeviation >= eachTime)
				{
					ret = OK;
					TracePrint(TSK_ID_SCH,"Sch Im;  now:%d S Each: %d LastCount: %d,last Exec: %d;\n",\
											secondsNow, eachTime,lastCount,lastExecTime);
				}
				else
				{
					*ptrIdleTime = (eachTime - seondsDeviation)*TO_MS_UNIT;
					TracePrint(TSK_ID_SCH,"Sch in %d;  now:%d S Each: %d LastCount: %d, last Exec: %d;\n",\
											*ptrIdleTime, secondsNow, eachTime,lastCount,lastExecTime);
				}
			}
			else
			{
				ret = OK;
				*ptrIdleTime = 0;
				TracePrint(TSK_ID_SCH,"Sch in %d;  now:%d S Each: %d lastExecTime: %d < StartTime: %d;\n",\
										*ptrIdleTime, secondsNow, eachTime,lastExecTime,secondStart);
			}
		}
		else
		{
			*ptrIdleTime = (secondStart - secondsNow)*TO_MS_UNIT;
		}
	}
	return ret;
}




uint16_t ChkCalibrationSCH(void)
{
	uint16_t ret = FATAL_ERROR;
	uint32_t lastExecTime = 0;
	uint32_t delayTime = 0;

	if(schInfo.lastCaliTime[measSch.measCaliIndex] > schInfo.lastCaliFinish[measSch.measCaliIndex])
	{
		TraceDBG(TSK_ID_SCH,"calibartion finish time is before start time, use last valid result time %d -> finish time: %d < start time%d!\n",
				calibSch.calibration_ST[measSch.measCaliIndex],\
				schInfo.lastCaliFinish[measSch.measCaliIndex],\
				schInfo.lastCaliTime[measSch.measCaliIndex]);

		lastExecTime = GetSecondsFromST(calibSch.calibration_ST[measSch.measCaliIndex]);
	}
	else
	{
		lastExecTime = GetSecondsFromST(schInfo.lastCaliTime[measSch.measCaliIndex]);
	}

	ret = SchTskCheck(&delayTime, calibSch.calibrationMode, &calibSch.calibrationStartTime, \
			 (uint32_t)calibSch.calibrationInterval * FACTOR_UNIT_HOUR, lastExecTime);

	return ret;
}



static uint16_t SchTskCalibration(uint32_t* ptrIdleTime)
{
	uint16_t ret = FATAL_ERROR;
	uint32_t lastExecTime = 0;
	//todo
	schRangeSelection = measSch.measCaliIndex;
	*ptrIdleTime = osWaitForever;

	if(schInfo.lastCaliTime[measSch.measCaliIndex] > schInfo.lastCaliFinish[measSch.measCaliIndex])
	{
		TraceDBG(TSK_ID_SCH,"calibartion finish time is before start time, use last valid result time %d -> finish time: %d < start time%d!\n",
				calibSch.calibration_ST[measSch.measCaliIndex],\
				schInfo.lastCaliFinish[measSch.measCaliIndex],\
				schInfo.lastCaliTime[measSch.measCaliIndex]);

		lastExecTime = GetSecondsFromST(calibSch.calibration_ST[measSch.measCaliIndex]);
	}
	else
	{
		lastExecTime = GetSecondsFromST(schInfo.lastCaliTime[measSch.measCaliIndex]);
	}

	ret = SchTskCheck(ptrIdleTime, calibSch.calibrationMode, &calibSch.calibrationStartTime, \
			 (uint32_t)calibSch.calibrationInterval * FACTOR_UNIT_HOUR, lastExecTime);

	if(ret == OK)
	{
		schRangeSelection = measSch.measCaliIndex;
		schRangeSelection &= MSK_RANGE_SEL;
		schRangeSelection |= MSK_CALI_SCH;
	}
	return ret;
}
static uint16_t SchTskClean(uint32_t* ptrIdleTime)
{
	uint16_t ret = FATAL_ERROR;
	uint32_t lastExecTime = 0;
	//todo
	schRangeSelection = measSch.measCaliIndex;
	*ptrIdleTime = osWaitForever;
#define MIN_CLEAN_TIME	4
	if(cleanSch.cleanInterval < MIN_CLEAN_TIME)
	{
		TraceDBG(TSK_ID_SCH,"Sch Clean Interval error:  %d, change to: %d\n",\
				cleanSch.cleanInterval, MIN_CLEAN_TIME);
	}
//	assert(cleanSch.cleanInterval >= MIN_CLEAN_TIME);
	if(cleanSch.cleanInterval <= MIN_CLEAN_TIME)
		cleanSch.cleanInterval = MIN_CLEAN_TIME;


	if(schInfo.lastCleanTime > schInfo.lastCleanFinish)
	{
		lastExecTime = GetSecondsFromST(schInfo.lastCleanFinish);
	}
	else
	{
		lastExecTime = GetSecondsFromST(schInfo.lastCleanTime);
	}
	ret = SchTskCheck(ptrIdleTime, cleanSch.cleanMode, &cleanSch.cleanStartTime, \
		 (uint32_t)cleanSch.cleanInterval * FACTOR_UNIT_MINUTES, lastExecTime);


	if(ret == OK)
	{
		schRangeSelection = measSch.measCaliIndex;
		schRangeSelection &= MSK_RANGE_SEL;
		schRangeSelection |= MSK_CLEAN_SCH;
	}

	return ret;
}


static uint16_t SchTskMeas(uint32_t* ptrIdleTime)
{
	uint16_t ret = FATAL_ERROR;
	uint32_t lastExecTime = 0;
	//todo
	schRangeSelection = measSch.measCaliIndex;
	*ptrIdleTime = osWaitForever;

	if(schInfo.lastMeasTime > schInfo.lastMeasFinish)
	{
		lastExecTime = GetSecondsFromST(schInfo.lastMeasFinish);
	}
	else
	{
		lastExecTime = GetSecondsFromST(schInfo.lastMeasTime);
	}

	ret = SchTskCheck(ptrIdleTime, measSch.measureMode, &measSch.measStartTime, \
		 (uint32_t)measSch.measInterval * FACTOR_UNIT_MINUTES, lastExecTime);


	if(ret == OK)
	{
		schRangeSelection = measSch.measCaliIndex;
		schRangeSelection &= MSK_RANGE_SEL;
		schRangeSelection |= MSK_MEAS_SCH;
	}

	return ret;
}


void WakeUpSchedule(void)
{
	SendTskMsg(SCH_ID, TSK_INIT, SCH_TSK_IDLE, NULL, NULL);
}





static uint32_t TrigNewSchedule(SCH_STATE* ptrState)
{
	SCH_STATE _tskState = SCH_IDLE;
	uint32_t tickOut = osWaitForever;
	uint32_t tskTrigger = osWaitForever;
	UpdateTriggerStatus();
	if( Sch_Mode == PERIOD_MODE)
	{
		if(*ptrState == SCH_IDLE)
		{
			uint32_t tskDelayCali = osWaitForever;
			uint32_t tskDelayMeas = osWaitForever;
			uint32_t tskDelayClean = osWaitForever;

			uint16_t schState = FATAL_ERROR;
			_tskState = SCH_IDLE;
			//TaracePrint(taskID,"schedule task - SCH_IDLE get time\n");
			schState = SchTskClean(&tskDelayClean);
			if(schState == OK)
				_tskState = SCH_CLEANING;
			else
			{
				schState = SchTskCalibration(&tskDelayCali);
				if(schState != OK)
				{
					schState = SchTskMeas(&tskDelayMeas);
					if(schState == OK)
					{
						_tskState = SCH_MEASURE;
						if(autoRangeMode<2)
						{
							_tskState = SCH_AR_MEASURE;
						}
					}
				}
				else
				{
					_tskState = SCH_CALIBRATION;
				}
			}
			if(SCH_IDLE == _tskState)
			{
				tickOut = tskDelayCali;
				if(tickOut > tskDelayMeas)
				{
					tickOut = tskDelayMeas;
				}
				if(tickOut > tskDelayClean)
				{
					tickOut = tskDelayClean;
				}
				TraceMsg(TSK_ID_SCH,"Schedule will be called in %d, Cali:%d,Meas:%d,Clean:%d !\n",tickOut, tskDelayCali, tskDelayMeas, tskDelayClean);
				if(tickOut > SCH_TSK_CYCLE)
					tickOut = SCH_TSK_CYCLE;
			}
			else
			{
				*ptrState = _tskState;
				SendTskMsg(SCH_ID, TSK_SUBSTEP, 0, NULL, NULL);
			}
		}
	}

	if(SCH_IDLE == *ptrState)
		tskTrigger = TrigNewTrigger(ptrState, &schRangeSelection);

	if(tskTrigger < tickOut)
		tickOut = tskTrigger;
	return tickOut;
}


//todo:
//FST issues
//when task is waiting for something timeout(if the task is not responde well), no timeout function is called;
#define UI_START_TIME  60000

static uint16_t IsUIReady(uint32_t tickStart)
{
	uint16_t ret = OK;
	if( UI_Start == WAIT_FOR_UI)
	{
		uint32_t tickN = HAL_GetTick();
		if(tickN - tickStart >= UI_START_TIME)
		{
			UI_Start = UI_TIMEOUT;
		}
		else
		{
			ret = FATAL_ERROR;
		}
	}
	return ret;

}
void StartSchTask(void const * argument)
{
	(void) argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	tickOut = UI_START_TIME;//1 MINUTES
	TSK_MSG locMsg;
	locMsg.callBackFinish = NULL;
	const uint8_t taskID = TSK_ID_SCH;
//	validCaliFlag = GetCaliFlag(taskID, measSch.measCaliIndex);
//	GetCaliResult(&caliResultRealTime,measSch.measCaliIndex);
	//validCalibrationData = CheckValidCalibrationData(measSch.measCaliIndex);
	uint16_t startAction = 0;
	uint8_t evData[6];

	if(instrumentType == ADVANCED_VERSION)
	{
		startAction = ADVANCED_VERSION_STARTACTION;
	}
	else
	{
		startAction = STANDARD_VERSION_STARTACTION;
	}
	TracePrint(taskID,"started\n");
	if(masterState != 5)
	{
		tickOut = 10*1000;//wait for canopen ready
		TraceMsg(taskID,"schedule task - Wait canopen ready: T%d\n",tickOut);
	}
	if(tickOut < UI_START_TIME)
		tickOut = UI_START_TIME;

	UI_Start = WAIT_FOR_UI;
	uint32_t tickStart = HAL_GetTick();

	while(1)
	{
		tickOut = 1000;
		event = osMessageGet(SCH_ID, tickOut);
		if(IsUIReady(tickStart) != OK)
		{
			tickOut = 1000;
		}
		else
		{
			if (event.status == osEventMessage)
			{
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
			}
			break;
		}
	}

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_ID, tickOut);
		// substep
		//new event
		if (event.status == osEventMessage)
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_SUBSTEP)
			{

				switch (tskState)
				{
					case SCH_STOP:
						//stop post action;
						triggerFlowStep = STANDARD_VERSION_STOPACTION;
						if(instrumentType == ADVANCED_VERSION)
						{
							triggerFlowStep = ADVANCED_VERSION_STOPACTION;
						}

						tskState = SCH_FLOW_1;
						locMsg.tskState = TSK_SUBSTEP;
						MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_MEASURE:
					{
						UpdateSchFlushCfg();
						ChkRangeValid(schRangeSelection);
						SendTskMsg(SCH_MEAS_ID, TSK_INIT, schRangeSelection, SchTskFinish, NULL);
						tskState = SCH_MEASURE_DELAY;
						//tickOut = 10800000;//3hours
						tickOut = GetMeasDuringTime_Ms(schRangeSelection);

						TraceMsg(taskID,"schedule task - SCH_MEASURE: T%d\n",tickOut);
					}
						//trigger measure tsk
						break;
					case SCH_AR_MEASURE:
					{
						UpdateSchFlushCfg();
						ChkRangeValid(schRangeSelection);
						SendTskMsg(SCH_AUTORANGE_ID, TSK_INIT, schRangeSelection, SchTskFinish, NULL);
						tskState = SCH_MEASURE_DELAY;
						tickOut = 10800000;//3hours

						TraceMsg(taskID,"schedule task - SCH Auto Range MEASURE: T%d\n",tickOut);
					}
						//trigger measure tsk
						break;
					case SCH_CALIBRATION:
					{
						uint16_t  caliNum = 1;
						ChkRangeValid(schRangeSelection);
						//trigger calibration tsk
						//assert(MEAS_RANGE_MAX > schRangeSelection);
						UpdateSchFlushCfg();
						SendTskMsg(SCH_CALI_ID, TSK_INIT, schRangeSelection, SchTskFinish, NULL);
						tskState = SCH_CALIBRATION_DELAY;
						tickOut = GetCaliDuringTime_Ms(schRangeSelection&MSK_RANGE_SEL , &caliNum);
						//todo
						//remove EPA calibration triggerred action;
						ClrCalibration_EPA(schRangeSelection);

						TraceMsg(taskID,"schedule task - SCH_CALIBRATION T%d\n",tickOut);
					}
						break;
					case SCH_CLEANING:
						//trigger cleaning tsk
						//assert(MEAS_RANGE_MAX > schRangeSelection);
						ChkRangeValid(schRangeSelection);
						SendTskMsg(SCH_CLEAN_ID, TSK_INIT, schRangeSelection, SchTskFinish, NULL);
						tskState = SCH_CLEANING_DELAY;
						tickOut = GetCleanDuringTime_Ms(schRangeSelection);

						flowStepRun[0].startTime = GetCurrentST();
						flowStepRun[0].duringTime = (tickOut+500)/1000;
						TraceMsg(taskID,"schedule task - SCH_CLEANING T%d\n",tickOut);
						break;

					case SCH_DRAIN:
					{
						uint32_t stTime = GetCurrentST();
						ChkRangeValid(schRangeSelection);
						assert(MEAS_RANGE_MAX > schRangeSelection);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, drainSchSteps[schRangeSelection], SchTskFinish, NULL);
						tickOut = CalcDuringTimeMsStep_WithDelay(drainSchSteps[schRangeSelection]);
						tskState = SCH_DRAIN_DELAY;

						flowStepRun[0].startTime = stTime;
						actionExecuteTime_ST[ Trigger_Drain] = stTime;
						actionRuningDetail = schRangeSelection;
						mainActionDetail = (uint16_t)((schRangeSelection<<8) | 0xFF);
						flowStepRun[0].duringTime = (tickOut+500)/1000;
						memset((void*)&evData[0], 0, sizeof(evData));
						evData[0] = (uint8_t)schRangeSelection;
						evData[1] = (uint8_t)drainSchSteps[schRangeSelection];
						NewEventLog(EV_DRAIN, evData);
						TraceMsg(taskID,"schedule task - SCH_DRAIN T%d\n",tickOut);
					}
						break;

					case SCH_FLUSH:
					{
						uint32_t stTime = GetCurrentST();
						ChkRangeValid(schRangeSelection);
						UpdateSchFlushCfg();
						assert(MEAS_RANGE_MAX > schRangeSelection);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, flushSchSteps[schRangeSelection], SchTskFinish, NULL);
						tskState = SCH_FLUSH_DELAY;
						actionRuningDetail = schRangeSelection;
						mainActionDetail = (uint16_t)((schRangeSelection<<8) | 0xFF);
						tickOut = CalcDuringTimeMsStep_WithDelay(flushSchSteps[schRangeSelection]);

						flowStepRun[0].duringTime = (tickOut+500)/1000;
						flowStepRun[0].startTime = stTime;
						actionExecuteTime_ST[ Trigger_Flush] = stTime;
						memset((void*)&evData[0], 0, sizeof(evData));
						evData[0] = (uint8_t)schRangeSelection;
						evData[1] = (uint8_t)drainSchSteps[schRangeSelection];
						NewEventLog(EV_FLUSH, evData);


						TraceMsg(taskID,"schedule task - SCH_FLUSH T%d\n",tickOut);
					}
						break;
					case SCH_PRIME:
					{
						uint32_t stTime = GetCurrentST();
						ChkRangeValid(schRangeSelection);
						assert(MEAS_RANGE_MAX > schRangeSelection);
						actionRuningDetail = schRangeSelection;
						mainActionDetail = (uint16_t)((schRangeSelection<<8) | 0xFF);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, primeSchSteps[schRangeSelection], SchTskFinish, NULL);
						tskState = SCH_FLUSH_DELAY;
						tickOut = CalcDuringTimeMsStep_WithDelay(primeSchSteps[schRangeSelection]);

						flowStepRun[0].duringTime = (tickOut+500)/1000;
						flowStepRun[0].startTime = stTime;
						actionExecuteTime_ST[ Trigger_Prime] = stTime;

						memset((void*)&evData[0], 0, sizeof(evData));
						evData[0] = (uint8_t)schRangeSelection;
						evData[1] = (uint8_t)primeSchSteps[schRangeSelection];
						NewEventLog(EV_PRIME, evData);


						TraceMsg(taskID,"schedule task - SCH_PRIME T%d\n",tickOut);
					}
						break;
					case SCH_FLOW_1:
					{
						uint32_t stTime = GetCurrentST();

						SendTskMsg(FLOW_TSK_ID, TSK_INIT, triggerFlowStep, SchTskFinish, NULL);
						tskState = SCH_FLOW_DELAY;
						tickOut = CalcDuringTimeMsStep_WithDelay(triggerFlowStep);
						actionRuningDetail = triggerFlowStep;
						mainActionDetail = triggerFlowStep;
						flowStepRun[0].duringTime = (tickOut+500)/1000;
						flowStepRun[0].startTime = stTime;
						actionExecuteTime_ST[ Trigger_FlowSteps] = stTime;
						memset((void*)&evData[0], 0, sizeof(evData));
						evData[0] = (uint8_t)triggerFlowStep;
						NewEventLog(EV_FLOWSTEP, evData);

						triggerFlowStep = 0;

						TraceMsg(taskID,"schedule task - Steps:%d -> SCH_FLOW_1 T%d\n",triggerFlowStep, tickOut);
					}
						break;
					case SCH_FLOW:
					{
						//if(triggerFlowStep != 0)
						{
							uint32_t stTime = GetCurrentST();
							SendTskMsg(FLOW_TSK_ID, TSK_INIT, triggerFlowStep, SchTskFinish, NULL);
							tskState = SCH_FLOW_DELAY;
							tickOut = CalcDuringTimeMsStep_WithDelay(triggerFlowStep);
							actionRuningDetail = triggerFlowStep;
							mainActionDetail = triggerFlowStep;
							flowStepRun[0].duringTime = (tickOut+500)/1000;
							flowStepRun[0].startTime = stTime;
							actionExecuteTime_ST[ Trigger_FlowSteps] = stTime;
							memset((void*)&evData[0], 0, sizeof(evData));
							evData[0] = (uint8_t)triggerFlowStep;
							NewEventLog(EV_FLOWSTEP, evData);

							triggerFlowStep = 0;

							TraceMsg(taskID,"schedule task - Steps:%d -> SCH_FLOW T%d\n",triggerFlowStep, tickOut);
						}
						//else
						//{
						//	tskState = SCH_FINISH;
						//	locMsg.tskState = TSK_SUBSTEP;
						//	MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						//	tickOut = 0;
						//}
					}
						break;
					case SCH_FLUSH_DELAY:
					case SCH_PRIME_DELAY:
					case SCH_DRAIN_DELAY:
					case SCH_FLOW_DELAY:
					//case SCH_IDLE:
						tskState = SCH_FINISH;
						locMsg.tskState = TSK_SUBSTEP;
						MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_CALIBRATION_DELAY:
						tskState = SCH_FINISH;
						if(calibSch.calibrationPostAction)
						{
							if(caliPostMeas)
							{
								schRangeSelection &= MSK_RANGE_SEL;
								schRangeSelection |= MSK_MEAS_STD1;
								tskState = SCH_MEASURE;
								TraceMsg(taskID,"schedule task - post measure for calibration\n");
							}
							else
							{
								tskState = SCH_FLUSH;
								schRangeSelection &= MSK_RANGE_SEL;
								TraceMsg(taskID,"schedule task - post flush for calibration\n");
							}
						}
						locMsg.tskState = TSK_SUBSTEP;
						MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_MEASURE_DELAY:
						tskState = SCH_FINISH;
						if(measPostEnable)
						{
							TraceMsg(taskID,"schedule task - post flow for measure\n");
							tskState = SCH_FLOW;
							schRangeSelection &= MSK_RANGE_SEL;
							triggerFlowStep = measPostStep[schRangeSelection];
						}

						locMsg.tskState = TSK_SUBSTEP;
						MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_CLEANING_DELAY:
						tskState = SCH_FINISH;
						if(cleanSch.cleanPostAction == POST_FLUSH)
						{
							tskState = SCH_FLUSH;
							schRangeSelection &= MSK_RANGE_SEL;
							TraceMsg(taskID,"schedule task - post flush for cleaning\n");
						}
						else if(cleanSch.cleanPostAction == POST_CALIBRATION)
						{
							tskState = SCH_CALIBRATION;
							schRangeSelection &= MSK_RANGE_SEL;
							TraceMsg(taskID,"schedule task - post calibration for cleaning\n");
						}
						else
						{}
						locMsg.tskState = TSK_SUBSTEP;
						MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_FINISH:
						mainActionDetail = 0x0;
						flowStepRun[0].step = MAINACT_NONE;
						flowStepRun[0].duringTime = 0;
						flowStepRun[0].remainTime = 0;
						TraceMsg(taskID,"schedule task -schedule is finished!\n");
						tskState = SCH_IDLE;
						locMsg.tskState = TSK_SUBSTEP;
						if(locMsg.callBackFinish)
						{
							//locMsg.callBackFinish();
						}
						locMsg.callBackFinish = NULL;
						MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
						break;
					case SCH_IDLE:
						tickOut = 0;
						break;
					default:
						TraceDBG(taskID, "the schedule of schedule task have bugs! %d \n", tskState);
						tskState = SCH_FINISH;
						tickOut = 0;
						break;
				}
			}
			else if ( mainTskState == TSK_FORCE_BREAK)
			{
				mainActionDetail = 0x0;
				SendTskMsg(SCH_MEAS_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_CALI_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_CLEAN_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_IO_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_AUTORANGE_ID, TSK_FORCE_BREAK, 0, NULL, NULL);

				tskState = SCH_FINISH;
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
				TraceMsg(taskID,"schedule task - TSK_FORCE_BREAK\n");
			}
			else if ( mainTskState == TSK_RESETIO)
			{
				mainActionDetail = 0x0;
				SendTskMsg(SCH_MEAS_ID, TSK_RESETIO, 0, NULL, NULL);
				SendTskMsg(SCH_CALI_ID, TSK_RESETIO, 0, NULL, NULL);
				SendTskMsg(SCH_CLEAN_ID, TSK_RESETIO, 0, NULL, NULL);
				SendTskMsg(SCH_IO_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_AUTORANGE_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				tskState = SCH_FINISH;
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
				TraceMsg(taskID,"schedule task - TSK_RESETIO\n");
			}
			else if ( mainTskState == TSK_FORCE_STOP)
			{
				mainActionDetail = 0x0;
				SendTskMsg(SCH_IO_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_MEAS_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_CALI_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(SCH_CLEAN_ID, TSK_FORCE_BREAK, 0, NULL, NULL);

				SendTskMsg(SCH_IO_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, SchTskFinish, NULL);
				SendTskMsg(SCH_AUTORANGE_ID, TSK_FORCE_BREAK, 0, NULL, NULL);
				//osDelay(100);
				tskState = SCH_STOP;

				tickOut = 1000;
				TraceMsg(taskID,"schedule task - TSK_FORCE_STOP\n");
			}
			else if ( mainTskState == TSK_INIT)
			{

				uint16_t reqResult = DEVICE_BUSY_STATE;


				TraceMsg(taskID,"schedule task - TSK_INIT: %d\n", locMsg.val.value);
				deviceBusy = WARNING;

				if(startAction != 0)
				{
					triggerFlowStep = startAction;
					tskState = SCH_FLOW_1;
					startAction = 0;
					locMsg.tskState = TSK_SUBSTEP;
					locMsg.callBackFinish = NULL;
					MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
				}
				else if(tskState != SCH_IDLE)
				{
					if( TSK_MSG_CONVERT(event.value.p)->callBackFinish )
					{
						TSK_MSG_CONVERT(event.value.p)->callBackFinish(reqResult,0);
					}
					if(TSK_MSG_CONVERT(event.value.p)->val.value != SCH_IDLE)
					{
						TraceDBG(taskID, "the request of schedule task is not executed immediately!\n");
					}
				}
				else
				{
					locMsg = *TSK_MSG_CONVERT(event.value.p);
					locMsg.tskState = TSK_SUBSTEP;
					MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
				}
			}
			else
			{
				TraceDBG(taskID, "the request of schedule task is Error %d->MainState: %d!\n",tskState,mainTskState);
			//	tskState = SCH_IDLE;
			}
		}
		else //timeout
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			if(SCH_IDLE == tskState)
			{
				if(startAction != 0)
				{
					triggerFlowStep = startAction;
					tskState = SCH_FLOW_1;
					startAction = 0;
					locMsg.tskState = TSK_SUBSTEP;
					MsgPush(SCH_ID, (uint32_t) &locMsg, 0);
				}
				else
				{
					UpdateToCurrentSch(0xFFFF);
					validCaliFlag = GetCaliFlag(taskID, measSch.measCaliIndex);
					tickOut = TrigNewSchedule(&tskState);
				}
				if(tskState == SCH_IDLE)
				{
					deviceBusy = OK;
				}
				else
				{
					deviceBusy = WARNING;
				}
			}
			else
			{
				deviceBusy = OK;
				TraceDBG(taskID,"the schedule has bugs, original state is:%d - %s!\n",tskState, taskStateDsp[tskState]);
				tskState = SCH_IDLE;
				tickOut = SCH_TSK_CYCLE;
			}
		}
	}
}


