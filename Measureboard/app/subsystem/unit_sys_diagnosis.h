/*
 * unit_cpu_DIAGNOSIS.h
 *
 *  Created on: 2017骞�5鏈�27鏃�
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_SYS_DIAGNOSIS_H_
#define SUBSYSTEM_UNIT_SYS_DIAGNOSIS_H_

#include "t_data_obj_measdata.h"

#define MAX_DIAGNOSIS_ID  48

enum
{
	VOL_RG_A,
	VOL_RG_B,
	VOL_RG_C,
	VOL_STD_0,
	VOL_STD_1,
	VOL_STD_2,
	VOL_CLEAN,
	VOL_MAX_ID,
};

enum
{
	LIFESPAN_W_TUBING_VALVE = 0,
	LIFESPAN_W_TUBING_PUMP1 = 1,
	LIFESPAN_W_TUBING_PUMP2 = 2,
	LIFESPAN_W_TUBING_PUMP3 = 3,
	LIFESPAN_W_TUBING_MAX = 4,
	LIFESPAN_W_PUMP1 = LIFESPAN_W_TUBING_MAX,
	LIFESPAN_W_PUMP2,
	LIFESPAN_W_PUMP3,
	LIFESPAN_W_MIX,

	LIFESPAN_W_MAX_ID,
};


enum
{
	MEAS_OUT_OF_RANGE,				//0
	LOCAL_COMMUNICATION_ERR,		//1
	LED_OUTPUT_LOW,					//2
	NO_SAMPLE_FLOW,					//3
	HEAT_OUT_CTRL,					//4
	DOOR_OPEN,						//5
	AD_SPI_FAILURE,					//6
	FLASH_FAILURE,					//7
	EEPROM_ERROR,					//8
	RAM_FAILURE,					//9
	POWER_FAILUR,					//10
	VOLT_OUT_RANGE,					//11
	TEMP_SENSOR_FAILURE,			//12
	CASE_TEMP_WARNING,				//13
	FAILED_CALIBRATION,				//14
	LEAKAGE_WARNING,				//15
	CHECK_OPTICS,					//16
	REAGENT_INVALID,				//17
	CURRENT_ERROR,					//18
	ALARM_LOW,						//19
	ALARM_HIGH,						//20
	REAGNET_EMPTY_A,				//21
	REAGNET_EMPTY_B,				//22
	REAGNET_EMPTY_C,				//23
	STD_EMPTY_0,					//24
	STD_EMPTY_1,					//25
	STD_EMPTY_2,					//26
	CLEANING_EMPTY,					//27
	REAGNET_LOW_A,					//28
	REAGNET_LOW_B,					//29
	REAGNET_LOW_C,					//30
	STD_LOW_0,						//31
	STD_LOW_1,						//32
	STD_LOW_2,						//33
	CLEANING_LOW,					//34
	LIFESPAN_TUBING_VALVE,			//35
	LIFESPAN_TUBING_PUMP,			//36
	LIFESPAN_MOTOR_PUMP1,			//37
	LIFESPAN_MOTOR_PUMP2,			//38
	LIFESPAN_MOTOR_PUMP3,			//39
	LIFESPAN_MOTOR_MIX,				//40
	RTC_ALARM,						//41
	LED_ERROR,						//42
	CANOPEN_WARNING,				//43
	CANOPEN_ERROR,					//44
	CANOPEN_ER_CONNECT,				//45
	CANOPEN_ER_CONFIG,				//46
	FIX_RANGE_WARNING,				//47
	DIAG_MAX_ID,					//48
};

//7: enable/disable
//56: type: 1:info, 2: warning: 3: error;
//4: relay_event;
//3: ao_exception
//2: auto_clear
//1: rev
//0: stop
enum
{
	DIA_ENABLE = 0x80,
	DIA_ERROR = 0x60,
	DIA_WARNING = 0x40,
	DIA_REMINDER = 0x20,
	DIA_RELAY_EVENT = 0x10,
	DIA_AO_EXCEPTION = 0x08,
	DIA_AUTO_CLR = 0x04,
	DIA_MASK = 0x02,
	DIA_STOPACTION = 0x01,
	DIA_NONE = 0x0,
}DiaMsk;

//Mask	Relay_Event	AO_Exception	AutoClear	Stop action	Type




typedef struct
{
	int16_t lowLimit;
	int16_t highLimit;
	int16_t hysteresis;
	int16_t times;
}SysAlarm;


enum
{
	E_IDX_POWER_ALARM = 0,
	E_IDX_LEAKAGE = 1,
	E_IDX_RTC_ALARM = 2,
	E_IDX_CTRL_TEMP = 3,
	E_IDX_CASE_TEMP = 4,
	E_IDX_SENSOR_TEMP1 = 5,
	E_IDX_SENSOR_TEMP2 = 6,
	E_IDX_SENSOR_TEMP3 = 7,
};

enum
{
	OBJ_IDX_DIA_MSK = 6,
	OBJ_IDX_DIA_EVENT = 20,
};
extern uint32_t eventMask;
extern const T_UNIT diagnosisCfg;
extern SysAlarm leakageAlarm;
extern SysAlarm rtcAlarm;
extern SysAlarm voltageAlarm;
extern const float 	fluidVolume_Set_Default[VOL_MAX_ID];
extern float		fluidVolume_Set[VOL_MAX_ID];
extern uint8_t		_diagnosisLoc[DIAG_MAX_ID];
extern uint8_t		diagnosisOutput[DIAG_MAX_ID];

uint16_t Initialize_Diagnosis(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_Diagnosis(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_Diagnosis(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
#define DIA_EX_FUNC		10


typedef void (*UpdateDiagnosis_Func)(void);
uint16_t UpdateDiag2UI(void);
uint16_t UpdateEvent2UI(void);
extern UpdateDiagnosis_Func Dia_FuncList[DIA_EX_FUNC];
void Dia_UpdateDiagnosis(uint16_t id, uint16_t val);
void Dia_SetDiagnosis(uint16_t st, uint16_t val);
void Dia_ClrDiagnosis(uint16_t st);
void Dia_UpdatePreHeatCtrl(uint16_t tempPreHeat);
void Dia_ClrDiagnosis_Ex(void);
uint16_t Dia_SampleFlow(void);


void RefreshDiagnosisOutput(void);
EventData* GetAvailableEventBuff(uint16_t type);
EventData* GetAvailableDiagBuff(uint16_t type);
#endif /* SUBSYSTEM_UNIT_SYS_DIAGNOSIS_H_ */
