/*
 * tsk_sch_meas.c
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
#include "unit_sys_diagnosis.h"
#include "dev_log_sp.h"
#include "dev_logdata.h"
#include "dev_temp.h"
#include "unit_meas_data.h"
#include "unit_io_cfg.h"
#include "unit_cfg_ex.h"
#define LIMIT_IDX_BIG				10.0f
#define LIMIT_IDX_SAMLL				6.0f
#define MAX_AVG_TIMES				5

enum
{
	MEAS_LONG_CALC = 0,
	MEAS_SHORT_CALC,
};
static MeasDataRaw  measDataRaw[MAX_AVG_TIMES];
static __IO uint16_t _longShortSwitch = MEAS_SHORT_CALC;
uint32_t measDuringTime = 7*60;//7 minutes

static float measVal_Final[2];
static const char* taskStateDsp[] =
{
	TO_STR(SCH_MEAS_IDLE),
	TO_STR(SCH_MEAS_PRETREATMENT),
	TO_STR(SCH_MEAS_SAMPLEFLOW_DELAY),
	TO_STR(SCH_MEAS_PRETREATMENT_DELAY),
	TO_STR(SCH_MEAS_MEASURE),
	TO_STR(SCH_MEAS_MEASURE_DELAY),
	TO_STR(SCH_MEAS_FINISH),
	TO_STR(SCH_MEAS_BREAK),
};


static void MeasureFinished(uint32_t val)
{
	(void)val;
	SendTskMsg(SCH_MEAS_ID, TSK_SUBSTEP, 0, NULL);
}

static uint16_t GetNewRangeIdx(float concentration, uint16_t flag, uint16_t rangeIdx)
{
	uint16_t ret = rangeIdx;
	if( (FLAG_SHORT_SAT & flag) == 0)
	{
		for(uint16_t id = 0; id < rangeIdx; id++)
		{
			if(concentration < autoRangeLimit[rangeIdx][id])
			{
				ret = id;
				break;
			}
		}
		if(ret == rangeIdx)
		{
			for(uint16_t id = (uint16_t)(rangeIdx+1); id < MEAS_RANGE_MAX; id++)
			{
				if(concentration > autoRangeLimit[rangeIdx][id])
				{
					ret = id;
					break;
				}
			}
		}
	}
	else
	{
		if(rangeIdx < MEAS_RANGE_HIGH2)
			rangeIdx = AutoRangeChangetoIdx( MEAS_RANGE_HIGH2, 1, __LINE__);
		else
			rangeIdx = AutoRangeChangetoIdx( MEAS_RANGE_HIGH_DILUSION, 1, __LINE__ );
	}
	return ret;

}

static uint16_t RetestCheck(const float concentration, uint16_t rangeIdx)
{
	uint16_t ret = OK;
	if(measSch.retestCfg && (measSch.measAvgMode == 0))
	{
		if(concentration > measSch.retestThreshHold[1] || concentration < measSch.retestThreshHold[0])
		{
			TraceDBG(TSK_ID_SCH_MEAS,"retest is found->%.4f, Max: %.4f, Min:%.4f\n",concentration, measSch.retestThreshHold[1], measSch.retestThreshHold[0]);
			ret = FATAL_ERROR;
		}
		else if(deviationRetestEnable[rangeIdx] != RETEST_NONE)
		{
			if(lastMeasureForRetest < RETEST_DEFAULT_INIT)
			{
				if(deviationRetestEnable[rangeIdx] != RETEST_RELATIVE_CHECK)//check abs deviation
				{
					if( (concentration- lastMeasureForRetest) >= absDeviation[rangeIdx]  || \
							(concentration- lastMeasureForRetest) <= -1.0f*absDeviation[rangeIdx])
					{
						ret |= (FATAL_ERROR | 0x100);
						TraceMsg(TSK_ID_SCH_MEAS,"retest check abs: , current: %.4f - last: %.4f, maxDeviation: %.4f!\n",concentration, lastMeasureForRetest, absDeviation[rangeIdx]);
					}

				}
				if(deviationRetestEnable[rangeIdx] != RETEST_ABS_CHECK)//check relative deviation
				{
					if(( (concentration- lastMeasureForRetest)/lastMeasureForRetest >= relativeDeviation[rangeIdx] ) || \
							( (concentration- lastMeasureForRetest)/lastMeasureForRetest <= -1.0f* relativeDeviation[rangeIdx] ))
					{
						TraceMsg(TSK_ID_SCH_MEAS,"retest check relative: , current: %.4f - last: %.4f, relative_deviation: %.4f!\n",concentration, lastMeasureForRetest, relativeDeviation[rangeIdx]);
						ret |= (FATAL_ERROR | 0x200);
					}
				}
			}
			else
			{
				//no action
			}
		}
	}

	//todo check the meas is in right range;
	//warning: means the data is valid but need to calibrate later;
	//fatal-error meas the data is invalid the calibrate needs to be called immediately;
	return ret;

}


static void GetAvgRawAbs(const MeasDataRaw* result , MeasDataRaw* measDst, uint16_t times)
{

	assert(times>0);
	assert(times <= MAX_AVG_TIMES);
	if(measSch.retestCfg && (measSch.measAvgMode == 0))
	{
		memcpy((void*)measDst, (void*)&result[times-1], sizeof(MeasDataRaw));
	}
	else if(measSch.measAvgMode == 0)
	{
		assert(times ==  1);
		memcpy((void*)measDst, (void*)&result[times-1], sizeof(MeasDataRaw));
	}
	else //measSch.measAvgMode =1
	{
		float val[MAX_AVG_TIMES] = {0,0,0,0,0,};
		uint16_t except[MAX_AVG_TIMES] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
		uint16_t usedId[MAX_AVG_TIMES] =  {0,0,0,0,0,};
		uint16_t actTImes = 0;

		measDst->rawAbs[0]=0.0f;
		measDst->rawAbs[1]=0.0f;
		measDst->rawAbs[2]=0.0f;
		measDst->rawAbs[3]=0.0f;
		measDst->rawFlag = result[0].rawFlag;

		for(uint16_t i=0;i<times;i++)
		{
			if((result[i].rawFlag & FLAG_SHORT_SAT) != 0)
			{
				except[i] = i;
				TraceMsg(TSK_ID_SCH_MEAS, "average calculation is executed, raw measure: %d is not used\n",i);
			}
			else
			{
				val[actTImes] = result[i].rawAbs[MEA_STEP_660nm_SHORT] - calibSch.caliFactor880 * result[i].rawAbs[MEA_STEP_880nm_SHORT];
				usedId[actTImes] = i;
				actTImes ++;
			}
		}
		if(actTImes != times)
		{
			TraceDBG(TSK_ID_SCH_MEAS, "Some Invalid measure Result for average is not used! actual: %d < set:%d\n", actTImes, times);

			if(actTImes == 0)
			{
				TraceDBG(TSK_ID_SCH_MEAS, "No valid measure Result for average is found! use last value instead\n");
				memcpy((void*)measDst, (void*)&result[times-1], sizeof(MeasDataRaw));
				return;
			}
		}

		if(actTImes < 3)
		{
			for(uint16_t i=0;i<times;i++)
			{
				if(except[i] != i)
				{
					measDst->rawAbs[0] += result[i].rawAbs[0];
					measDst->rawAbs[1] += result[i].rawAbs[1];
					measDst->rawAbs[2] += result[i].rawAbs[2];
					measDst->rawAbs[3] += result[i].rawAbs[3];
					measDst->rawFlag |= result[i].rawFlag;
				}
			}
			measDst->rawAbs[0] /= (1.0f* actTImes);
			measDst->rawAbs[1] /= (1.0f* actTImes);
			measDst->rawAbs[2] /= (1.0f* actTImes);
			measDst->rawAbs[3] /= (1.0f* actTImes);
			TraceMsg(TSK_ID_SCH_MEAS, "average calculation is executed for avg times:%d\n",actTImes);
		}
		else if(actTImes == 3)
		{
			uint16_t idx = 0;
			if(val[0] >= val[1])
			{
				if(val[2] >= val[0])		idx = usedId[0];
				else if(val[2] <= val[1])	idx = usedId[1];
				else						idx = usedId[2];
			}
			else
			{
				if(val[2] >= val[1])		idx = usedId[1];
				else if(val[2] <= val[0])	idx = usedId[0];
				else						idx = usedId[2];
			}
			TraceMsg(TSK_ID_SCH_MEAS, "average calculation is executed for avg times:%d, use the mid measure result: %d\n",actTImes,idx);
			memcpy((void*)measDst, (void*)&result[idx], sizeof(MeasDataRaw));
		}
		else//actTImes>3?
		{
			float min = val[0];
			float max = val[0];
			float ration = (float)(1.0f*(float)(actTImes-2));
			uint16_t minIdx = 0;
			uint16_t maxIdx = 0;
			for(uint16_t i=1;i<actTImes;i++)
			{
				if(val[i] <= min)
				{
					TracePrint(TSK_ID_SCH_MEAS, "min is found: old %f, new %f, id: %d\n",min, val[i], i);
					minIdx = i;
					min = val[i];
				}
				else if(val[i] >= max)
				{
					TracePrint(TSK_ID_SCH_MEAS, "max is found: old %f, new %f, id: %d\n",max, val[i], i);
					maxIdx = i;
					max = val[i];
				}
				else
				{}
			}
			assert(minIdx != maxIdx );

			for(uint16_t i=0;i<actTImes;i++)
			{
				if( (i != minIdx) && (i != maxIdx))
				{
					uint16_t id = usedId[i];

					measDst->rawAbs[0] += result[id].rawAbs[0];
					measDst->rawAbs[1] += result[id].rawAbs[1];
					measDst->rawAbs[2] += result[id].rawAbs[2];
					measDst->rawAbs[3] += result[id].rawAbs[3];
					measDst->rawFlag |= result[id].rawFlag;
				}
			}
			measDst->rawAbs[0] /= ration;
			measDst->rawAbs[1] /= ration;
			measDst->rawAbs[2] /= ration;
			measDst->rawAbs[3] /= ration;
			TracePrint(TSK_ID_SCH_MEAS, "Short valid value: %f, %f, %f, %f, %f,id is removed: %d,%d\n",val[0],val[1],val[2],val[3],val[4], minIdx, maxIdx);

			TraceMsg(TSK_ID_SCH_MEAS, "average calculation is executed for avg times:%d, remove the Max %d,Min %d\n",actTImes,usedId[maxIdx], usedId[minIdx]);
		}
	}
}


float CalcConcentration(const MeasDataRaw* measResult, uint16_t rangeIdx, uint16_t type)
{
	float val = 0.0f;
	float absLong  = 55.0f;
	float absShort = 55.0f;
	uint16_t used_longShortSwitch = _longShortSwitch;
	assert(rangeIdx < MEAS_RANGE_MAX);


	if(measDataSaturation == NONE_SATURATION)
	{
		absLong = measResult->rawAbs[MEA_STEP_660nm_LONG] - calibSch.caliFactor880 * measResult->rawAbs[MEA_STEP_880nm_LONG];
	}
	//if long==55
	if(measDataSaturation < SHORT_SATURATION)
	{
		used_longShortSwitch = MEAS_SHORT_CALC;
		if(type == 1)
			_longShortSwitch = MEAS_SHORT_CALC;

		absShort = measResult->rawAbs[MEA_STEP_660nm_SHORT] - calibSch.caliFactor880 * measResult->rawAbs[MEA_STEP_880nm_SHORT];
	}
	if(rangeIdx < MEAS_RANGE_HIGH2)
		absMeasConcentration[0] = (absLong - calibSch.calibrationOffset_Long[rangeIdx])* calibSch.calibrationSlope_Long[rangeIdx];
	else
		absMeasConcentration[0] = 0.0f;

	absMeasConcentration[1] = (absShort - calibSch.calibrationOffset_Short[rangeIdx])* calibSch.calibrationSlope_Short[rangeIdx];

	if(rangeIdx == MEAS_RANGE_HIGH_DILUSION)
	{
		absMeasConcentration[1] = (absShort - calibSch.calibrationOffset_Short[rangeIdx]) * calibSch.calibrationSlope_Short[rangeIdx] * calibSch.calibDilution;
	}

	if(rangeIdx <= MEAS_RANGE_HIGH)
	{
		if(used_longShortSwitch == MEAS_LONG_CALC)
		{
			val = absMeasConcentration[0];
			if(type == CHECK_LONG_SHORT)
			{
				if(val >= longShortSwitchLimit[rangeIdx][0])
				{
					TraceMsg(TSK_ID_SCH_MEAS, "change to use short calculation: %.3f > %.3f -> %.3f\n", val, longShortSwitchLimit[rangeIdx][0], absMeasConcentration[1]);
					val = absMeasConcentration[1];
					_longShortSwitch = MEAS_SHORT_CALC;
				}
			}
		}
		else
		{
			val = absMeasConcentration[1];
			if(type == CHECK_LONG_SHORT)
			{
				if(val <= longShortSwitchLimit[rangeIdx][1])
				{
					TraceMsg(TSK_ID_SCH_MEAS, "change to use long calculation: %.3f < %.3f ->%.3f\n", val, longShortSwitchLimit[rangeIdx][1], absMeasConcentration[0]);
					val = absMeasConcentration[0];
					_longShortSwitch = MEAS_LONG_CALC;
				}
			}
		}
	}
	else
	{
		if(type == 1)
			_longShortSwitch = MEAS_SHORT_CALC;
		val = absMeasConcentration[1];
	}
	absMeasConcentration[2] = val;

	if(type != TEMP_CALC_CONCENTRATION)
	{
		TraceMsg(TSK_ID_SCH_MEAS, "Raw Meas: %.3f, [ %.3f ,%.3f , %.3f ]\n", val, absMeasConcentration[0], absMeasConcentration[1], absMeasConcentration[2]);
		if(type == CHECK_LONG_SHORT)
		{
			memcpy((void*)&_absMeasFinal[0], (void*)&absMeasConcentration[0], sizeof(absMeasConcentration));
			TraceMsg(TSK_ID_SCH_MEAS, "Final Meas for Adaption: %.3f, [ %.3f ,%.3f , %.3f ]\n", val, _absMeasFinal[0], _absMeasFinal[1], _absMeasFinal[2]);
		}
	}

	return val;
}


static float CalcConcentrationWithAdaption(MeasDataRaw* measResult, uint16_t rangeIdx, uint16_t type)
{
	float val = CalcConcentration(measResult, rangeIdx, type);

	float val1 = (val * calibSch.adaptionFactor[rangeIdx]) + calibSch.adaptionOffset[rangeIdx];
	//(val + calibSch.adaptionOffset[rangeIdx]) * calibSch.adaptionFactor[rangeIdx];
	measVal_Final[0] = (_absMeasFinal[0] * calibSch.adaptionFactor[rangeIdx]) + calibSch.adaptionOffset[rangeIdx];
	measVal_Final[1] = (_absMeasFinal[1] * calibSch.adaptionFactor[rangeIdx]) + calibSch.adaptionOffset[rangeIdx];
	TraceUser("CalcConcentrationWithAdaption: %.06f,\t %.06f,\t %.06f,\t %.06f,\t %.06f,\t %.06f,\t, Flag: 0x%x\n",\
			val, val1,_absMeasFinal[0],_absMeasFinal[1],measVal_Final[0], measVal_Final[1], measDataFlag);
	return val1;
}

//calc the final measure result;
static float CalcMeasureResult(MeasDataSt* measResult, MeasDataRaw* _measDataRaw, uint16_t _measureTimes, uint16_t rangeIdx)
{
	float val = 0.0f;
	MeasDataRaw avgResult;

	assert(MEAS_RANGE_MAX > rangeIdx);


	GetAvgRawAbs(_measDataRaw, &avgResult, _measureTimes);



	val = CalcConcentrationWithAdaption(&avgResult, rangeIdx, CHECK_LONG_SHORT);

	memcpy( (void*)&measResult->rawAbs[0], (void*)&avgResult.rawAbs[0],sizeof(avgResult.rawAbs));

	if(rangeIdx <= MEAS_RANGE_HIGH)
	{
		if(_longShortSwitch == MEAS_LONG_CALC)
		{
			measResult->caliOffset = calibSch.calibrationOffset_Long[rangeIdx];
			measResult->caliSlope = calibSch.calibrationSlope_Long[rangeIdx];
		}
		else
		{
			measResult->caliOffset = calibSch.calibrationOffset_Short[rangeIdx];
			measResult->caliSlope = calibSch.calibrationSlope_Short[rangeIdx];
		}
	}
	else
	{
		measResult->caliOffset = calibSch.calibrationOffset_Short[rangeIdx];
		measResult->caliSlope = calibSch.calibrationSlope_Short[rangeIdx];
	}
	measResult->caliTimeST = calibSch.calibration_ST[rangeIdx];

	measResult->adaptionOffset = calibSch.adaptionOffset[rangeIdx];
	measResult->adaptionFactor = calibSch.adaptionFactor[rangeIdx];

	measResult->dilutionFactor = 1.0;
	measResult->temperature[0] = (int16_t)GetTempChn(0);
	measResult->temperature[1] = (int16_t)GetTempChn(1);
	measResult->temperature[2] = (int16_t)GetTempChn(2);
	measResult->temperature[3] = (int16_t)GetTempChn(3);
	//todo
	measResult->measFlag = FLAG_CALI_STATUS_OK;
	uint16_t validCaliFlag = GetCaliFlag(TSK_ID_SCH_MEAS, rangeIdx);
	if(validCaliFlag != OK)
	{
		measResult->measFlag = FLAG_CALI_STATUS_ER;
	}
	measResult->measFlag = (uint16_t)(measResult->measFlag | (avgResult.rawFlag & FLAG_MEAS_MSK));
	measResult->measRangeIdx = (uint16_t)(rangeIdx | _longShortSwitch<<15);
	return val;


}

void CheckMeasRange(uint16_t rangeIdx, float measValue)
{

	//check range
	uint16_t val1 =  ( measValue < measRangeAlarmLimit[0])? 1: 0;
	rangeIdx = rangeIdx & FLAG_RANGE_MSK;
	if(val1)
		SendTskMsg(SCH_IO_ID, TSK_INIT, RO_ALARMLOW_SET , NULL);
	else
		SendTskMsg(SCH_IO_ID, TSK_INIT, RO_ALARMLOW_CLR, NULL);

	val1 =  ( measValue > measRangeAlarmLimit[1])? 2: 0;
	if(val1)
		SendTskMsg(SCH_IO_ID, TSK_INIT, RO_ALARMHIGH_SET, NULL);
	else
		SendTskMsg(SCH_IO_ID, TSK_INIT, RO_ALARMHIGH_CLR , NULL);


	Dia_UpdateDiagnosis(ALARM_LOW, ( measValue < measRangeAlarmLimit[0]));
	Dia_UpdateDiagnosis(ALARM_HIGH, ( measValue > measRangeAlarmLimit[1]));

	val1 =  ( measValue > measLimitRange[rangeIdx][1])? 1: 0;
	uint16_t val2 =  ( measValue < measLimitRange[rangeIdx][0])? 1: 0;

	Dia_UpdateDiagnosis(MEAS_OUT_OF_RANGE, (uint16_t)(val1 + val2));

}

static void NewMeasuredResult(MeasDataSt* measResult, MeasDataRaw* _measDataRaw, uint16_t __measureTimes, uint32_t measureStartTime, uint16_t rangeIdx)
{

	measResult->measValue = CalcMeasureResult(measResult, _measDataRaw, __measureTimes, rangeIdx);
	measResult->startTimeST = measureStartTime ;
	TraceUser("Calc Final measure Result with adaption: \t %.06f, flag: 0x%x\n",measResultRealTime.measValue, measResultRealTime.measFlag);
	//save to flash
	const LogData* dataInst = GetLogDataInst(HISTORY_MEASDATA_LOG);
	if(dataInst)
	{
		MeasData* measInst = dataInst->wrData.ptrMeasData;

		dataInst->Lock();
		memcpy((void*)&measInst->measDataSt, (void*)measResult, sizeof(measInst->measDataSt));
		dataInst->UnLock();

		dataInst->UpdateData(dataInst);
		Trigger_Save2FF(NULL);
	}

}


void Fake_TrigMeasStorage(void)
{
	const LogData* dataInst = GetLogDataInst(HISTORY_MEASDATA_LOG);
	if (dataInst)
	{
		MeasData* measInst = dataInst->wrData.ptrMeasData;
		measInst->measDataSt.startTimeST = GetCurrentST();
		dataInst->UpdateData(dataInst);
		Trigger_Save2FF(NULL);
	}
}
static uint16_t firstMeasure = 0;
static uint32_t measureType = 0;
static uint16_t validMeasureData = 0;
static uint32_t changedRange = 0;
static uint16_t sampleFlow = OK;
static const uint8_t taskID = TSK_ID_SCH_MEAS;
static uint32_t measureStartTime = 0;

static void IssueToUI(uint32_t stTime)
{
	static uint32_t oldTime = 0;
	//todo
	if (stTime != oldTime)
	{
		if((measResultRealTime.measFlag & FLAG_POST_STD1) == FLAG_POST_STD1)
		{
			TraceMsg(taskID, "Post std1 verification: %.4f flag: %x not updated to UI and modbus\n",measResultRealTime.measValue, measResultRealTime.measFlag);
		}
		else
			UpdateResult2UI_Pre(UPDATE_MEASURE_SCH_ID);
	}
	oldTime = stTime;
}

uint32_t GetMeasDuringTime_Ms(uint32_t rangeIdx)
{

	uint32_t idx = ( rangeIdx & MSK_RANGE_SEL);
	uint32_t measureType = (rangeIdx & MSK_MEAS_CALI_TYPE);

	assert(MEAS_RANGE_MAX > idx);


	uint16_t measureSchSteps = measSchSteps[idx];
/*	if(measureType == MSK_MEAS_OFFLINE)
	{
		measureSchSteps = measOffLineSteps[idx];
	}
	else
	*/
	if(measureType == MSK_MEAS_ONLINE)
	{
		measureSchSteps = measOnLineSteps[idx];
	}
	else if(measureType == MSK_MEAS_STD0)
	{
		measureSchSteps = measStandard0Steps[idx];
	}
	else if( (measureType == MSK_MEAS_STD1) || (measureType == MSK_POST_STD1) )
	{
		measureSchSteps = measStandard1Steps[idx];
	}
	else
	{
		measureSchSteps = measSchSteps[idx];
	}
	uint32_t val = CalcDuringTimeMsStep_WithDelay(measureSchSteps);
	if(measSch.measAvgMode != 0)
		val = (uint32_t)measSch.measAvgTime * val;
	else if(measSch.retestCfg != 0)
		val = 2 * val;
	val += (uint32_t)measSch.preTreatDelay*1000;
	return val;
}





