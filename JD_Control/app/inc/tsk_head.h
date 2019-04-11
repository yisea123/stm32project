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
void SetMotorSpeed(float duty, uint16_t updateType);
void UpdateWeldSetting(void);
uint32_t GetInputPins(void);
void StartADCMonitor(void const * argument);
void StartMotorTsk(void const * argument);
void StartOutputTsk(void const * argument);
void StartWeldTask(void const * argument);
void StartLBTask(void const * argument);
void StartEthernet(void const * argument);
