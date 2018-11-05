/*
 * tsk_meas_calc.c
 *
 *  Created on: 2017骞�2鏈�10鏃�
 *      Author: pli
 */

#include <string.h>
#include <stdint.h>
#include "shell_io.h"
#include "mxconstants.h"
#include "main.h"
#include "ad717x.h"
#include "dev_ad7172.h"
#include "unit_meas_cfg.h"
#include "unit_temp_cfg.h"
#include "dev_temp.h"
#include <math.h>
#include "spi.h"
#include "rtc.h"
#include "tsk_measure.h"
#include "tsk_sch.h"
#include "unit_flow_act.h"
#include "unit_sch_cfg.h"
#include "unit_meas_data.h"
#include "unit_rtc_cfg.h"
#include "t_data_obj_measdata.h"
#include "unit_meas_data.h"
#include "dev_log_sp.h"

extern __IO float    realTimeConcentration;

#define MAP_IDX_ABS_0				0
#define MAP_IDX_ABS_1				1
#define MAP_IDX_ABS_2				2
#define MAP_IDX_ABS_3				3


#define ABS_MIN_VAL		4000
__IO uint16_t measDataFlag = FLAG_MEA_MEAS;


static SemaphoreHandle_t lock = NULL;

__IO uint16_t blankMeasure = 0;
//sum of 10second measure abs of each chn
static float _LocSumData[MEA_DATA_MAX] = {0,0,0,0};
//measure abs of each chn
float absChnValue[MEA_DATA_MAX] = {0,0,0,0};
//average abs of each chn
static float absAvgValue[MEA_DATA_MAX] = {0,0,0,0};

int32_t mea_mean[4][4];
int32_t ref_mean[4][4];


int32_t mea_calc_mean[MEA_DATA_MAX];
int32_t ref_calc_mean[MEA_DATA_MAX];

static __IO uint16_t newMeasData = 0;



static __IO uint16_t _LocAvgIdx = 0;

extern uint16_t measRawValueTimes;
extern uint16_t printAllRawADC;
extern __IO uint32_t osCPU_Usage;

uint16_t GetNewRawCaliDataResult(CaliDataRaw* val)
{
	uint16_t ret = FATAL_ERROR;

	if(lock)	OS_Use(lock);
	if(newMeasData)
	{
		newMeasData = 0;
		ret = OK;
	}
	memcpy(&val->rawAbs[0], &measRawDataResult.rawAbs[0],sizeof(val->rawAbs));
	memcpy(&val->temperature[0], &measRawDataResult.temperature[0],sizeof(val->temperature));

	//remove the rawflag
	if(lock)	OS_Unuse(lock);


	return ret;

}


uint16_t GetNewRawMeasDataResult(MeasDataRaw* val)
{
	uint16_t ret = FATAL_ERROR;

	if(lock)	OS_Use(lock);
	if(newMeasData)
	{
		newMeasData = 0;
		ret = OK;
	}
	memcpy(&val->rawAbs[0], &measRawDataResult.rawAbs[0],sizeof(val->rawAbs));
	memcpy(&val->temperature[0], &measRawDataResult.temperature[0],sizeof(val->temperature));
	val->rawFlag = measRawDataResult.rawFlag;

	if(lock)	OS_Unuse(lock);


	return ret;

}



static char* PrintTemperature(void)
{
	static char TemperatureStr[4][90];
	static uint8_t id = 0;
	uint8_t i = id++;
	snprintf((void*)&TemperatureStr[i%4][0],90,"T:%04d,%04d,%04d\tV:%04d,%04d,%04d;%d,%d;\t TH:%04d,%04d; \t AD:%04d,%04d\t Step:%d",\
			GetTempChn(0),GetTempChn(1),GetTempChn(2),\
			GetVoltageIdx(3),GetVoltageIdx(4),GetVoltageIdx(5),GetVoltageIdx(6),GetVoltageIdx(7),\
			hadc1080_Val[0],hadc1080_Val[1],\
			AdcVolts[0],AdcVolts[1], (int)flowStepRun[2].step);

	return &TemperatureStr[i%4][0];
}