static uint16_t HandlePostVerify(uint16_t schMeasSel, float measValue, float* final)
{
	static uint16_t times = 0;
	uint16_t ret = OK;
	schMeasSel = ( schMeasSel & MSK_RANGE_SEL);
	float deviation = std1VeriDeviation * calibSch.caliConcentration[schMeasSel];
	float currDeviation = calibSch.caliConcentration[schMeasSel] - measValue;
	float currDeviation_S = calibSch.caliConcentration[schMeasSel] - final[0];
	float currDeviation_L = calibSch.caliConcentration[schMeasSel] - final[1];
//	float absVal = abs(calibSch.caliConcentration[schMeasSel] - measValue);
	if(currDeviation < 0)
		currDeviation = -1.0f * currDeviation;
	if(currDeviation_S < 0)
		currDeviation_S = -1.0f * currDeviation_S;
	if(currDeviation_L < 0)
		currDeviation_L = -1.0f * currDeviation_L;

	if( deviation <= currDeviation )
	{
		ret = FATAL_ERROR;
	}
	else if(schMeasSel <= MEAS_RANGE_HIGH)
	{
		if( (deviation <= currDeviation_S) || (deviation <= currDeviation_L ))
		{
			ret = FATAL_ERROR;
		}
	}
	TraceMsg(taskID, "Post std1 verification: %.4f,%.4f,%.4f,calc: %.4f, %.4f; C:%.4f,D:%.4f range index: %d, ret: %d\n",measValue, final[0], final[1],currDeviation,0,
			calibSch.caliConcentration[schMeasSel],
			deviation, schMeasSel, ret);

	times++;
	if(ret != OK)
	{
		//retore the calibration result;
		UpdateCalibResult(RECOVER_FROM_TMP);
		if(times >= caliTimesMax)
		{
			ret = OK;
			TraceMsg(taskID, "restore the calibration result: %d, times: %d, and force not to calibration again\n",ret, times);
			times = 0;
		}
		else
		{
			TraceMsg(taskID, "restore the calibration result: %d, times: %d\n",ret, times);
		}
	}
	else
	{
		TraceMsg(taskID, "Check OK, not to restore: %d, times: %d\n",ret, times);
		times = 0;
	}
	if(std1VeriEnable && (ret != OK))
	{
		TrigAction locTrigger;
		uint16_t schRequest = 1;
		locTrigger.byte.triggerType = IO_ACTION;
		locTrigger.byte.action = Trigger_Calibration;
		locTrigger.byte.value0 = 0xFF;
		SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
		SCH_Put(OBJ_SYSTEM_STARTTRIG,WHOLE_OBJECT,(void*)&schRequest);

		TraceMsg(taskID, "another calibration is triggered: %x\n",locTrigger.u32Val);
	}
	return ret;
}



