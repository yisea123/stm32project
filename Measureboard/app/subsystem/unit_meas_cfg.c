/*
 * unit_meas_cfg.c
 *
 *  Created on: 2016锟斤拷8锟斤拷19锟斤拷
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_meas_cfg.h"
#include "unit_sys_diagnosis.h"
#include "tsk_sch.h"
#include "ad717x.h"
#include "dev_ad7172.h"
#include "dev_eep.h"
#include "tsk_measure.h"
#include <math.h>
#include <stdlib.h>
#include "dev_log_sp.h"
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

__IO uint32_t measResultPrint = MEASURE_DATA_NONE;
uint16_t	measRawValueTimes = 0;
CalibrationAct calAct;


#define RAW_AD_MAX		0xE66666 // 0.9 max
#define RAW_AD_M		0x733333
#define RAW_AD_MIN		0x199999 // 0.1 max

#if 0

typedef struct
{
	float gainFactor;
	uint8_t gainIO;
}ADGain;
#define ADIO_GAIN_NUM		8
static const ADGain meaGain[ADIO_GAIN_NUM] =
{
		{1.0f,	5},
		{2.0f,	7},
		{4.0f,	4},
		{8.0f,	6},
		{16.0f,	1},
		{32.0f,	3},
		{64.0f,	0},
		{128.0f,2},
};

static const ADGain refGain[ADIO_GAIN_NUM] =
{
		{1.0f,	5},
		{2.0f,	7},
		{4.0f,	4},
		{8.0f,	6},
		{16.0f,	1},
		{32.0f,	3},
		{64.0f,	0},
		{128.0f,2},
};


static uint16_t UpdateGain(const ADGain* ag, const int32_t rawAd, uint8_t* gainIO)
{
	uint16_t 	ret = OK;
	int32_t 	id = 0;
	int32_t 	idx1;
	float 		gainFactor = 1.0f;

	for(int16_t idx = 0; idx <ADIO_GAIN_NUM;idx++)
	{
		if(*gainIO == ag[idx].gainIO )
		{
			id = idx;
			gainFactor = ag[idx].gainFactor;
			break;
		}
	}
	if(rawAd > RAW_AD_MAX)
	{
		if(id >0)
		{
			*gainIO = ag[id-1].gainIO;
		}
		else
		{
			*gainIO = ag[0].gainIO;
		}
		return WARNING;
	}
	idx1 = (id+1);
	for( ;idx1 <ADIO_GAIN_NUM; idx1++)
	{
		if( (int32_t)(ag[idx1].gainFactor/gainFactor * (float)rawAd) < (int32_t)RAW_AD_MAX)
		{
			*gainIO = ag[idx1].gainIO;
			ret = WARNING;
		}
		else
		{
			break;
		}
	}


	return ret;

}

#else
#define ADIO_GAIN_NUM		4

static const uint8_t _AdGain[ADIO_GAIN_NUM] =
{
	2,3,0,1,
};
#endif


#define FILE_ID			0x18041716


static uint32_t fileID1						__attribute__ ((section (".configbuf_meas")));
static uint8_t gainMaskMea[MEA_STEP_DONE] 	__attribute__ ((section (".configbuf_meas")));
static uint8_t gainMaskRef[MEA_STEP_DONE] 	__attribute__ ((section (".configbuf_meas")));
uint16_t measTimeCfg[e_ad_test_tmax]		__attribute__ ((section (".configbuf_meas")));
AbsInfo	absInfo[MEA_STEP_DONE]				__attribute__ ((section (".configbuf_meas")));
uint32_t motorEnableStatus					__attribute__ ((section (".configbuf_meas")));
uint8_t  testLedRefGain[MEA_STEP_DONE]		__attribute__ ((section (".configbuf_meas")));
uint32_t testLedRefLimit[MEA_STEP_DONE]		__attribute__ ((section (".configbuf_meas")));
float    blankOriginalData[BLANK_MAX]   	__attribute__ ((section (".configbuf_meas")));
float    blankWarningLimit[BLANK_MAX]   	__attribute__ ((section (".configbuf_meas")));
float    blankWarningHyst[BLANK_MAX]   		__attribute__ ((section (".configbuf_meas")));
uint8_t 	ledCurrentCtrolSet[MEA_STEP_DONE] 			__attribute__ ((section (".configbuf_meas")));
static uint32_t fileID2						__attribute__ ((section (".configbuf_meas")));


static uint32_t testLedRef[MEA_STEP_DONE] = {1677721,1677721,1677721,1677721};
static uint32_t testLedMeas[MEA_STEP_DONE] = {1677721,1677721,1677721,1677721};
float absValue[4];
static uint32_t regCfg[ATR_REGCFG_PUT_MAX];
static const  uint32_t fileID_Default = FILE_ID;
uint16_t ledOutMask[MEA_DATA_MAX] = {1,1,1,1};
extern int32_t mea_mean[4][4];
extern int32_t ref_mean[4][4];
extern int32_t mea_calc_mean[MEA_DATA_MAX];
extern int32_t ref_calc_mean[MEA_DATA_MAX];


static uint32_t motorEnableStatus_Default = 1;
static uint8_t ledCurrentCtrolSet_Default[4] = {0,0,0,0};
static const uint8_t  testLedRefGain_Default[MEA_STEP_DONE] = {1,3,1,3};

static const float blankOriginalData_Default[BLANK_MAX] = {1000,1000};
static const float blankWarningLimit_Default[BLANK_MAX] = {0.9f,0.9f};
static const float blankWarningHyst_Default[BLANK_MAX] = {0.1f,0.1f};


static const uint32_t testLedRefLimit_Default[MEA_STEP_DONE] = {1677721,1677721,1677721,1677721};

static const uint16_t measTimeCfg_Default[e_ad_test_tmax] =
{
		2500, 0, 332, 8, 2500, 10285,\
		2500, 0, 332, 8, 2500, 10285,
};

static const uint32_t regCfg_Default[ATR_REGCFG_PUT_MAX] = {
		0x0b, 0x00, 0x0b,0x0,
};


static const AbsInfo absInfo_Default[MEA_STEP_DONE] =
{
		{0.0f,0.0f,0.0f,1.0f},
		{0.0f,0.0f,0.0f,1.0f},
		{0.0f,0.0f,0.0f,1.0f},
		{0.0f,0.0f,0.0f,1.0f},
};

// Gain mask combination for all test steps, 4 bits for each wave length and colorimetric length:
// 660nm mea long | 660nm ref long | 880nm mea long | 880nm ref long | 660nm mea short | 660nm ref short | 880nm mea short | 880nm ref short
static const uint8_t gainMaskMea_Default[MEA_STEP_DONE] = {2,2,2,2};
static const uint8_t gainMaskRef_Default[MEA_STEP_DONE] = {1,3,1,3};



static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_DYNAMIC(regCfg,regCfg_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(gainMaskMea,gainMaskMea_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(gainMaskRef,gainMaskRef_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measTimeCfg,measTimeCfg_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(absInfo,absInfo_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(motorEnableStatus,motorEnableStatus_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(testLedRefLimit,testLedRefLimit_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(testLedRefGain,testLedRefGain_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(blankOriginalData,blankOriginalData_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(blankWarningLimit,blankWarningLimit_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(blankWarningHyst,blankWarningHyst_Default),

	CONSTRUCTOR_DC_STATIC_CONSTDEF(ledCurrentCtrolSet, ledCurrentCtrolSet_Default),

};

//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
		//0
	CONSTRUCT_ARRAY_SIMPLE_U32(
			regCfg,
			AD717X_REG_MAX,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U32(
			regCfg,
			AD717X_REG_MAX,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(
			gainMaskMea,
			MEA_STEP_DONE,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(
			gainMaskRef,
			MEA_STEP_DONE,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			measTimeCfg,
			e_ad_test_tmax,
			NON_VOLATILE),
//5
	CONSTRUCT_SIMPLE_U32(
				&measResultPrint,
				RAM),

	CONSTRUCT_ARRAY_SIMPLE_U16(
					&calAct,
					sizeof(calAct)/sizeof(uint16_t),
					RAM),

	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
			&absInfo[MEA_STEP_660nm_LONG],
			sizeof(AbsInfo)/sizeof(float),
			RAM),

	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
			&absInfo[MEA_STEP_880nm_LONG],
			sizeof(AbsInfo)/sizeof(float),
			RAM),

	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
			&absInfo[MEA_STEP_660nm_SHORT],
			sizeof(AbsInfo)/sizeof(float),
			RAM),
//10
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
			&absInfo[MEA_STEP_880nm_SHORT],
			sizeof(AbsInfo)/sizeof(float),
			RAM),
	CONSTRUCT_SIMPLE_U32(
				&motorEnableStatus,
				NON_VOLATILE),

	CONSTRUCT_ARRAY_SIMPLE_I32(
			&mea_mean[0][0],
			sizeof(mea_mean)/sizeof(int32_t),
			RAM),

	CONSTRUCT_ARRAY_SIMPLE_I32(
			&ref_mean[0][0],
			sizeof(ref_mean)/sizeof(int32_t),
			RAM),


	CONSTRUCT_ARRAY_SIMPLE_I32(
			&mea_calc_mean[0],
			sizeof(mea_calc_mean)/sizeof(int32_t),
			RAM),
//15
	CONSTRUCT_ARRAY_SIMPLE_I32(
			&ref_calc_mean[0],
			sizeof(ref_calc_mean)/sizeof(int32_t),
			RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(
			&testLedRefGain[0],
			sizeof(testLedRefGain),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U32(
			&testLedRefLimit[0],
			sizeof(testLedRefLimit)/sizeof(uint32_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U32(
			&testLedRef[0],
			sizeof(testLedRef)/sizeof(uint32_t),
			READONLY_RAM),

	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
			&blankOriginalData[0],
			sizeof(blankOriginalData)/sizeof(float),
			NON_VOLATILE),
//20
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
				&blankWarningLimit[0],
				sizeof(blankWarningLimit)/sizeof(float),
				NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(
			&ledCurrentCtrolSet[0],
			sizeof(ledCurrentCtrolSet)/sizeof(uint8_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
				&blankWarningHyst[0],
				sizeof(blankWarningHyst)/sizeof(float),
				NON_VOLATILE),

	CONSTRUCT_ARRAY_SIMPLE_U16(
			&ledOutMask[0],
			sizeof(ledOutMask)/sizeof(uint16_t),
			RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(
			&testLedMeas[0],
			sizeof(testLedMeas)/sizeof(uint32_t),
			READONLY_RAM),
};



//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT measCfg =
{
	(uint8_t*)"measCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_MeasCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_MeasCfg,
	Put_MeasCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};




uint8_t gainMaskMeaSet[MEA_STEP_DONE];
uint8_t gainMaskRefSet[MEA_STEP_DONE];
uint16_t ledTest = 0;
uint16_t UpdateGainSetting(uint16_t type)
{
	//the standard ways;
	if(type == 0)
	{
		ledTest = 0;
		memcpy((void*)&gainMaskMeaSet[0], (void*)&gainMaskMea[0], sizeof(gainMaskMea));
		memcpy((void*)&gainMaskRefSet[0], (void*)&gainMaskRef[0], sizeof(gainMaskRef));
	}
	//to test led;
	else
	{
		ledTest = 1;
		memcpy((void*)&gainMaskMeaSet[0], (void*)&gainMaskMea[0], sizeof(gainMaskMea));
		memcpy((void*)&gainMaskRefSet[0], (void*)&testLedRefGain[0], sizeof(testLedRefGain));
	}
	return OK;
}



static uint8_t startLedCalc = 0;
static uint8_t adGainRef_Idx[MEA_STEP_DONE] = {0,0,0,0,};
static uint8_t adGainMeas_Idx[MEA_STEP_DONE] = {0,0,0,0,};

static uint8_t adGainMeas_State[MEA_STEP_DONE] = {0,0,0,0,};
static uint8_t adGainRef_State[MEA_STEP_DONE] = {0,0,0,0,};




uint16_t UpdateGain(const int32_t* rawAdMeas, const int32_t* rawAdRef)
{
	static int32_t adRawMeas[MEA_STEP_DONE][ADIO_GAIN_NUM] ;
	static int32_t adRawRef[MEA_STEP_DONE][ADIO_GAIN_NUM] ;
	static uint8_t gainSel = 0;
	static uint16_t ledCurrentChecked = 0;
	uint16_t 	ret = FATAL_ERROR;
	//if the first start
	if(startLedCalc == 1)
	{
		memset((void*)&adRawMeas[0][0], 0, sizeof(adRawMeas));
		memset((void*)&adRawRef[0][0], 0, sizeof(adRawRef));
		gainSel = 0;
		ledCurrentChecked = 0;
	}
	for(uint16_t id =0; id< MEA_STEP_DONE; id++)
	{
		{
			adRawMeas[id][gainSel] = rawAdMeas[id];
			adRawRef[id][gainSel] = rawAdRef[id];


		}

	}
	gainSel++;
	if(gainSel < ADIO_GAIN_NUM)
	{
		for(uint16_t id =0; id< MEA_STEP_DONE; id++)
		{
			gainMaskMea[id] = _AdGain[gainSel];
			gainMaskRef[id] = _AdGain[gainSel];
		}
	}
	else
	{
		ret = WARNING;

	}

	startLedCalc = 0;

	if(ret == WARNING)
	{
		if(ledCurrentChecked == 0)
		{
			//need to check the ref value, whether to change led current;
			for(uint8_t id =0; id< MEA_STEP_DONE; id++)
			{
				ledCurrentCtrolSet[id] = 1;

				for(uint16_t idx = 0; idx < ADIO_GAIN_NUM; idx++)
				{
					if(adRawRef[id][idx] >  RAW_AD_MIN)
					{
						ledCurrentCtrolSet[id] = 0;
						break;
					}
				}
			}
			for(uint8_t id =0; id< MEA_STEP_DONE; id++)
			{
				if(ledCurrentCtrolSet[id] == 1 )
				{
					memset((void*)&adRawMeas[id][0], 0, sizeof(adRawMeas[id]));
					memset((void*)&adRawRef[id][0], 0, sizeof(adRawRef[id]));
					gainSel = 0;
					ret = FATAL_ERROR;
				}
			}
		}
		ledCurrentChecked = 1;

		if(ret == WARNING)
		{
			for(uint8_t id =0; id< MEA_STEP_DONE; id++)
			{
				int32_t adValRef = 0;
				int32_t adValMeas = 0;
				gainMaskMea[id] = 0;
				gainMaskRef[id] = 0;
				uint8_t refId = 0;
				for(uint8_t idx = 0; idx < ADIO_GAIN_NUM; idx++)
				{
					if(RAW_AD_MAX >= adRawMeas[id][idx])
					{
						if(adRawMeas[id][idx] > adValMeas)
						{
							adValMeas = adRawMeas[id][idx];
							gainMaskMea[id] = _AdGain[idx];
						}
					}
					if(RAW_AD_MAX >= adRawRef[id][idx])
					{
						if(adRawRef[id][idx] > adValRef)
						{
							adValRef = adRawRef[id][idx];
							gainMaskRef[id] = _AdGain[idx];
							refId = idx;
						}
					}
				}

				if(adRawRef[id][refId] <  (int32_t)(RAW_AD_MIN*0.9))
				{
					TraceDBG(TSK_ID_MEASURE, "led gain->chn: %d raw ad value %d is quit small !\n",id,adRawRef[id][gainMaskRef[id]]);
				}

			}
			ret = OK;
		}
	}
	SetLedCurrentGain();


	if(ret == OK)
	{
		Trigger_EEPSave((void*)&ledCurrentCtrolSet, sizeof(ledCurrentCtrolSet), SYNC_CYCLE);
		Trigger_EEPSave((void*)&gainMaskMea, sizeof(gainMaskMea), SYNC_CYCLE);
		Trigger_EEPSave((void*)&gainMaskRef, sizeof(gainMaskRef), SYNC_IM);
	}
	UpdateGainSetting(0);
	return ret;

}


void InitGainReCalc(void)
{
	ledTest = 0;

	for(uint16_t id =0; id< MEA_STEP_DONE; id++)
	{
		gainMaskMeaSet[id] = _AdGain[0];
		adGainRef_Idx[id] = 0;
		adGainMeas_Idx[id] = 0;
		//set to enable led with low current;
		ledCurrentCtrolSet[id] = LED_LOW_CURRENT;
		adGainMeas_State[id] = 0;
		adGainRef_State[id] = 0;
		//start with low amplify, and low current;
		gainMaskMea[id] = _AdGain[adGainMeas_Idx[id]];
		gainMaskRef[id] = _AdGain[adGainRef_Idx[id]];

	}
	startLedCalc = 1;
	SetLedCurrentGain();
	UpdateGainSetting(0);
}


#define VALID_RANGE_OPTICS		3.0f

static float AbsFloat(const float val)
{
	if(val < 0)
		return -val;
	return val;
}

void CheckBlank_Optics(float* ptrBlank)
{
	//todo: not consider the gain is changed!
	static uint16_t dia = 0;
	uint16_t _dia = 0;
	for(uint16_t id = 0; id < BLANK_MAX; id++)
	{
		if( AbsFloat(ptrBlank[id]) < VALID_RANGE_OPTICS)
		{
			if(AbsFloat(blankOriginalData[id]) < VALID_RANGE_OPTICS)
			{
				//valid original and valid blank
				if(ptrBlank[id] > blankOriginalData[id])
				{
					if(ptrBlank[id] - blankOriginalData[id] > blankWarningLimit[id])
					{
						_dia = (uint16_t)(_dia | (1<<id));
					}
				}
			}
			else
			{
				blankOriginalData[id] = ptrBlank[id];
			}
		}
		else
		{
			_dia = (uint16_t)(0x10<<id);
		}
	}
	dia = _dia;
	Trigger_EEPSave((void*)&blankOriginalData[0],sizeof(blankOriginalData),SYNC_CYCLE);
	Dia_UpdateDiagnosis(CHECK_OPTICS,dia);
}

extern uint32_t testRefData[MEA_STEP_DONE];
extern uint32_t testMeasData[MEA_STEP_DONE];
extern uint8_t	testADCNum;

static const uint32_t ADC_limitLow = 625000;
static const uint32_t ADC_limitHigh = 695000 ;

void CheckLedError(void)
{
	uint16_t ledLow = 0;
	uint16_t dia = 0;
	uint32_t val = 0xFFFFFFFF;
	if(testADCNum)
	{
		for(uint8_t idx= MEA_STEP_START; idx <MEA_STEP_DONE; idx++)
		{
			testLedRef[idx] = testRefData[idx] / testADCNum;
			testLedMeas[idx] = testMeasData[idx] / testADCNum;
			if(testLedRef[idx] <= testLedRefLimit[idx])
			{
				dia |= (1<<idx);
				val = testLedRef[idx];
			}
		}
		if(dia)
				TraceDBG(TSK_ID_MEASURE,"Led output low: chn info: %x, value: %d\n",dia, val);
		ledLow = dia;



		dia = 0;
		for(uint8_t idx= MEA_STEP_START; idx <MEA_STEP_DONE; idx++)
		{
			if( (testLedRef[idx] >= ADC_limitLow) && (testLedRef[idx] <= ADC_limitHigh))
			{
				if((testLedMeas[idx] >= ADC_limitLow) && (testLedMeas[idx] <= ADC_limitHigh))
				{
					dia |= (1<<(idx));
					val = testLedMeas[idx];
					//那么报警提示  灯出现异常.
				}
			}
		}
		if( (testLedMeas[MEA_STEP_660nm_LONG] >= ADC_limitLow) && (testLedMeas[MEA_STEP_660nm_LONG] <= ADC_limitHigh))
		{
			if((testLedMeas[MEA_STEP_880nm_LONG] >= ADC_limitLow) && (testLedMeas[MEA_STEP_880nm_LONG] <= ADC_limitHigh))
			{
				//那么报警提示  连接线 J2出现异常.
				dia |= (1<<4);
				val = testLedMeas[MEA_STEP_880nm_LONG];
			}
		}
		if( (testLedRef[MEA_STEP_660nm_LONG] >= ADC_limitLow) && (testLedRef[MEA_STEP_660nm_LONG] <= ADC_limitHigh))
		{
			if((testLedRef[MEA_STEP_880nm_LONG] >= ADC_limitLow) && (testLedRef[MEA_STEP_880nm_LONG] <= ADC_limitHigh))
			{
				//那么报警提示  连接线 J4出现异常.
				dia |= (1<<5);
				val = testLedRef[MEA_STEP_660nm_LONG];
			}
		}
		if( (testLedMeas[MEA_STEP_660nm_SHORT] >= ADC_limitLow) && (testLedMeas[MEA_STEP_660nm_SHORT] <= ADC_limitHigh))
		{
			if((testLedMeas[MEA_STEP_880nm_SHORT] >= ADC_limitLow) && (testLedMeas[MEA_STEP_880nm_SHORT] <= ADC_limitHigh))
			{
				//那么报警提示  连接线 J3出现异常.
				dia |= (1<<6);
				val = testLedMeas[MEA_STEP_660nm_LONG];
			}
		}
		if( (testLedRef[MEA_STEP_660nm_SHORT] >= ADC_limitLow) && (testLedRef[MEA_STEP_660nm_SHORT] <= ADC_limitHigh))
		{
			if((testLedRef[MEA_STEP_880nm_SHORT] >= ADC_limitLow) && (testLedRef[MEA_STEP_880nm_SHORT] <= ADC_limitHigh))
			{
				//那么报警提示  连接线 J5出现异常.
				dia |= (1<<7);
				val = testLedRef[MEA_STEP_660nm_LONG];
			}
		}
	}
	if(dia)
		TraceDBG(TSK_ID_MEASURE,"Led error: chn info: %x, value: %d\n",dia, val);

	UpdateLedDiag(ledLow, dia);
}



uint16_t UpdateAbsInfo(const float* absFloat)
{
	if((absInfo[0].lastAbs < 0.001f) || (calAct.refreshAbs) )
	{
		absInfo[0].firstAbs = absFloat[0];
		absInfo[1].firstAbs = absFloat[1];
		absInfo[2].firstAbs = absFloat[2];
		absInfo[3].firstAbs = absFloat[3];
		absInfo[0].diffAbs = 0.0f;
		absInfo[1].diffAbs = 0.0f;
		absInfo[2].diffAbs = 0.0f;
		absInfo[3].diffAbs = 0.0f;
		calAct.refreshAbs = 0;
	}
	else
	{
		absInfo[0].diffAbs += (absFloat[0] - absInfo[0].lastAbs);
		absInfo[1].diffAbs += (absFloat[1] - absInfo[1].lastAbs);
		absInfo[2].diffAbs += (absFloat[2] - absInfo[2].lastAbs);
		absInfo[3].diffAbs += (absFloat[3] - absInfo[3].lastAbs);
	}
	absInfo[0].lastAbs = absFloat[0];
	absInfo[1].lastAbs = absFloat[1];
	absInfo[2].lastAbs = absFloat[2];
	absInfo[3].lastAbs = absFloat[3];
	Trigger_EEPSave((void*)absInfo, sizeof(absInfo), SYNC_CYCLE);
	return OK;
}

uint16_t Initialize_MeasCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&measCfg);
	result = Initialize_T_UNIT(me,typeOfStartUp);
	if(result==OK)
	{
		/*if( (typeOfStartUp & INIT_HARDWARE) != 0)
		{
		}

		if( (typeOfStartUp & INIT_TASKS) != 0)
		{
		}
		*/
		if((typeOfStartUp & INIT_DATA) != 0)
		{
			if((fileID1 != fileID_Default) || (fileID2 != fileID_Default))
			{
				(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
				TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);
				result = WARNING;
			}
		}
		UpdateGainSetting(0);
		SetLedCurrentGain();
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}




