/*
 * tsk_sch_cali.c
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
#include "unit_meas_cfg.h"
#include "unit_sch_cfg.h"
#include "dev_eep.h"
#include "tsk_sch.h"
#include "tsk_measure.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "dev_log_sp.h"
#include "dev_logdata.h"
#include "dev_temp.h"
#include "unit_meas_data.h"
#include "unit_sys_diagnosis.h"






static const char* taskStateDsp[] =
{
	TO_STR(SCH_CALI_IDLE),
	TO_STR(SCH_CALI_PRE),
	TO_STR(SCH_CALI_STD0_PRE),
	TO_STR(SCH_CALI_STD0_PRE_DELAY),
	TO_STR(SCH_CALI_STD0),
	TO_STR(SCH_CALI_STD0_DELAY),
	TO_STR(SCH_CALI_STD0_FINISH),

	TO_STR(SCH_CALI_STD1_PRE),
	TO_STR(SCH_CALI_STD1_PRE_DELAY),

	TO_STR(SCH_CALI_STD1),
	TO_STR(SCH_CALI_STD1_DELAY),
	TO_STR(SCH_CALI_RETRY),
	TO_STR(SCH_CALI_FINISH),
	TO_STR(SCH_CALI_BREAK),
};

static void CaliFinished(uint32_t val)
{
	(void)val;
	SendTskMsg(SCH_CALI_ID, TSK_SUBSTEP, 0, NULL);
}



static uint16_t CheckCaliResult(CaliDataRaw* _rawAbs, uint16_t __measureTimes, uint16_t standard, float* absVal)
{
	assert(_rawAbs);
	uint16_t count = __measureTimes;
	assert(__measureTimes >= 1);

	if(standard == 0)		//std0
	{
		//use the last measure result:
		memcpy(absVal, &_rawAbs[__measureTimes-1].rawAbs[0], sizeof(_rawAbs[__measureTimes-1].rawAbs));
	}
	//measure std1
	else
	{
		if(__measureTimes > 1)
		{
		//	float  countF = (float)(1.0f*measureTimes - 1.0f);
			float  countF = (float)(1.0f*__measureTimes);
			absVal[0] = 0.0f;
			absVal[1] = 0.0f;
			absVal[2] = 0.0f;
			absVal[3] = 0.0f;

			//todo: remove the bad points;
			//remove first bad points
			//for(uint16_t idx=1;idx<measureTimes;idx++)
			for(uint16_t idx=0;idx<__measureTimes;idx++)
			{
				absVal[0] += _rawAbs[idx].rawAbs[0];
				absVal[1] += _rawAbs[idx].rawAbs[1];
				absVal[2] += _rawAbs[idx].rawAbs[2];
				absVal[3] += _rawAbs[idx].rawAbs[3];
			}
			if(countF > 1)
			{
				absVal[0] /= countF;
				absVal[1] /= countF;
				absVal[2] /= countF;
				absVal[3] /= countF;
			}
		}
		else
		{
			absVal[0] = _rawAbs[0].rawAbs[0];
			absVal[1] = _rawAbs[0].rawAbs[1];
			absVal[2] = _rawAbs[0].rawAbs[2];
			absVal[3] = _rawAbs[0].rawAbs[3];
		}
	}
	return count;

}



static void UpdateCaliPoint(CaliPoint* ptrPoint, uint32_t rangeIdx)
{
	if(calibSch.calibrationType)
	{
		assert(rangeIdx < MEAS_RANGE_MAX);

		ptrPoint->calibrationPointsNum_Zero = calibSch.calibrationPointsNum_Zero;
		ptrPoint->calibrationPointsNumMax_Zero = calibSch.calibrationPointsNumMax_Zero;
		ptrPoint->calibrationPointsNum = calibSch.calibrationPointsNum[rangeIdx];
		ptrPoint->calibrationPointsNumMax = calibSch.calibrationPointsNumMax[rangeIdx];
		ptrPoint->calibrationTimesMax = calibSch.calibrationTimesMax;
	}
	else
	{
		ptrPoint->calibrationPointsNum_Zero = 1;
		ptrPoint->calibrationPointsNumMax_Zero = 1;
		ptrPoint->calibrationPointsNum = 1;
		ptrPoint->calibrationPointsNumMax = 1;
		ptrPoint->calibrationTimesMax = 2;
	}
}


static uint16_t SaveCalibrationLog(uint16_t schCaliRangeSel, uint16_t status, uint32_t _caliType)
{
	const LogData* dataInst = GetLogDataInst(HISTORY_CALIDATA_LOG);
	if(dataInst)
	{
		dataInst->Lock();
		CaliData* logInst = dataInst->wrData.ptrCaliData;
		assert(MEAS_RANGE_MAX > schCaliRangeSel);

		if(status == OK)
		{
			if(schCaliRangeSel <= 1)
			{
				caliResultRealTime.caliSlope[0] = calibSch.calibrationSlope_Long[schCaliRangeSel];
				caliResultRealTime.caliOffset[0] = calibSch.calibrationOffset_Long[schCaliRangeSel];
			}
			else
			{
				caliResultRealTime.caliSlope[0] = 0;
				caliResultRealTime.caliOffset[0] = 0;
			}
			caliResultRealTime.caliOffset[1] = calibSch.calibrationOffset_Short[schCaliRangeSel];
			caliResultRealTime.caliSlope[1] = calibSch.calibrationSlope_Short[schCaliRangeSel];
		}
		else
		{
			caliResultRealTime.caliSlope[0] = caliSlope[0];
			caliResultRealTime.caliOffset[0] = caliOffset[0];

			caliResultRealTime.caliSlope[1] = caliSlope[1];
			caliResultRealTime.caliOffset[1] = caliOffset[1];
		}
		caliResultRealTime.caliRangeIdx = schCaliRangeSel;
		_caliType = _caliType&(MSK_CALI_TRIG|MSK_CALI_SCH|MSK_CALI_AR);

		if(_caliType == MSK_CALI_TRIG)
			status |= 0x4000;
		else if (_caliType == MSK_CALI_AR)
			status |= 0x2000;
		else
			status |= 0x8000;

		caliResultRealTime.caliStatus = status;
		memcpy((void*)logInst, (void*)&caliResultRealTime, sizeof(CaliDataSt));
		dataInst->UnLock();
		dataInst->UpdateData(dataInst);
		Trigger_Save2FF(NULL);
	}
	UpdateResult2UI_Pre(UPDATE_CALIBRATION_ID);
	return OK;
}


void Fake_TrigCaliStorage(void)
{
	caliResultRealTime.startTimeST = GetCurrentST();
	SaveCalibrationLog(0,OK,0);
}

uint32_t GetCaliDuringTime_Ms(uint32_t rangeIdx, uint16_t* ptrCaliNum)
{
	rangeIdx = (MSK_RANGE_SEL&rangeIdx);
	assert(MEAS_RANGE_MAX > rangeIdx);
	CaliPoint caliPoint;
	UpdateCaliPoint(&caliPoint, rangeIdx);

	uint32_t val = (uint32_t)caliPoint.calibrationPointsNumMax_Zero * CalcDuringTimeMsStep_WithDelay(caliSchSteps[rangeIdx][0]);

	val += (uint32_t)caliPoint.calibrationPointsNumMax * CalcDuringTimeMsStep_WithDelay(caliSchSteps[rangeIdx][1]);
	val += CalcDuringTimeMsStep_WithDelay(caliPreActStd[rangeIdx][0]);
	val += CalcDuringTimeMsStep_WithDelay(caliPreActStd[rangeIdx][1]);
//	val += CalcDuringTimeMsStep_WithDelay(flushSchSteps[rangeIdx]);

	*ptrCaliNum = 1;
	if(caliPoint.calibrationTimesMax)
	{
		*ptrCaliNum = caliPoint.calibrationTimesMax;
		val *= caliPoint.calibrationTimesMax;
	}
	return val;

}

static CaliDataRaw caliDataRawStd0[5];
static CaliDataRaw caliDataRawStd1[5];


static void AlignCliTime(uint32_t schCaliRangeSel)
{
	if(schCaliRangeSel == MEAS_RANGE_HIGH2)
	{
		schInfo.lastCaliTime[MEAS_RANGE_HIGH_DILUSION] = schInfo.lastCaliTime[MEAS_RANGE_HIGH2] ;
		schInfo.lastCaliFinish[MEAS_RANGE_HIGH_DILUSION] = schInfo.lastCaliFinish[MEAS_RANGE_HIGH2] ;

	}
	else if(schCaliRangeSel == MEAS_RANGE_HIGH_DILUSION)
	{
		schInfo.lastCaliTime[MEAS_RANGE_HIGH2] = schInfo.lastCaliTime[MEAS_RANGE_HIGH_DILUSION] ;
		schInfo.lastCaliFinish[MEAS_RANGE_HIGH2] = schInfo.lastCaliFinish[MEAS_RANGE_HIGH_DILUSION] ;
	}
}




void StartCalibrationTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	uint16_t caliCycles = 0;
	uint16_t validResult = FATAL_ERROR;
	uint16_t caliTimes = 0;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_SCH_CALI;
	locMsg.callBack = NULL;
	float std0Abs[4];
	float std1Abs[4];
	uint32_t schCaliRangeSel = 0;//
	uint32_t caliType = 0;
	SCH_CALI_STATE tskState = SCH_CALI_IDLE;
	TracePrint(taskID,"sch calibration task started\n");
	CaliPoint caliPoint;

//	uint32_t caliStartTime = 0;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_CALI_ID, tickOut);


		//new event
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			TraceDBG(taskID,"the cali_task failure-- %d!\n", tskState);
			switch(tskState)
			{
				case SCH_CALI_STD0_DELAY:
					tskState = SCH_CALI_STD0;
					MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
					break;

				case SCH_CALI_STD1_DELAY:
					tskState = SCH_CALI_STD1;
					MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
					break;
				case SCH_CALI_STD0_PRE_DELAY:
					tskState = SCH_CALI_STD0;
					MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
					break;
				case SCH_CALI_STD1_PRE_DELAY:
					tskState = SCH_CALI_STD1;
					MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
					break;
				default:
					tskState = SCH_CALI_IDLE;
					break;
			}
		}
		//new event
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if( mainTskState == TSK_SUBSTEP)
			{
				uint16_t validDataTimes = 0;
				switch(tskState)
				{
					case SCH_CALI_PRE:
					{
						uint16_t caliNum = 1;
						uint32_t caliStartTime = GetCurrentST();
						flowStepRun[0].step = MAINACT_CALIB;
						flowStepRun[0].startTime = GetCurrentST();
						flowStepRun[0].duringTime = (GetCaliDuringTime_Ms(schCaliRangeSel, &caliNum)+500)/1000;
						flowStepRun[0].duringTime /= caliNum;

						tskState = SCH_CALI_STD0_PRE;
						caliTimes = 0;
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						memset((void*)&caliResultRealTime,0,sizeof(caliResultRealTime) );
						memset((void*)&std0Abs[0], 0, sizeof(std0Abs));
						memset((void*)&std1Abs[0], 0, sizeof(std1Abs));
						memset((void*)&caliDataRawStd0[0],0,sizeof(caliDataRawStd0) );
						memset((void*)&caliDataRawStd1[0],0,sizeof(caliDataRawStd1) );
						caliResultRealTime.startTimeST = caliStartTime;
						caliResultRealTime.caliStd0.startTimeST = caliStartTime;
						caliResultRealTime.caliStd1.startTimeST = caliStartTime;
					}
						break;
					case SCH_CALI_STD0_PRE:
						tskState = SCH_CALI_STD0_PRE_DELAY;
						assert(MEAS_RANGE_MAX > schCaliRangeSel);
						tickOut = CalcDuringTimeMsStep_WithDelay(caliPreActStd[schCaliRangeSel][0]);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, caliPreActStd[schCaliRangeSel][0], CaliFinished);
						break;
					case SCH_CALI_STD0_PRE_DELAY:
						tskState = SCH_CALI_STD0;
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						break;
					case SCH_CALI_STD1_PRE:
						tskState = SCH_CALI_STD1_PRE_DELAY;
						assert(MEAS_RANGE_MAX > schCaliRangeSel);
						tickOut = CalcDuringTimeMsStep_WithDelay(caliPreActStd[schCaliRangeSel][1]);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, caliPreActStd[schCaliRangeSel][1], CaliFinished);
						break;
					case SCH_CALI_STD1_PRE_DELAY:
						tskState = SCH_CALI_STD1;
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						break;
					case SCH_CALI_STD0:
					{
						tskState = SCH_CALI_STD0_DELAY;
						assert(MEAS_RANGE_MAX > schCaliRangeSel);
						caliResultRealTime.caliStd0.startTimeST = GetCurrentST();
						tickOut = CalcDuringTimeMsStep_WithDelay(caliSchSteps[schCaliRangeSel][0]);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, caliSchSteps[schCaliRangeSel][0], CaliFinished);

						measDataFlag = (uint16_t)(FLAG_CALI_STD0 | schCaliRangeSel);

						caliDataRawStd0[caliTimes].startTimeST = GetCurrentST();
					}

						break;
					case SCH_CALI_STD0_DELAY:
					//add to have time for UI to record data;
						//caliDataRawStd0[caliTimes].temperature[0] = GetTempChn(0);
						//caliDataRawStd0[caliTimes].temperature[1] = GetTempChn(1);
						//caliDataRawStd0[caliTimes].temperature[2] = GetTempChn(2);
						//caliDataRawStd0[caliTimes].temperature[3] = GetTempChn(3);
						if( GetNewRawCaliDataResult(&caliDataRawStd0[caliTimes]) == OK )
						{
							caliTimes++;
						}
						else
						{
							TraceDBG(taskID,"no valid measure step is called, did not get measure result.\n");
						}
						if(caliTimes > 0)
							validDataTimes = CheckCaliResult(caliDataRawStd0, caliTimes, 0, std0Abs);

						//get valid calibration data;
						TraceMsg(taskID,"sch calibration task --- SCH_CALI_STD0_DELAY: valid count: %d, time: %d, retry cycle: %d\n",validDataTimes, caliTimes,caliCycles);
						if(validDataTimes >= caliPoint.calibrationPointsNum_Zero)
						{
							tskState = SCH_CALI_STD0_FINISH;
						if (OK != CalcOffset(std0Abs, (uint16_t)schCaliRangeSel))
							{
								validResult = FATAL_ERROR;
								tskState = SCH_CALI_RETRY;
							}
							else
							{
								validResult = OK;
							}
						}
						//not get enough valid calibration data and max is not exceed;
						else if(caliTimes <= caliPoint.calibrationPointsNumMax_Zero)
						{
							tskState = SCH_CALI_STD0;
						}
						//failed to get enough calibration data and max is exceed;
						else
						{
							validResult = FATAL_ERROR;
							tskState = SCH_CALI_RETRY;
						}
						if(caliTimes > validDataTimes)
						{
							TraceDBG(taskID,"the cali tsk failed to get valid data: %d!\n", caliTimes-validDataTimes);
						}
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						break;
					case SCH_CALI_STD0_FINISH:
						caliResultRealTime.caliTimes[0] = caliTimes;
						tskState = SCH_CALI_STD1_PRE;
						caliTimes = 0;
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						break;
					case SCH_CALI_STD1:
					{
						TraceMsg(taskID,"sch calibration task --- SCH_CALI_STD1 \n");
						caliResultRealTime.caliStd1.startTimeST = GetCurrentST();
						tskState = SCH_CALI_STD1_DELAY;
						assert(MEAS_RANGE_MAX > schCaliRangeSel);
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, caliSchSteps[schCaliRangeSel][1], CaliFinished);


						tickOut = CalcDuringTimeMsStep_WithDelay(caliSchSteps[schCaliRangeSel][1]);
						measDataFlag = (uint16_t)(FLAG_CALI_STD1 | schCaliRangeSel);

						caliDataRawStd1[caliTimes].startTimeST = GetCurrentST();
					}
						break;
					case SCH_CALI_STD1_DELAY:
					//	caliDataRawStd1[caliTimes].temperature[0] = (int16_t)GetTempChn(0);
					//	caliDataRawStd1[caliTimes].temperature[1] = (int16_t)GetTempChn(1);
					//	caliDataRawStd1[caliTimes].temperature[2] = (int16_t)GetTempChn(2);
					//	caliDataRawStd1[caliTimes].temperature[3] = (int16_t)GetTempChn(3);
						if( GetNewRawCaliDataResult(&caliDataRawStd1[caliTimes]) == OK )
						{
							caliTimes++;
						}


						validDataTimes = CheckCaliResult(caliDataRawStd1, caliTimes, 1, std1Abs);
						assert(MEAS_RANGE_MAX > schCaliRangeSel);
						TraceMsg(taskID,"sch calibration task --- SCH_CALI_STD1_DELAY: valid count: %d, time: %d, retry cycle: %d\n",validDataTimes, caliTimes,caliCycles);
						if(validDataTimes >= caliPoint.calibrationPointsNum)
						{
							validResult = OK;
							tskState = SCH_CALI_RETRY;
						}
						else if(caliTimes <= caliPoint.calibrationPointsNumMax)
						{
							tskState = SCH_CALI_STD1;
						}
						else
						{
							validResult = FATAL_ERROR;
							tskState = SCH_CALI_RETRY;
						}
						if(caliTimes > validDataTimes)
						{
							TraceDBG(taskID,"the meas_task failed to get valid data: %d!\n", caliTimes-validDataTimes);
						}
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						break;


					case SCH_CALI_RETRY:
						caliResultRealTime.caliTimes[1] = caliTimes;
						TraceMsg(taskID,"sch calibration task --- SCH_CALI_RETRY \n");
						tskState = SCH_CALI_FINISH;
						caliTimes = 0;
						caliOffset[0] = caliOffset[1] = 0.0f;
						caliSlope[0] = caliSlope[1] = 0.0f;
						if(validResult == OK)
						{
							validResult |= UpdateSlopeOffset(std0Abs, std1Abs,caliResultRealTime.startTimeST, (uint16_t)schCaliRangeSel);
						}
						(void)memcpy((void*)&caliResultRealTime.caliStd0.temperature[0], (void*)&caliDataRawStd0[0].temperature[0], sizeof(caliResultRealTime.caliStd0.temperature));
						(void)memcpy((void*)&caliResultRealTime.caliStd1.temperature[0], (void*)&caliDataRawStd1[0].temperature[0], sizeof(caliResultRealTime.caliStd1.temperature));
						(void)memcpy((void*)&caliResultRealTime.caliStd0.rawAbs[0], (void*)&std0Abs[0], sizeof(caliResultRealTime.caliStd0.rawAbs));
						(void)memcpy((void*)&caliResultRealTime.caliStd1.rawAbs[0], (void*)&std1Abs[0], sizeof(caliResultRealTime.caliStd1.rawAbs));


						if(validResult != OK)
						{
							caliCycles++;
							if(caliCycles < caliPoint.calibrationTimesMax)
							{
								TraceDBG(taskID,"sch calibration task Failed Once! \n");
								tskState = SCH_CALI_PRE;
							}
							else
							{
								TraceDBG(taskID,"sch calibration task Failed Final! \n");
							}
						}
						else
						{
							TraceMsg(taskID,"sch calibration task finalized OK! \n");
						}
						Dia_UpdateDiagnosis(FAILED_CALIBRATION, (validResult != OK));
						assert(MEAS_RANGE_MAX > schCaliRangeSel);
						//todo: only update time when calibration is valid
						//RTC_Get( IDX_RTC_TIME, 0, &calibSch.calibrationTime[schCaliRangeSel]);
						//Trigger_EEPSave((uint8_t*)&calibSch.calibrationTime[0], sizeof(calibSch.calibrationTime),SYNC_IM );

						//calibSch.caliStatus = validResult;
						//Trigger_EEPSave((uint8_t*)&calibSch.caliStatus, 2,SYNC_IM );
						SaveCalibrationLog((uint16_t)schCaliRangeSel, validResult, caliType );
						MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
						break;
					case SCH_CALI_FINISH:
						lastMeasureForRetest = calibSch.caliConcentration[schCaliRangeSel];

						if(schCaliRangeSel == MEAS_RANGE_HIGH_DILUSION)
							lastMeasureForRetest = calibSch.caliConcentration[MEAS_RANGE_HIGH2];

						schInfo.lastCaliFinish[schCaliRangeSel] = GetCurrentST();
						AlignCliTime(schCaliRangeSel);

						Trigger_EEPSave((void*)&schInfo, sizeof(schInfo),SYNC_CYCLE);
						if( locMsg.callBack )
						{
							locMsg.callBack(validResult);
						}
						tskState = SCH_CALI_IDLE;
						break;
					case SCH_CALI_IDLE:
						break;
					default:
						if( locMsg.callBack )
						{
							locMsg.callBack(OK);
						}
						locMsg.callBack = NULL;
						TraceDBG(taskID,"sch_cali has bugs: %s: %d,\t%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);
						tskState = SCH_CALI_IDLE;

						break;
				}
				// substep
			}
			else if ( mainTskState == TSK_FORCE_BREAK)
			{
				validResult = FATAL_ERROR;caliTimes = 0;
				tskState = SCH_CALI_IDLE;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_CALI_ID, (uint32_t ) &locMsg, 0);
			//	SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, NULL);
			}
			else if (TSK_RESETIO == mainTskState)
			{
				validResult = FATAL_ERROR;caliTimes = 0;
				tskState = SCH_CALI_IDLE;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_CALI_ID, (uint32_t ) &locMsg, 0);

			//	SendTskMsg(FLOW_TSK_ID, TSK_RESETIO, 0, NULL);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if(tskState == SCH_CALI_IDLE)
				{
					uint32_t stTime = GetCurrentST();
					locMsg = *TSK_MSG_CONVERT(event.value.p);
					//updateFlag[UPDATE_CALIBRATION_ID] = UPDATE_ING;
					locMsg.tskState = TSK_SUBSTEP;
					schCaliRangeSel = (locMsg.val.value & MSK_RANGE_SEL );
					caliType = (locMsg.val.value & (MSK_CALI_TRIG|MSK_CALI_SCH|MSK_CALI_AR) );
					assert(schCaliRangeSel < MEAS_RANGE_MAX);
					mainActionDetail = (uint16_t)((schCaliRangeSel<<8) | 0xFF);
					actionRuningDetail = locMsg.val.value;
					currentMeasCaliRangIdx = (uint16_t)schCaliRangeSel;
					caliType = (locMsg.val.value & MSK_MEAS_CALI_TYPE);
					schInfo.lastCaliTime[schCaliRangeSel] = GetCurrentST();
					AlignCliTime(schCaliRangeSel);
					if(caliType != MSK_CALI_SCH)
					{
						actionExecuteTime_ST[ Trigger_Calibration] = stTime;
						schInfo.lastTrigCali = stTime;
					}
					else
					{
						actionExecuteTime_ST[ Sch_Cali] = stTime;
					}
					Trigger_EEPSave((void*)&schInfo, sizeof(schInfo),SYNC_CYCLE);


					UpdateCaliPoint( &caliPoint, schCaliRangeSel );



					caliCycles = 0;
					validResult = FATAL_ERROR;
					caliTimes = 0;
					tskState = SCH_CALI_PRE;
					measDataFlag = (uint16_t)(FLAG_CALI_STD0 | schCaliRangeSel);
					MsgPush (SCH_CALI_ID, (uint32_t)&locMsg, 0);
					uint8_t evData[6];
					evData[0] = (uint8_t)schCaliRangeSel;
					NewEventLog(EV_CALIBRATION, evData);
				}
				else
				{
					if( TSK_MSG_CONVERT(event.value.p)->callBack )
					{
						TSK_MSG_CONVERT(event.value.p)->callBack(DEVICE_BUSY_STATE);
					}
					TraceDBG(taskID,"the request of sch calibration failed!\n");
				}
			}
			else
			{
				tskState = SCH_CALI_IDLE;
			}
		}
	}
}



