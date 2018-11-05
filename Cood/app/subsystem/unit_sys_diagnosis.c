/*
 * unit_sys_DIAGNOSIS.c
 *
 *  Created on: 2017骞�5鏈�27鏃�
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_data_obj_measdata.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_sys_diagnosis.h"
#include "unit_flow_cfg.h"
#include "unit_statistics_data.h"
#include "unit_temp_cfg.h"
#include "unit_rtc_cfg.h"
#include "dev_temp.h"
#include "unit_sch_cfg.h"
#include "tsk_sch.h"
#include "unit_io_cfg.h"
#include "dev_log_sp.h"
#include "unit_cfg_ex.h"


#define FILE_ID			0x18081515

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;
static uint32_t fileID1 										__attribute__ ((section (".configbuf_diagnosis")));
SysAlarm leakageAlarm											__attribute__ ((section (".configbuf_diagnosis")));
SysAlarm rtcAlarm 												__attribute__ ((section (".configbuf_diagnosis")));
SysAlarm voltageAlarm 											__attribute__ ((section (".configbuf_diagnosis")));
SysAlarm ctrlTempAlarm											__attribute__ ((section (".configbuf_diagnosis")));
SysAlarm caseTempAlarm											__attribute__ ((section (".configbuf_diagnosis")));
SysAlarm sensorTempAlarm										__attribute__ ((section (".configbuf_diagnosis")));

float	fluidVolume_Set[VOL_MAX_ID]								__attribute__ ((section (".configbuf_diagnosis")));
//unit: hours
static uint32_t lifeSpan_W_Set[LIFESPAN_W_MAX_ID]				__attribute__ ((section (".configbuf_diagnosis")));

float		volumeUsageLimit[VOL_MAX_ID]						__attribute__ ((section (".configbuf_diagnosis")));

uint8_t 	diaBehavior[DIAG_MAX_ID]							__attribute__ ((section (".configbuf_diagnosis")));
uint16_t	diaCounter[DIAG_MAX_ID]								__attribute__ ((section (".configbuf_diagnosis")));
uint32_t	diagSetTimeST[DIAG_MAX_ID]							__attribute__ ((section (".configbuf_diagnosis")));
uint32_t	diagClrTimeST[DIAG_MAX_ID]							__attribute__ ((section (".configbuf_diagnosis")));
uint32_t	eventMask											__attribute__ ((section (".configbuf_diagnosis")));


static uint32_t fileID2 										__attribute__ ((section (".configbuf_diagnosis")));
uint16_t AdcVolts[ADC3_MAX_CHN];
uint8_t	_diagnosisLoc[DIAG_MAX_ID] = {0,0,};
uint8_t		diagnosisOutput[DIAG_MAX_ID] = {0,0,};
uint8_t diag_IO[2];
int16_t leakageVolt;
int16_t humidity;
int16_t rtcVolt;
EventData lastEventData;
EventData lastDiagData;
static uint16_t ledDiagnosis = 0x0;
static const uint32_t eventMask_Default = EV_CMD_PUT_OBJ | EV_CMD_PUT_MEM | EV_FAKE_TEST | EV_MEASURE;
//
static const uint32_t lifeSpan_W_Set_Default[LIFESPAN_W_MAX_ID] =
{
		180*24,90*24,90*24,180*24,//days
		0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
};
static const uint8_t diaBehavior_Default[DIAG_MAX_ID]=
{
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	0	MEAS_OUT_OF_RANGE	/* 0 */    TO_STR(MEAS_OUT_OF_RANGE),
		DIA_ENABLE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	1	LOCAL_COMMUNICATION_ERR	/* 1 */    TO_STR(LOCAL_COMMUNICATION_ERR),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	2	LED_OUTPUT_LOW	/* 2 */    TO_STR(LED_OUTPUT_LOW),
		DIA_ENABLE|	DIA_MASK|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	3	NO_SAMPLE_FLOW	/* 3 */    TO_STR(NO_SAMPLE_FLOW),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	4	HEAT_OUT_CTRL	/* 4 */    TO_STR(HEAT_OUT_CTRL),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	5	DOOR_OPEN	/* 5 */    TO_STR(DOOR_OPEN),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	6	AD_SPI_FAILURE	/* 6 */    TO_STR(AD_SPI_FAILURE),
		DIA_DISABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_NONE|	DIA_NONE|	DIA_WARNING	,//	7	FLASH_FAILURE	/* 7 */    TO_STR(FLASH_FAILURE),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_NONE|	DIA_STOPACTION|	DIA_ERROR	,//	8	EEPROM_ERROR	/* 8 */    TO_STR(EEPROM_ERROR),
		DIA_ENABLE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_NONE	,//	9	RAM_FAILURE	/* 9 */    TO_STR(RAM_FAILURE),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	10	POWER_FAILUR	/* 10 */    TO_STR(POWER_FAILUR),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	11	VOLT_OUT_RANGE	/* 11 */    TO_STR(VOLT_OUT_RANGE),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	12	TEMP_SENSOR_FAILURE	/* 12 */    TO_STR(TEMP_SENSOR_FAILURE),
		DIA_ENABLE|	DIA_MASK|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	13	CASE_TEMP_WARNING	/* 13 */    TO_STR(CASE_TEMP_WARNING),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	14	FAILED_CALIBRATION	/* 14 */    TO_STR(FAILED_CALIBRATION),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_NONE|	DIA_STOPACTION|	DIA_ERROR	,//	15	LEAKAGE_WARNING	/* 15 */    TO_STR(LEAKAGE_WARNING),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	16	CHECK_OPTICS	/* 16 */    TO_STR(CHECK_OPTICS),
		DIA_ENABLE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_NONE|	DIA_REMINDER	,//	17	REAGENT_INVALID	/* 17 */    TO_STR(REAGENT_INVALID),
		DIA_ENABLE|	DIA_MASK|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	18	CURRENT_ERROR	/* 18 */    TO_STR(CURRENT_ERROR),
		DIA_DISABLE|	DIA_MASK|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	19	ALARM_LOW	/* 19 */    TO_STR(ALARM_LOW),
		DIA_DISABLE|	DIA_MASK|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	20	ALARM_HIGH	/* 20 */    TO_STR(ALARM_HIGH),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	21	REAGNET_EMPTY_A 	/* 21 */    TO_STR(REAGNET_EMPTY_A ),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	22	REAGNET_EMPTY_B	/* 22 */    TO_STR(REAGNET_EMPTY_B),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	23	REAGNET_EMPTY_C	/* 23 */    TO_STR(REAGNET_EMPTY_C),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	24	STD_EMPTY_0	/* 24 */    TO_STR(STD_EMPTY_0),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	25	STD_EMPTY_1	/* 25 */    TO_STR(STD_EMPTY_1),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	26	STD_EMPTY_2	/* 26 */    TO_STR(STD_EMPTY_2),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	27	CLEANING_EMPTY	/* 27 */    TO_STR(CLEANING_EMPTY),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	28	REAGNET_LOW_A	/* 28 */    TO_STR(REAGNET_LOW_A),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	29	REAGNET_LOW_B	/* 29 */    TO_STR(REAGNET_LOW_B),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	30	REAGNET_LOW_C	/* 30 */    TO_STR(REAGNET_LOW_C),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	31	STD_LOW_0	/* 31 */    TO_STR(STD_LOW_0),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	32	STD_LOW_1	/* 32 */    TO_STR(STD_LOW_1),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	33	STD_LOW_2	/* 33 */    TO_STR(STD_LOW_2),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	34	CLEANING_LOW	/* 34 */    TO_STR(CLEANING_LOW),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	35	LIFESPAN_TUBING_VALVE	/* 35 */    TO_STR(LIFESPAN_TUBING_VALVE),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	36	LIFESPAN_TUBING_PUMP	/* 36 */    TO_STR(LIFESPAN_TUBING_PUMP),
		DIA_DISABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	37	LIFESPAN_MOTOR_PUMP1	/* 37 */    TO_STR(LIFESPAN_MOTOR_PUMP1),
		DIA_DISABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	38	LIFESPAN_MOTOR_PUMP2	/* 38 */    TO_STR(LIFESPAN_MOTOR_PUMP2),
		DIA_DISABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	39	LIFESPAN_MOTOR_PUMP3	/* 39 */    TO_STR(LIFESPAN_MOTOR_PUMP3),
		DIA_DISABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	40	LIFESPAN_MOTOR_MIX	/* 40 */    TO_STR(LIFESPAN_MOTOR_MIX),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	41	RTC_ALARM	/* 41 */    TO_STR(RTC_ALARM),
		DIA_ENABLE|	DIA_NONE|	DIA_RELAY_EVENT|	DIA_AO_EXCEPTION|	DIA_NONE|	DIA_STOPACTION|	DIA_ERROR	,//	42	LED_ERROR	/* 42 */    TO_STR(LED_ERROR),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	43	CANOPEN_WARNING	/* 43 */    TO_STR(CANOPEN_WARNING),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_ERROR	,//	44	CANOPEN_ERROR	/* 44 */    TO_STR(CANOPEN_ERROR),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	45	CANOPEN_ER_CONNECT	/* 45 */    TO_STR(CANOPEN_ER_CONNECT),
		DIA_ENABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_WARNING	,//	46	CANOPEN_ER_CONFIG	/* 46 */    TO_STR(CANOPEN_ER_CONFIG),
		DIA_DISABLE|	DIA_MASK|	DIA_NONE|	DIA_NONE|	DIA_AUTO_CLR|	DIA_NONE|	DIA_REMINDER	,//	47	FIX_RANGE_WARNING	/* 47 */    TO_STR(FIX_RANGE_WARNING),

};