//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put meas cfg
 \param  me = pointer to subsystem
 \param  objectIndex = object index.
 \param  attributeIndex = attribute index.
 \param  ptrValue = pointer to object value.
 \return T_UNIT error code.
 \warning
 \test	NA
 \n by:
 \n intention:
 \n result module test:
 \n result Lint Level 3:
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Put_MeasCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &measCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	switch(objectIndex)
	{
		case OBJ_IDX_REG_MEA:
			if(attributeIndex <= AD717X_REG_MAX)
			{
				uint32_t val = *(uint32_t*)ptrValue;
				result = DevAD_WriteReg(AD7172_MEA_ID,(uint8_t)attributeIndex,val);
			}
			else
				result = ILLEGAL_ATTRIB_IDX;
			break;
		case OBJ_IDX_REG_REF:
			if(attributeIndex <= AD717X_REG_MAX)
			{
				uint32_t val = *(uint32_t*)ptrValue;
				result = DevAD_WriteReg(AD7172_REF_ID,(uint8_t)attributeIndex,val);
			}
			else
				result = ILLEGAL_ATTRIB_IDX;
			break;
		case OBJ_IDX_ADTIME:
			result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
			Init_Tim9_Value();
			break;
		case OBJ_IDX_GAIN_MEA:
		case OBJ_IDX_GAIN_REF:
			result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
			UpdateGainSetting(0);
			break;
		case OBJ_IDX_CALC_GAIN:
			result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
			calAct.calcGainStatus = FATAL_ERROR;
			break;
		case OBJ_IDX_LED_CURR:
			result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
			SetLedCurrentGain();
			break;
		case OBJ_IDX_LED_MASK:
			result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);

			if(result == OK)
			{
				for(uint16_t i =0; i<sizeof(ledOutMask)/sizeof(uint16_t);i++)
				{
					SetLedOutMask(i, ledOutMask[i]);
				}

			}
			break;

		default:
			result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
			break;
	}

	return result;
}



//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get meas cfg
 \param  me = pointer to subsystem
 \param  objectIndex = object index.
 \param  attributeIndex = attribute index.
 \param  ptrValue = pointer to object value.
 \return T_UNIT error code.
 \warning
 \test	NA
 \n by:
 \n intention:
 \n result module test:
 \n result Lint Level 3:
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Get_MeasCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &measCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	switch(objectIndex)
	{
		case OBJ_IDX_REG_MEA:
			if(attributeIndex <= AD717X_REG_MAX)
			{
				result = DevAD_ReadReg(AD7172_MEA_ID,(uint8_t)attributeIndex,(uint32_t*)ptrValue);
			}
			else
				result = ILLEGAL_ATTRIB_IDX;
			break;
		case OBJ_IDX_REG_REF:
			if(attributeIndex <= AD717X_REG_MAX)
			{
				result = DevAD_ReadReg(AD7172_REF_ID,(uint8_t)attributeIndex,(uint32_t*)ptrValue);
			}
			else
				result = ILLEGAL_ATTRIB_IDX;
			break;
		default:
			result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	}

	return result;
}
