/*
 * unit_flow_act.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_FLOW_ACT_H_
#define SUBSYSTEM_UNIT_FLOW_ACT_H_

#include "t_data_obj_stepinfo.h"

#ifdef __cplusplus
 extern "C" {
#endif

 typedef enum
 {
 	OBJ_FLOW_ACT_0 = 0,
 	IDX_FLOW_STEP = 0,
 	IDX_POLL_ACTION,
 	IDX_LOAD_ROM_DF,
	IDX_SET_PASSWORD,
	IDX_SET_MIX,
	IDX_SET_PRINTCHN,
	IDX_SET_SCH,
	IDX_SET_SCH_MEAS,
	IDX_SET_SCH_CALI,
	IDX_SET_SCH_CLEAN,
	IDX_SET_SCH_IO,
	IDX_SYSTEM_RST,
	IDX_SET_PRINT_MAP,
 	OBJ_CLR_DBG_MSG = 33,
 	OBJ_GET_FLOW1_STATUS = 34,
	OBJ_GET_FLOW2_STATUS = 35,
	OBJ_GET_FLOW3_STATUS = 36,
	OBJ_POLL_OR_SCH = 38,
	OBJ_ACTION_NAME = 39,
	OBJ_MAINSTEP_NAME = 40,
	OBJ_SUBSTEP_NAME = 41,
	OBJ_MEAS_EXECUTE_ONCE = 42,
	OBJ_MEAS_EXTENDACTION = 43,

	OBJ_MEAS_FAKE_PARA = 60,
 }OBJ_IDX_ACT;


 enum
 {
	 FLOW_STEP_ACT,
	 FLOW_STEP_MAIN,
	 FLOW_STEP_SUB,
	 FLOW_STEP_MAX,
 };


 typedef struct
 {
 	uint32_t 	dummy0;
 	uint32_t 	dummy1;
 	uint32_t 	cycle;
 	uint32_t  	steps;
 	uint32_t  	start;
 }PollConfig;

 typedef struct
 {
 	uint16_t	mixPps_Run;
 	uint16_t	mixPps_Idle;
 	uint16_t	mixIdle_RunTime;
 	uint16_t	mixIdle_IdleTime;
 	uint16_t	mixPps_AccPps;
 	uint16_t	mixPps_AccTime;
 	uint16_t	mixPps_Mode;
 	uint16_t	mixPps_AccMode;
 	uint16_t	mixPps_AccStartPps;
 }MixConfig;
 typedef struct
 {
 	uint16_t duringTime;
 	uint16_t cycle;
 	uint16_t subStep;
 }PollSch;

typedef struct
{
	uint16_t onDelay;
	uint16_t onDuty;
	uint16_t offDelay;
	uint16_t offDelay_Cyc;
}ValveCtrl;


 typedef struct
 {
	 uint16_t startIdx;
	 uint16_t resetCounter;
	 uint16_t endConfiguration;
	 uint16_t start;
	 uint16_t currentIdx;
 }MeasExtendAction;


 enum
 {
 	MEACTION_ST_IDX = 0,
	MEACTION_RESET,
	MEACTION_END_CFG,
	MEACTION_START,
	MEACTION_CUR_ID,

 };

 typedef struct
 {
	 uint16_t valveIO;
	 uint16_t maxCycles;
	 uint16_t measureInterval;
	 uint16_t counter;
 }MeasExtend;




extern uint8_t	mainStepName[16];
extern uint8_t	subStepName[16];
#define POLL_SCH_SIZE		16

extern ValveCtrl valveCtrl;
extern uint16_t	pollFunctionEnabled;
extern FlowStepRun flowStepRun[FLOW_STEP_MAX];

extern MeasExtend		measExtend[0x08];
extern MeasExtendAction measExtendAction;
extern 	MixConfig	mixConfig;
extern  PollSch     pollSchCfg[POLL_SCH_SIZE];
extern  uint8_t		pollSchTrig[POLL_SCH_SIZE];
extern const T_UNIT flowAct;
extern uint16_t mainActionDetail;

void SetFlowStep(uint16_t id, uint16_t step, uint32_t _duringTime);
int32_t GetFlowStepRemainTime(uint16_t id);

uint16_t Initialize_FlowAct(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_FlowAct(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_FlowAct(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t MeasExecuteOnce(void);

#define FLOWACT_Put(objId,attributeIndex,ptrValue)		Put_FlowAct(&flowAct,objId,attributeIndex,ptrValue)


#ifdef __cplusplus
 }
#endif


#endif /* SUBSYSTEM_UNIT_FLOW_ACT_H_ */
