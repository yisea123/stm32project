/*
 * unit_flow_cfg.h
 *
 *  Created on: 2018年1月19日
 *      Author: pli
 */

#ifndef START_BOARDSUPPORT_RSK_R32C111_APP_UNIT_FLOW_CFG_H_
#define START_BOARDSUPPORT_RSK_R32C111_APP_UNIT_FLOW_CFG_H_

#include "t_data_obj_flowstep.h"
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

#define VALVE_PINS_NUM		11

#define CW_DIR				GPIO_PIN_RESET
#define CCW_DIR				GPIO_PIN_SET

#define FULL_STEPS			GPIO_PIN_RESET
#define HALF_FULL_STEPS		GPIO_PIN_SET

#define MOTOR_CURR_HOLD		GPIO_PIN_SET
#define MOTOR_CURR_START	GPIO_PIN_RESET

#define MOTOR_ENABLE		GPIO_PIN_RESET
#define MOTOR_DISABLE		GPIO_PIN_SET
typedef enum{
    e_open_cera_v,// = 1,
	e_open_air_v,// = 2,
	e_open_samp_v,// = 3,
	e_open_waste_v,// = 4,
	e_open_dichro_v,// = 5,
	e_open_blank_v,// = 6,
	e_open_H2SO4_v,// = 7,
	e_open_HgSO4_v,// = 8,
	e_open_std_v,// = 9,
	e_open_safety_v,// = 10,
	e_open_lock,// = 11,

	e_close_cera_v,// = 12,
	e_close_air_v,// = 13,
	e_close_samp_v,// = 14,
	e_close_waste_v,// 15,
	e_close_dichro_v,// = 16,
	e_close_blank_v,// = 17,
	e_close_H2SO4_v,// = 18,
	e_close_HgSO4_v,// = 19,
	e_close_std_v,// = 20,
	e_close_safety_v,// = 21,
	e_close_lock,// = 22,

	e_open_relay1,// =23,//继电器，触发外部信号，
	e_open_relay2,// = 24,
	e_close_relay1,// = 25,
	e_close_relay2,// = 26,

	e_open_coolfan,//27
	e_close_coolfan,
	e_open_casefan,
	e_close_casefan,
	e_open_pcbfan,
	e_close_pcbfan,


	e_read_safety,
	e_read_leakage,
	e_read_pumpstatus,

	e_read_pumppos,
	e_read_ceramic,
	e_read_heatstatus,
    
    //internal
    e_pull_pump,//27
	e_push_pump,//28
    e_pull_pump_slow,//29
	e_push_pump_slow,//30

	e_stop_pump,//31
	e_pump2bottom,//32
	e_pump2top,//33
	e_heat_ctrl,//34
    
    e_cmd_id_max,
}IIC_CMD_ENUM;


typedef enum {
	e_valve_start = 1,
	open_cera_v = e_valve_start,// = 1,
	open_air_v,// = 2,
	open_samp_v,// = 3,
	open_waste_v,// = 4,
	open_dichro_v,// = 5,
	open_blank_v,// = 6,
	open_H2SO4_v,// = 7,
	open_HgSO4_v,// = 8,
	open_std_v,// = 9,
	open_safety_v,// = 10,
	open_lock,// = 11,

	close_cera_v,// = 12,
	close_air_v,// = 13,
	close_samp_v,// = 14,
	close_waste_v,// 15,
	close_dichro_v,// = 16,
	close_blank_v,// = 17,
	close_H2SO4_v,// = 18,
	close_HgSO4_v,// = 19,
	close_std_v,// = 20,
	close_safety_v,// = 21,
	close_lock,// = 22,
	e_valve_end1,
	open_relay1 = e_valve_end1,// =23,//继电器，触发外部信号，
	open_relay2,// = 24,
	close_relay1,// = 25,
	close_relay2,// = 26,
    e_device_reset = 39,
	e_valve_end = 39,

	e_pump_start = 40,
	//todo
	e_pump_so4_in,
	e_pump_end = 79,

	e_timedelay_start = 80,
	//todo
	time_delay1,

	time_delay2,

	e_timedelay_end = 89,

	e_sp_ctrl_start = 90,
	//todo

    
	led0_detect_enable = 90,
	led0_detect_disable = 91,
	led1_detect_enable = 92,
	led1_detect_disable = 93,
	HEAT_TO_PREHEAT = 94,
	HEAT_TO_MEASURE = 95,
	HEAT_TO_DIG = 96,
	HEAT_TO_STEP2 = 97,
	COOLING_TO_DRAIN = 98,
	DEVICE_DIG_INITIALIZE_COOLING = 99,
	MEASURE_STEP = 100,
	BLANK_STEP = 101,
	PRE_COOLING_BEFORE_SAMPLING = 102,
	PUMP_RESTORE_CHECK = 103,
	HEAT_TO_CLEAN = 104,
	COOLING_BEFORE_IDLE = 105,
    VALVE_RESET_MPP = 106,
    VALVE_RESET_HPP = 107,
    REMOVE_BURBLE_EX_LED0 = 108,
    //108
    REMOVE_BURBLE_LED0 = 109,

    REMOVE_BURBLE_LED1 = 110,
        

    HEAT_INCREASE_DETECT = 111,
    MEASURE_TEMP_HOLD = 111,
    SENSOR_GAIN_UPDATE = 112,
	UPDATE_CALI_BLANK = 113,
	UPDATE_CALI_SLOPE = 114,
    SP_IDLE_SCT = 115,
    CHK_LEAKAGE_DISABLE = 116,
    CHK_SAFETY_LOCK_ENABLE = 117,
    CHK_SAFETY_LOCK_DISABLE = 118,
    CHK_LEAKAGE_ENABLE = 119,
   
    e_sp_ctrl_end = 119,    
    e_micro_step_end = 119,
    e_pre_sub_start = 120,
	e_valve_initialize = 128,
	e_pump_initialize = 130,
    e_pre_sub_end = 139,

	e_sub_step_start = 140,
	//todo
	to_dig_so4 = 140,

	e_sub_step_end = 209,
    e_pre_main_start = 210,
    e_pre_main_end = 229,
	e_main_step_start = 230,
	//todo
	e_main_calibration = 230,

	e_main_step_end = 250,


}FLOW_PROCESS;