static SCH_MEAS_STATE Handle_MeasurementDelay(uint32_t schMeasSel)
{
	SCH_MEAS_STATE tskState;

	//get raw measure data from measure calc task;
	measDataRaw[measureTimes].rawFlag = measDataFlag;

	if( GetNewRawMeasDataResult(&measDataRaw[measureTimes]) == OK )
	{
		//measureTimes++;
		validMeasureData = 1;
		firstMeasure++;
	}
	else
	{
		validMeasureData = 0;
		firstMeasure++;
		tskState = SCH_MEAS_FINISH;
		TraceDBG(taskID,"no valid measure step is called, did not get measure result.\n");
	}
	//if get valid measure data;
	if(validMeasureData)
	{
		float concentration = CalcConcentrationWithAdaption(&measDataRaw[0], (uint16_t)schMeasSel, CHECK_LONG_SHORT);

		uint16_t autoRangeCheck = 0;
		//if range idx is same, do auto range check
		if(measSch.measCaliIndex == schMeasSel)
		{
			if(( (measureType == MSK_MEAS_SCH) || (measureType == MSK_MEAS_TRIG ))  && (autoRangeMode<2) )
			{
				autoRangeCheck = firstMeasure;
			}
		}

		//if auto range check is required
		if(autoRangeCheck == 1)
		{
			//first measure result
			//float measureValue = CalcMeasureResult(&measResultRealTime,&measDataRaw[0], 1, (uint16_t)schMeasSel);
			//if saturation
			if(measDataSaturation >= SHORT_SATURATION)
			{
				if(schMeasSel < MEAS_RANGE_HIGH2)
					changedRange = AutoRangeChangetoIdx( MEAS_RANGE_HIGH2, 1, __LINE__);
				else
					changedRange = AutoRangeChangetoIdx( MEAS_RANGE_HIGH_DILUSION, 1, __LINE__ );
			}
			else
			{
				uint16_t _rangeIdx = GetNewRangeIdx(concentration,0x0, (uint16_t)schMeasSel);
				//get new range idx;
				if(_rangeIdx != schMeasSel)
				{
					changedRange = AutoRangeChangetoIdx(_rangeIdx, 0, __LINE__);
				}
			}
			if(changedRange)
				autoRangeCheck = 0xFFFF;
		}

		//No need to do auto range immediately
		if(autoRangeCheck != 0xFFFF)
		{
			if(measSch.measAvgMode == 0)
			{
				tskState = SCH_MEAS_FINISH;

				if(measSch.retestCfg)
				{
					if(reTestCount == 0)
					{
						uint16_t validRange = RetestCheck(concentration, (uint16_t)schMeasSel);

						if(validRange != OK)
						{
							reTestCount++;
							TraceDBG(taskID,"retest is required->%d, ret: %x -%d!\n",reTestCount, validRange, validRange);
							tskState = SCH_MEAS_MEASURE;
							measureTimes = 0;
						}
						else
						{
							measureTimes++;
						}
					}
					else
					{
						TraceDBG(taskID,"retest is executed, use last value, count == %d!\n",reTestCount);
						measureTimes++;
					}
				}
				else
				{
					measureTimes++;
				}
				if(measureTimes)
				{
					//calc and store
					NewMeasuredResult(&measResultRealTime,&measDataRaw[0], measureTimes, measureStartTime, (uint16_t)schMeasSel);
					IssueToUI(measResultRealTime.startTimeST);
					measureTimes = 0;
				}
			}
			//average mode
			else
			{
				measureTimes++;
				tskState = SCH_MEAS_MEASURE;
				if(measureTimes >= measSch.measAvgTime)
				{
					tskState = SCH_MEAS_FINISH;
					NewMeasuredResult(&measResultRealTime,&measDataRaw[0], measureTimes,measureStartTime, (uint16_t)schMeasSel);
					IssueToUI(measResultRealTime.startTimeST);
				}
			}
		}
		//need to do auto range immediately
		else
		{
			measureTimes++;
			measResultRealTime.measValue = CalcMeasureResult(&measResultRealTime, &measDataRaw[0], measureTimes, (uint16_t)schMeasSel);
			measResultRealTime.startTimeST = measureStartTime ;
			tskState = SCH_MEAS_FINISH;
		}
	//	Trigger_EEPSave((void*)&measResultRealTime, sizeof(measResultRealTime), SYNC_CYCLE);
	}
	else
	{
		tskState = SCH_MEAS_FINISH;
	}

	return tskState;
}

