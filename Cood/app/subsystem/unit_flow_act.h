/*
 * unit_flow_exec.h
 *
 *  Created on: 2018Äê2ÔÂ11ÈÕ
 *      Author: vip
 */

#ifndef APP_UNIT_FLOW_ACT_H_
#define APP_UNIT_FLOW_ACT_H_

#include "shell_io.h"
#include "t_data_obj_stepinfo.h"
#include "t_data_obj_flowstep.h"
#define ATR_POLL_START		2

enum
{
	LED0_BUBBLE,
	LED1_BUBBLE,
    LED0_BUBBLE_EX,
	MAX_BUBBLE,
};
enum
{
	ID_MAIN_STEP,
    ID_PRE_MAIN,
	ID_SUB_STEP,
    ID_PRE_SUB,
	ID_MICRO_STEP,
	ID_MAX_STEP,
};


typedef enum
{
	OBJ_FLOW_ACT_0 = 0,
	IDX_FLOW_STEP = 0,
	IDX_POLL_ACTION,
	IDX_LOAD_ROM_DF,
	IDX_SET_PASSWORD,
	IDX_SET_BUBBLE0 = 4,
	IDX_SET_BUBBLE1 = 5,
    IDX_IIC_CMD = 6,
	IDX_SYSTEM_RST = 11,
	IDX_SET_PRINT_MAP,
	OBJ_CLR_DBG_MSG = 33,
	OBJ_GET_FLOW1_STATUS = 34,
	OBJ_GET_FLOW2_STATUS = 35,
	OBJ_GET_FLOW3_STATUS = 36,
	OBJ_ACTION_NAME = 39,
	OBJ_MAINSTEP_NAME = 40,
	OBJ_SUBSTEP_NAME = 41,
	OBJ_MEAS_FAKE_PARA = 60,
}OBJ_IDX_ACT;
enum
{
    MSG_TYPE_PRINT,
    MSG_TYPE_MSG,
    MSG_TYPE_DBG,
    MSG_TYPE_USER,
    MSG_TYPE_MAX,
};


typedef struct
{
	uint16_t stepEachTime;
	uint16_t speed;
	uint16_t steps;
	uint16_t retryTimes;
	uint16_t errorAction;
    uint16_t delayMs;
}BubbleCfg;
typedef struct
{
	uint16_t delayTime;
	uint16_t cycle;
	uint16_t subStep;
}PollSch;
typedef struct
{
	uint32_t 	cycle;
	uint32_t  	stepIdx;
	uint32_t  	start;
}PollConfig;

typedef struct
{
	uint16_t dir;
	uint16_t mode;
	uint16_t pps;
	uint16_t steps;
}PumpStepCfg;


typedef struct
{
	uint16_t 	pumpId;

	PumpCfg    	pumpCfg;
}PumpCmd;

typedef struct
{
	uint16_t valveId;
	uint16_t valveStatus;
}ValveCmd;
#define ALL_STEPS_RUN		0xFFFFFFFF
extern PumpCfg	pumpCfg2;
extern const T_UNIT flowAct;

extern uint16_t pumpExecDelay;
extern FlowStepRun flowStepRun[ID_MAX_STEP];
extern BubbleCfg bubbleCfg[ MAX_BUBBLE ];
extern FlowStepInfo gFlowStep;
extern uint16_t ctrlTemp;
extern uint16_t mainActionDetail;
#define POLL_SCH_SIZE   16
extern uint8_t pollSchTrig[POLL_SCH_SIZE];
PollSch	pollSchCfg[POLL_SCH_SIZE];
uint16_t Initialize_FlowAct(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_FlowAct(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_FlowAct(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint32_t GetStepsMS_WithDelay(uint16_t subStep);
uint16_t CalcDuringSecondsStep_WithDelay(uint16_t subStep);
#endif /* APP_UNIT_FLOW_ACT_H_ */
