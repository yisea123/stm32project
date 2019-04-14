/*
 * tsk_head.h
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */


enum
{
	DA_OUT_REFRESH_CURR = 0x10,
	DA_OUT_REFRESH_SPEED = 0x20,
	DO_OUT_REFRESH = 0x40,
	OUTPUT_REFRESH = 0x70,
};

typedef enum
{
	ST_WELD_IDLE,
	ST_WELD_INITPARA,
	ST_WELD_PRE_GAS,
	ST_WELD_PRE_GAS_DELAY,
	ST_WELD_ARC_ON,
	ST_WELD_ARC_ON_DELAY,
	ST_WELD_UPSLOPE,
	ST_WELD_UPSLOPE_CYC,
	ST_WELD_MOTION,
	ST_WELD_MOTION_CYC,
	ST_WELD_STOP,
	ST_WELD_POST_HOME,
	ST_WELD_POST_HOME_DELAY,
	ST_WELD_POST_GAS,
	ST_WELD_POST_GAS_DELAY,


	ST_WELD_FINISH,
}ST_WELD_STATE;

typedef enum
{
	CURR_CALI_IDLE,
	CURR_CALI_START,
	CURR_CALI_PREGAS,
	CURR_CALI_PREGAS_DELAY,
	CURR_CALI_ARC,
	CURR_CALI_ARC_DELAY,
	CURR_CALI_CURRENT,
	CURR_CALI_CURRENT_DELAY,
	CURR_CALI_POSTGAS,
	CURR_CALI_POSTGAS_DELAY,
	CURR_CALI_FINISH,
}CURR_CALI_STATE;
typedef enum
{
	ST_MOTOR_IDLE,
	ST_MOTOR_JOGP,
	ST_MOTOR_JOGN,
	ST_MOTOR_JOG_INIT,
	ST_MOTOR_JOG_DELAY,
	ST_MOTOR_JOG_FINISH,
	ST_MOTOR_WELD_MOTION_START,
	ST_MOTOR_WELD_MOTION_CYC,
	ST_MOTOR_WELD_MOTION_FINISH,
	ST_MOTOR_HOME,
	ST_MOTOR_HOME_PID,
	ST_MOTOR_HOME_FINISH,
	ST_MOTOR_FINISH,
}MOTOR_STATE;

void SetCurrOutVolt(float curr);
void SetSpeedOutVolt(float duty);
void UpdateWeldSetting(void);
uint32_t GetInputPins(void);
void StartADCMonitor(void const * argument);
void StartMotorTsk(void const * argument);
void StartOutputTsk(void const * argument);
void StartWeldTask(void const * argument);
void StartLBTask(void const * argument);
void StartEthernet(void const * argument);