void SubStep_CalcAndStoreRawMeasAbs(void)
{
	const LogData* dataInst = GetLogDataInst(HISTORY_RAW_MEASDATA_LOG);
	if(lock) OS_Use(lock);
	if(_LocAvgIdx)
	{
		newMeasData = (uint16_t)(newMeasData+_LocAvgIdx);
		absAvgValue[0] = _LocSumData[0]/_LocAvgIdx;
		absAvgValue[1] = _LocSumData[1]/_LocAvgIdx;
		absAvgValue[2] = _LocSumData[2]/_LocAvgIdx;
		absAvgValue[3] = _LocSumData[3]/_LocAvgIdx;
	}

	//todo: remove the blank data
	if(blankEnable)
	{
		absAvgValue[0] -= blankRawAbs[BLANK_LONG];
		absAvgValue[1] -= 0;
		absAvgValue[2] -= blankRawAbs[BLANK_SHORT];
		absAvgValue[3] -= 0;

	}
	else
	{
		TraceUser("raw measure: blank is not used for calculation,\n");
	}
	if(measRawValueTimes != _LocAvgIdx)
	{
		TraceDBG(TSK_ID_MEASURE, "Measure times is not same as required %d != %d!\n",measRawValueTimes,_LocAvgIdx );
	}


	if(lock) OS_Unuse(lock);

//save one measure result;
//	extern MeasDataRaw	 		measRawDataResult;
	measRawDataResult.rawAbs[0] = absAvgValue[0];
	measRawDataResult.rawAbs[1] = absAvgValue[1];
	measRawDataResult.rawAbs[2] = absAvgValue[2];
	measRawDataResult.rawAbs[3] = absAvgValue[3];
	measRawDataResult.temperature[0] = (int16_t)GetTempChn(0);
	measRawDataResult.temperature[1] = (int16_t)GetTempChn(1);
	measRawDataResult.temperature[2] = (int16_t)GetTempChn(2);
	measRawDataResult.temperature[3] = (int16_t)GetTempChn(3);
	measRawDataResult.startTimeST = GetCurrentST();
	if( (measDataSaturation & LONG_SATURATION) != 0)
	{
		measDataFlag |= FLAG_LONG_SAT;
	}
	if( (measDataSaturation & SHORT_SATURATION) != 0)
	{
		measDataFlag |= FLAG_SHORT_SAT;
	}

	if(blankEnable)
		measDataFlag |= FLAG_BLANK_ENABLE;
	measRawDataResult.rawFlag = measDataFlag;
	UpdateResult2UI_Pre(UPDATE_MEASURE_RAW_ID);
	if(dataInst)
	{
		dataInst->Lock();
		MeasDataRawLog* logInst = dataInst->wrData.ptrMeasDataRaw;
		memcpy((void*)&logInst->measDataRaw, (void*)&measRawDataResult, sizeof(measRawDataResult));
		dataInst->UnLock();
		dataInst->UpdateData(dataInst);
		Trigger_Save2FF(NULL);
	}





//print result from shell
	uint16_t caliFlag = (uint16_t)( measDataFlag&FLAG_CALI_MSK );
	uint16_t rangeIdx =  (uint16_t)( measDataFlag&FLAG_RANGE_MSK );
	if( caliFlag == 0 )
	{
		if((measDataFlag & FLAG_MEA_TRIG) != 0)
		{
			TraceUser("Trig measure result:\n\n");
		}
		else if((measDataFlag & FLAG_MEA_ONLINE) != 0)
		{
			TraceUser("Online measure result:\n\n");
		}
		else if((measDataFlag & FLAG_MEA_STD0) != 0)
		{
			TraceUser("Verify Std0 measure result:\n\n");
		}
		else if((measDataFlag & FLAG_MEA_STD1) != 0)
		{
			TraceUser("Verify Std1 measure result:\n\n");
		}
		else if ( (measDataFlag & FLAG_POST_STD1) != 0)
		{
			TraceUser("Post Std1 measure result:\n\n");
		}
		else
		{}

//		if(blankMeasure == 0)
		{
			TraceUser("Used Blank value: \t %.06f,\t %.06f,\t\n",\
					blankRawAbs[BLANK_LONG], blankRawAbs[BLANK_SHORT]);

			TraceUser("Meas avg abs: \t %.06f,\t %.06f,\t %.06f,\t %.06f, %s\n",\
					absAvgValue[0],absAvgValue[1],absAvgValue[2],absAvgValue[3],PrintTemperature());

			TraceUser("Meas Abs Cal: \t %.06f,\t %.06f,\t %.06f,\t %.06f, %s\n",\
					absAvgValue[0],absAvgValue[0]-absAvgValue[1],absAvgValue[2],absAvgValue[2]-absAvgValue[3], PrintTemperature());
		}
		if(rangeIdx < MEAS_RANGE_MAX)
		{
			float val = CalcConcentration( &measRawDataResult, rangeIdx, NOT_CHECK_LONG_SHORT);

			{
				TraceUser("SubStep_CalcAndStoreRawMeasAbs: \t %.06f,\t %.06f,\t %.06f,\t %.06f,\t,%s, Flag: 0x%x\n",\
						val, absMeasConcentration[0],absMeasConcentration[1],absMeasConcentration[2],PrintTemperature(),measDataFlag);
			}
		}
		else
		{
			TraceDBG(TSK_ID_MEASURE,"CalcAndStoreMeasAbs with range index: %x, %x\n",\
					caliFlag, rangeIdx);
		}
	}
	else
	{
		TraceUser("Cali avg abs: \t %.06f,\t %.06f,\t %.06f,\t %.06f, %s\n",\
				absAvgValue[0],absAvgValue[1],absAvgValue[2],absAvgValue[3],PrintTemperature());
	}

}