void IssueFinalResult(void)
{
	schInfo.lastMeasFinish = GetCurrentST();
	lastMeasureForRetest = measResultRealTime.measValue;
	UpdateAOSrc(measResultRealTime.measValue,measResultRealTime.measRangeIdx, measResultRealTime.measFlag, 0);
	TraceUser("Used Concentration: \t %.06f, flag: 0x%x\n",measResultRealTime.measValue, measResultRealTime.measFlag);
	IssueToUI(measResultRealTime.startTimeST);
}

void StartSchMeasTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	uint32_t duringTime;
	osEvent event;
	uint16_t flowStep;

	TSK_MSG locMsg;

	locMsg.callBack = NULL;
	uint32_t schMeasSel = 0xFFFFFFF;
	uint16_t measureSchSteps = 0xFFFF;
	SCH_MEAS_STATE tskState = SCH_MEAS_IDLE;
	uint32_t measResult = OK;



	TracePrint(taskID,"started\n");
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_MEAS_ID, tickOut);


		//new event
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
				case SCH_MEAS_SAMPLEFLOW_DELAY:
				{
					sampleFlow = Dia_SampleFlow();
					if(sampleFlow != OK)
					{
						tskState = SCH_MEAS_FINISH;
						locMsg.tskState = TSK_SUBSTEP;
						SendTskMsg(SCH_IO_ID, TSK_INIT, RO_PRETREATMENT_CLR, NULL);
						MsgPush (SCH_MEAS_ID, (uint32_t)&locMsg, 0);
					}
					else
					{
						if(measSch.sampleFlowDetectTime > measSch.preTreatDelay)
						{
							tskState = SCH_MEAS_MEASURE;
							locMsg.tskState = TSK_SUBSTEP;
							MsgPush (SCH_MEAS_ID, (uint32_t)&locMsg, 0);
						}
						else
						{
							//output IO value
							tskState = SCH_MEAS_PRETREATMENT_DELAY;
							tickOut = (uint32_t)(measSch.preTreatDelay - measSch.sampleFlowDetectTime)*1000;
						}
					}
				}
					break;
				case SCH_MEAS_PRETREATMENT_DELAY:
					SendTskMsg(SCH_IO_ID, TSK_INIT, RO_PRETREATMENT_CLR, NULL);
					if(measSch.sampleFlowDetectTime > measSch.preTreatDelay)
					{
						tskState = SCH_MEAS_SAMPLEFLOW_DELAY;
						tickOut = (uint32_t)(measSch.sampleFlowDetectTime  - measSch.preTreatDelay)*1000;
					}
					else
					{
						tskState = SCH_MEAS_MEASURE;
						locMsg.tskState = TSK_SUBSTEP;
						MsgPush (SCH_MEAS_ID, (uint32_t)&locMsg, 0);
					}
					break;
				case SCH_MEAS_MEASURE_DELAY:
					TraceDBG(taskID,"the meas_task failed -- SCH_MEAS_MEASURE_DELAY!\n");
					tskState = SCH_MEAS_MEASURE;
					locMsg.tskState = TSK_SUBSTEP;
					MsgPush (SCH_MEAS_ID, (uint32_t)&locMsg, 0);
					break;

				default:
					tskState = SCH_MEAS_IDLE;
					break;
			}
		}
		//new event
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if(  mainTskState == TSK_SUBSTEP)
			{
				switch(tskState)
				{
					case SCH_MEAS_PRETREATMENT:
						duringTime = measSch.preTreatDelay;
						SetFlowStep(FLOW_STEP_ACT, MAINACT_PRE_TREATMENT, duringTime);

						TraceMsg(taskID,"sch_meas task execute: SCH_MEAS_PRETREATMENT\n");
						RTC_Get(IDX_RTC_ST,0,(void*)&measureStartTime);
						if(measSch.preTreatDelay)
						{
							SendTskMsg(SCH_IO_ID, TSK_INIT, RO_PRETREATMENT_SET, NULL);
						}

						if(measSch.sampleFlowDetectTime > measSch.preTreatDelay)
						{
							//output IO value
							tskState = SCH_MEAS_PRETREATMENT_DELAY;
							tickOut = (uint32_t)measSch.preTreatDelay*1000;
						}
						else
						{
							//output IO value
							tskState = SCH_MEAS_SAMPLEFLOW_DELAY;
							tickOut = (uint32_t)measSch.sampleFlowDetectTime*1000;
						}

						break;
					case SCH_MEAS_MEASURE:
					{
						 uint32_t stTime = GetCurrentST();

						TraceMsg(taskID,"sch_meas task execute: SCH_MEAS_MEASURE\n");
						tskState = SCH_MEAS_MEASURE_DELAY;
						SendTskMsg(FLOW_TSK_ID, TSK_INIT, measureSchSteps, MeasureFinished);
						tickOut = CalcDuringTimeMsStep_WithDelay(measureSchSteps);
						/*
						if(measureType == MSK_MEAS_OFFLINE)
						{
							actionExecuteTime_ST[Trigger_Offline] = stTime;
							flowStep = MAINACT_OFFLINE;
						}
						else
							*/
						if(measureType == MSK_MEAS_ONLINE)
						{
							actionExecuteTime_ST[Trigger_Online] = stTime;
							flowStep = MAINACT_ONLINE;
						}
						else if(measureType == MSK_MEAS_STD0)
						{
							actionExecuteTime_ST[Standard0_Verification] = stTime;
							flowStep = MAINACT_STD0_V;
						}
						else if( (measureType == MSK_MEAS_STD1) || (measureType == MSK_POST_STD1) )
						{
							actionExecuteTime_ST[Standard1_Verification] = stTime;
							flowStep = MAINACT_STD1_V;
						}
						else if(MSK_MEAS_TRIG == measureType)
						{
							actionExecuteTime_ST[ Trigger_Measure] = stTime;
							flowStep = MAINACT_MEAS;
						}
						else
						{
							actionExecuteTime_ST[ Sch_Measure] = stTime;
							flowStep = MAINACT_MEAS;
						}
						duringTime = (uint32_t)(tickOut + 500)/1000;
						SetFlowStep(FLOW_STEP_ACT, flowStep, duringTime);
					}
						break;
					case SCH_MEAS_MEASURE_DELAY:

						tskState = Handle_MeasurementDelay(schMeasSel);

						MsgPush (SCH_MEAS_ID, (uint32_t)&locMsg, 0);
						break;
					case SCH_MEAS_FINISH:

						firstMeasure = 0;
						TraceMsg(taskID,"sch_meas task execute: SCH_MEAS_FINISH\n");
						tskState = SCH_MEAS_IDLE;
						measResult = OK;
						if( (std1VeriEnable != 0) && (measureType == MSK_POST_STD1))
						{
							HandlePostVerify((uint16_t)schMeasSel, measResultRealTime.measValue, measVal_Final);
						}

						//if the measure idx is changed, the following case is not accessed!
						if(sampleFlow != OK)
						{
							TraceDBG(taskID, "Measure is broken due to No SampleFlow.\n");
							if((schMeasSel == measSch.measCaliIndex) && ((measureType == MSK_MEAS_SCH) || (measureType == MSK_MEAS_TRIG) ) )
							{
								schInfo.lastMeasFinish = GetCurrentST();
								TraceUser("Update measurement finish time:%d\n",schInfo.lastMeasFinish);
								TraceUser("Used last Concentration(no other actions): \t %.06f, flag: 0x%x \n",measResultRealTime.measValue, measResultRealTime.measFlag);
							}
							else
							{
								TraceUser("Not update measurement time and also no result if issued\n");
							}
						}
						else if((schMeasSel == measSch.measCaliIndex) && ( validMeasureData != 0))
						{
							if((measureType == MSK_MEAS_SCH) || (measureType == MSK_MEAS_TRIG) )
							{

								//todo: finalize the auto range mode;
								//not request range to change yet;

								if(autoRangeMode<2)
								{
									measResult = FATAL_ERROR;
									TraceMsg(taskID,"auto range check: %d,\n",changedRange);
									//if no need to change range
									if(changedRange == 0)
									{
										uint16_t _newRangeIdx = GetNewRangeIdx(measResultRealTime.measValue,measResultRealTime.measFlag, (uint16_t)schMeasSel);
										if(_newRangeIdx != schMeasSel)
										{
											if( 0 == AutoRangeChangetoIdx(_newRangeIdx, 2, __LINE__) )
											{
												measResult = OK;
												IssueFinalResult();
											}
										}
										else
										{
											measResult = OK;
											IssueFinalResult();
										}
									}
									if(measResult != OK)
										TraceDBG(taskID,"auto range has no result!\n");
									//auto range mode, to remove the fix range alarm if this happens
									Dia_UpdateDiagnosis(FIX_RANGE_WARNING, 0);
								}
								else
								{
									//finish only when finalized ok;
									IssueFinalResult();
									uint16_t ret = GetNewRangeIdx(measResultRealTime.measValue, measResultRealTime.measFlag, (uint16_t)schMeasSel);
									if(ret != schMeasSel)
									{
										Dia_UpdateDiagnosis(FIX_RANGE_WARNING, 1);
										TraceMsg(taskID,"final measure range shall change %d to %d,\n",measSch.measCaliIndex , ret);
									}
									else
									{
										Dia_UpdateDiagnosis(FIX_RANGE_WARNING, 0);
									}
								}

							}
						}
						changedRange = 0;
						Trigger_EEPSave((void*)&schInfo, sizeof(schInfo),SYNC_CYCLE);

						if( locMsg.callBack )
						{
							locMsg.callBack(measResult);
						}
						break;
					case SCH_MEAS_IDLE:
						break;
				//	case SCH_MEAS_SAMPLEFLOW_DELAY:
				//		tickOut = 0;//to timeout -> SCH_MEAS_SAMPLEFLOW_DELAY
				//		locMsg.tskState = TSK_SUBSTEP;
				//		break;
					default:
						if( locMsg.callBack )
						{
							locMsg.callBack(OK);
						}
						locMsg.callBack = NULL;
						TraceDBG(taskID,"sch_meas has bugs: %s: %d,\t%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);
						tskState = SCH_MEAS_IDLE;
						break;
				}
				// substep
			}
			else if ( mainTskState == TSK_FORCE_BREAK)
			{
				reTestCount = 0;
				tskState = SCH_MEAS_IDLE;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_MEAS_ID, (uint32_t )&locMsg, 0);

			//	SendTskMsg(FLOW_TSK_ID, TSK_FORCE_BREAK, 0, NULL);
			//	SendTskMsg(SCH_IO_ID, TSK_FORCE_BREAK, 0, NULL);
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = SCH_MEAS_IDLE;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(SCH_MEAS_ID, (uint32_t ) &locMsg, 0);
			//	SendTskMsg(FLOW_TSK_ID, TSK_RESETIO, 0, NULL);
			}
			else if ( mainTskState == TSK_INIT)
			{
				if(tskState == SCH_MEAS_IDLE)
				{
					//updateFlag[2] = UPDATE_ING;
					reTestCount = 0;

					locMsg = *TSK_MSG_CONVERT(event.value.p);
					firstMeasure = 0;
					sampleFlow = OK;
					schMeasSel = ( locMsg.val.value & MSK_RANGE_SEL);
					measureType = (locMsg.val.value & MSK_MEAS_CALI_TYPE);
					currentMeasCaliRangIdx = (uint16_t)schMeasSel;
					assert(MEAS_RANGE_MAX > schMeasSel);
					actionRuningDetail = locMsg.val.value;
					mainActionDetail = (uint16_t)((currentMeasCaliRangIdx<<8) | 0xFF);
					uint32_t stTime = GetCurrentST();

					if(measureType == MSK_MEAS_SCH)
					{
						measDataFlag = (uint16_t)(FLAG_MEA_MEAS | schMeasSel);
						measureSchSteps = measSchSteps[schMeasSel];
						schInfo.lastMeasTime = stTime;
						Trigger_EEPSave((void*)&schInfo, sizeof(schInfo),SYNC_CYCLE);
						//this code is only enabled when poll is enabled
#define POLL_CMD
#ifdef POLL_CMD
						uint16_t check = MeasExecuteOnce();
						if(check != OK)
						{
							//no action is taken;
							tskState = SCH_MEAS_FINISH;
						}
						else
#endif
						{
							//scheduled by schedule task;
							measureTimes = 0;
							tskState = SCH_MEAS_PRETREATMENT;
							//assert(measSch.sampleFlowDetectTime <= measSch.preTreatDelay);
							//the put value has no meanings.

						}
					}
					else if(measureType==MSK_MEAS_TRIG)
					{
						measDataFlag = (uint16_t)(FLAG_MEA_TRIG | schMeasSel);
						measureSchSteps = measSchSteps[schMeasSel];
						measureTimes = 0;
						schInfo.lastTrigMeas = stTime;
						if(measSch.measCaliIndex == schMeasSel)
							schInfo.lastMeasTime = schInfo.lastTrigMeas;
						tskState = SCH_MEAS_PRETREATMENT;
					}
					else
					{
						RTC_Get(IDX_RTC_ST,0,(void*)&measureStartTime);
						measureTimes = 0;
						tskState = SCH_MEAS_MEASURE;
						//standard 0 and standard 1 and online and offline
						if(measureType == MSK_MEAS_ONLINE)
						{
							schInfo.lastTrigOffline = stTime;
							measDataFlag = (uint16_t)(FLAG_MEA_ONLINE | schMeasSel);
							measureSchSteps = measOffLineSteps[schMeasSel];
						}
						else if(measureType == MSK_MEAS_STD0)
						{
							schInfo.lastTrigStd0 = stTime;
							measDataFlag = (uint16_t)(FLAG_MEA_STD0 | schMeasSel);
							measureSchSteps = measStandard0Steps[schMeasSel];
						}
						else if(measureType == MSK_MEAS_STD1)
						{
							schInfo.lastTrigStd1 = stTime;
							measDataFlag = (uint16_t)(FLAG_MEA_STD1 | schMeasSel);
							measureSchSteps = measStandard1Steps[schMeasSel];
						}
						else if(measureType == MSK_POST_STD1)
						{
							schInfo.lastTrigStd1 = stTime;
							measDataFlag = (uint16_t)(FLAG_POST_STD1 | schMeasSel);
							measureSchSteps = measStandard1Steps[schMeasSel];
						}
						else
						{
							schInfo.lastTrigMeas = stTime;
							measDataFlag = (uint16_t)(FLAG_MEA_MEAS | schMeasSel);
							measureSchSteps = measSchSteps[schMeasSel];
							tskState = SCH_MEAS_PRETREATMENT;
						}


					}
					memset((void*)&measResultRealTime.temperature[0],0,sizeof(measResultRealTime.temperature));
					memset((void*)&measResultRealTime.rawAbs[0],0,sizeof(measResultRealTime.rawAbs));
					measResultRealTime.startTimeST = measResultRealTime.caliTimeST = 0;


					uint8_t evData[6];
					evData[0] = (uint8_t)schMeasSel;
					evData[1] = (uint8_t)measureType;
					NewEventLog(EV_MEASURE, evData);
					locMsg.tskState = TSK_SUBSTEP;
					MsgPush (SCH_MEAS_ID, (uint32_t)&locMsg, 0);
				}
				else
				{
					if( TSK_MSG_CONVERT(event.value.p)->callBack )
					{
						TSK_MSG_CONVERT(event.value.p)->callBack(DEVICE_BUSY_STATE);
					}

					TraceDBG(taskID,"the request of sch_meas failed!\n");
				}
			}
			else
			{
				tskState = SCH_MEAS_IDLE;
			}
		}
	}
}

