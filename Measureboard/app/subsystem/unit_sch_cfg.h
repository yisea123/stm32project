/*
 * unit_sch_cfg.h
 *
 *  Created on: 2016��10��25��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_SCH_CFG_H_
#define SUBSYSTEM_UNIT_SCH_CFG_H_


#include"t_data_obj_Time.h"
#include "tsk_sch.h"

#define QUEUE_CLEAN_MAX		0x05
#define QUEUE_MEAS_MAX		0x01
#define QUEUE_CALI_MAX		0x01

enum
{
	DISABLE_MODE =0,
	ENABLE_MODE,
};

enum
{
	POST_DISABLE =0,
	POST_FLUSH,
	POST_CALIBRATION,
};

enum
{
	MANUAL_MODE =0,
	PERIOD_MODE,
};
enum
{
	SYSTEM_NORMAL =0,
	SYSTEM_STOP,
};

enum
{
	RO_OFF = 0,
	RO_ON,
};

enum
{
	NOT_CHECK_LONG_SHORT,
	CHECK_LONG_SHORT,
};

enum
{
	DEV_ST_NORMAL = 0,
	DEV_ST_MAINTENANCE,
	DEV_ST_TRIGGER_ONLY,
	DEV_ST_BREAK,
	DEV_ST_WAITFORUI,
};

enum
{
	WAIT_FOR_UI = 0,
	UI_CONNECTED = 1,
	UI_TIMEOUT = 2,
};
typedef enum
{
	MAINACT_NONE = 0,
	MAINACT_CLEAN = 1,
	MAINACT_CALIB,
	MAINACT_MEAS,
	MAINACT_PRIME,
	MAINACT_FLUSH,
	MAINACT_DRAIN,
	MAINACT_ONLINE,
	MAINACT_OFFLINE,
	MAINACT_SUBSTEP,
	MAINACT_STD0_V,
	MAINACT_STD1_V,
	MAINACT_PRE_TREATMENT,
	MAINACT_INITIALIZATION,
	MAINACT_MAX,
}MainAction;

#define MAX_TRIGGER 0x30
#define MAX_TRIGGER_UI 0x20
enum
{
	EXECUTE_NOT,
	EXECUTE_ING,
	EXECUTE_END,
};

#pragma pack(push)
#pragma pack(1)

typedef union{
	uint32_t u32Val;
	struct
	{
		uint8_t action;
		uint8_t value0; // rang idx
		uint8_t triggerType; // times
		uint8_t status; //0: not execute; 1 running; other finalized;
	}byte;
}TrigAction;

#pragma pack(pop)



enum
{
	UI_NORMAL_ACTION,
	UI_MAINTENANCE,
	MODBUS_ACTION,
	IO_ACTION,
};

typedef enum
{
	Trigger_MIN = 0,
	Trigger_Drain = 1,
	Trigger_Flush = 2,
	Trigger_Prime = 3,
	Trigger_Offline = 4,
	Trigger_Online = 5,
	Standard0_Verification = 6,
	Standard1_Verification = 7,
	Trigger_Measure = 8,
	Trigger_Calibration = 9,
	Trigger_Clean = 10,
	Trigger_FlowSteps = 11,
	Trigger_Dev_Start = 12,
	Trigger_Sch_off = 13,
	Trigger_MAX,
	Sch_Measure = Trigger_MAX,
	Sch_Cali,
	Sch_Clean,
	Power_On,
	Power_Off,
	MAX_ACTION_ID
}TrigActionId;


typedef struct
{
	uint16_t 	sampleFlowDetectTime;
	uint16_t 	preTreatDelay;
	uint16_t	measAvgMode;
	uint16_t	measAvgTime;
	uint16_t 	retestCfg;
	uint16_t	measureMode	;
	float		retestThreshHold[2];
	uint16_t	measInterval;
	uint16_t	measCaliIndex;
	/**** measure interval ******/
	TimeCfg		measStartTime;
	uint16_t 	sampleFlowDetectEnable;

}Measure_Sch;