static void CalcMeasAdc(MEAS_DATA _measData, int32_t* _mea_mean, int32_t* _ref_mean, int len)
{
	uint16_t avgNum = 8;
	assert(len > 0);

	for(int i =0; i< len; i++)
	{
		_mea_mean[i] = 0;
		_ref_mean[i] = 0;
	}
	// Average ad data and put them in mean queue
	for (int32_t k=0; k<_measData.len; k+=len)
	{
		for(int i =0; i< len; i++)
		{
			_mea_mean[i] += _measData.meas[k+i] ;
			_ref_mean[i] += _measData.ref[k+i] ;
		}
	}

	avgNum = (uint16_t)(_measData.len/len);

	for(int i =0; i< len; i++)
	{
		_mea_mean[i] /= avgNum;;
		_ref_mean[i] /= avgNum;;
	}
}

static void CalcNewGain(int32_t* _mea_mean, int32_t* _ref_mean)
{
	static uint16_t calcGain = 0;
	if(calAct.reCalcGain)
	{
		UpdateAbsInfo(absChnValue);

		calAct.reCalcGain = 0;
		calcGain = 0x63;
		InitGainReCalc();
	}
	if(calcGain)
	{
		calcGain--;

		if(calcGain%3  == 0)
		{
			uint16_t ret = OK;
			ret = UpdateGain(_mea_mean, _ref_mean);
			if(ret == OK)
			{
				NewEventLog(EV_CALC_GAIN, NULL);
				calcGain = 0;
				TraceUser("ad gain adjust finished!\n");
			}
		}
	}
}



