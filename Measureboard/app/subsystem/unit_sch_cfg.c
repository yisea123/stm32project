/*
 * unit_sch_cfg.c
 *
 *  Created on: 2016ï¿½ï¿½10ï¿½ï¿½25ï¿½ï¿½
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
#include "unit_meas_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_sys_info.h"
#include "unit_rtc_cfg.h"
#include "unit_statistics_data.h"
#include "dev_eep.h"
#include "dev_log_sp.h"
#include "tsk_sch.h"
#include "unit_cfg_ex.h"


#define FILE_ID			0x18042514



//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

static uint32_t		fileID1									__attribute__ ((section (".configbuf_sch")));
float				measRangeAlarmLimit[2]					__attribute__ ((section (".configbuf_sch")));//??????
//0: mode, 1: volume; 2: time
FlushMode			flushMode   							__attribute__ ((section (".configbuf_sch")));//??????
Measure_Sch			_measSch								__attribute__ ((section (".configbuf_sch")));//??????
Calibration_Sch		_calibSch								__attribute__ ((section (".configbuf_sch")));
static  Clean_Sch	_cleanSch								__attribute__ ((section (".configbuf_sch")));
SchInfo				schInfo									__attribute__ ((section (".configbuf_sch")));
uint16_t			cleanSchSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t			measSchSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t			measOffLineSteps[MEAS_RANGE_MAX]		__attribute__ ((section (".configbuf_sch")));
uint16_t			measOnLineSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t			measStandard0Steps[MEAS_RANGE_MAX]		__attribute__ ((section (".configbuf_sch")));
uint16_t			measStandard1Steps[MEAS_RANGE_MAX]		__attribute__ ((section (".configbuf_sch")));
uint16_t			caliSchSteps[MEAS_RANGE_MAX][2]			__attribute__ ((section (".configbuf_sch")));
uint16_t			flushSchSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t			flushSubSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t			drainSchSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t			primeSchSteps[MEAS_RANGE_MAX]			__attribute__ ((section (".configbuf_sch")));
uint16_t 			_autoRangeMode							__attribute__ ((section (".configbuf_sch")));
uint16_t 			caliPreActStd[MEAS_RANGE_MAX][2]		__attribute__ ((section (".configbuf_sch")));
float				autoRangeLimit[MEAS_RANGE_MAX][MEAS_RANGE_MAX]		__attribute__ ((section (".configbuf_sch")));
float				longShortSwitchLimit[2][2]							__attribute__ ((section (".configbuf_sch")));
float				absDeviation[MEAS_RANGE_MAX]						__attribute__ ((section (".configbuf_sch")));
float				relativeDeviation[MEAS_RANGE_MAX]					__attribute__ ((section (".configbuf_sch")));
uint16_t			deviationRetestEnable[MEAS_RANGE_MAX]				__attribute__ ((section (".configbuf_sch")));


static uint32_t		fileID2									__attribute__ ((section (".configbuf_sch")));

static uint16_t		_deviceStatus = DEV_ST_NORMAL;

float caliSlope[2];
float caliOffset[2];
uint16_t 			autoRangeMode = 0;
uint16_t			UI_Start = WAIT_FOR_UI;
uint16_t 			reTestCount = 0;
uint16_t 			measureTimes = 0;
uint16_t			deviceBusy = 0;
Calibration_Sch		calibSch;
Clean_Sch			cleanSch;
Measure_Sch			measSch;
static uint32_t		schStartTime_ST[START_TIME_MAX];
static TrigAction	trigAction[MAX_TRIGGER];
static TrigAction	trigActionUI[MAX_TRIGGER_UI];
uint16_t 			presetRangeIdx = 0xFFFF;
uint16_t 			currentMeasCaliRangIdx	= 0;

uint32_t 			actionExecuteTime_ST[MAX_ACTION_ID];
static TrigAction	pushNewTrigAction = {0};
uint16_t 			clearTrigAction = 0;
uint16_t 			startTrigAction = 0;
__IO uint16_t		trigNum[2] = {0,0};
#define TriggerDef	{0,0xFFFF}
uint16_t 			Sch_Mode = PERIOD_MODE;
uint16_t triggerFlowStep = 0;
uint32_t actionRuningDetail = 0;

#ifdef TEST_PRE_ALPHA
float caliAbsStd0Abs[4];
float caliAbsStd1Abs[4];

float caliConcentrationOffset[2] = {0,0};
float caliConcentrationSlope[2] = {1,1,};

#endif

#define     HIGH_LIM				100000.0f
#define     LOW_LIM					-100000.0f

static const uint32_t	fileID_Default = FILE_ID;

/* 0: mode; 1: volume; 2: time; */
static const FlushMode	flushMode_Default =
/* 0: mode; 1: enable; 2: disable; */
	{DISABLE_MODE, 59400, 59400};


static const float	measRangeAlarmLimit_Default[2] = {-100.0f, 1200.0f};

static uint16_t caliPreActStd_Default[MEAS_RANGE_MAX][2] =
{
		{181,184},
		{182,185},
		{183,186},
		{183,186},
};

static const float absDeviation_Default[MEAS_RANGE_MAX] =
{
		3.0f,
		10.0f,
		20.0f,
		50.0f,
};

static const float relativeDeviation_Default[MEAS_RANGE_MAX] =
{
		0.2f,
		0.2f,
		0.2f,
		0.2f,
};
static const uint16_t deviationRetestEnable_Default[MEAS_RANGE_MAX] =
{
		0,
		0,
		0,
		0,
};


static const SchInfo schInfo_Default = {{0,0,0,0},{0,0,0,0},0,0,0,0,0,0,0,0,0,0,0};
static const float longShortSwitchLimit_Default[2][2] = { {5.5,6}, {9,10}};
static const Measure_Sch measSch_Default =
{
	0,						//uint16_t 	sampleFlowDetectTime;
	0,						//uint16_t  preTreatDelay
	0,						//measAvgMode
	2,						//uint16_t	measAvgTime;
	DISABLE_MODE,			//uint16_t 	retestCfg;
	PERIOD_MODE,			//MANUAL_MODE,		//uint16_t	measureMode	;
	{-100.0f,100.0f},		//float		retestThreshHold[2];
	120,	 				//uint16_t 	measInterval
	1, 						//uint16_t 	measCaliIndex
	{2017,7,6,12,0,0},		//TimeCfg	measStartTime;
	DISABLE_MODE,			//sampleFlowDetectEnable
};


static const Calibration_Sch calibSch_Default =
{
	0,						//uint16_t	calibrationType;
	PERIOD_MODE,			//uint16_t	calibrationMode;
	DEFAULT_CALIBRATION_INTERVAL,		//uint16_t	calibrationInterval;
	POST_DISABLE,			//uint16_t	calibrationPostAction;
	{2017,7,6,12,0,0},		//TimeCfg	calibrationStartTime;
	2,						//uint16_t	calibrationPointsNum_Zero
	2,						//uint16_t	calibrationPointsNumMax_Zero
	{2, 2, 2, 2},				//uint8_t	calibrationPointsNum[3];
	{2, 2, 2, 2},				//uint8_t 	calibrationPointsNumMax[3];
	2,						//uint16_t		calibrationTimesMax
	0.1f,					//float		calibrationDeviation;
	{0.0f,0.0f},					//calibrationOffset_Long
	{3.0f,4.920f},					//calibrationSlope_Long
	{0.0f,0.0f,0.0f, 0.0f},			//float		calibrationOffset[RANGE_IDX_MAX];
	{14.1f,23.10f,60.2f, 602.0f},			//float		calibrationSlope[RANGE_IDX_MAX];
	{0,0,0,0},					//calibrationTime
	{1.0f,1.0f,1.0f,1.0f},			//float		adaptionFactor[RANGE_IDX_MAX];
	{0.0f,0.0f,0.0f,0.0f},			//float		adaptionOffset[RANGE_IDX_MAX];
	{{10.0f,18.0f},{16.0f,30.0f},{45.0f,80.0f},{450.0f,800.0f}},//caliSlopeRange
	{{-0.3f,0.3f},{-0.3f,0.3f},{-0.3f,0.3f},{-0.3f,0.3f}},//caliOffsetRange
	{{-0.3f,0.3f},{-0.3f,0.3f}},				//float		caliOffsetRange_Long[2][2];
	{{2.00f,4.0f},{4.0f,6.0f}},				//float		caliSlopeRange_Long[2][2];
	{5.0f,5.0f,50.0f,500.0f},//caliConcentration
	1.0,//caliFactor880
	1.0,//calibDilution
	0,//caliStatus
};

