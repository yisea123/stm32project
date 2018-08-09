/*
 * unit_flow_cfg.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_FLOW_CFG_H_
#define SUBSYSTEM_UNIT_FLOW_CFG_H_

#include "t_data_obj_flowstep.h"

#ifdef __cplusplus
 extern "C" {
#endif


#define VALVE0		0x0000
#define VALVE1		0x8000
#define VALVE2		0x4000
#define VALVE3		0x2000
#define VALVE4		0x1000
#define VALVE5		0x0800
#define VALVE6		0x0400
#define VALVE7		0x0200
#define VALVE8		0x0100
#define VALVE9		0x0080
#define VALVE10		0x0040
#define VALVE11		0x0020
#define VALVE12		0x0010
#define VALVE13		0x0008
#define VALVE14		0x0004
#define VALVE15		0x0002
#define VALVE16		0x0001

#define CW			0x00
#define CCW			0x01

 enum
 {
	 FTP = 0,
	 MTP = 1,
	 FTP2 = 1,
	 FTP4 = 2,
	 FTP8 = 3,
	 FTP16 = 4,
	 FTP32 = 5,
	 SPEED_MAX_CFG,
 };

#define ATR_0					0
#define MAX_SUBSTEP_PUMP		100
#define ATR_MAX_TIMECONFIG		32
#define MAX_MAIN_SUBSTEPS		32
#define MAX_MAIN_STEPS			30
#define MAX_SP_STEPS			20

#define CW_DIR				GPIO_PIN_RESET
#define CCW_DIR				GPIO_PIN_SET

#define FULL_STEPS			GPIO_PIN_RESET
#define HALF_FULL_STEPS		GPIO_PIN_SET

#define MOTOR_CURR_HOLD		GPIO_PIN_SET
#define MOTOR_CURR_START	GPIO_PIN_RESET
#define VALVE_PINS_NUM		10

#define MOTOR_ENABLE		GPIO_PIN_RESET
#define MOTOR_DISABLE		GPIO_PIN_SET

extern const uint8_t MotorSpeed_Cfg[SPEED_MAX_CFG][3];

typedef enum{
	SUB_STEP = 1,
	STEP_0 = 1,
	PRIME_CLEAN=1,
	PRIME_0_STD,
	PRIME_STD,
	PRIME_DI_WATER,
	PRIME_REAGENT,//5

	FLUSH_SAMPLE_CM,
	FLUSH_SAMPLE_DC,
	FLUSH_DI_SAMPLE,
	FLUSH_0_STD,
	FLUSH_STD_CM,
	FLUSH_STD_DC,
	FLUSH_DIWATER_DC,//12

	IN_MEAS_1,//13
	IN_MEAS_2,
	IN_SAMPLE_DC,
	IN_0_STD_1,
	IN_0_STD_2,
	IN_STD_1,
	IN_STD_2,//19
	IN_STD_DC,
	IN_DI_SAMPLE,
	IN_DIWATER_DC,
	IN_AIR,//23

	IN_CLEANING_CM,
	IN_CLEAN_DC,
	IN_CLEAN_DT,//26


	DRAIN_FLUSH_DIWATER,
	DRAIN_DI_SAMPLE,

	DRAIN_CLEAN_CM,
	DRAIN_CLEAN_DC,//30
	PRE_MIX_AB,
	IN_SAMPLE_DC2,
	IN_STD_DC2,

	STEP_MAX = MAX_SUBSTEP_PUMP+STEP_0,

	SPECIAL_IDX_0=101,
	MEASUREMENT=101,
	PREHEATING=102,
	HEATING_CTRL=103,
	HEATING_STOP=104,
	PUMP_RESET=105,
	VALVE_STOP=106,
	PUMP_STOP=107,
	MIXTURE_STOP=108,
	MIXTURE_RUN=109,
	MIXTURE_CYC=110,
	RECALC_GAIN=111,

	PUMP_ENABLE=112,
	PUMP_DISABLE=113,
	TEST_LED_REF=114,
	BLANK_CALC_START = 115,
	BLANK_CALC_END = 116,
	BLANK_HOLD = 117,
	BLANK_CONTINUE = 118,
	BLANK_MEAS= 120,
	SPECIAL_IDX_MAX=MAX_SP_STEPS+SPECIAL_IDX_0,


	TIME_0 = 121,
	TIME1=122,
	TIME2,
	TIME3,
	TIME4,
	TIME5,
	TIME6,
	TIME7,
	TIME8,
	TIME9,
	TIME10,
	TIME11,
	TIME12,
	TIME13,
	TIME14,
	TIME15,
	TIME16,
	TIME17,
	TIME18,
	TIME19,
	TIME20,
	TIME21,
	TIME22,
	TIME23,
	TIME24,
	TIME25,
	TIME_MAX=ATR_MAX_TIMECONFIG+TIME_0,

	SUB_STEP_MAX = TIME_MAX,

	A_0 = 160,
	A_PRIME = 160,
	A_PRIME_DI,
	A_MEAS_01,
	A_MEAS_02,
	A_MEAS_DI,
	A_CALI_0STD_01,
	A_CALI_0STD_02,
	A_CALI_0STD_DI,
	A_CALI_STD_01,
	A_CALI_STD_02,
	A_CALI_STD_DI,
	A_CLEAN,
	A_CLEAN_DI,
	A_STD_ADD_VERIFICATION,
	A_MAX = MAX_MAIN_STEPS+A_0,
	OBJ_IDX_MAX,
	STEP_STOP=1000,



}STEPS;




typedef struct
{
	uint16_t stepId;
	char* stepName;
}StepNameSt;

extern StepConfig allStepsConfig[MAX_SUBSTEP_PUMP];
extern uint32_t specialConfig[MAX_SP_STEPS];
extern uint32_t timeConfig[ATR_MAX_TIMECONFIG];
extern uint8_t stepsConfig[MAX_MAIN_STEPS][MAX_MAIN_SUBSTEPS];

extern const T_UNIT flowCfg;


uint16_t Initialize_FlowCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint32_t CalcDuringTimeMsStep_WithDelay(uint16_t step);

const char* GetActionName(uint16_t chn);
const char* GetStepName(uint16_t chn);

void FakeParaInit(uint16_t val);


#ifdef __cplusplus
 }
#endif



#endif /* SUBSYSTEM_UNIT_FLOW_CFG_H_ */