typedef struct
{
	uint8_t calibrationPointsNum_Zero;
	uint8_t calibrationPointsNumMax_Zero;
	uint8_t calibrationPointsNum;
	uint8_t calibrationPointsNumMax;
	uint16_t calibrationTimesMax;
}CaliPoint;




typedef struct
{
	uint16_t	calibrationType;
	uint16_t	calibrationMode;
	uint16_t	calibrationInterval;
	uint16_t	calibrationPostAction;
	TimeCfg		calibrationStartTime;
	uint8_t		calibrationPointsNum_Zero;
	uint8_t		calibrationPointsNumMax_Zero;
	uint8_t		calibrationPointsNum[MEAS_RANGE_MAX];
	uint8_t		calibrationPointsNumMax[MEAS_RANGE_MAX];
	uint16_t	calibrationTimesMax;
	float		calibrationDeviation;
	float		calibrationOffset_Long[2];
	float		calibrationSlope_Long[2];
	float		calibrationOffset_Short[MEAS_RANGE_MAX];
	float		calibrationSlope_Short[MEAS_RANGE_MAX];
	uint32_t	calibration_ST[MEAS_RANGE_MAX];
	float		adaptionFactor[MEAS_RANGE_MAX];
	float		adaptionOffset[MEAS_RANGE_MAX];
	float		caliSlopeRange_Short[MEAS_RANGE_MAX][2];
	float		caliOffsetRange_Short[MEAS_RANGE_MAX][2];
	float		caliOffsetRange_Long[2][2];
	float		caliSlopeRange_Long[2][2];
	float		caliConcentration[MEAS_RANGE_MAX];
	float		caliFactor880;
	float		calibDilution;
	uint16_t    caliStatus;
}Calibration_Sch;


typedef struct
{
	uint16_t 	cleanMode;
	uint16_t	cleanInterval;
	uint16_t	cleanPostAction;
	TimeCfg		cleanStartTime;
}Clean_Sch;




typedef struct
{
	uint32_t 	lastCaliTime[MEAS_RANGE_MAX];
	uint32_t	lastCaliFinish[MEAS_RANGE_MAX];
	uint32_t 	lastCleanTime;
	uint32_t	lastCleanFinish;
	uint32_t 	lastMeasTime;
	uint32_t 	lastMeasFinish;
	uint32_t	lastTrigClean;
	uint32_t	lastTrigMeas;
	uint32_t	lastTrigCali;
	uint32_t	lastTrigStd0;
	uint32_t	lastTrigStd1;
	uint32_t	lastTrigOffline;
	uint32_t	lastTrigOnline;
}SchInfo;

#define DEFAULT_CALIBRATION_INTERVAL	72//3*24
extern float caliSlope[2];
extern float caliOffset[2];
extern uint16_t				UI_Start;
extern uint16_t 			reTestCount;
extern uint16_t 			measureTimes;
extern uint16_t 			startTrigAction;
extern uint16_t 			currentMeasCaliRangIdx;
extern float				autoRangeLimit[MEAS_RANGE_MAX][MEAS_RANGE_MAX];
extern float				longShortSwitchLimit[2][2];
extern uint16_t 			caliPreActStd[MEAS_RANGE_MAX][2];
#define PRETREATMENT_TIME_UNIT			1000//second -> ms