static const Clean_Sch  cleanSch_Default =
{
		MANUAL_MODE,
		1440,
		POST_DISABLE,
		{2017,7,6,12,0,0},
};


//static const uint16_t Sch_Mode_Default = PERIOD_MODE;

static const uint16_t cleanSchSteps_Default[MEAS_RANGE_MAX] = {170,170,170,171};
static const uint16_t caliSchSteps_Default[MEAS_RANGE_MAX][2] = {{164,167},{165,168},{166,169},{166,169}};
static const uint16_t measSchSteps_Default[MEAS_RANGE_MAX] = {160,161,162,163};
static const uint16_t flushSchSteps_Default[MEAS_RANGE_MAX] = {176,176,176,177};//todo for 177
static const uint16_t flushSubSteps_Default[MEAS_RANGE_MAX] = {16,16,16,16};//todo for 177

static const uint16_t primeSchSteps_Default[MEAS_RANGE_MAX] = {174,174,174,175};
static const uint16_t drainSchSteps_Default[MEAS_RANGE_MAX] = {178,178,178,179};

//tobe updated by qiuxun
static const uint16_t measOffLineSteps_Default[MEAS_RANGE_MAX] = {160,161,162,163};
static const uint16_t measOnLineSteps_Default[MEAS_RANGE_MAX] = {160,161,162,163};

static const uint16_t measStandard0Steps_Default[MEAS_RANGE_MAX] = {164,165,166,166};
static const uint16_t measStandard1Steps_Default[MEAS_RANGE_MAX] = {167,168,169,169};


uint16_t 	validCaliFlag = OK;
uint16_t 	systemBreakState = OK;
uint16_t 	systemStop = SYSTEM_NORMAL;

static const uint16_t 			_autoRangeMode_Default = 3;
static const uint16_t 			presetRangeIdx_Default = 0xFFFF;


static const float				autoRangeLimit_Default[MEAS_RANGE_MAX][MEAS_RANGE_MAX] = {
		{LOW_LIM,15.0f,30.0f,HIGH_LIM},
		{1.0f,1.0f,30.0f,50.0f},
		{1.0f,7.50f,5.0f,100.0f},
		{1.0f,7.50f,80.0f,HIGH_LIM}
};

