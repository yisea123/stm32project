/*
 * unit_statistics_data.h
 *
 *  Created on: 2016Äê9ÔÂ6ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_STATISTICS_DATA_H_
#define SUBSYSTEM_UNIT_STATISTICS_DATA_H_


#include "t_data_obj_flowstep.h"
#include "unit_sys_diagnosis.h"
#ifdef __cplusplus
 extern "C" {
#endif

enum
{
	OBJ_IDX_FLOW_PERCENTAGE = 0,
	OBJ_IDX_CHANGE_TUBING = 1,
	OBJ_IDX_RESET_FACT = 2,
	OBJ_IDX_TUBING_LIFE = 3,
	OBJ_IDX_CHANGE_MOTOR = 4,


	OBJ_IDX_INSTALL_TIME,
	OBJ_IDX_MOTOR_STEP,//4 motor
	OBJ_IDX_MOTOR_TIME,//time
	OBJ_IDX_FLOW_STA,
	OBJ_IDX_STA_MAX,
};
extern volatile float		fluidVolume_Remain[VOL_MAX_ID];
extern volatile uint32_t 	lifeSpan_InstalledSeconds[LIFESPAN_W_MAX_ID];
extern volatile uint32_t	lifeSpanTubing_StaticSeconds[LIFESPAN_W_TUBING_MAX];
void UpdateTubingLifeTime(void);
typedef struct
{
	uint32_t powerOnST;
	uint32_t lastPowerDownST;
}PowerOnOff;

typedef struct
{
	uint32_t powerOnSeconds;
	uint32_t lastPowerDownSeconds;
}STA_LifeTime;



extern volatile PowerOnOff powerOnOff_ST;
extern volatile STA_LifeTime lifeTime_STA;

enum
{
	COM_MAX_TICK,
	COM_MIN_TICK,
	COM_AVG_TICK,
	COM_ERROR_COUNT,
	COM_WARNING_COUNT,
	COM_OK_COUNT,
	COM_TICK_USED,
	COM_MAX_COUNT,
};


typedef struct
{
	uint16_t	pumpId;
	uint16_t	valveState;
}PumpValve;
#define MAX_STA_FLOW_SIZE			16

extern volatile uint32_t communicationStatus[COM_MAX_COUNT];
extern volatile uint32_t communicationHappenST[COM_MAX_COUNT];
extern const T_UNIT STA_Data;

uint16_t Initialize_STA_Data(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_STA_Data(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_STA_Data(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);


uint16_t LoadRomDefaults_STA_Data(const T_UNIT *me, int16_t dataClassIndex);

void UpdateSTA_LifeTime(void);


uint16_t Sta_AddMixTime(uint32_t mixTime,uint32_t pps);
uint16_t Sta_AddValveCount(uint16_t pins);


#ifdef __cplusplus
 }
#endif



#endif /* SUBSYSTEM_UNIT_STATISTICS_DATA_H_ */