static const uint16_t	diaCounter_Default[DIAG_MAX_ID] = {0,0,0,0};
static const uint32_t	diagSetTimeST_Default[DIAG_MAX_ID] = {0,0,0,0};
static const uint32_t	diagClrTimeST_Default[DIAG_MAX_ID] = {0,0,0,0};
static const SysAlarm ctrlTempAlarm_Default =
{
		200,
		200,
		100,
		166,//each time is 60ms, max 10s
};

static const SysAlarm caseTempAlarm_Default =
{
		500,
		5300,
		100,
		3,
};

static const SysAlarm sensorTempAlarm_Default =
{
		100,
		7500,
		100,
		3,
};
const float fluidVolume_Set_Default[VOL_MAX_ID] =
{
		970.0f,970.0f,1000.0f,
		1000.0f,1000.0f,1000.0f,
		2000.0f,
};



static const float volumeUsageLimit_Default[VOL_MAX_ID] =
{
		0.3f,0.3f,0.3f,
		0.3f,0.3f,0.3f,
		0.3f
};

static const uint32_t fileID_Default = FILE_ID;

static const SysAlarm rtcAlarm_Default =
{
		2800,3400,100,3,
};

static const SysAlarm leakageAlarm_Default =
{
		1500,3000,100,3,
};