#define e_max_valve  		(e_valve_end - e_valve_start + 1)
#define e_max_pump  		(e_pump_end - e_pump_start + 1)
#define e_max_timedelay  	(e_timedelay_end - e_timedelay_start + 1)
#define e_max_sp  			(e_sp_ctrl_end - e_sp_ctrl_start + 1)
#define e_max_substep		(e_sub_step_end - e_sub_step_start + 1)
#define e_max_mainstep		(e_main_step_end - e_main_step_start + 1)
#define e_max_presub		(e_pre_sub_end - e_pre_sub_start + 1)
#define e_max_premain		(e_pre_main_end - e_pre_main_start + 1)

typedef struct
{
	uint16_t cmd;
	char* str;
}IIC_CMD_INFO;

extern const IIC_CMD_INFO cmdInfo[e_cmd_id_max];
extern uint16_t cmdDelay[e_cmd_id_max];


typedef enum
{
    ACT_NONE = 0,
    RETRY_CURR = 0x1000,
    RETRY_SUB = 0x2000,
    RETRY_MAIN = 0x4000,
    FAILED = 0x8000,
    LOG_FLASH = 0x0100,
    JMP_TO = 0x0200,
    FIN_STEP = 0x0400,
}ER_ACT;

typedef enum
{
	END_POS = 0x01,
	LED0_CHK = 0x02,
	LED1_CHK = 0x03,

	LED0_POS = 0x04,
	LED1_POS = 0x05,
	FREE_POS = 0x06,
	STOP_ACT = 0x07,
    E_POS_MASK =  0x0F,
	E_POS_NON_MASK = 0xF0,
}E_Pos;
#define DELAY_CFG_EN      0x80
#define DELAY_CFG_EN_MSK      0x7F
typedef enum
{
	PULL = 0x00,
	PUSH = 0x80,
	E_DIR_MASK = 0x80,
}E_Dir;

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


typedef struct
{
    uint16_t timeExpect;
    uint16_t errAction;
}SPAction;

#define PRE_MAIN_STEP_MAX  50
#define MAIN_STEP_MAX  40
#define SUB_STEP_MAX   22
#define PRESUB_STEP_MAX 30
#define PUMP_TIME_MAX		40000//10seconds
#define SP_TIME_MAX			41000//20seconds

extern uint8_t mainSteps[e_max_mainstep][MAIN_STEP_MAX];
extern uint8_t subSteps[e_max_substep][SUB_STEP_MAX];
extern uint8_t preMainSteps[e_max_premain][PRE_MAIN_STEP_MAX];
extern uint8_t preSubSteps[e_max_presub][PRESUB_STEP_MAX];
extern PumpCfg pumpCfg[e_max_pump];
extern uint16_t timeDelay[e_max_timedelay];
extern SPAction spAction[e_max_sp];

extern const T_UNIT flowCfg;

uint16_t Initialize_FlowCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_FlowCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_FlowCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
const char* GetStepName(uint16_t chn);

uint32_t GetDuringMs_MicroStep(uint16_t subStep);
#endif /* START_BOARDSUPPORT_RSK_R32C111_APP_UNIT_FLOW_CFG_H_ */