uint32_t testRefData[MEA_STEP_DONE];
uint32_t testMeasData[MEA_STEP_DONE];
uint8_t	testADCNum = 0;
#define MAX_BLANK_SMAPLE 32
#define MAX_BLANK_SMAPLE_MSK 0x1F
static float GetBlankRawTemp(const float* data, float* ptrSum, uint32_t index, uint32_t idLast, uint16_t sumCalc)
{

	if(sumCalc == 1)
	{
		*ptrSum = 0.0f;
		for(uint16_t id = 0; id <=index;id++)
		{
			*ptrSum += data[id];
		}
	}
	else if(sumCalc == 2)
	{
		*ptrSum += (data[index] - data[idLast]);
	}
	else
	{

	}
	if(*ptrSum < -20.0f)
	{
		assert(*ptrSum < -20.0f);
	}
	return *ptrSum;
}


void Calc_BlankRawABS(void)
{
	static float blankBuffer[BLANK_MAX][MAX_BLANK_SMAPLE];
	static float blankRawData[BLANK_MAX] = {100000.0f,100000.0f};
	static uint32_t index = 0;

	static float blankRawData_Temp[2] = {100000.0f,100000.0f};;
	uint16_t refresh = 0;

	if( (blankMeasure == BLANK_ENABLE1) || (blankMeasure == BLANK_ENABLE2) )
	{
		if((blankMeasure == BLANK_ENABLE1) && (blankSettingUsed.delayTimeStart > 0))
		{
			blankSettingUsed.delayTimeStart--;
			index = 0;
		}
		else if((blankMeasure == BLANK_ENABLE2) && (blankSettingUsed.delayTimeContinue > 0))
		{
			blankSettingUsed.delayTimeContinue--;
		}
		else
		{
			uint16_t sumCalc = 0;
			uint32_t index1 = index & MAX_BLANK_SMAPLE_MSK;
			uint32_t index2 = (index-blankSettingUsed.duringTime) & MAX_BLANK_SMAPLE_MSK;
			blankBuffer[BLANK_LONG][index1] = absChnValue[MEA_STEP_660nm_LONG] - absChnValue[MEA_STEP_880nm_LONG];
			blankBuffer[BLANK_SHORT][index1] = absChnValue[MEA_STEP_660nm_SHORT] - absChnValue[MEA_STEP_880nm_SHORT];
			index++;
			if(index ==  blankSettingUsed.duringTime)
			{
				sumCalc = 1;
			}
			else if(index > blankSettingUsed.duringTime)
			{
				sumCalc = 2;
			}
			else
			{}
			for(uint16_t id = 0; id <BLANK_MAX; id++)
			{
				GetBlankRawTemp(&blankBuffer[id][0], &blankRawData_Temp[id], index1, index2, sumCalc);
				if(blankRawData[id] > blankRawData_Temp[id])
				{
					blankRawData[id] = blankRawData_Temp[id];
					refresh = 1;
				}
			}

			if(refresh)
			{
				TracePrint(TSK_ID_MEASURE, "blank sum raw abs value: \t %.06f,\t %.06f\n",\
						blankRawData[BLANK_LONG], blankRawData[BLANK_SHORT]);
			}

		}
	}
	else if( (blankMeasure == BLANK_END) || (blankMeasure = BLANK_NONE))
	{
		if(index != 0)
		{
			if(index < blankSettingUsed.duringTime)
			{
				TraceDBG(TSK_ID_MEASURE, "blank time is too short: \t %d,\t %d, %d, %d\n",\
						index, blankSettingUsed.duringTime, blankSettingUsed.delayTimeStart, blankSettingUsed.delayTimeContinue);
			}
			for(uint16_t id = 0; id < BLANK_MAX; id++)
			{
				blankRawAbs[id] = blankRawData[id]/blankSettingUsed.duringTime;
				blankRawData[id] = 1000000.0f;
				blankRawData_Temp[id] = 1000000.0f;
			}
			CheckBlank_Optics(blankRawAbs);
			TraceUser("blank raw abs value: \t %.06f,\t %.06f, %s\n",\
					blankRawAbs[BLANK_LONG], blankRawAbs[BLANK_SHORT], PrintTemperature());
			index = 0;
			if(0==blankEnable)
			{
				TraceUser("blank update: blank is not used for calculation,\n");
			}
		}
		blankMeasure = BLANK_DIABLE;

	}
	else
	{}
}
extern uint16_t ledTest;

