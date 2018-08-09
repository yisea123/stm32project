/*
 * unit_temp_cfg.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_TEMP_CFG_H_
#define SUBSYSTEM_UNIT_TEMP_CFG_H_


#include "t_data_obj_pid.h"
typedef enum
{
	PID_ADJ_TEMP05,
	PID_ADJ_TEMP10,
	PID_ADJ_TEMP15,
	PID_ADJ_TEMP20,
	PID_ADJ_TEMP25,
	PID_ADJ_TEMP30,
	PID_ADJ_TEMP35,
	PID_ADJ_TEMP40,
	PID_ADJ_TEMP45,
	PID_ADJ_MAX
}PID_IDX;


typedef enum
{
	OBJ_IDX_CHN_0 = 0,
	OBJ_IDX_CHN_1,
	OBJ_IDX_CHN_2,
	ADC_MAX_CHN=3,
	ADC_IDX_ACT_TEMP = 5,
	OBJ_IDX_TEMPCTL = 12,
}OBJ_TEMP;

typedef struct
{
	int16_t preHeat_Mode;
	int16_t preHeat_Low;
	int16_t preHeat_High;
}PreHeatMode;


typedef struct
{
	int16_t tempPreHeat_Low;
	int16_t tempPreHeat_High;
}PreHeat;


enum
{
	AD_DATA,
	AD_VOLT,

	TEMPERATURE,//9
	RESISTOR,
	TEMP_MAX_ATR,
};
enum
{
	RESIST_0,
	RESIST_1,
	VOLTAGE_0,
	VOLTAGE_1,
	OFFSET,
	GAIN,
	CLI_STATE,//6
};

#define TEMP_PREHEAT_CMD		0x55FF
#define TEMP_CTRL_CMD			0xFF55
#define TEMP_STOP_CMD			0x0

#define MAX_TEMP_IDX		50




#ifdef __cplusplus
extern "C" {
#endif


extern int16_t PIDOffset[MAX_TEMP_IDX];
extern PID_VAL sPID;
extern uint16_t pwmOutput;
extern const T_UNIT tempCfg;
extern uint16_t	tempCtrlDebug[2];
uint16_t Initialize_TempCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Get_TempCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Put_TempCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

int16_t GetPreHeatTemp(void);
#ifdef __cplusplus
 }
#endif


#endif /* SUBSYSTEM_UNIT_TEMP_CFG_H_ */
