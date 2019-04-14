/*
 * unit_weld_cfg.h
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_WELD_CFG_H_
#define SUBSYSTEM_UNIT_WELD_CFG_H_

#define	MAX_SEG_SIZE 20
#define	MAX_CALI_CURR 20



#define WELD_DELAY_TIME				10
#define HOME_DELAY_TIME				60000

#define SCH_DELAY_TIME				50
#define MOTOR_SPEED_UPDATE_TIME		25
#define JOG_TIME					200
#define MOTOR_CTRL_TIME 			50
#define MOTOR_WELD_CYC_TIME 		10
#define SCH_DELAY_TIME				50





typedef struct
{
	float homeSpeed;
	float jogSpeed;
	float accSpeedPerSeond;
}MotorSpeed;


typedef struct
{
	uint16_t preGasTime;
	uint16_t postGasTime;
	uint16_t preCurr;
	uint16_t preDelay;
	uint16_t upSlopeAcc;
	uint16_t upSlopeAccTime;
	uint16_t downSploeAcc;
	uint16_t downSlopeAcc;
}WeldProcessCfg;

enum
{
	CHN_DA_CURR_OUT,
	CHN_DA_SPEED_OUT,
	CHN_DA_MAX,
};

enum
{
	CHN_CURR_READ,
	CHN_VOLT_READ,
	CHN_REV_0,
	CHN_REV_1,
	CHN_AD_MAX,
};

enum
{
	CHN_OUT_MOTOR_DIR,
	CHN_OUT_ARC_ON,
	CHN_OUT_WATER,
	CHN_OUT_GAS,
	CHN_OUT_AD_CUT,
	CHN_OUT_MAX=16,
};

enum
{
	CHN_IN_JOG_DIR,
	CHN_IN_JOG_ACT,
	CHN_IN_GAS_ON,
	CHN_IN_STARTSTOP,
	CHN_IN_FAULT_MOTOR,
	CHN_IN_MAX,
};
#define CURR_DETECT_LIMIT  30.0f

enum
{
	BTN_UNKNOWN = 1,
	BTN_PUSHDOWN = 2,
	BTN_RELEASE = 4,

	BTN_CHKED = 0x8000,
};





extern const SegWeld* 	ptrCurrWeldSeg;
extern float 			currConvertRation;
extern float 			speedAdjust;
extern float  		currMicroAdjust;


extern WeldProcessCfg 	weldProcess;
extern MotorSpeed 		motorSpeedSet  ;
extern uint16_t 		weldDir;
extern volatile int32_t  motorPos_Read;
extern float 	 	motorSpeed_Read;
extern float     	weldCurr_Read;
extern float 	  	weldVolt_Read;

extern float   		ang2CntRation;
extern int32_t  	motorPos_WeldStart;
extern int32_t  	motorPos_WeldFinish;
extern uint16_t  	weldState;
extern int32_t 		motorPosHome;


extern float 	adcValue[CHN_AD_MAX];
extern float 	daOutputSet[CHN_DA_MAX];
extern uint16_t daOutputRawDA[CHN_DA_MAX];

extern uint32_t 	digitOutput;
extern uint32_t  	digitInput;
extern uint32_t  	digitInputWeldBtn;
extern int32_t		lastMotorPos_PowerDown;


extern uint16_t uiBtn_Weld;
extern uint16_t uiBtn_JogP;
extern uint16_t uiBtn_JogN;
extern uint16_t voltCaliReq;
extern uint16_t currCaliReq;
extern uint16_t speedCaliReq;
extern uint16_t caliAllReq;


extern float currCaliSetDiff;



extern uint16_t   	devLock;
extern uint16_t  	daOutputPwm[2];
extern uint16_t  	daOutputPwmTime[2];

extern CaliSpeed 	speedCaliPoint[2];


extern const T_UNIT weldCfg;

float GetSpeedDuty(float speed);
float GetWeldSpeed(int32_t cnt);
uint32_t GetWeldFinishPos(uint16_t id);
void UpdateWeldFInishPos(void);
const SegWeld* GetWeldSeg(int32_t cnt);
float GetWeldSegSpeed(uint16_t id);

uint16_t Initialize_WeldCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_WeldCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
                     void * ptrValue);
uint16_t GetInputState(uint16_t chn);

#define WeldPut(a,b,c)	Put_WeldCfg(&weldCfg, a, b,c);


float GetSpeedCtrlOutput(float speed);
float GetCurrCtrlOutput(float curr);
float GetVoltRead(float curr);
float GetCurrRead(float curr);

enum
{
	OBJ_IDX_OUTPUTDA,
	OBJ_IDX_OUTPUTDO,
	OBJ_IDX_OUTPUTAD,
	OBJ_IDX_INPUT_DI,
	OBJ_IDX_VOLT0_CALI = 10,
	OBJ_IDX_VOLT1_CALI = 11,
	OBJ_IDX_SPEED_RATION = 14,
	OBJ_IDX_CURR_CALI_NEW = 20,
	OBJ_IDX_CURR_CALI_CLR = 22,

};
#endif /* SUBSYSTEM_UNIT_WELD_CFG_H_ */
