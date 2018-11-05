/*
 * tsk_meas_calc.c
 *
 *  Created on, 2017骞�2鏈�10鏃�
 *      Author, pli
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

extern float    realTimeConcentration;

#define MAP_IDX_ABS_0				0
#define MAP_IDX_ABS_1				1


#define ABS_MIN_VAL		4000
volatile uint16_t measDataFlag = FLAG_MEA_MEAS;


static SemaphoreHandle_t lock = NULL;

volatile uint16_t blankMeasure = 0;
//sum of 10second measure abs of each chn
static float _LocSumData[MEA_DATA_MAX] = {0,0,};
//measure abs of each chn
float absChnValue[MEA_DATA_MAX] = {0,0,};
//average abs of each chn
static float absAvgValue[MEA_DATA_MAX] = {0,0};

int32_t mea_mean[MEA_DATA_MAX][4];
int32_t ref_mean[MEA_DATA_MAX][4];



int32_t mea_calc_mean[MEA_DATA_MAX];
int32_t ref_calc_mean[MEA_DATA_MAX];
int32_t mea_calc_dark[MEA_DATA_MAX];
int32_t ref_calc_dark[MEA_DATA_MAX];


static volatile uint16_t newMeasData = 0;



static volatile uint16_t _LocAvgIdx = 0;

extern uint16_t measRawValueTimes;
extern uint16_t printAllRawADC;
extern volatile uint32_t osCPU_Usage;

uint16_t GetNewRawCaliDataResult(CaliDataRaw* val)
{
	uint16_t ret = FATAL_ERROR;

	if(lock)	OS_Use(lock);
	if(newMeasData)
	{
		newMeasData = 0;
		ret = OK;
	}
//	memcpy(&val->rawAbs[0], &measRawDataResult.rawAbs[0],sizeof(val->rawAbs));
//	memcpy(&val->temperature[0], &measRawDataResult.temperature[0],sizeof(val->temperature));

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
//	memcpy(&val->rawAbs[0], &measRawDataResult.rawAbs[0],sizeof(val->rawAbs));
//	memcpy(&val->temperature[0], &measRawDataResult.temperature[0],sizeof(val->temperature));
//	val->rawFlag = measRawDataResult.rawFlag;

	if(lock)	OS_Unuse(lock);


	return ret;

}



static char* PrintTemperature(void)
{
	static char TemperatureStr[4][30];
	static uint8_t id = 0;
	uint8_t i = (id++)%4;
	snprintf((void*)&TemperatureStr[i][0],30,"T, %4d, %4d\t",\
			GetTempChn(1),GetTempChn(2));

	return &TemperatureStr[i][0];
}



void SubStep_CalcAndStoreRawMeasAbs(void)
{
	const LogData* dataInst = GetLogDataInst(HISTORY_RAW_MEASDATA_LOG);


}


#define MEASURE_MAINSTEP		233
#define CALI_MAINSTEP			230
#define CALI_PRESTEP_STD0		210
#define CALI_PRESTEP_STD1		211

static uint16_t tIdx_Blank = 0;
static uint16_t tIdx_Meas = 0;

void InitCaliIndexValue(void)
{
	tIdx_Blank = 0;
	tIdx_Meas = 0;
}

static void UpdateMeasAbs(float val, float valN, uint16_t type)
{

//	if(gFlowStep.mainStep == CALI_MAINSTEP)
//	{
		if(gFlowStep.mainStep == CALI_MAINSTEP)
		{
            if(type == 0)
            {
                if(tIdx_Meas <= 1)
                {
                	caliTime = GetCurrentST();
                	Trigger_EEPSave(&caliTime, sizeof(caliTime), SYNC_CYCLE);
                    caliFactor[0].std0 = val;
                    caliFactor[1].std0 = valN;
                    TraceUser("calibration blank abs, %.6f, Avg, %.6f, ", val, valN);
                    shell_Add("temp, %d, %d, %d, %d, %d, %d\n", tempIdle[0],tempIdle[1],tempIdle[2],tempMeas[0],tempMeas[1],tempMeas[2]);
                    tIdx_Meas++;
                }
                else
                {
                    tIdx_Meas = 0;
                    caliFactor[0].std1 = val;
                    caliFactor[1].std1 = valN;
                    TraceUser("calibration slope abs, %.6f, Avg, %.6f, ", val, valN);
                    shell_Add("temp: %d, %d, %d, %d, %d, %d\n", tempIdle[0],tempIdle[1],tempIdle[2],tempMeas[0],tempMeas[1],tempMeas[2]);
                    Trigger_EEPSave((void*)&caliFactor,sizeof(caliFactor),SYNC_CYCLE);
                }
            }
            else
            {
                if(tIdx_Blank <= 1)
                {
                    caliFactor[2].std0 = val;
                    caliFactor[3].std0 = valN;
                    TraceUser("calibration blank abs, %.6f, Avg, %.6f, ", val, valN);
                    shell_Add("temp, %d, %d, %d, %d, %d, %d\n", tempIdle[0],tempIdle[1],tempIdle[2],tempMeas[0],tempMeas[1],tempMeas[2]);
                    tIdx_Blank++;
                }
                else
                {
                    tIdx_Blank = 0;
                    caliFactor[2].std1 = val;
                    caliFactor[3].std1 = valN;
                    TraceUser("calibration slope abs, %.6f, Avg, %.6f, ", val, valN);
                    shell_Add("temp, %d, %d, %d, %d, %d, %d\n", tempIdle[0],tempIdle[1],tempIdle[2],tempMeas[0],tempMeas[1],tempMeas[2]);
                    Trigger_EEPSave((void*)&caliFactor,sizeof(caliFactor),SYNC_CYCLE);
                }
            }

		}
        else
        {
            if(type == 0)
            {
                tIdx_Meas = 0;
            }
            else
            {
                tIdx_Blank = 0;
            }
        }

    {
		float avgVal = 0;
        if(type == 0)
        {
            avgVal = CalcConcentraction(valN, &caliFactor[1]);
            realTimeConcentration = CalcConcentraction(val, &caliFactor[0]);
        }
        else
        {
            avgVal = CalcConcentraction(valN, &caliFactor[3]);
             realTimeConcentration = CalcConcentraction(val, &caliFactor[2]);
        }
        TraceUser("%d, type,%d, Measure Value, %.6f, abs, %.6f, Avg, %.6f, abs, %.6f, ", gFlowStep.mainStep, type, \
        		realTimeConcentration, val, avgVal, valN);

        shell_Add("temp, %d, %d, %d, %d, %d, %d\n", tempIdle[0],tempIdle[1],tempIdle[2],tempMeas[0],tempMeas[1],tempMeas[2]);
	}
//	}
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
//		InitGainReCalc();
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

}
extern uint16_t ledTest;


volatile uint16_t adcAction = AD_NONE_ACT;

#define MAX_ABS_CALC 70

static float measAbsAll[MAX_ABS_CALC];


static float AvgHandleAbs(float* val, uint16_t cnt,float midVal)
{
    //remove the limit only
#define ERR_ABS_DEVIATION 3.5f

    float abs = ERR_ABS_DEVIATION/1500.0f;
    float maxVal = midVal + abs;
    float minVal = midVal - abs;
    if(cnt > MAX_ABS_CALC)
		cnt = MAX_ABS_CALC-1;
    float valCnt = 0.0f;
    float total = 0.0f;
    for( int i=0; i<=cnt;i++)
	{
		if( (val[i] <= minVal) || (val[i] >= maxVal))
		{
		}
        else
        {
            total += val[i];
            valCnt += 1.0f;
        }
	}

    if(valCnt > 1.0)
        midVal = (total/valCnt);
    TraceUser("avg, %.6f, total,%.6f, cnt,%.0f, limit, %.4f, %.4f,cnt, %d, %s\n",midVal,total, valCnt, maxVal, minVal, cnt,\
    		PrintTemperature());
    return midVal;
}


static void AvgMeasVal(void)
{
	static float measAbs = 0.0f;
	static int32_t tempMeasLoc[2] = {0, 0};
	static uint16_t absCnt =0;
	float measVal = CalcConcentraction(realTimeAbs, &caliFactor[3]);
	tmpConcentration = measVal;
	switch(adcAction)
	{
	case AD_MEASURE_ACT:
	case AD_BLANK_ACT:
		measAbsAll[absCnt%MAX_ABS_CALC] = realTimeAbs;
		measAbs += realTimeAbs;
		tempMeasLoc[0] += GetTempChn(1);
		tempMeasLoc[1] += GetTempChn(2);
		absCnt++;
		TraceUser("temp ABS, %.6f, meas,%.6f, cnt,%d, %s\n",realTimeAbs, measVal, absCnt,\
				PrintTemperature());
		break;
	case AD_MEASURE_ACT_FINISH:
		{
			float tmpAbs = 0;
			float avgAbs = 0;
			measAbs += realTimeAbs;
			measAbsAll[absCnt%MAX_ABS_CALC] = realTimeAbs;
			tempMeasLoc[0] += GetTempChn(1);
			tempMeasLoc[1] += GetTempChn(2);
			absCnt++;
			tmpAbs = (measAbs/absCnt);
			tempMeas[1] = (uint16_t)(tempMeasLoc[0]/absCnt);
			tempMeas[2] = (uint16_t)(tempMeasLoc[1]/absCnt);

			avgAbs = AvgHandleAbs(measAbsAll, absCnt, tmpAbs);
			TraceUser("temp ABS, %.6f, meas,%.6f, cnt,%d, %s\n", realTimeAbs, measVal, absCnt,\
					PrintTemperature());
			UpdateMeasAbs(tmpAbs, avgAbs, 0);
			TraceUser("Final Measure ABS, avg,%.6f, new avg, %.6f\n",tmpAbs,avgAbs);
			absCnt = 0;
			measAbs = 0;
			tempMeasLoc[0] = 0;
			tempMeasLoc[1] = 0;
			adcAction = AD_NONE_ACT;
		}
		break;
	case AD_BLANK_ACT_FINISH:
		{
			float tmpAbs = 0;
			float avgAbs = 0;
			measAbs += realTimeAbs;
			measAbsAll[absCnt%MAX_ABS_CALC] = realTimeAbs;
			tempMeasLoc[0] += GetTempChn(1);
			tempMeasLoc[1] += GetTempChn(2);
			absCnt++;
			tmpAbs = (measAbs/absCnt);
			avgAbs = AvgHandleAbs(measAbsAll, absCnt, tmpAbs);
			tempMeas[1] = (uint16_t)(tempMeasLoc[0]/absCnt);
			tempMeas[2] = (uint16_t)(tempMeasLoc[1]/absCnt);

			TraceUser("temp ABS, %.6f, meas,%.6f, cnt,%d, %s\n", realTimeAbs, measVal, absCnt,\
					PrintTemperature());
			UpdateMeasAbs(tmpAbs, avgAbs, 1);
			TraceUser("Final Measure ABS, avg,%.6f, new avg, %.6f\n",tmpAbs,avgAbs);
			absCnt = 0;
			measAbs = 0;
			tempMeasLoc[0] = 0;
			tempMeasLoc[1] = 0;
			adcAction = AD_NONE_ACT;

		}
		break;
	default:
		absCnt = 0;
		break;
        }
}
static char* PrintStepInfo(void)
{
	static char stepStr[4][90];
	static uint8_t id = 0;
	uint8_t i = id++;
	snprintf((void*)&stepStr[i%4][0],90,"T,%5d,%5d,%5d,step,%3d,%3d,%3d,%3d,%3d,",\
			GetTempChn(1),GetTempChn(2),\
			ctrlTemp,
			gFlowStep.mainStep,\
			gFlowStep.preMain,\
			gFlowStep.subStep,\
			gFlowStep.preSub,\
			gFlowStep.microStep);

	return &stepStr[i%4][0];
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
			CalcMeasAdc(_measData,&mea_mean[0][0],&ref_mean[0][0],8);
			for(uint16_t id =0; id<MEA_STEP_DONE; id++)
			{
				mea_gain_calc[id] = mea_mean[id][3];
				ref_gain_calc[id] = ref_mean[id][3];
			}
			//calc new gain
			CalcNewGain(&mea_gain_calc[0],&ref_gain_calc[0]);

			//start led test; remove first point


			//calc four channel raw abs
		//	for(uint8_t idx= MEA_STEP_START; idx <=MEA_STEP_START; idx++)
			{
				uint8_t idx= MEA_STEP_START;
				mea_calc_dark[idx] = mea_mean[idx][1] - mea_mean[idx][0];
				ref_calc_dark[idx] = ref_mean[idx][1] - ref_mean[idx][0];

				valAbsRef = ref_calc_mean[idx] = (ref_mean[idx][3] - ref_mean[idx][2] - ref_mean[idx][1] + ref_mean[idx][0]);
				valAbsMea = mea_calc_mean[idx] = (mea_mean[idx][3] - mea_mean[idx][2] - mea_mean[idx][1] + mea_mean[idx][0]);
				if( valAbsMea > ABS_MIN_VAL)
				{
					float val = valAbsRef/valAbsMea;
					if(val>0)
						absChnValue[idx] = (float)log10(val);
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

				_LocSumData[0] += absChnValue[MAP_IDX_ABS_0];
				_LocSumData[1] += absChnValue[MAP_IDX_ABS_1];
				_LocAvgIdx++;
				measDataSaturation = _measDataSaturation;
				TraceUser("raw abs value, \t %.06f, %s, flag,%x\n",\
						absChnValue[MAP_IDX_ABS_0], PrintTemperature(), _measDataSaturation);
			}
			else
			{
				_measDataSaturation = NONE_SATURATION;
				_LocSumData[0] = 0.0f;
				_LocSumData[1] = 0.0f;
				_LocAvgIdx = 0;
			}
			OS_Unuse(lock);

			realTimeAbs = absChnValue[0];
			AvgMeasVal();

			//use xprintf to print raw AD value
			if(printAllRawADC)
			{
				uint8_t idx = MAP_IDX_ABS_0;
				xprintfRTC("660L_Mea,%6d,%6d,%6d,%8d,", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				xprintf("660L_Ref,%6d,%6d,%6d,%8d,", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_1;
				xprintf("660S_Mea, %6d,%6d,%6d,%8d,", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				xprintf("660S_Ref, %6d,%6d,%6d,%8d,", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				xprintf("%s,\n",PrintStepInfo());
			}
			else
			{
				xprintfRTC("Load,%2d, C, %.06f, abs, %.06f, %.06f, ref, %8d, %8d, meas, %8d, %8d, %s,\n",\
						osCPU_Usage,realTimeConcentration ,tmpConcentration, \
						absChnValue[0], ref_calc_mean[0],ref_calc_dark[0],mea_calc_mean[0],mea_calc_dark[0],PrintStepInfo());
			}
			//use shell to print raw AD value only once
			if(measResultPrint&MEASURE_DATA_INVALID_MSK)
			{
				uint8_t idx = MAP_IDX_ABS_0;
				measResultPrint &= MEASURE_DATA_VALID_MSK;
				TraceUser("660L_Mea, %6d,%6d,%6d,%8d,\n", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				TraceUser("660L_Ref, %6d,%6d,%6d,%8d,\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);

				idx = MAP_IDX_ABS_1;
				TraceUser("660S_Mea, %6d,%6d,%6d,%8d,\n", mea_mean[idx][0], mea_mean[idx][1], mea_mean[idx][2], mea_mean[idx][3]);
				TraceUser("660S_Ref, %6d,%6d,%6d,%8d,\n", ref_mean[idx][0], ref_mean[idx][1], ref_mean[idx][2], ref_mean[idx][3]);
			}
		}
	}

}