#ifdef __cplusplus
 extern "C" {
#endif


#define RETEST_DEFAULT_INIT			5000.0f

 enum
 {
	 RETEST_NONE,
	 RETEST_ABS_CHECK,
	 RETEST_RELATIVE_CHECK,
 };

 enum
 {
	 START_TIME_CLEAN,
	 START_TIME_CALI,
	 START_TIME_MEAS,
	 START_TIME_MAX,
 };


typedef struct
{
	uint16_t mode;
	uint16_t enableSteps;
	uint16_t disableSteps;
}FlushMode;


extern float			absDeviation[MEAS_RANGE_MAX];
extern float			relativeDeviation[MEAS_RANGE_MAX];
extern uint16_t			deviationRetestEnable[MEAS_RANGE_MAX];

extern float			lastMeasureForRetest;
extern float			measRange[4];
extern FlushMode		flushMode;
extern Measure_Sch		measSch;
extern Measure_Sch 		_measSch;
extern Calibration_Sch	calibSch;
extern Clean_Sch		cleanSch;
extern SchInfo			schInfo;
extern uint32_t 		actionRuningDetail;
extern uint16_t 		triggerFlowStep;
extern uint16_t			cleanSchSteps[MEAS_RANGE_MAX];
extern uint16_t			caliSchSteps[MEAS_RANGE_MAX][2];
extern uint16_t			measSchSteps[MEAS_RANGE_MAX];
extern uint16_t			flushSchSteps[MEAS_RANGE_MAX];
extern uint16_t			drainSchSteps[MEAS_RANGE_MAX];
extern uint16_t			primeSchSteps[MEAS_RANGE_MAX];
extern uint16_t			measOffLineSteps[MEAS_RANGE_MAX];
extern uint16_t			measOnLineSteps[MEAS_RANGE_MAX];
extern uint16_t			measStandard0Steps[MEAS_RANGE_MAX];
extern uint16_t			measStandard1Steps[MEAS_RANGE_MAX];
extern uint32_t 		actionExecuteTime_ST[MAX_ACTION_ID];


extern __IO uint16_t			trigNum[2];

extern uint16_t 		Sch_Mode;
extern uint16_t 		autoRangeMode;
extern uint16_t		RO_PWM_Cfg[2];
extern uint16_t 	IO_Poll_Time;
extern TimeCfg		RO_StartTime;
extern uint16_t		deviceBusy;


extern const T_UNIT schCfg;
uint16_t CheckCalibrationFactor(float val1, float* range1, float val2,float* range2);
uint16_t CalcOffset(float* absVal0, uint16_t cliRange);
uint16_t UpdateSlopeOffset(float* absVal0, float* absVal1, uint32_t caliTime, uint16_t cliRange);
void UpdateSchFlushCfg(void);
void UpdateCalibResult(void);
uint16_t GetCaliFlag(uint16_t taskid, uint16_t rangeIdx);


uint16_t Initialize_SchCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Get_SchCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Put_SchCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Copy2UITrigger(void);
void UpdateTriggerStatus(void);
void UpdateToCurrentSch(uint16_t type);
uint16_t AutoRangeChangetoIdx(uint16_t newRangeIdx, uint16_t flag, uint32_t lineNum);
uint16_t CheckRangeValid(const uint32_t rangeIdx, const uint32_t line);
uint16_t CheckMeasureRetryTimes(uint32_t startTime);

uint32_t TrigNewTrigger(SCH_STATE* ptrState, uint32_t* ptrSelection);

#define ChkRangeValid(x)	CheckRangeValid(x,__LINE__)
#define SCH_Put(objId,attributeIndex,ptrValue)		Put_SchCfg(&schCfg,objId,attributeIndex,ptrValue)

enum
{
	OBJ_SYSTEM_BREAK,
	OBJ_SCH_MODE = 1,
	OBJ_SYSTEM_STOP = 2,
	OBJ_SYSTEM_PUSH_NEW = 4,
	OBJ_SYSTEM_CLEARTRIG = 5,
	OBJ_SYSTEM_STARTTRIG = 6,
	OBJ_SYSTEM_ALLTRIGGERS = 7,
	OBJ_SYSTEM_TRIGNUM = 8,
	OBJ_SYSTEM_STATUS = 9,
	OBJ_ACTION_EXECUTE_TIME = 11,
	OBJ_IDX_START_TIME = 15,
	OBJ_IDX_AUTORANGE=16,
	OBJ_IDX_UI_START = 61,
	OBJ_NULL_DEF,
};

#ifdef __cplusplus
}
#endif


#endif /* SUBSYSTEM_UNIT_SCH_CFG_H_ */