static const  T_DATACLASS _ClassList[]=
{
	//lint -e545  -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measRangeAlarmLimit,measRangeAlarmLimit_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(flushMode,flushMode_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_measSch,measSch_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_calibSch,calibSch_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_cleanSch,cleanSch_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(schInfo,schInfo_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(cleanSchSteps,cleanSchSteps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliSchSteps,caliSchSteps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measSchSteps,measSchSteps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(flushSchSteps,flushSchSteps_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(primeSchSteps,primeSchSteps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(drainSchSteps,drainSchSteps_Default),
	//lint -e545
//	CONSTRUCTOR_DC_DYNAMIC(Sch_Mode,Sch_Mode_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_autoRangeMode,_autoRangeMode_Default),
	//lint -e545
	CONSTRUCTOR_DC_DYNAMIC(presetRangeIdx,presetRangeIdx_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(autoRangeLimit,autoRangeLimit_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measOffLineSteps,measOffLineSteps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measOnLineSteps,measOnLineSteps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measStandard0Steps,measStandard0Steps_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measStandard1Steps,measStandard1Steps_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(longShortSwitchLimit,longShortSwitchLimit_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliPreActStd,caliPreActStd_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(absDeviation,absDeviation_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(relativeDeviation,relativeDeviation_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(deviationRetestEnable,deviationRetestEnable_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(flushSubSteps,flushSubSteps_Default),

};


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
//0
		CONSTRUCT_SIMPLE_U16(&systemBreakState, RAM),
		CONSTRUCT_SIMPLE_U16(&Sch_Mode, RAM),
		CONSTRUCT_SIMPLE_U16(&systemStop, RAM),
		CONSTRUCT_SIMPLE_U16(&currentMeasCaliRangIdx, READONLY_RAM),
		CONSTRUCT_SIMPLE_U32(&pushNewTrigAction, RAM),

//5
		CONSTRUCT_SIMPLE_U16(&clearTrigAction, RAM),
		CONSTRUCT_SIMPLE_U16(&startTrigAction, RAM),
		CONSTRUCT_ARRAY_SIMPLE_U32(&trigActionUI[0], sizeof(trigActionUI)/sizeof(uint32_t),READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U16(&trigNum[0],  sizeof(trigNum)/sizeof(uint16_t), READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&_deviceStatus, READONLY_RAM),

//10
		CONSTRUCT_SIMPLE_U32(&actionRuningDetail, READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_T32(&actionExecuteTime_ST[0], sizeof(actionExecuteTime_ST)/sizeof(uint32_t),READONLY_RAM),
		NULL_T_DATA_OBJ,
		CONSTRUCT_SIMPLE_U16(&_measSch.sampleFlowDetectEnable, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_measSch.sampleFlowDetectTime, NON_VOLATILE),

//15
		CONSTRUCT_ARRAY_SIMPLE_T32(schStartTime_ST,sizeof(schStartTime_ST)/sizeof(uint32_t), RAM),
		CONSTRUCT_SIMPLE_U16(&_autoRangeMode, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&presetRangeIdx, RAM),
		CONSTRUCT_SIMPLE_U16(&_measSch.measCaliIndex, READONLY_NOV),
		CONSTRUCT_ARRAY_SIMPLE_U16(&flushMode, sizeof(flushMode)/sizeof(uint16_t), NON_VOLATILE),
//20
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&measRangeAlarmLimit[0],sizeof(measRangeAlarmLimit)/sizeof(float),  NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_measSch.preTreatDelay, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_measSch.measAvgTime, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_measSch.retestCfg, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_measSch.measureMode, NON_VOLATILE),

//25
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_measSch.retestThreshHold[0],2, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_measSch.measInterval, NON_VOLATILE),
		CONSTRUCT_STRUCT_TIME(&_measSch.measStartTime,NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.adaptionFactor[0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.adaptionOffset[0],MEAS_RANGE_MAX, NON_VOLATILE),
//30

		CONSTRUCT_SIMPLE_U16(&_measSch.measAvgMode, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&measureTimes, READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&reTestCount, READONLY_RAM),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
//35
		CONSTRUCT_SIMPLE_U16(&_calibSch.calibrationType, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_calibSch.calibrationMode, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_calibSch.calibrationInterval, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_calibSch.calibrationPostAction, NON_VOLATILE),
		CONSTRUCT_STRUCT_TIME(&_calibSch.calibrationStartTime,NON_VOLATILE),
//40
		CONSTRUCT_SIMPLE_U8(&_calibSch.calibrationPointsNum_Zero, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U8(&_calibSch.calibrationPointsNumMax_Zero, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U8(&_calibSch.calibrationPointsNum[0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U8(&_calibSch.calibrationPointsNumMax[0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_calibSch.calibrationTimesMax,NON_VOLATILE),

//45
		CONSTRUCT_SIMPLE_FLOAT(&_calibSch.calibrationDeviation, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.calibrationOffset_Long[0],sizeof(_calibSch.calibrationOffset_Long)/sizeof(float), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.calibrationSlope_Long[0],sizeof(_calibSch.calibrationSlope_Long)/sizeof(float), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.calibrationOffset_Short[0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.calibrationSlope_Short[0],MEAS_RANGE_MAX, NON_VOLATILE),
//50
		CONSTRUCT_ARRAY_SIMPLE_T32(&_calibSch.calibration_ST[0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.caliSlopeRange_Short[0],MEAS_RANGE_MAX*2, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.caliOffsetRange_Short[0],MEAS_RANGE_MAX*2, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.caliOffsetRange_Long[0][0],sizeof(_calibSch.caliOffsetRange_Long)/sizeof(float), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.caliSlopeRange_Long[0][0],sizeof(_calibSch.caliSlopeRange_Long)/sizeof(float), NON_VOLATILE),

//55
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_calibSch.caliConcentration[0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_SIMPLE_FLOAT(&_calibSch.caliFactor880, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_calibSch.caliStatus, READONLY_NOV),
		CONSTRUCT_SIMPLE_U16(&validCaliFlag, READONLY_RAM),
		CONSTRUCT_SIMPLE_FLOAT(&_calibSch.calibDilution, NON_VOLATILE),
//60
		CONSTRUCT_SIMPLE_U16(&deviceBusy, READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&UI_Start, RAM),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,

//65
		CONSTRUCT_SIMPLE_U16(&_cleanSch.cleanMode, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_cleanSch.cleanInterval, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&_cleanSch.cleanPostAction, NON_VOLATILE),
		CONSTRUCT_STRUCT_TIME(&_cleanSch.cleanStartTime,NON_VOLATILE),
		NULL_T_DATA_OBJ,

//70
		CONSTRUCT_ARRAY_SIMPLE_T32(&schInfo,sizeof(schInfo)/sizeof(uint32_t), NON_VOLATILE),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
		CONSTRUCT_ARRAY_SIMPLE_U16(&flushSubSteps, sizeof(flushSubSteps)/sizeof(uint16_t), NON_VOLATILE),
//75
		CONSTRUCT_ARRAY_SIMPLE_U16(&cleanSchSteps[0],sizeof(cleanSchSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&measSchSteps[0],sizeof(measSchSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&caliSchSteps[0][0],sizeof(caliSchSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&flushSchSteps[0],sizeof(flushSchSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&primeSchSteps[0],sizeof(primeSchSteps)/sizeof(uint16_t), NON_VOLATILE),
//80
		CONSTRUCT_ARRAY_SIMPLE_U16(&drainSchSteps[0],sizeof(drainSchSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&measOnLineSteps[0],sizeof(measOnLineSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&measOffLineSteps[0],sizeof(measOffLineSteps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&measStandard0Steps[0],sizeof(measStandard0Steps)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&measStandard1Steps[0],sizeof(measStandard1Steps)/sizeof(uint16_t), NON_VOLATILE),
//85
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&autoRangeLimit[0][0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&autoRangeLimit[1][0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&autoRangeLimit[2][0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&autoRangeLimit[3][0],MEAS_RANGE_MAX, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&longShortSwitchLimit[0][0],2, NON_VOLATILE),
//90
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&longShortSwitchLimit[1][0],2, NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&caliPreActStd[0][0],sizeof(caliPreActStd)/sizeof(uint16_t), NON_VOLATILE),

		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&absDeviation[0],sizeof(absDeviation)/sizeof(float), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&relativeDeviation[0],sizeof(relativeDeviation)/sizeof(float), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&deviationRetestEnable[0],sizeof(deviationRetestEnable)/sizeof(uint16_t), NON_VOLATILE),
//95
		CONSTRUCT_ARRAY_SIMPLE_U32(&trigAction[0],sizeof(trigAction)/sizeof(U32), READONLY_RAM),

};





//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT schCfg =
{
	(uint8_t*)"schCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_SchCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_SchCfg,
	Put_SchCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};

uint16_t AutoRangeChangetoIdx(uint16_t newRangeIdx, uint16_t flag, uint32_t lineNum)
{
	//user did not change the calirange when measure
	uint16_t _changedRange = 0;
	TraceMsg(TSK_ID_SCH_MEAS,"AutoRangeChangetoIdx is called in lines:%d \n", lineNum);
	if((autoRangeMode == 0) && (newRangeIdx > MEAS_RANGE_HIGH2))
	{
		TraceDBG(TSK_ID_MEASURE, "range check error: range %d is wrong for range mode %d \n",\
				newRangeIdx, autoRangeMode);

		return _changedRange;
	}
	if( OK != ChkRangeValid(newRangeIdx))
		return _changedRange;

	if(measSch.measCaliIndex == _measSch.measCaliIndex)
	{

		if(flag == 1)
		{
			TraceMsg(TSK_ID_SCH_MEAS,"measDataSaturation >= SHORT_SATURATION; from: %d to %x, Range change is required\n",_measSch.measCaliIndex , newRangeIdx);
		}
		else if(flag == 0)
			TraceMsg(TSK_ID_SCH_MEAS,"first meas==auto range change %d to %d,\n",_measSch.measCaliIndex , newRangeIdx);
		else
			TraceMsg(TSK_ID_SCH_MEAS,"final meas==auto range change %d to %d,\n",_measSch.measCaliIndex , newRangeIdx);
		if(autoRangeMode<2)
		{
			_measSch.measCaliIndex = newRangeIdx;
			Trigger_EEPSave((void*)&_measSch,sizeof(_measSch), SYNC_IM);
		}
		uint8_t revData[6];
		revData[0] = (uint8_t)measSch.measCaliIndex;
		revData[1] = (uint8_t)newRangeIdx;
		NewEventLog(EV_AUTO_RANGE_REQ, revData);
		_changedRange = 1;
	}
	return _changedRange;
}




uint16_t CheckRangeValid(const uint32_t rangeIdx_,const uint32_t line)
{
	uint16_t ret = OK;
	uint32_t rangeIdx = rangeIdx_ & MSK_RANGE_SEL;

	if(instrumentType != ADVANCED_VERSION)
	{
		if(rangeIdx >= MEAS_RANGE_HIGH_DILUSION)
			ret = FATAL_ERROR;
	}
	else
	{
		if(rangeIdx > MEAS_RANGE_HIGH_DILUSION)
			ret = FATAL_ERROR;
	}
	if(ret != OK)
	{
		TraceDBG(TSK_ID_MEASURE, "range check error: range %d is wrong and instrument type is %d, called in line: %d\n",\
				rangeIdx, instrumentType, line);
	}

	return ret;
}


uint16_t CheckMeasureRetryTimes(uint32_t startTime_T32)
{
	uint16_t ret = OK;
	static uint16_t retryTimes = 1;
	static uint32_t oldStartTime_T32 = 0;
	uint32_t times1 = 0;
	uint32_t times2 = 0;
	if (oldStartTime_T32 == 0)
	{
		retryTimes = 1;
		ret = OK;
	}
	else
	{
		uint32_t secondStart = GetSeconds_TimeCfg(&measSch.measStartTime);
		uint32_t duringSeconds = (uint32_t)measSch.measInterval * FACTOR_UNIT_MINUTES;

		uint32_t startTimeSeconds = GetSecondsFromST(startTime_T32);
		uint32_t oldTimeSeconds = GetSecondsFromST(oldStartTime_T32);

		if( (startTimeSeconds >= secondStart) && (oldTimeSeconds >= secondStart) )
		{
			times1 = (startTimeSeconds - secondStart)/duringSeconds;
			times2 = (oldTimeSeconds - secondStart)/duringSeconds;

			if( times1 == times2)
			{
				retryTimes = (uint16_t)(retryTimes + 1);
			}
			else
			{
				retryTimes = 1;
			}
		}
		else
		{
			retryTimes = 1;
		}
		if (retryTimes >= retryIimesMax)
		{
			ret = FATAL_ERROR;
		}
	}
	TraceMsg(TSK_ID_SCH_MEAS, "CheckMeasureRetryTimes,oldSt:%d, currST:%d, time1:%d, time2:%d, times: %d, ret:%d\n",\
			oldStartTime_T32,startTime_T32 ,times1,times2,retryTimes,ret );

	oldStartTime_T32 = startTime_T32;
	return ret;

}

void UpdateToCurrentSch(uint16_t type)
{
	if(type == 0)
	{
		memcpy((void*)&cleanSch, (void*)&_cleanSch, sizeof(cleanSch));
	}
	else if(type == 1)
	{
		memcpy((void*)&calibSch, (void*)&_calibSch, sizeof(calibSch));
	}
	else if(type == 2)
	{
		memcpy((void*)&measSch, (void*)&_measSch, sizeof(measSch));
		autoRangeMode = _autoRangeMode;
	}
	else
	{
		memcpy((void*)&cleanSch, (void*)&_cleanSch, sizeof(cleanSch));
		memcpy((void*)&calibSch, (void*)&_calibSch, sizeof(calibSch));
		memcpy((void*)&measSch, (void*)&_measSch, sizeof(measSch));
		autoRangeMode = _autoRangeMode;
	}
}



static uint16_t CheckDataValid(float val1,float* range1, uint32_t lineNum)
{
	uint16_t ret = OK;

	if( (val1<range1[0]) || (val1>range1[1]))
	{
		ret = FATAL_ERROR;
	}
	if(ret != OK)
	{
		TraceDBG(TSK_ID_SCH_CALI, "CheckDataValid error£¡line %d val£º %.3f Range: %.3f -> %.3f.\n", lineNum, val1, range1[0], range1[1]);
	}
	return ret;
}


uint16_t CheckCalibrationFactor(float val1,float* range1 ,float val2,float* range2)
{
	uint16_t ret = OK;
	ret = CheckDataValid(val1, range1,__LINE__);
	ret |= CheckDataValid(val2, range2,__LINE__);
	return ret;
}

uint16_t CalcOffset(float* absVal0, uint16_t rangeIdx)
{

	uint16_t ret = OK;
	rangeIdx = (MSK_RANGE_SEL&rangeIdx);
	float factor = calibSch.caliFactor880;
	assert(MEAS_RANGE_MAX > rangeIdx);

	float OffsetLong = absVal0[MEA_STEP_660nm_LONG] - factor* absVal0[MEA_STEP_880nm_LONG];

	float OffsetShort = absVal0[MEA_STEP_660nm_SHORT] - factor * absVal0[MEA_STEP_880nm_SHORT];

	ret = CheckDataValid(OffsetShort,&calibSch.caliOffsetRange_Short[rangeIdx][0] ,__LINE__);



	if(ret == OK)
	{
		if(rangeIdx == 0)
		{
			//todo: double check the function!
			ret = CheckDataValid(OffsetLong, &calibSch.caliOffsetRange_Long[rangeIdx][0],__LINE__);
		}
	}
	//
	return ret;
}


static void AlignCaliResult(uint16_t schCaliSel)
{
	if(schCaliSel == MEAS_RANGE_HIGH2)
	{
		//update range 3 also
		calibSch.calibrationOffset_Short[MEAS_RANGE_HIGH_DILUSION] = calibSch.calibrationOffset_Short[MEAS_RANGE_HIGH2];
		calibSch.calibrationSlope_Short[MEAS_RANGE_HIGH_DILUSION] = calibSch.calibrationSlope_Short[MEAS_RANGE_HIGH2] *10.0f;

		calibSch.calibration_ST[MEAS_RANGE_HIGH_DILUSION] = calibSch.calibration_ST[MEAS_RANGE_HIGH2] ;
		TraceUser("Cali range 3 also is updated as %d\n",schCaliSel);
	}
	else if(schCaliSel == MEAS_RANGE_HIGH_DILUSION)
	{
		//update range 3 also
		calibSch.calibrationOffset_Short[MEAS_RANGE_HIGH2] = calibSch.calibrationOffset_Short[MEAS_RANGE_HIGH_DILUSION];
		calibSch.calibrationSlope_Short[MEAS_RANGE_HIGH2] = calibSch.calibrationSlope_Short[MEAS_RANGE_HIGH_DILUSION] / 10.0f;

		calibSch.calibration_ST[MEAS_RANGE_HIGH2] = calibSch.calibration_ST[MEAS_RANGE_HIGH_DILUSION] ;

		TraceUser("Cali range 2 also is updated as %d\n",schCaliSel);
	}
}

uint16_t UpdateSlopeOffset(float* absVal0, float* absVal1, uint32_t _caliStartTime,uint16_t schCaliSel)
{
	uint16_t ret = OK;
	assert(MEAS_RANGE_MAX > schCaliSel);

	float factor = calibSch.caliFactor880;
	float concentration = calibSch.caliConcentration[schCaliSel];

//long are only allpied for range idx 0 and 1;
	float OffsetLong = absVal0[MEA_STEP_660nm_LONG] - factor* absVal0[MEA_STEP_880nm_LONG];

	float slopeLong = concentration / ( (absVal1[MEA_STEP_660nm_LONG] - factor * absVal1[MEA_STEP_880nm_LONG]) - OffsetLong );

//short shall be always valid for all range idx;
	float Offset = absVal0[MEA_STEP_660nm_SHORT] - factor * absVal0[MEA_STEP_880nm_SHORT];
	float slope = concentration / ( (absVal1[MEA_STEP_660nm_SHORT] - factor * absVal1[MEA_STEP_880nm_SHORT]) - Offset );
//check validation of short calibration;
	ret = CheckCalibrationFactor(Offset,&calibSch.caliOffsetRange_Short[schCaliSel][0], \
			slope, &calibSch.caliSlopeRange_Short[schCaliSel][0]);


	caliSlope[0] = slopeLong;
	caliSlope[1] = slope;
	caliOffset[0] = OffsetLong;
	caliOffset[1] = Offset;
	TraceUser("Cali offset: \tindex: %d \tret: %d \tlong %.06f,\t short %.06f,\n",\
			schCaliSel, ret, OffsetLong,Offset);
	TraceUser("Cali slope: index: %d \tret: %d \tlong %.06f,\tshort %.06f,\n",\
			schCaliSel, ret, slopeLong,slope);
	//load to default value


	calibSch.caliStatus = ret;
	if(ret == OK)
	{
//		calibSch.calibrationOffset_Short[schCaliSel] = 0.0f;
//		calibSch.calibrationSlope_Short[schCaliSel] = 0.0f;
		if(schCaliSel <= MEAS_RANGE_HIGH)
		{
//			calibSch.calibrationOffset_Long[schCaliSel] = 0.0f;
//			calibSch.calibrationSlope_Long[schCaliSel] = 0.0f;
			ret = CheckCalibrationFactor(OffsetLong, &calibSch.caliOffsetRange_Long[schCaliSel][0], \
					slopeLong, &calibSch.caliSlopeRange_Long[schCaliSel][0]);
			if(ret == OK)
			{
				calibSch.calibrationOffset_Long[schCaliSel] = OffsetLong;
				calibSch.calibrationSlope_Long[schCaliSel] = slopeLong;
			}
			TraceUser("Cali long update: index: %d \tret: %d \toffset %.06f,\tslope %.06f,\n",\
									schCaliSel, ret, OffsetLong,slopeLong);
		}
		else
		{
			TraceUser("Cali long use zero!!!: index: %d \tret: %d \toffset %.06f,\tslope %.06f,\n",\
												schCaliSel, ret, OffsetLong,slopeLong);
		}
		TraceUser("Cali long of range 0\tret: %d \toffset %.06f,\tslope %.06f,\n",\
											ret, calibSch.calibrationOffset_Long[0],calibSch.calibrationSlope_Long[0]);
		//fix bug when short is valid but long is invalid
		if(ret == OK)
		{
			calibSch.calibrationOffset_Short[schCaliSel] = Offset;
			calibSch.calibrationSlope_Short[schCaliSel] = slope;

			calibSch.calibration_ST[schCaliSel] = _caliStartTime;
			AlignCaliResult(schCaliSel);
			TraceUser("Cali short update: index: %d \tret: %d \toffset %.06f,\tslope %.06f,\n",\
					schCaliSel, ret, Offset,slope);
		}


	}

	UpdateCalibResult(STORE_TO_EEP_IM);
	//
	return ret;
}


uint16_t GetCaliFlag(uint16_t taskid, uint16_t rangeIdx)
{
	uint32_t secondsNow = GetCurrentSeconds();
	uint32_t interval = _calibSch.calibrationInterval;

//	if(_calibSch.calibrationMode == DISABLE_MODE)
//		interval = DEFAULT_CALIBRATION_INTERVAL;

	uint32_t caliTimeLen = (uint32_t)(interval*FACTOR_UNIT_HOUR);


	uint16_t _ValidCalibrationData = CheckCalibrationFactor(_calibSch.calibrationOffset_Short[rangeIdx],\
								 &_calibSch.caliOffsetRange_Short[rangeIdx][0], \
								 _calibSch.calibrationSlope_Short[rangeIdx], \
								 &_calibSch.caliSlopeRange_Short[rangeIdx][0]);

	uint32_t validCaliTime = GetSecondsFromST(_calibSch.calibration_ST[rangeIdx]);

	//check whether the calibration is executed

	if(_ValidCalibrationData != OK)
	{
		TraceMsg(taskid,"Calibration data check error.\n");
	}
	else
	{
		if(calibSch.calibrationInterval < 1)
		{
			caliTimeLen = FACTOR_UNIT_HOUR*1;
		}
		if(validCaliTime < secondsNow)
		{
			uint32_t tick = secondsNow - validCaliTime;
			if(tick > caliTimeLen)
			{
				_ValidCalibrationData = WARNING;
				TraceMsg(taskid,"Flag: Calibration Valid, But interval expired, Range: %d.\n", rangeIdx);
			}
		}
	}
	return _ValidCalibrationData;
}

uint16_t Get_SchCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &schCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);


	if(objectIndex == OBJ_IDX_START_TIME )
	{
		schStartTime_ST[START_TIME_CLEAN] = CalcTime_ST(&_cleanSch.cleanStartTime);
		schStartTime_ST[START_TIME_CALI] = CalcTime_ST(&_calibSch.calibrationStartTime);
		schStartTime_ST[START_TIME_MEAS] = CalcTime_ST(&_measSch.measStartTime);
	}
	else if(OBJ_ACTION_EXECUTE_TIME == objectIndex)
	{
		actionExecuteTime_ST[Power_On] = powerOnOff_ST.powerOnST;
		actionExecuteTime_ST[Power_Off] = powerOnOff_ST.lastPowerDownST;
	}
	else if(OBJ_SYSTEM_STATUS == objectIndex)
	{
		if(UI_Start == WAIT_FOR_UI)
		{
			_deviceStatus = DEV_ST_WAITFORUI;
		}
		else
		{
			_deviceStatus = DEV_ST_NORMAL;
			if(systemBreakState != 0)
			{
				_deviceStatus = DEV_ST_BREAK;
			}
			else
			{
				//stop
				if(systemStop != SYSTEM_NORMAL)
				{
					_deviceStatus = DEV_ST_MAINTENANCE;
				}
				else
				{

					if(Sch_Mode == MANUAL_MODE)
					{
						_deviceStatus = DEV_ST_TRIGGER_ONLY;
					}
	#if 0
					else
					{
						if( (_measSch.measureMode == MANUAL_MODE) && \
								(_calibSch.calibrationMode == MANUAL_MODE) && \
								(_cleanSch.cleanMode == MANUAL_MODE) )
						{
							_deviceStatus = DEV_ST_TRIGGER_ONLY;
						}
					}
	#endif
				}
			}
		}
	}
	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	return result;
}


void UpdateCalibResult(uint16_t type)
{
	static Calibration_Sch caliFactor_Old;
	if(STORE_TO_EEP_IM == type)
	{
		memcpy((void*)&_calibSch.calibrationOffset_Long, (void*)&calibSch.calibrationOffset_Long, sizeof(calibSch.calibrationOffset_Long));
		memcpy((void*)&_calibSch.calibrationOffset_Short, (void*)&calibSch.calibrationOffset_Short, sizeof(calibSch.calibrationOffset_Short));
		memcpy((void*)&_calibSch.calibrationSlope_Long, (void*)&calibSch.calibrationSlope_Long, sizeof(calibSch.calibrationSlope_Long));
		memcpy((void*)&_calibSch.calibrationSlope_Short, (void*)&calibSch.calibrationSlope_Short, sizeof(calibSch.calibrationSlope_Short));
		memcpy((void*)&_calibSch.calibration_ST, (void*)&calibSch.calibration_ST, sizeof(calibSch.calibration_ST));
		memcpy((void*)&_calibSch.caliStatus, (void*)&calibSch.caliStatus, sizeof(calibSch.caliStatus));
		TraceMsg(TSK_ID_SCH_CALI, "store to eep immediately, %d, short: %.4f, %.4f,%.4f,%.4f, \
				short slope: %.4f,%.4f,%.4f,%.4f,\n",
				_calibSch.calibration_ST, \
				_calibSch.calibrationOffset_Short[0],\
				_calibSch.calibrationOffset_Short[1],\
				_calibSch.calibrationOffset_Short[2],\
				_calibSch.calibrationOffset_Short[3],\
				_calibSch.calibrationSlope_Short[0], \
				_calibSch.calibrationSlope_Short[1], \
				_calibSch.calibrationSlope_Short[2], \
				_calibSch.calibrationSlope_Short[3]);
	}
	else if(STORE_TMP_FACTOR == type)
	{
		memcpy((void*)&caliFactor_Old.calibrationOffset_Long, (void*)&calibSch.calibrationOffset_Long, sizeof(calibSch.calibrationOffset_Long));
		memcpy((void*)&caliFactor_Old.calibrationOffset_Short, (void*)&calibSch.calibrationOffset_Short, sizeof(calibSch.calibrationOffset_Short));
		memcpy((void*)&caliFactor_Old.calibrationSlope_Long, (void*)&calibSch.calibrationSlope_Long, sizeof(calibSch.calibrationSlope_Long));
		memcpy((void*)&caliFactor_Old.calibrationSlope_Short, (void*)&calibSch.calibrationSlope_Short, sizeof(calibSch.calibrationSlope_Short));
		memcpy((void*)&caliFactor_Old.calibration_ST, (void*)&calibSch.calibration_ST, sizeof(calibSch.calibration_ST));
		memcpy((void*)&caliFactor_Old.caliStatus, (void*)&calibSch.caliStatus, sizeof(calibSch.caliStatus));

		TraceMsg(TSK_ID_SCH_CALI, "store temp data, %d, short: %.4f, %.4f,%.4f,%.4f, \
						short slope: %.4f,%.4f,%.4f,%.4f,\n",
						caliFactor_Old.calibration_ST, \
						caliFactor_Old.calibrationOffset_Short[0],\
						caliFactor_Old.calibrationOffset_Short[1],\
						caliFactor_Old.calibrationOffset_Short[2],\
						caliFactor_Old.calibrationOffset_Short[3],\
						caliFactor_Old.calibrationSlope_Short[0], \
						caliFactor_Old.calibrationSlope_Short[1], \
						caliFactor_Old.calibrationSlope_Short[2], \
						caliFactor_Old.calibrationSlope_Short[3]);
	}
	else if(RECOVER_FROM_TMP == type)
	{
		memcpy((void*)&_calibSch.calibrationOffset_Long, (void*)&caliFactor_Old.calibrationOffset_Long, sizeof(calibSch.calibrationOffset_Long));
		memcpy((void*)&_calibSch.calibrationOffset_Short, (void*)&caliFactor_Old.calibrationOffset_Short, sizeof(calibSch.calibrationOffset_Short));
		memcpy((void*)&_calibSch.calibrationSlope_Long, (void*)&caliFactor_Old.calibrationSlope_Long, sizeof(calibSch.calibrationSlope_Long));
		memcpy((void*)&_calibSch.calibrationSlope_Short, (void*)&caliFactor_Old.calibrationSlope_Short, sizeof(calibSch.calibrationSlope_Short));
		memcpy((void*)&_calibSch.calibration_ST, (void*)&caliFactor_Old.calibration_ST, sizeof(calibSch.calibration_ST));
		memcpy((void*)&_calibSch.caliStatus, (void*)&caliFactor_Old.caliStatus, sizeof(calibSch.caliStatus));

		TraceMsg(TSK_ID_SCH_CALI, "recover from temp immediately, %d, short: %.4f, %.4f,%.4f,%.4f, \
						short slope: %.4f,%.4f,%.4f,%.4f,\n",
						_calibSch.calibration_ST, \
						_calibSch.calibrationOffset_Short[0],\
						_calibSch.calibrationOffset_Short[1],\
						_calibSch.calibrationOffset_Short[2],\
						_calibSch.calibrationOffset_Short[3],\
						_calibSch.calibrationSlope_Short[0], \
						_calibSch.calibrationSlope_Short[1], \
						_calibSch.calibrationSlope_Short[2], \
						_calibSch.calibrationSlope_Short[3]);
	}
	else
	{}
	Trigger_EEPSave((uint8_t*)&_calibSch, sizeof(_calibSch),  SYNC_IM );
}
void UpdateSchFlushCfg(void)
{
	uint16_t steps = flushMode.disableSteps;
	if(flushMode.mode)
	{
		steps = flushMode.enableSteps;
	}

	for(uint16_t i=0;i<MEAS_RANGE_MAX; i++)
	{
	//	assert(flushSchSteps[i] <= MAX_SUBSTEP_PUMP);
		assert(flushSubSteps[i] > 0);
		if((flushSchSteps[i] <= MAX_SUBSTEP_PUMP) && ( flushSchSteps[i] >= STEP_0))
		{
			allStepsConfig[ flushSubSteps[i] - STEP_0].pump1.steps = (uint32_t)(steps);
			allStepsConfig[ flushSubSteps[i] - STEP_0 ].pump2.steps = (uint32_t)(steps);
			allStepsConfig[ flushSubSteps[i] - STEP_0 ].pump3.steps = (uint32_t)(steps);
			Trigger_EEPSave((uint8_t*)&allStepsConfig[ flushSubSteps[i] - STEP_0 ], sizeof(StepConfig),  SYNC_CYCLE );
		}
	}

}

uint16_t Initialize_SchCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&schCfg);
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
			if(_autoRangeMode == 0)//autorange 0
			{
				if(_measSch.measCaliIndex >= MEAS_RANGE_MAX)
				{
					_measSch.measCaliIndex = 1;
				}
			}
			else if(_autoRangeMode > 1)
			{
				_measSch.measCaliIndex = (uint16_t)(_autoRangeMode-2);
			}
			else
			{}
			autoRangeMode = _autoRangeMode;
			//schInfo.lastCaliTime = calibSch.calibration_ST[_measSch.measCaliIndex];
			//Trigger_EEPSave((void*)&schInfo, sizeof(schInfo), SYNC_IM);
			Trigger_EEPSave((void*)&_measSch, sizeof(_measSch), SYNC_CYCLE);
			Trigger_EEPSave((void*)&_autoRangeMode, sizeof(_autoRangeMode), SYNC_IM);
			UpdateToCurrentSch(0xFFFF);
			memcpy((void*)&calibSch, (void*)&_calibSch, sizeof(calibSch));

		}
		UpdateSchFlushCfg();

		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}

static void ClearTriggers(void)
{
	memset((void*)&trigActionUI[0],0,sizeof(trigActionUI));
	memset((void*)&trigAction[0],0,sizeof(trigAction));
	trigNum[0] = 0;
	trigNum[1] = 0;
}

uint16_t ClrCalibration_EPA(uint32_t rangeIdx_)
{
	uint16_t ret = OK;
	uint32_t rangeIdx = rangeIdx_ & MSK_RANGE_SEL;
	for(uint16_t i=0;i<MAX_TRIGGER;i++)
	{
		if((trigAction[i].byte.triggerType == IO_ACTION) && (Trigger_Calibration == trigAction[i].byte.action))
		{
			if(trigAction[i].byte.status == EXECUTE_NOT)
			{
				if((trigAction[i].byte.value0 == 0xFF) || (trigAction[i].byte.value0 == rangeIdx) )
				{
					TraceMsg(TSK_ID_CAN_IO, "IO trigger calibration (undo) is removed from the list, %d, %x",trigAction[i].u32Val,trigAction[i].u32Val);
					trigAction[i].byte.status = EXECUTE_END;
				}
			}
		}
	}
	return ret;
}

void UpdateTriggerStatus(void)
{
	if(schCfg.ptrDataSemaphore)
	{
		OS_Use_Unit(schCfg.ptrDataSemaphore);  // lint ok --> return value not from interest
	}
	for(uint16_t idx = 0; idx < MAX_TRIGGER; idx++)
	{
		if(trigAction[idx].byte.status == EXECUTE_ING)
		{
			TraceMsg(TSK_ID_SCH,"trigger id:%d -> %x is finished !\n",idx,trigAction[idx].u32Val);
			trigAction[idx].byte.status = EXECUTE_END;
		}
	}
	for(uint16_t idx = 0; idx <= trigNum[0] && idx < MAX_TRIGGER_UI; idx++)
	{
		if(trigActionUI[idx].byte.status == EXECUTE_ING)
		{
			TraceMsg(TSK_ID_SCH,"UI trigger id:%d -> %x is finished !\n",idx,trigActionUI[idx].u32Val);
			trigActionUI[idx].byte.status = EXECUTE_END;
		}
	}
	if(schCfg.ptrDataSemaphore)
	{
		OS_Unuse_Unit(schCfg.ptrDataSemaphore);  // lint ok --> return value not from interest
	}
}

static uint16_t AddNewTrigger(TrigAction pushTrig)
{
	uint16_t result = OK;
	if(schCfg.ptrDataSemaphore)
	{
		OS_Use_Unit(schCfg.ptrDataSemaphore);  // lint ok --> return value not from interest
	}
	if(pushTrig.byte.triggerType == UI_NORMAL_ACTION)
	{
		if((pushTrig.byte.action > Trigger_MIN) && ( Trigger_MAX>pushTrig.byte.action) )
		{
			if(trigNum[0] >= MAX_TRIGGER_UI)
				result = GREATERTHAN_RANGEMAX;
			else
			{
				trigActionUI[trigNum[0]].u32Val = pushTrig.u32Val;
				trigNum[0]++;
			}
		}
	}
	else
	{
		if((pushTrig.byte.action > Trigger_MIN) && ( Trigger_MAX>pushTrig.byte.action) )
		{
			uint16_t newAction = 1;
			if(pushTrig.byte.triggerType == IO_ACTION)
			{
				for(uint16_t i=0;i<MAX_TRIGGER;i++)
				{
					if((pushTrig.byte.triggerType == IO_ACTION) && (pushTrig.byte.action == trigAction[i].byte.action))
					{
						if(trigAction[i].byte.status == EXECUTE_NOT)
						{
							newAction = 0;
							TraceMsg(TSK_ID_CAN_IO, "IO trigger already exist and not executed, %d, %x",trigAction[i].u32Val,trigAction[i].u32Val);
							break;
						}
					}
				}
			}
			if(pushTrig.byte.triggerType == MODBUS_ACTION)
			{
				for(uint16_t i=0;i<MAX_TRIGGER;i++)
				{
					if((pushTrig.byte.triggerType == MODBUS_ACTION) && (pushTrig.byte.action == trigAction[i].byte.action))
					{
						if(trigAction[i].byte.status == EXECUTE_NOT)
						{
							newAction = 0;
							TraceMsg(TSK_ID_CAN_IO, "MODBUS trigger already exist and not executed, %d, %x",trigAction[i].u32Val,trigAction[i].u32Val);
							break;
						}
					}
				}
			}
			if(newAction == 1)
			{
				uint16_t id1 = 0;
				uint16_t trigMaxId = 0xFFFF;
				for( ; id1 <MAX_TRIGGER;id1++)
				{
					if(trigAction[id1].byte.status == EXECUTE_END)
					{
						trigAction[id1].u32Val = 0;
					}
				}
				for( uint16_t id = 0; id <MAX_TRIGGER;id++)
				{
					if(trigAction[id].u32Val == 0)
					{
						id1 = id;
						for( ; id1 <MAX_TRIGGER;id1++)
						{
							if((trigAction[id1].byte.status != EXECUTE_END) && (trigAction[id1].byte.action != 0))
							{
								trigAction[id].u32Val = trigAction[id1].u32Val;
								trigAction[id1].u32Val = 0;
							}
						}
					}
					if(trigAction[id].u32Val != 0)
						trigMaxId = id;
					else
						break;

				}
				pushTrig.byte.status = 0;
				if(trigMaxId == 0xFFFF)
					trigMaxId = 0;
				else
					trigMaxId = trigMaxId+1;
				assert(trigMaxId < MAX_TRIGGER );
				trigAction[trigMaxId] = pushTrig;

			}

		}

	}
	if(schCfg.ptrDataSemaphore)
	{
		OS_Unuse_Unit(schCfg.ptrDataSemaphore);  // lint ok --> return value not from interest
	}
	return result;
}
uint16_t Put_SchCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &schCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	if(OBJ_IDX_AUTORANGE == objectIndex)
	{
		if((*(uint16_t*)ptrValue <= 4) || (*(uint16_t*)ptrValue == 5 && instrumentType == ADVANCED_VERSION))
		{

		}
		else
			result = GREATERTHAN_RANGEMAX;
	}
	if(result == OK)
		result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
	//	if(objectIndex == IDX_FLUSH_MODE)
	//		UpdateSchFlushCfg();
		switch(objectIndex)
		{
		case OBJ_IDX_UI_START:
			if(UI_Start == UI_CONNECTED)
			{
				WakeUpSchedule();
			}
			break;
		case OBJ_SYSTEM_BREAK:
		{
			if (systemBreakState != 0)
			{
				systemStop = SYSTEM_STOP;
				ClearTriggers();
				SendTskMsg(SCH_ID, TSK_FORCE_BREAK, SCH_TSK_IDLE, NULL);
				uint16_t mode = MANUAL_MODE;
				SCH_Put(OBJ_SCH_MODE, WHOLE_OBJECT,(void*)&mode);
			}
			else
			{
				//@2018/6/1 add specail code to clear the leakage error
				Dia_ClrDiagnosis_Ex();
				uint16_t triggerStop = 0;
				SCH_Put(OBJ_SYSTEM_STOP,WHOLE_OBJECT,(void*)&triggerStop);
			}
		}
			break;
		case OBJ_SCH_MODE:
			if (Sch_Mode == PERIOD_MODE)
			{
				NewEventLog(EV_SCH_START , NULL);
			}
			else
			{
				NewEventLog(EV_SCH_STOP, NULL);
			}
			break;
		case OBJ_SYSTEM_STOP:
		{
			uint16_t mode =0;
			if(systemStop == SYSTEM_NORMAL)
			{
				if(systemBreakState != 0)
				{
					systemStop = SYSTEM_STOP;
					result = RULE_VIOLATION_ERR;
				}
				else
				{
					//recovery from stop action
					mode = PERIOD_MODE;
					SCH_Put(OBJ_SCH_MODE, WHOLE_OBJECT,(void*)&mode);
					NewEventLog(EV_START_ACT , NULL);
					WakeUpSchedule();//when sch is changed!
				}
			}
			else
			{
				//stop:
				mode = MANUAL_MODE;
				SCH_Put(OBJ_SCH_MODE, WHOLE_OBJECT,(void*)&mode);
				ClearTriggers();			
				SendTskMsg(SCH_ID, TSK_FORCE_STOP, SCH_TSK_IDLE, NULL);
				NewEventLog(EV_STOP_ACT , NULL);
			}
		}
			break;

		case OBJ_IDX_AUTORANGE:
			if(_autoRangeMode <= 1)//autorange 0
			{
				if(presetRangeIdx < MEAS_RANGE_MAX)
				{
					_measSch.measCaliIndex = presetRangeIdx;
					presetRangeIdx = 0xFFFF;
				}
				if(_measSch.measCaliIndex >= MEAS_RANGE_MAX)
				{
					_measSch.measCaliIndex = 1;
				}
			}
			else
			{
				//to remove the fix range alarm if this happens
				//Dia_UpdateDiagnosis(FIX_RANGE_WARNING, 0);
				_measSch.measCaliIndex = (uint16_t)(_autoRangeMode-2);
			}
			Trigger_EEPSave((void*)&_measSch, sizeof(_measSch), SYNC_CYCLE);
			Trigger_EEPSave((void*)&_autoRangeMode, sizeof(_autoRangeMode), SYNC_IM);
			WakeUpSchedule();
			break;
		case OBJ_SYSTEM_PUSH_NEW:
			result = AddNewTrigger(pushNewTrigAction);
			break;
		case OBJ_SYSTEM_CLEARTRIG:
			ClearTriggers();
			WakeUpSchedule();//when sch is changed!
			break;
		case OBJ_SYSTEM_STARTTRIG:
			trigNum[1] = 0;
			WakeUpSchedule();//when sch is changed!
			break;

		case OBJ_IDX_START_TIME:
			if(attributeIndex == START_TIME_CLEAN)
			{
				ConvertBack_U32Time(schStartTime_ST[START_TIME_CLEAN], &_cleanSch.cleanStartTime);
				Trigger_EEPSave((void*)&_cleanSch.cleanStartTime, sizeof(_cleanSch.cleanStartTime), SYNC_IM);
			}
			else if(attributeIndex == START_TIME_CALI)
			{
				ConvertBack_U32Time(schStartTime_ST[START_TIME_CALI], &_calibSch.calibrationStartTime);
				Trigger_EEPSave((void*)&_calibSch.calibrationStartTime, sizeof(_calibSch.calibrationStartTime), SYNC_IM);
			}
			else if(attributeIndex == START_TIME_MEAS)
			{
				ConvertBack_U32Time(schStartTime_ST[START_TIME_MEAS], &_measSch.measStartTime);
				Trigger_EEPSave((void*)&_measSch.measStartTime, sizeof(_measSch.measStartTime), SYNC_IM);
			}
			else
			{}
			break;

		default:
			WakeUpSchedule();//when sch is changed!
			break;
		}


	}
	return result;
}




static uint16_t GetNewTrigger(TrigAction* ptrTrig,uint16_t idx, SCH_STATE* ptrState, uint32_t* ptrSelection)
{
	uint16_t newAct = 0;
	uint32_t schRangeSelection = *ptrSelection;
	uint32_t oldschRangeSelection = *ptrSelection;
	if(ptrTrig->byte.status == EXECUTE_NOT)
	{
		newAct = 1;
		schRangeSelection = ptrTrig->byte.value0;
		if(schRangeSelection > MEAS_RANGE_MAX)
		{
			TraceMsg(TSK_ID_SCH,"Range is wrong %d, change to %d !\n",idx, schRangeSelection,measSch.measCaliIndex );
			schRangeSelection = measSch.measCaliIndex;
		}
		switch(ptrTrig->byte.action)
		{
		case 	Trigger_Drain:
			*ptrState = SCH_DRAIN;
			break;
		case	Trigger_Flush:
			*ptrState = SCH_FLUSH;
			break;
		case	Trigger_Prime:
			*ptrState = SCH_PRIME;
			break;
			/*
		case	Trigger_Offline:
			*ptrState = SCH_MEASURE;
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_MEAS_OFFLINE;
			break;
			*/
		case	Trigger_Online:
			*ptrState = SCH_MEASURE;
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_MEAS_ONLINE;
			break;
		case	Standard0_Verification:
			*ptrState = SCH_MEASURE;
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_MEAS_STD0;
			break;
		case	Standard1_Verification:
			*ptrState = SCH_MEASURE;
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_MEAS_STD1;
			break;
		case	Trigger_Measure:
			*ptrState = SCH_MEASURE;
			if(autoRangeMode<2)
			{
				*ptrState = SCH_AR_MEASURE;
			}
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_MEAS_TRIG;
			break;
		case	Trigger_Calibration:
			*ptrState = SCH_CALIBRATION;
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_CALI_TRIG;
			break;
		case	Trigger_Clean:
			*ptrState = SCH_CLEANING;
			schRangeSelection &= MSK_RANGE_SEL;
			schRangeSelection |= MSK_CLEAN_TRIG;
			break;
		case	Trigger_FlowSteps:
			*ptrState = SCH_FLOW;
			triggerFlowStep = ptrTrig->byte.value0;
			schRangeSelection |= MSK_CLEAN_TRIG;
			break;
		case	Trigger_Dev_Start:
		{
			uint16_t triggerStop = 0;
			schRangeSelection = oldschRangeSelection;
			actionExecuteTime_ST[Trigger_Dev_Start] = GetCurrentST();
			//recover stop status
			SCH_Put(OBJ_SYSTEM_STOP,WHOLE_OBJECT,(void*)&triggerStop);
		}
			//SCH_Put(OBJ_SCH_MODE, 0, (void*)&Sch_Mode);
			break;
		case	Trigger_Sch_off:
		{
			schRangeSelection = oldschRangeSelection;
			uint16_t mode = MANUAL_MODE;
			SCH_Put(OBJ_SCH_MODE, WHOLE_OBJECT,(void*)&mode);
			actionExecuteTime_ST[Trigger_Sch_off] = GetCurrentST();
		}
			//SCH_Put(OBJ_SCH_MODE, 0, (void*)&Sch_Mode);
			break;
		}


		SendTskMsg(SCH_ID, TSK_SUBSTEP, 0, NULL);
		TraceMsg(TSK_ID_SCH, "Trigger ID: %d, Action: %d,\tVal: 0x%x \t;Range:0x%x is found\n",idx, ptrTrig->byte.action,ptrTrig->u32Val, schRangeSelection);
		ptrTrig->byte.status = EXECUTE_ING;
	}
	*ptrSelection = schRangeSelection;
	return newAct;
}

uint32_t TrigNewTrigger(SCH_STATE* ptrState, uint32_t* ptrSelection)
{
	uint32_t tickOut = SCH_TSK_CYCLE;
	UpdateTriggerStatus();

	if(schCfg.ptrDataSemaphore)
	{
		OS_Use_Unit(schCfg.ptrDataSemaphore);  // lint ok --> return value not from interest
	}
	if(startTrigAction != 0)
	{
		uint16_t newAct = 0;

		TraceMsg(TSK_ID_SCH,"Check trigger which is not UI_NORMAL_ACTION !\n");

		for(uint16_t idx = 0; idx < MAX_TRIGGER; idx++)
		{
			if(trigAction[idx].u32Val == 0)
				break;
			if(trigAction[idx].byte.triggerType != UI_NORMAL_ACTION)
			{
				newAct = GetNewTrigger(&trigAction[idx],idx, ptrState,ptrSelection);

				if(newAct != 0)
					break;
			}
		}

		if(newAct == 0)
		{
			TraceMsg(TSK_ID_SCH,"Check trigger which is UI_NORMAL_ACTION !\n");
			for(uint16_t idx = 0; idx <= trigNum[0] && idx < MAX_TRIGGER; idx++)
			{
				if(trigActionUI[idx].u32Val == 0)
					break;
				if(trigActionUI[idx].byte.triggerType == UI_NORMAL_ACTION)
				{
					newAct = GetNewTrigger(&trigActionUI[idx],idx, ptrState,ptrSelection);
					if(newAct != 0)
						break;
				}
			}
		}
		if(newAct == 0)
		{
			startTrigAction = 0;
			TraceMsg(TSK_ID_SCH,"No trigger action is found, trigger is finished for all!\n");
		}
		else
		{
			tickOut = 0;
		}
	}
	if(schCfg.ptrDataSemaphore)
	{
		OS_Unuse_Unit(schCfg.ptrDataSemaphore);  // lint ok --> return value not from interest
	}




	return tickOut;
}


static const StepNameSt ActionName[MAINACT_MAX] =
{
		{MAINACT_NONE, TO_STR(MAINACT_NONE) },
		{MAINACT_CLEAN, TO_STR(MAINACT_CLEAN) },
		{MAINACT_CALIB, TO_STR(MAINACT_CALIB) },
		{MAINACT_MEAS, TO_STR(MAINACT_MEAS) },
		{MAINACT_PRIME, TO_STR(MAINACT_PRIME) },
		{MAINACT_FLUSH, TO_STR(MAINACT_FLUSH) },
		{MAINACT_DRAIN, TO_STR(MAINACT_DRAIN) },
		{MAINACT_ONLINE, TO_STR(MAINACT_ONLINE) },
		{MAINACT_OFFLINE, TO_STR(MAINACT_OFFLINE) },
		{MAINACT_SUBSTEP, TO_STR(MAINACT_SUBSTEP) },
		{MAINACT_STD0_V, TO_STR(MAINACT_STD0_V) },
		{MAINACT_STD1_V, TO_STR(MAINACT_STD1_V) },
		{MAINACT_PRE_TREATMENT, TO_STR(MAINACT_PRE_TREATMENT) },
};

const char* GetActionName(uint16_t chn)
{
	static const char defaultStepName[] = "UNKNOWN_NAME";
	static const char nullStepName[] = "\0\0";
	const char* adr = NULL;

	if(chn == 0)
		adr = nullStepName;
	else if(chn < MAINACT_MAX)
		adr = ActionName[chn-1].stepName;

	if(adr == NULL)
		adr = &defaultStepName[0];
	return adr;
}