static float CalcTempConcentration(float* rawAbs1, uint16_t rangeIdx)
{
	float val = 0.0f;
	MeasDataRaw measResult;
	//blank is considered;
	if(blankEnable)
	{
		measResult.rawAbs[0] = rawAbs1[0] - blankRawAbs[BLANK_LONG];
		measResult.rawAbs[1] = rawAbs1[1] - 0;
		measResult.rawAbs[2] = rawAbs1[2] - blankRawAbs[BLANK_SHORT];
		measResult.rawAbs[3] = rawAbs1[3] - 0;
		measResult.rawFlag = rangeIdx;
	}

	assert(MEAS_RANGE_MAX > rangeIdx);

	val = CalcConcentration(&measResult, rangeIdx, TEMP_CALC_CONCENTRATION);

	return val;
}


void StartMeasCalcTask(void const* argument)
{
	(void)argument; // pc lint

	double valAbsMea = 0;
	double valAbsRef = 0;
	uint16_t _measDataSaturation	= NONE_SATURATION;
	osEvent event;
	const uint8_t taskID = TSK_ID_MEASURE;
	TracePrint(taskID,"started  \n");
	lock = OS_CreateSemaphore();
	int32_t  mea_gain_calc[MEA_STEP_DONE];
	int32_t  ref_gain_calc[MEA_STEP_DONE];


	while(TASK_LOOP_ST)
	{
		//this function needs higher priority
	//	DQ_UpdateTemp();
		freeRtosTskTick[taskID]++;
		event = osMessageGet(MEAS_CTL_ID, osWaitForever );
		if( event.status == osEventMessage )
		{
			MEAS_DATA _measData = *(MEAS_DATA*)(event.value.p);

			//calc average measure ADC
			CalcMeasAdc(_measData,&mea_mean[0][0],&ref_mean[0][0],16);
			for(uint16_t id =0; id<MEA_STEP_DONE; id++)
			{
				mea_gain_calc[id] = mea_mean[id][3];
				ref_gain_calc[id] = ref_mean[id][3];
			}
			//calc new gain
			CalcNewGain(&mea_gain_calc[0],&ref_gain_calc[0]);

			//start led test; remove first point


			//calc four channel raw abs
			for(uint8_t idx= MEA_STEP_START; idx <MEA_STEP_DONE; idx++)
			{
				valAbsRef = ref_calc_mean[idx] = (ref_mean[idx][3] - ref_mean[idx][2] - ref_mean[idx][1] + ref_mean[idx][0]);
				valAbsMea = mea_calc_mean[idx] = (mea_mean[idx][3] - mea_mean[idx][2] - mea_mean[idx][1] + mea_mean[idx][0]);
				if( valAbsMea > ABS_MIN_VAL)
				{
					valAbsRef = valAbsRef/valAbsMea;
					if(valAbsRef>0)
						absChnValue[idx] = (float)log10(valAbsRef);
					else
						absChnValue[idx] = 55.0;
				}
				else
					absChnValue[idx] = 55.0;
			}
			if(ledTest == 1)
			{
				testADCNum = 0;
				memset(testRefData,0,sizeof(testRefData));
				memset(testMeasData,0,sizeof(testMeasData));
				ledTest = 2;
			}
			else if(ledTest == 2)
			{
				for(uint8_t idx= MEA_STEP_START; idx <MEA_STEP_DONE; idx++)
				{
					testRefData[idx] += ref_mean[idx][3];//(uint32_t)ref_calc_mean[idx];
					testMeasData[idx] += mea_mean[idx][3];//(uint32_t)mea_calc_mean[idx];
				}
				testADCNum++;
			}
			Calc_BlankRawABS();
/*
			static uint32_t testRefData[MEA_STEP_DONE];
			static uint8_t	testRefNum = 0;
			*/
			//print raw abs and temperature for last 10 second of measure
			OS_Use(lock);
			if(measResultPrint&MEASURE_DATA_VALID_MSK)
			{
				//measure raw data;
				//long saturation
				if(absChnValue[0] >= 50 ||absChnValue[1] >= 50 )
					_measDataSaturation |= LONG_SATURATION;
				//short saturation
				if(absChnValue[2] >= 50 ||absChnValue[3] >= 50 )
					_measDataSaturation |= SHORT_SATURATION;

				_LocSumData[0] += absChnValue[MAP_IDX_ABS_0];
				_LocSumData[1] += absChnValue[MAP_IDX_ABS_1];
				_LocSumData[2] += absChnValue[MAP_IDX_ABS_2];
				_LocSumData[3] += absChnValue[MAP_IDX_ABS_3];
				_LocAvgIdx++;
				measDataSaturation = _measDataSaturation;
				TraceUser("raw abs value: \t %.06f,\t %.06f,\t %.06f,\t %.06f, %s; ->flag:%x\n",\
						absChnValue[MAP_IDX_ABS_0], absChnValue[MAP_IDX_ABS_1], absChnValue[MAP_IDX_ABS_2], absChnValue[MAP_IDX_ABS_3],PrintTemperature(), _measDataSaturation);
			}
			else
			{
				_measDataSaturation = NONE_SATURATION;
				_LocSumData[0] = 0.0f;
				_LocSumData[1] = 0.0f;
				_LocSumData[2] = 0.0f;
				_LocSumData[3] = 0.0f;
				_LocAvgIdx = 0;
			}
			OS_Unuse(lock);

			realTimeConcentration = CalcTempConcentration(absChnValue, currentMeasCaliRangIdx);
			//use xprintf to print raw AD value
			if(printAllRawADC)
			{
				uint8_t idx = MAP_IDX_ABS_0;
				xprintfRTC("660L_Mea: %06d,\t %06d,\t %06d,\t %06d,\t", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				xprintf("660L_Ref: %06d,\t %06d,\t %06d,\t %06d,\t", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_1;
				xprintf("880L_Mea: %06d,\t %06d,\t %06d,\t %06d,\t", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				xprintf("880L_Ref: %06d,\t %06d,\t %06d,\t %06d,\t", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_2;
				xprintf("660S_Mea: %06d,\t %06d,\t %06d,\t %06d,\t", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				xprintf("660S_Ref: %06d,\t %06d,\t %06d,\t %06d,\t", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_3;
				xprintf("880S_Mea: %06d,\t %06d,\t %06d,\t %06d,\t", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				xprintf("880S_Ref: %06d,\t %06d,\t %06d,\t %06d,\t\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);
			}
			else
			{
				xprintfRTC("Load:%03d \tC%.06f;\t raw abs value: %.06f,\t %.06f,\t %.06f,\t %.06f,%s\n",\
						osCPU_Usage,realTimeConcentration , \
						absChnValue[0], absChnValue[1], absChnValue[2], absChnValue[3], PrintTemperature());
			}
			//use shell to print raw AD value only once
			if(measResultPrint&MEASURE_DATA_INVALID_MSK)
			{
				uint8_t idx = MAP_IDX_ABS_0;
				measResultPrint &= MEASURE_DATA_VALID_MSK;
				TraceUser("660L_Mea: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				TraceUser("660L_Ref: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_1;
				TraceUser("880L_Mea: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				TraceUser("880L_Ref: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_2;
				TraceUser("660S_Mea: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				TraceUser("660S_Ref: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_3;
				TraceUser("880S_Mea: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				TraceUser("880S_Ref: \t %06d,\t %06d,\t %06d,\t %06d,\t\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);
			}
		}
	}

}