static const SysAlarm voltageAlarm_Default =
{
		11500,13800,100,3,
};


static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(rtcAlarm,rtcAlarm_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(leakageAlarm,leakageAlarm_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(voltageAlarm,voltageAlarm_Default),

	CONSTRUCTOR_DC_STATIC_CONSTDEF(volumeUsageLimit,volumeUsageLimit_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fluidVolume_Set,fluidVolume_Set_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(diaBehavior,diaBehavior_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(ctrlTempAlarm, ctrlTempAlarm_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(caseTempAlarm, caseTempAlarm_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(lifeSpan_W_Set,lifeSpan_W_Set_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(sensorTempAlarm,sensorTempAlarm_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(diaCounter, diaCounter_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(diagClrTimeST, diagClrTimeST_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(diagSetTimeST, diagSetTimeST_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(eventMask, eventMask_Default),

};

//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
		//0
	CONSTRUCT_ARRAY_SIMPLE_I16(
			&leakageAlarm,
			sizeof(leakageAlarm)/sizeof(uint16_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_I16(
			&rtcAlarm,
			sizeof(rtcAlarm)/sizeof(uint16_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_I16(
			&voltageAlarm,
			sizeof(voltageAlarm)/sizeof(uint16_t),
			NON_VOLATILE),

	CONSTRUCT_SIMPLE_U16(
							&ledDiagnosis,
							READONLY_RAM),
				//unit: hours
	CONSTRUCT_ARRAY_SIMPLE_U32(
			&lifeSpan_W_Set,
			sizeof(lifeSpan_W_Set)/sizeof(uint32_t),
			NON_VOLATILE),
	//5
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
									&fluidVolume_Set,
									sizeof(fluidVolume_Set)/sizeof(float),
									NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(
									&diaBehavior[0],
									sizeof(diaBehavior)/sizeof(uint8_t),
									NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_I16(
			&ctrlTempAlarm,
			sizeof(ctrlTempAlarm)/sizeof(uint16_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_I16(
			&caseTempAlarm,
			sizeof(caseTempAlarm)/sizeof(uint16_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_I16(
				&sensorTempAlarm,
				sizeof(sensorTempAlarm)/sizeof(uint16_t),
				NON_VOLATILE),
	//10
	CONSTRUCT_ARRAY_SIMPLE_U8(
										&diagnosisOutput[0],
										sizeof(diagnosisOutput)/sizeof(uint8_t),
										READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(
									&_diagnosisLoc[0],
									sizeof(_diagnosisLoc)/sizeof(uint8_t),
									READONLY_RAM),



	NULL_T_DATA_OBJ,
	CONSTRUCT_SIMPLE_I16(
						&leakageVolt,
						READONLY_RAM),
	CONSTRUCT_SIMPLE_I16(
						&humidity,
						READONLY_RAM),
	//15
	CONSTRUCT_SIMPLE_I16(
							&rtcVolt,
							READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(
							&diag_IO[0],
							sizeof(diag_IO)/sizeof(uint8_t),
							READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
								&diaCounter[0],
								sizeof(diaCounter)/sizeof(uint16_t),
								READONLY_NOV),

	CONSTRUCT_ARRAY_SIMPLE_T32(
								&diagSetTimeST[0],
								sizeof(diagSetTimeST)/sizeof(uint32_t),
								READONLY_NOV),
	CONSTRUCT_ARRAY_SIMPLE_T32(
			&diagClrTimeST[0],
			sizeof(diagClrTimeST)/sizeof(uint32_t),
			READONLY_NOV),

	//20
	CONSTRUCT_STRUCT_EVENTDATA
	(&lastEventData,READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
								&volumeUsageLimit,
								sizeof(volumeUsageLimit)/sizeof(float),
								NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
								&AdcVolts[0],
								sizeof(AdcVolts)/sizeof(uint16_t),
								READONLY_RAM),
	CONSTRUCT_STRUCT_EVENTDATA
	(&lastDiagData,READONLY_RAM),
	CONSTRUCT_SIMPLE_U32(
						&eventMask,
						NON_VOLATILE),


};



//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT diagnosisCfg =
{
	(uint8_t*)"diagnosisCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_Diagnosis, // will be overloaded
	LoadRomDefaults_T_UNIT,
	Get_Diagnosis,
	Put_Diagnosis,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};



const char*  DiaErrorStr[DIAG_MAX_ID] =
{
/* 0 */    TO_STR(MEAS_OUT_OF_RANGE),
/* 1 */    TO_STR(LOCAL_COMMUNICATION_ERR),
/* 2 */    TO_STR(LED_OUTPUT_LOW),
/* 3 */    TO_STR(NO_SAMPLE_FLOW),
/* 4 */    TO_STR(HEAT_OUT_CTRL),
/* 5 */    TO_STR(DOOR_OPEN),
/* 6 */    TO_STR(AD_SPI_FAILURE),
/* 7 */    TO_STR(FLASH_FAILURE),
/* 8 */    TO_STR(EEPROM_ERROR),
/* 9 */    TO_STR(RAM_FAILURE),
/* 10 */    TO_STR(POWER_FAILUR),
/* 11 */    TO_STR(VOLT_OUT_RANGE),
/* 12 */    TO_STR(TEMP_SENSOR_FAILURE),
/* 13 */    TO_STR(CASE_TEMP_WARNING),
/* 14 */    TO_STR(FAILED_CALIBRATION),
/* 15 */    TO_STR(LEAKAGE_WARNING),
/* 16 */    TO_STR(CHECK_OPTICS),
/* 17 */    TO_STR(REAGENT_INVALID),
/* 18 */    TO_STR(CURRENT_ERROR),
/* 19 */    TO_STR(ALARM_LOW),
/* 20 */    TO_STR(ALARM_HIGH),
/* 21 */    TO_STR(REAGNET_EMPTY_A ),
/* 22 */    TO_STR(REAGNET_EMPTY_B),
/* 23 */    TO_STR(REAGNET_EMPTY_C),
/* 24 */    TO_STR(STD_EMPTY_0),
/* 25 */    TO_STR(STD_EMPTY_1),
/* 26 */    TO_STR(STD_EMPTY_2),
/* 27 */    TO_STR(CLEANING_EMPTY),
/* 28 */    TO_STR(REAGNET_LOW_A),
/* 29 */    TO_STR(REAGNET_LOW_B),
/* 30 */    TO_STR(REAGNET_LOW_C),
/* 31 */    TO_STR(STD_LOW_0),
/* 32 */    TO_STR(STD_LOW_1),
/* 33 */    TO_STR(STD_LOW_2),
/* 34 */    TO_STR(CLEANING_LOW),
/* 35 */    TO_STR(LIFESPAN_TUBING_VALVE),
/* 36 */    TO_STR(LIFESPAN_TUBING_PUMP),
/* 37 */    TO_STR(LIFESPAN_MOTOR_PUMP1),
/* 38 */    TO_STR(LIFESPAN_MOTOR_PUMP2),
/* 39 */    TO_STR(LIFESPAN_MOTOR_PUMP3),
/* 40 */    TO_STR(LIFESPAN_MOTOR_MIX),
/* 41 */    TO_STR(RTC_ALARM),
/* 42 */    TO_STR(LED_ERROR),
/* 43 */    TO_STR(CANOPEN_WARNING),
/* 44 */    TO_STR(CANOPEN_ERROR),
/* 45 */    TO_STR(CANOPEN_ER_CONNECT),
/* 46 */    TO_STR(CANOPEN_ER_CONFIG),
/* 47 */    TO_STR(FIX_RANGE_WARNING),
};


void Dia_UpdateDiagnosis(uint16_t id, uint16_t val)
{
//	if(id == LED_OUTPUT_LOW)
//	ledDiagnosis = val;
	if(val)
		Dia_SetDiagnosis(id, val);
	else
		Dia_ClrDiagnosis(id);
}

static uint16_t testId = AD_SPI_FAILURE;

void Dia_SetDiagnosis(uint16_t id, uint16_t val)
{
	assert(id < DIAG_MAX_ID);
	_diagnosisLoc[id] = (uint8_t)val;
	if(_diagnosisLoc[id] == 0)
		_diagnosisLoc[id] = (val!=0)?1:0;
	uint8_t msk = diaBehavior[id] & DIA_ENABLE;
	if(msk != 0)
	{
		msk = diaBehavior[id] & DIA_STOPACTION;

		if(msk)
		{
			//device enter break state;
#if 0
			uint16_t brk = 1;
			schCfg.Get(&schCfg,OBJ_SYSTEM_BREAK, 0, &brk );
			if(brk == 0 )
			{
				brk = 1;
				schCfg.Put(&schCfg,OBJ_SYSTEM_BREAK, 0, &brk );
			}
#endif
		}
	}


}

void Dia_ClrDiagnosis_Ex(void)
{
	//clear all which is not auto-cleared diagnosis;
	for(uint16_t id = 0; id < DIAG_MAX_ID; id++)
	{
		uint8_t msk = diaBehavior[id] & DIA_AUTO_CLR;
		if(msk == 0)
			_diagnosisLoc[id] = 0;
	}
}

void Dia_ClrDiagnosis(uint16_t id)
{
	assert(id < DIAG_MAX_ID);
	uint8_t msk = diaBehavior[id] & DIA_AUTO_CLR;
	if(msk)
	{
		_diagnosisLoc[id] = 0;
	}
	msk = diaBehavior[id] & DIA_STOPACTION;
	if(msk)
	{
		//device enter break state;
#ifdef AUTO_RECOVER
		uint16_t brk = 0;
		schCfg.Get(&schCfg,OBJ_SYSTEM_BREAK, 0, &brk );
		if(brk != 0 )
		{
			brk = 0;
			schCfg.Put(&schCfg,OBJ_SYSTEM_BREAK, 0, &brk );
		}
#endif
	}
}



static uint16_t GetDiaState(const SysAlarm* ptrAlarm, const uint16_t id, int16_t val)
{
#define MAX_SYS_ALARM 10
	static uint16_t lastDia[MAX_SYS_ALARM] = {0,0,0,0,0,};
	static int16_t lastTimesBig[MAX_SYS_ALARM] = {0,0,0,0,0,};
	static int16_t lastTimesSmal[MAX_SYS_ALARM] = {0,0,0,0,0,};
	uint16_t ret = 0;
	assert(id < MAX_SYS_ALARM);
	if(lastDia[id] == 0)
	{
		if(val > ptrAlarm->highLimit)
		{
			lastTimesBig[id]++;
			lastTimesSmal[id] = 0;
			ret = 1;
		}
		else if(val < ptrAlarm->lowLimit)
		{
			lastTimesSmal[id]++;
			lastTimesBig[id] = 0;
			ret = 2;
		}
		else
		{
			lastTimesBig[id] = 0;
			lastTimesSmal[id] = 0;
		}

		if( (ret == 1) && (lastTimesBig[id] >= ptrAlarm->times))
		{
			lastDia[id] = ret;
		}
		else if( (ret == 2) && (lastTimesSmal[id] >= ptrAlarm->times))
		{
			lastDia[id] = ret;
		}
		else
		{}
		if(id == E_IDX_CTRL_TEMP)
			TraceMsg(TSK_ID_MCU_STATUS,"GetDiaState id: %d, Set:%d curr:%d,ret:%d limit: %d, %d, hystsis %d, times: B %d,S %d , \n", \
				id, lastDia[id],val, ret, ptrAlarm->highLimit,ptrAlarm->lowLimit,ptrAlarm->hysteresis,lastTimesBig[id], lastTimesSmal[id]);
	}
	else
	{
		lastTimesBig[id] = 0;
		lastTimesSmal[id] = 0;
		ret = 0;
		if((lastDia[id] == 1) && (val <= ptrAlarm->highLimit - ptrAlarm->hysteresis))
		{
			lastDia[id] = 0;
			if(id == E_IDX_CTRL_TEMP)
				TraceMsg(TSK_ID_MCU_STATUS,"GetDiaState id: %d, Clr%d: curr:%d,ret:%d limit: %d, %d, hystsis %d, times: B %d,S %d , \n", \
					id, lastDia[id], val, ret, ptrAlarm->highLimit,ptrAlarm->lowLimit,ptrAlarm->hysteresis,lastTimesBig[id], lastTimesSmal[id]);
		}
		else if( (lastDia[id] == 2)  && (val >= ptrAlarm->lowLimit + ptrAlarm->hysteresis) )
		{
			lastDia[id] = 0;
			if(id == E_IDX_CTRL_TEMP)
				TraceMsg(TSK_ID_MCU_STATUS,"GetDiaState id: %d, Clr%d: curr:%d,ret:%d limit: %d, %d, hystsis %d, times: B %d,S %d , \n", \
					id, lastDia[id], val, ret, ptrAlarm->highLimit,ptrAlarm->lowLimit,ptrAlarm->hysteresis,lastTimesBig[id], lastTimesSmal[id]);
		}
		else
		{}
	}

	return lastDia[id];
}


static void Dia_UpdateVolumeUsageWarning(void)
{
#define MIN_VOLUME_EMPTY		0.01f
	for(uint16_t i=0;i<VOL_MAX_ID;i++)
	{
		if(fluidVolume_Remain[i] <= MIN_VOLUME_EMPTY)
		{
			//if(i == 0)
			TracePrint(TSK_ID_MCU_STATUS,"REAGNET_EMPTY_ %d is set. %f\n", i, fluidVolume_Remain[i]);
			Dia_UpdateDiagnosis((uint16_t)(REAGNET_EMPTY_A+i),1);
		}
		else
		{
			//if(i == 0)
			TracePrint(TSK_ID_MCU_STATUS,"REAGNET_EMPTY_ %d is clr. %f\n", i, fluidVolume_Remain[i]);
			Dia_UpdateDiagnosis((uint16_t)(REAGNET_EMPTY_A+i),0);
		}


		if(fluidVolume_Remain[i] > MIN_VOLUME_EMPTY)
		{
			Dia_UpdateDiagnosis((uint16_t)(REAGNET_LOW_A+i),(fluidVolume_Remain[i] < fluidVolume_Set[i] * volumeUsageLimit[i]));
		}
		else
		{
			Dia_UpdateDiagnosis((uint16_t)(REAGNET_LOW_A+i),0);
		}
	}
}


static void Dia_UpdateLifeTimeSpan_ST_Warning(void)
{
	uint16_t _locDia[2] = {0,0};

	UpdateTubingLifeTime();

	for(uint16_t i=0;i<LIFESPAN_W_TUBING_MAX;i++)
	{
		//if(lifeSpan_W_Set[i] * FACTOR_UNIT_HOUR < lifeSpanTubing_StaticSeconds[i])
		if(lifeSpan_W_Set[i] * FACTOR_UNIT_HOUR < lifeSpan_InstalledSeconds[i])
		{
			if(i == LIFESPAN_W_TUBING_VALVE)
				_locDia[0] = 1;
			else
				_locDia[1] = (uint16_t)(1<<i);
		}

	}

	Dia_UpdateDiagnosis(LIFESPAN_TUBING_VALVE,_locDia[0]);
	Dia_UpdateDiagnosis(LIFESPAN_TUBING_PUMP,_locDia[1]);
}

static void Dia_UpdateLifeTimeSpan_W_Warning(void)
{
	for(uint16_t i=LIFESPAN_W_PUMP1;i<LIFESPAN_W_MAX_ID;i++)
	{
		Dia_UpdateDiagnosis((uint16_t)(LIFESPAN_MOTOR_PUMP1 + i-LIFESPAN_W_PUMP1),(lifeSpan_W_Set[i] < lifeSpan_InstalledSeconds[i]/FACTOR_UNIT_HOUR));
	}
	Dia_UpdateLifeTimeSpan_ST_Warning();
}

uint16_t Dia_SampleFlow(void)
{
	uint16_t ret = OK;
	GPIO_PinState st1 = HAL_GPIO_ReadPin(SW_SNS1_GPIO_Port, SW_SNS1_Pin);
	//disable
	if(measSch.sampleFlowDetectEnable == DISABLE_MODE)
	{
		TraceMsg(TSK_ID_MCU_STATUS,"NO_SAMPLE_FLOW is disabled. %d\n", st1);
		st1 = GPIO_PIN_SET;
	}
	else
	{
		TraceMsg(TSK_ID_MCU_STATUS,"NO_SAMPLE_FLOW is enabled. %d\n", st1);
	}
	Dia_UpdateDiagnosis(NO_SAMPLE_FLOW,(st1 == GPIO_PIN_RESET));

	if(st1 == GPIO_PIN_RESET)
	{
		TraceMsg(TSK_ID_MCU_STATUS,"NO_SAMPLE_FLOW is detected. %d\n", st1);
		ret = FATAL_ERROR;
	}

	return ret;
}

static void Dia_IOMonitor(void)
{
	GPIO_PinState st2 = HAL_GPIO_ReadPin(SW_SNS2_GPIO_Port, SW_SNS2_Pin);
	diag_IO[1] = st2;
	diag_IO[0] = HAL_GPIO_ReadPin(SW_SNS1_GPIO_Port, SW_SNS1_Pin);
	Dia_UpdateDiagnosis(DOOR_OPEN,(st2 != GPIO_PIN_RESET));
	if(st2 != GPIO_PIN_RESET)
	{
		TraceMsg(TSK_ID_MCU_STATUS,"door open is detected. %d\n", st2);
	}


}

void Dia_UpdatePreHeatCtrl(uint16_t tempPreHeat)
{
	uint16_t _dia = 0;
	SysAlarm alarmCfg = ctrlTempAlarm;
	alarmCfg.highLimit = (int16_t)(tempPreHeat + ctrlTempAlarm.highLimit);
	alarmCfg.lowLimit = (int16_t)(tempPreHeat - ctrlTempAlarm.lowLimit);

	_dia = GetDiaState(&alarmCfg, E_IDX_CTRL_TEMP, GetTempChn(TARGET_CHN));

	Dia_UpdateDiagnosis(HEAT_OUT_CTRL, _dia);
}


static void Dia_UpdateTempWarning(void)
{
	//todo
	uint16_t _dia = GetDiaState(&caseTempAlarm,E_IDX_CASE_TEMP, GetTempChn(ENV_CHN));

	Dia_UpdateDiagnosis(CASE_TEMP_WARNING, _dia);
	uint16_t state = GetDiaState(&sensorTempAlarm,E_IDX_SENSOR_TEMP1, GetTempChn(0));
	_dia = state<<0;
	state = GetDiaState(&sensorTempAlarm,E_IDX_SENSOR_TEMP2, GetTempChn(1));
	_dia = (uint16_t)(_dia | (state<<2));
	state = GetDiaState(&sensorTempAlarm,E_IDX_SENSOR_TEMP3, GetTempChn(2));
	_dia = (uint16_t)(_dia | (state<<4));

	Dia_UpdateDiagnosis(TEMP_SENSOR_FAILURE, _dia);
}


static void Dia_UpdateRTCAlarm(void)
{
	uint16_t _dia = GetDiaState(&rtcAlarm,E_IDX_RTC_ALARM, rtcVolt);
	Dia_UpdateDiagnosis(RTC_ALARM, _dia);

}
#if 0
static void Dia_UpdatePowerAlarm(void)
{
	uint16_t _dia = 0;

	powerSupply[0] = AdcVolts[ADC1_12V_SUPPLY];

	if(powerSupply[0] > voltageAlarm.highLimit)
	{
		_dia = 1;
		TraceMsg(TSK_ID_MCU_STATUS,"voltageAlarm high is detected. %d\n",powerSupply[0]);
	}
	else if(powerSupply[0] < voltageAlarm.lowLimit)
	{
		_dia = 2;
		TraceMsg(TSK_ID_MCU_STATUS,"voltageAlarm low is detected. %d\n",powerSupply[0]);
	}
	else
	{}

	_dia = GetDiaState(&voltageAlarm,E_IDX_POWER_ALARM, (int16_t)powerSupply[0]);

//	Dia_UpdateDiagnosis(VOLT_OUT_RANGE,_dia);
}
#endif


static void Dia_UpdateLeakageAlarm(void)
{
	uint16_t _dia = 0;
	leakageVolt = (int16_t)GetVoltageIdx(LEAK_CHN);
	if(leakageVolt > leakageAlarm.highLimit)
	{
		_dia = 1;
		TraceMsg(TSK_ID_MCU_STATUS,"leakageAlarm high is detected. %d\n",leakageVolt);
	}
	else if(leakageVolt < leakageAlarm.lowLimit)
	{
		_dia = 1;
		TraceMsg(TSK_ID_MCU_STATUS,"leakageAlarm low is detected. %d\n",leakageVolt);

	}
	else
	{}

	_dia = GetDiaState(&leakageAlarm,E_IDX_LEAKAGE, leakageVolt);

	Dia_UpdateDiagnosis(LEAKAGE_WARNING,_dia);
}

uint16_t Initialize_Diagnosis(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&diagnosisCfg);
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
		//
		Dia_UpdateDiagnosis(POWER_FAILUR, ((RCC->CSR &RCC_CSR_LPWRRSTF) == RCC_CSR_LPWRRSTF) );
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}



UpdateDiagnosis_Func Dia_FuncList[DIA_EX_FUNC] =
{
	Dia_UpdateLeakageAlarm,
//	Dia_UpdatePowerAlarm,
	Dia_UpdateRTCAlarm,
	Dia_UpdateVolumeUsageWarning,
	Dia_UpdateLifeTimeSpan_W_Warning,
	Dia_UpdateTempWarning,
	Dia_UpdateLifeTimeSpan_ST_Warning,
	Dia_IOMonitor,
};

EventData* GetAvailableEventBuff(uint16_t type)
{
	#define MAX_EVENT_LOG_BUFF		64
	#define MSK_EVENT_LOG_BUFF		0x3F
	static EventData	EventBuff[MAX_EVENT_LOG_BUFF];
	static uint32_t idx_New = 0;
	static uint32_t idx_Old = 0;
	EventData* buff = NULL;
	if(type != GET_NEW) //get old
	{
		if(idx_Old < idx_New)
		{
			buff = &EventBuff[idx_Old&MSK_EVENT_LOG_BUFF];
			idx_Old++;
		}
		else
		{}
	}
	else
	{
		buff = &EventBuff[idx_New&MSK_EVENT_LOG_BUFF];
		idx_New++;
	}
	return buff;
}


EventData* GetAvailableDiagBuff(uint16_t type)
{
	#define MAX_DIAG_LOG_BUFF		32
	#define MSK_DIAG_LOG_BUFF		0x1F
	static EventData	DiagBuff[MAX_DIAG_LOG_BUFF];
	static uint32_t idx_New = 0;
	static uint32_t idx_Old = 0;
	EventData* buff = NULL;
	if(type != GET_NEW) //get old
	{
		if(idx_Old < idx_New)
		{
			buff = &DiagBuff[idx_Old&MSK_DIAG_LOG_BUFF];
			idx_Old++;
		}
		else
		{}
	}
	else
	{
		buff = &DiagBuff[idx_New&MSK_DIAG_LOG_BUFF];
		idx_New++;
	}
	return buff;
}



void RefreshDiagnosisOutput(void)
{
	uint8_t newState= 0;
	uint8_t aoException = 0;
	uint8_t relayEvent = 0;
	static uint8_t aoException_Bak = 0xFF;
	static uint8_t relayEvent_Bak = 0xFF;
	uint32_t timeST = GetCurrentST();

	for(uint16_t id = 0; id<DIAG_MAX_ID;id++)
	{
		//todo
		uint8_t msk = diaBehavior[id] & DIA_ENABLE;
		if(msk)
		{
			if(diagnosisOutput[id] != _diagnosisLoc[id])
			{

				uint8_t evData[6];
				evData[0] = (uint8_t)id;

				evData[1] = _diagnosisLoc[id];
				newState++;
				if(_diagnosisLoc[id])
				{
					//todo: alarm high or low
					diaCounter[id] ++;
					diagSetTimeST[id] = timeST;
					if( (DIA_ERROR & diaBehavior[id]) == DIA_ERROR)
						NewEventLog( (uint32_t)(EV_DIAG_ERR | EV_DIA_SET | id), evData);
					else if((DIA_WARNING & diaBehavior[id]) == DIA_WARNING)
						NewEventLog( (uint32_t)(EV_DIAG_WARN | EV_DIA_SET | id), evData);
					else
						NewEventLog((uint32_t)(EV_DIAG_INFO | EV_DIA_SET | id), evData);
				}
				else
				{
					//todo: alarm high or low
					diagClrTimeST[id] = timeST;
					if ((DIA_ERROR & diaBehavior[id]) == DIA_ERROR)
						NewEventLog((uint32_t)(EV_DIAG_ERR | EV_DIA_CLR | id), evData);
					else if( (DIA_WARNING & diaBehavior[id]) == DIA_WARNING)
						NewEventLog((uint32_t)(EV_DIAG_WARN | EV_DIA_CLR | id), evData);
					else
						NewEventLog((uint32_t)(EV_DIAG_INFO | EV_DIA_CLR | id), evData);
				}
				if(DiaErrorStr[id])
				{
					if(_diagnosisLoc[id] != 0)
						TraceMsg(TSK_ID_MCU_STATUS,"%s -> %d is set\n",DiaErrorStr[id],id);
					else
						TraceMsg(TSK_ID_MCU_STATUS,"%s -> %d is clear\n",DiaErrorStr[id],id);
				}
				else
				{
					if(_diagnosisLoc[id] != 0)
						TraceMsg(TSK_ID_MCU_STATUS," %d is set\n",DiaErrorStr[id],id);
					else
						TraceMsg(TSK_ID_MCU_STATUS," %d is clear\n",DiaErrorStr[id],id);
				}
			}
			diagnosisOutput[id] = _diagnosisLoc[id];

			if(diagnosisOutput[id])
			{
				msk = diaBehavior[id] & DIA_RELAY_EVENT;
				if(msk)
				{
					relayEvent++;
				}
				msk = diaBehavior[id] & DIA_AO_EXCEPTION;
				if(msk)
				{
					aoException++;
				}
			}
		}
		else
		{
			diagnosisOutput[id] = 0;
		}

	}
	if(relayEvent_Bak != relayEvent)
	{
		if(relayEvent)
		{
		//	DO_Event_Trig = FATAL_ERROR;
			SendTskMsg(SCH_IO_ID, TSK_INIT, RO_EVENT_SET, NULL, NULL);
		}
		else
		{
		//	DO_Event_Trig = OK;
			SendTskMsg(SCH_IO_ID, TSK_INIT, RO_EVENT_CLR, NULL, NULL);
		}
		relayEvent_Bak = relayEvent;
	}

	if(aoException_Bak != aoException)
	{
		if(aoException)
		{
			AO_Event_Trig = FATAL_ERROR;
			SendTskMsg(SCH_IO_ID, TSK_INIT, AO_OUTPUT, NULL, NULL);
		}
		else
		{
			AO_Event_Trig = OK;
			SendTskMsg(SCH_IO_ID, TSK_INIT, AO_OUTPUT, NULL, NULL);
		}
		aoException_Bak = aoException;
	}
	if(newState)
	{
		Trigger_EEPSave( (void*)&diaCounter, sizeof(diaCounter), SYNC_CYCLE);
		Trigger_EEPSave( (void*)&diagSetTimeST, sizeof(diagSetTimeST), SYNC_CYCLE);
		Trigger_EEPSave( (void*)&diagClrTimeST, sizeof(diagClrTimeST), SYNC_CYCLE);
	}
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put Diagnosis
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
uint16_t Put_Diagnosis(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &diagnosisCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	switch(objectIndex)
	{
	case OBJ_IDX_DIA_MSK:
		if(attributeIndex < DIAG_MAX_ID)
		{
			uint8_t dat = *(uint8_t*)ptrValue;
			uint8_t msk = DIA_MASK & diaBehavior_Default[attributeIndex];
			uint8_t msk2 = DIA_MASK & dat;
			if(msk != msk2)
			{
				result = RULE_VIOLATION;
			}
			else
			{
				uint8_t msk1 = DIA_ENABLE & dat;
				if((!msk1) && (!msk))
				{
					result = RULE_VIOLATION;
				}
			}
		}
		break;
	default:
		break;
	}
	if(result == OK)
	{
		result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	}

	return result;
}



uint16_t UpdateEvent2UI(void)
{
	uint16_t ret = OK;
	EventData* buff = GetAvailableEventBuff(GET_EXIST);
	if(buff != NULL)
	{
		memcpy((void*)&lastEventData, (void*)buff, sizeof(EventData));
		TracePrint(TSK_ID_LOCAL_BUS, "Update EV Type OK: T%d->%X =  %d\n",lastEventData.startTimeST,lastEventData.eventType,lastEventData.eventType );
	}
	else
	{
		ret = FATAL_ERROR;
		memset((void*)&lastEventData, 0, sizeof(EventData));
		TracePrint(TSK_ID_LOCAL_BUS, "Update EV Type Err: T%d->%X =  %d\n",lastEventData.startTimeST,lastEventData.eventType,lastEventData.eventType );
	}
	return ret;
}


uint16_t UpdateDiag2UI(void)
{
	uint16_t ret = OK;
	EventData* buff = GetAvailableDiagBuff(GET_EXIST);
	if(buff != NULL)
	{
		memcpy((void*)&lastDiagData, (void*)buff, sizeof(EventData));
		TracePrint(TSK_ID_LOCAL_BUS, "Update diag OK: T%d->%X =  %d\n",lastDiagData.startTimeST,lastDiagData.eventType,lastDiagData.eventType );
	}
	else
	{
		ret = FATAL_ERROR;
		memset((void*)&lastDiagData, 0, sizeof(EventData));
		TracePrint(TSK_ID_LOCAL_BUS, "Update diag Err: T%d->%X =  %d\n",lastDiagData.startTimeST,lastDiagData.eventType,lastDiagData.eventType );
	}
	return ret;
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get Diagnosis
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
uint16_t Get_Diagnosis(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &diagnosisCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
#if 0
	static uint16_t evAtr = 0xFFFF;
	switch(objectIndex)
	{
	case OBJ_IDX_DIA_EVENT:
	{
		if(evAtr>= 0xFF)//total 8 bits
		{
			EventData* buff = GetAvailableEventBuff(GET_EXIST);
			if(buff != NULL)
			{
				evAtr = 0;
				memcpy((void*)&lastEventData, (void*)buff, sizeof(EventData));
				TracePrint(TSK_ID_LOCAL_BUS, "Update EV Type OK: T%d->%X =  %d\n",lastEventData.startTimeST,lastEventData.eventType,lastEventData.eventType );
			}
			else
			{
				evAtr = 0xFFFF;
				memset((void*)&lastEventData, 0, sizeof(EventData));
				TracePrint(TSK_ID_LOCAL_BUS, "Update EV Type Err: T%d->%X =  %d\n",lastEventData.startTimeST,lastEventData.eventType,lastEventData.eventType );
			}
		}
		if((uint16_t)(attributeIndex) >= 0xFF )
		{
			evAtr = 0xFFFF;
		}
		else if(attributeIndex < 8)
		{
			evAtr |= (uint16_t)( 0x01 << attributeIndex);
		}
		else
		{}
		TracePrint(TSK_ID_LOCAL_BUS, "Get EV Type: T%d->%X =  %d, Atr:%d\n",lastEventData.startTimeST,lastEventData.eventType,lastEventData.eventType,attributeIndex );

	}
		break;
	default:
		break;
	}
#endif

	if(result == OK)
	{
		result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	}

	return result;
}



