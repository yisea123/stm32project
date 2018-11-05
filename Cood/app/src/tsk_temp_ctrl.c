/*
 * tsk_temp_ctrl.c
 *
 *  Created on: 2016锟斤拷12锟斤拷1锟斤拷
 *      Author: pli
 */




#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "unit_meas_cfg.h"
#include "unit_meas_data.h"
#include "t_data_obj_pid.h"
#include "unit_flow_cfg.h"
#include "unit_temp_cfg.h"
#include "unit_sys_diagnosis.h"
#include "dev_temp.h"
#include "tsk_temp_ctrl.h"
#include "gpio.h"
#include "unit_flow_act.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim4;


enum
{
	 HEAT_READY,
	 HEAT_NOT_READY,
};

#define TARGET_TEMP			3850
#define DIFF_TARGET_TEMP	500
#define TEMP_PWM_MAX		5600
#define TEMP_PWM_MIN		0
#define TEMP_DIFF_MAX		450
#define TEMP_TOL_MAX		600

#define HEAT_NO_ADJUST			    0x00
#define HEAT_NEEDS_SMALLER			0x01
#define HEAT_NEEDS_STRONGER			0x02

#define HEAT_NEEDS_STRONGER			0x00
#define HEAT_NEEDS_SMALLER			0x01

#define USE_PID					0x01
#define TEMP_TSK_CYCLE			50

#define MAX_DELAY_TIME			(uint16_t)(1500/TEMP_TSK_CYCLE)

#define MAX_DIFF_TIME		0


static uint16_t temp_PWM = 0 ;
osThreadId tempTaskHandle = NULL;


static void OutPutPWM(uint16_t period, uint32_t chn)
{
	htim4.Instance->ARR = (uint32_t)(TEMP_PWM_MAX - 1);
	if(chn == TIM_CHANNEL_1)
		htim4.Instance->CCR1 = period;
	else if(chn == TIM_CHANNEL_2)
		htim4.Instance->CCR2 = period;
	else if(chn == TIM_CHANNEL_3)
		htim4.Instance->CCR3 = period;
	else if(chn == TIM_CHANNEL_4)
		htim4.Instance->CCR4 = period;
	else
	{}
	htim4.Instance->EGR = TIM_EGR_UG;
	HAL_TIM_PWM_Start(&htim4,chn);
}



static void TempCtrlPwmOut(uint16_t pwmOut)
{
	temp_PWM = pwmOut;
	if(pwmOut)
	{
	//	OutPutPWM(pwmOut,TIM_CHANNEL_1);
		OutPutPWM(pwmOut,TIM_CHANNEL_2);
	}
	else
	{
	//	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);
	//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(TEMP1_CTR_GPIO_Port, TEMP1_CTR_Pin, GPIO_PIN_RESET);
	}
}



#define	TICK_HEAT_TIME  	75
#define OFFSET_JUDGE		150 //1.5度

typedef enum
{
	HEAT_IDLE,
	HEAT_INIT,
	HEAT_TO_TEMP,
	HEAT_TO_TEMP_DELAY,
	HEAT_TO_TEMP_PID,
	COOL_TO,
	COOL_TO_DELAY,
	TEMP_IS_REACHED,
	TEMP_IS_REACHED_DELAY,
	HEAT_FINISH,
	HEAT_FINISH_ER,
}HEAT_STATE;


static const char* taskStateDsp[] =
{
	TO_STR(HEAT_IDLE),
	TO_STR(HEAT_INIT),
	TO_STR(HEAT_TO_TEMP),
	TO_STR(HEAT_TO_TEMP_DELAY),
	TO_STR(HEAT_TO_TEMP_PID),
	TO_STR(COOL_TO),
	TO_STR(COOL_TO_DELAY),
	TO_STR(TEMP_IS_REACHED),
	TO_STR(TEMP_IS_REACHED_DELAY),
	TO_STR(HEAT_FINISH),
	TO_STR(HEAT_FINISH_ER),
};


static const char* heat_mode_dsp[] =
{
	TO_STR(HEAT_MODE_IDLE),
	TO_STR(HEAT_MODE_HEAT_HOLD),
	TO_STR(HEAT_MODE_HEAT_TO),
	TO_STR(HEAT_MODE_COOL_HOLD),
	TO_STR(HEAT_MODE_COOL_TO),
    TO_STR(HEAT_MODE_STOP),
};

typedef struct  {
	uint8_t dutyCycle; //占空比
	uint8_t heatTime; //以0.5秒为单位
}HeatCtrl;




enum {
	Heat,
	Cool,
	CoolHold,
	AllStop,
};




#define TARGET_TEMP			3850
#define DIFF_TARGET_TEMP	500
#define TEMP_PWM_MIN		0



#define PWM_ACT_RANGE_MAX   5000
#define PWM_ACT_RANGE_MIN   0

#define USE_PID					0x01
#define TEMP_TSK_CYCLE			50

#define MAX_DELAY_TIME			(uint16_t)(1500/TEMP_TSK_CYCLE)

#define MAX_DIFF_TIME		0



#pragma pack(push)
#pragma pack(2)

typedef struct PID
{
	double SetPoint; //闁跨喎锟界喎鐣鹃惄顕?鏁撻弬銈嗗 Desired Value
	double SumError; //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规總鍌炴晸閿燂拷
	double LastError; //Error[-1]
	double PrevError; //Error[-2]
	double sumInc;
	double offset;
	double sumOut;
	double sumCount;
	float Proportion; //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 Proportional Const
	float Integral; //闁跨喐鏋婚幏鐑芥晸鐞涙顒查幏鐑芥晸閺傘倖瀚? Integral Const
	float Derivative; //瀵邦噣鏁撶悰妤冾劜閹风兘鏁撻弬銈嗗 Derivative Const
	uint16_t timeCount;
	uint16_t timeDelay;
}PID;

#pragma pack(pop)


/*====================================================================================================
PID Function
The PID function is used in mainly
control applications. PIDCalc performs one iteration of the PID
algorithm.
While the PID function works, main is just a dummy program showing
a typical usage.
=====================================================================================================*/
extern PID_VAL   sPID[4];
static PID_VAL* pidCfg = &sPID[0];
static PID pidLoc;
static PID *sptr = &pidLoc;
static const int16_t OFFSET_CHANGE = 200;
extern PID_VAL pidUsed ;


uint32_t heatCountId = 0;

const PID_VAL pidVal[] =
{
	{2.5f, 0.15f, 0.5f,15,3000,},//175
	{2.5f, 0.15f, 0.36f,15,2700,},//170
	{2.4f, 0.15f, 0.35f,15,2500,},//165
	{2.3f, 0.15f, 0.35f,15,2500,},//160
	{2.2f, 0.15f, 0.35f,15,2500,},//155
	{2.1f, 0.15f, 0.35f,15,2500,},//150
	{2.0f, 0.15f, 0.35f,15,2400,},//145
	{1.9f, 0.15f, 0.35f,15,2400,},//140
	{1.8f, 0.15f, 0.35f,15,2400,},//135
	{1.7f, 0.15f, 0.35f,15,2400,},//130
	{1.65f, 0.15f, 0.35f,15,2400,},//125
	{1.6f, 0.15f, 0.35f,15,2400,},//120
	{1.55f, 0.15f, 0.35f,15,2400,},//115
	{1.5f, 0.15f, 0.45f,15,2300,},//110
	{1.65f, 0.15f, 0.45f,15,2300,},//105
	{1.6f, 0.15f, 0.45f,15,1200,},//100
	{1.56f, 0.15f, 0.45f,15,1100,},//95
	{1.54f, 0.15f, 0.45f,15,1000,},//90
	{1.45f, 0.15f, 0.35f,15,1000,},//85
	{1.45f, 0.15f, 0.35f,15,1000,},//80
	{1.45f, 0.15f, 0.35f,15,1000,},//75
	{1.45f, 0.15f, 0.35f,15,1000,},//70
	{1.45f, 0.15f, 0.35f,15,1000,},//65
	{1.45f, 0.15f, 0.35f,15,1000,},//60
	{1.45f, 0.15f, 0.35f,15,1000,},//55

};
/*====================================================================================================
Initialize PID Structure PID
=====================================================================================================*/
static uint16_t tempEnv_Fix = 4000;

void PIDInit(int32_t tempReq, uint8_t i)
{
	static uint16_t count_i = 0;
	assert(sptr);
	int32_t tempEnv = 2800;


	if(tempEnv > 11000)
		tempEnv = tempEnv_Fix;
    if(tempReq != 0)
        sptr->SetPoint = tempReq;

    uint16_t idx = (17500 - sptr->SetPoint)/500;
    if(sptr->SetPoint >= 15000)
    {
    	pidCfg = &sPID[0];
    }
    else if(sptr->SetPoint >= 9500)
    {
		pidCfg = &sPID[1];
    }
    else if(sptr->SetPoint >= 7500)
    {
        pidCfg = &sPID[2];
    }
    else
    {
        pidCfg = &sPID[3];
    }    
	//TraceMsg(TSK_ID_HEAT, "Pid:%.2f,%.2f,%.2f Offset:%d,\n",pidCfg->Proportion,\
	//		pidCfg->Integral, pidCfg->Derivative,pidCfg->offset);
	if(i==HEAT_NEEDS_SMALLER)//out cause final output bigger
	{
		if(sptr->sumCount > 3)
		{
			count_i = 0;

			pidCfg->offset = (int16_t)(pidCfg->offset - OFFSET_CHANGE);
			if (sptr->sumCount > 100)
				if(pidCfg->offset >= sptr->sumOut/sptr->sumCount)
				{
					pidCfg->offset = (int16_t)(sptr->sumOut/sptr->sumCount);

				}
			if(pidCfg->Proportion > 1.0)
			    pidCfg->Proportion = pidCfg->Proportion - 0.05;
			Trigger_EEPSave((void*)pidCfg, sizeof(PID_VAL), SYNC_CYCLE);

		//	TraceMsg(TSK_ID_TEMP_CTRL, "Adjust PID offset: PWM: %d %d, dir:%d;\t Offset:%d,\n",temp_PWM,tempReq, i,pidCfg->offset);
		}
	}
	else
	{

		if(sptr->sumCount > 3)
		{
			count_i = 0;
			pidCfg->offset= (int16_t)(pidCfg->offset + OFFSET_CHANGE);
			if (sptr->sumCount > 100)
				if(pidCfg->offset <= sptr->sumOut/sptr->sumCount)
				{
					pidCfg->offset = (int16_t)(sptr->sumOut/sptr->sumCount);

				}
			if(pidCfg->Proportion < 3.0)
				pidCfg->Proportion = pidCfg->Proportion + 0.05;
			Trigger_EEPSave((void*)pidCfg, sizeof(PID_VAL), SYNC_CYCLE);

		//	TraceMsg(TSK_ID_TEMP_CTRL, "Adjust PID offset: PWM:%d; %d, dir:%d;\t Offset: %d,\n",temp_PWM, tempReq, i,pidCfg->offset);
		}
	}
	sptr->timeCount = 0;
	sptr->SumError = 0;
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2]
	sptr->sumInc = 0;
	sptr->sumOut = 0;
	sptr->sumCount = 0;
	sptr->timeDelay = pidCfg->timeDelay;
	sptr->offset = pidCfg->offset;
	sptr->Proportion =  pidCfg->Proportion; //Proportional Const
	sptr->Integral = pidCfg->Integral; //Integral Const
	sptr->Derivative = pidCfg->Derivative; //Derivative Const
//	if(count_i++ % 10 == 1)
//		TraceMsg(TSK_ID_TEMP_CTRL, "Adjust Req: pwm:%d; %d, dir:%d;\t Offset: %.0f,\tP:%.3f, I:%.3f, D:%.3f\n",temp_PWM, tempReq, i,sptr->offset, sptr->Proportion, sptr->Integral, sptr->Derivative);

    heatCountId = 0;
    pidUsed = *pidCfg;
}

static float bias = 0;
static float Pwm = 0;
static float Integral_bias = 0;
static float Last_Bias = 0;

#define RANGE_0  17500
#define RANGE_1  9800
#define RANGE_2  5000

void Position_PID_Init(uint16_t target)
{
    bias = 0;
    Pwm = 0;
    Integral_bias = 0;
    Last_Bias = 0;
    if ( target != 0)
        sptr->SetPoint = target;
    
    if( sptr->SetPoint > RANGE_1 )
    {
        float ratio = (float)(target - RANGE_1)/(float)(RANGE_0 - RANGE_1);
        pidUsed.Proportion = (sPID[0].Proportion - sPID[1].Proportion)* ratio + sPID[1].Proportion;
        pidUsed.Integral = (sPID[0].Integral - sPID[1].Integral)* ratio + sPID[1].Integral;
        pidUsed.Derivative = (sPID[0].Derivative - sPID[1].Derivative)* ratio + sPID[1].Derivative;
        pidUsed.offset = (sPID[0].offset - sPID[1].offset)* ratio + sPID[1].offset;
    }
    else if( sptr->SetPoint >= RANGE_2 )
    {
        float ratio = (float)(target - RANGE_2)/(float)(RANGE_1 - RANGE_2);
        pidUsed.Proportion = (sPID[1].Proportion - sPID[2].Proportion)* ratio + sPID[2].Proportion;
        pidUsed.Integral = (sPID[1].Integral - sPID[2].Integral)* ratio + sPID[2].Integral;
        pidUsed.Derivative = (sPID[1].Derivative - sPID[2].Derivative)* ratio + sPID[2].Derivative;
        pidUsed.offset = (sPID[1].offset - sPID[2].offset)* ratio + sPID[2].offset;
    }       
    else if( sptr->SetPoint >= RANGE_2 )
    {
        float ratio = (float)(target - RANGE_2)/(float)(RANGE_1 - RANGE_2);
        pidUsed.Proportion = (sPID[2].Proportion - sPID[3].Proportion)* ratio + sPID[3].Proportion;
        pidUsed.Integral = (sPID[2].Integral - sPID[3].Integral)* ratio + sPID[3].Integral;
        pidUsed.Derivative = (sPID[2].Derivative - sPID[3].Derivative)* ratio + sPID[3].Derivative;
        pidUsed.offset = (sPID[2].offset - sPID[3].offset)* ratio + sPID[3].offset;
    }        
    else
    {
        pidUsed = sPID[3];
    }   
    pidCfg = &pidUsed;
}
static int16_t Position_PID (int16_t act,int16_t target)
{
    bias = target - act; //计算偏差
    Integral_bias += bias; //求出偏差的积分
    Pwm = pidCfg->Proportion*bias + pidCfg->Integral*Integral_bias + pidCfg->Derivative*(bias-Last_Bias) + pidCfg->offset; //位置式PID控制器
    Last_Bias = bias; //保存上一次偏差
    return Pwm; //增量输出
}



static int16_t PIDCalc(int32_t NextPoint)
{
	double dError, Error;

	Error = sptr->SetPoint - NextPoint;          // 閸嬪繘鏁撻弬銈嗗
	sptr->SumError += Error;                      // 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
	dError = sptr->LastError - sptr->PrevError;     // 闁跨喐鏋婚幏宄板瀵邦噣鏁撻弬銈嗗
	sptr->PrevError = sptr->LastError;
	sptr->LastError = Error;
	sptr->sumInc = (sptr->Proportion * Error              // 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
	+ sptr->Integral * sptr->SumError         // 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
	+ sptr->Derivative * dError             // 瀵邦噣鏁撻弬銈嗗闁跨喐鏋婚幏锟?
	);

	dError = sptr->sumInc + sptr->offset;

	if (dError < 0)
	{
		sptr->SumError -= Error;
		sptr->sumInc = -sptr->offset;
		dError = 0;
	}
	sptr->sumOut += dError;
	sptr->sumCount += 1.0;
	if(sptr->sumCount > 300)
	{
		int16_t offset = (int16_t)(sptr->sumOut/sptr->sumCount);
		if(abs(pidCfg->offset - offset)> 20)
		{
			pidCfg->offset = offset;
			Trigger_EEPSave((void*)&pidCfg->offset, sizeof(pidCfg->offset), SYNC_CYCLE);
		}
		sptr->SumError = 0;
		sptr->LastError = 0; //Error[-1]
		sptr->PrevError = 0; //Error[-2]
		sptr->sumInc = 0;
		sptr->sumOut = 0;
		sptr->sumCount = 0;
		sptr->timeDelay = pidCfg->timeDelay;
		sptr->offset = pidCfg->offset;
	}
	return (int16_t)(dError);
}






static int16_t PWMCalc(int16_t tempAct, int16_t tempReq)
{
	static int16_t 	pwmOut = TEMP_PWM_MIN;
	sptr->SetPoint = tempReq;

	if(tempAct <= (tempReq-(int16_t)TEMP_TOL_MAX))
	{
		sptr->timeCount = 0;
		pwmOut = TEMP_PWM_MAX;
		//PIDInit(tempReq, HEAT_NEEDS_STRONGER);
        Position_PID_Init(tempReq);
	}
	else if(tempAct >= (tempReq+(int16_t)TEMP_DIFF_MAX))
	{
		sptr->timeCount = 0;
		pwmOut = TEMP_PWM_MIN;
		//PIDInit(tempReq, HEAT_NEEDS_SMALLER);
        Position_PID_Init(tempReq);
	}
	else
	{
		sptr->timeCount++;
		if(sptr->timeCount%sptr->timeDelay == 1)
		{
			//pwmOut = PIDCalc((int32_t)tempAct);
            pwmOut = Position_PID(tempAct, tempReq) + 50;
            if ( pwmOut < 0)
                pwmOut = 0;
            if( pwmOut >= 8500)
                pwmOut = 8500;
		}
	}
	return pwmOut;
}


#define WARNING_OUT_CTRL		300//
#define MAX_HEAT_COUNT			3000//180s?
#define ERROR_SENSOR_VAL_BIG	8000
#define ERROR_SENSOR_VAL_SMALL	0



uint16_t GetHeatCtrl_PID(int16_t cur_temp, int16_t target_temp)
{
    static uint16_t x = 0;


    x = PWMCalc((int16_t)(cur_temp), target_temp);

    if(PWM_ACT_RANGE_MAX <= x)
        x = PWM_ACT_RANGE_MAX;
    if(x == PWM_ACT_RANGE_MIN)
        x = 0;
    return x;
}

static uint16_t currentDigTemp = 0;
void AvgDIgTemperature(void)
{
#define AVG_MAX_T 8
	static uint16_t avgData[AVG_MAX_T];
	static uint16_t id = 0;
	uint32_t total = 0;
	GetAllTemperature();
    avgData [(id++)%AVG_MAX_T]= GetTempChn(TARGET_CHN);
    for(uint16_t idx = 0; idx< AVG_MAX_T;idx++)
    {
    	total += avgData[idx];

    }
    currentDigTemp = total/AVG_MAX_T;
}


static uint16_t HeatCtrlOutput(int16_t mode, int16_t target, uint16_t* ptrState)
{
	//
#define COOL_FAN 			300

	static uint16_t heatMode = 0xFFFF;
    static uint16_t _count = 0;
	uint16_t currTemp = currentDigTemp;
	uint16_t tickOut = TICK_HEAT_TIME;
	assert(ptrState);
    *ptrState = HEAT_NOT_READY;
	switch(mode)
	{
		case Heat:
		{
			if(heatMode != Heat)
			{
				CoolFanCtrl(0);
			}
			if(currTemp >= (target - OFFSET_JUDGE))
				*ptrState = HEAT_READY;
            uint16_t x = GetHeatCtrl_PID(currTemp, target);
            TempCtrlPwmOut(x);
            tickOut = TICK_HEAT_TIME;
            if((_count++) % 13 == 0)
            {
                TracePrint(TSK_ID_TEMP_CTRL,"Count, %d Heat Mode, curr, %d, Req, %d; ctrl, duty, %d\n", \
                    heatCountId, currTemp, target, x);
                heatCountId++;
            }
		}
		break;
		case Cool:
		{
			if(currTemp <= (target + OFFSET_JUDGE))
				*ptrState = HEAT_READY;

			if(currTemp > target)
            {
				CoolFanCtrl(1);
				TempCtrlPwmOut(0);
            }
			else
            {
				CoolFanCtrl(0);
            }
            TracePrint(TSK_ID_TEMP_CTRL,"Count, %d Cool Mode, curr, %d, Req, %d; \n", \
                    heatCountId, currTemp, target);
            heatCountId++;
            tickOut = 1000;
		}
		break;
		default:
			TempCtrlPwmOut(0);
			CoolFanCtrl(0);
            heatCountId = 0;
			*ptrState = HEAT_READY;
			break;
	}

	heatMode = mode;
	if(tickOut < TICK_HEAT_TIME)
		tickOut = TICK_HEAT_TIME;

	return tickOut;
}
static TSK_MSG  localMsg;
void StartTempCtrlTask(void const * argument)
{
	(void)argument; // pc lint

	uint32_t tickOut = 10;
    const uint16_t taskID = TSK_ID_TEMP_CTRL;
    HEAT_STATE tskState;
    osEvent event;
    uint16_t reached = 0;
    uint16_t targetTemp = 0;
    uint32_t delayForStable = 2000;
    uint16_t bufferTemp = 0;
    HeatMsg heatMsg;
	while(1)
	{
		event = osMessageGet(TEMPERATURE_ID, tickOut);
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		ctrlTemp = targetTemp;
        AvgDIgTemperature();
        if (event.status != osEventMessage)
		{
			switch(tskState)
			{
			case HEAT_TO_TEMP_DELAY:

				tickOut = HeatCtrlOutput(Heat, targetTemp-bufferTemp, &reached);
				if(reached == HEAT_READY)
				{
					tskState = TEMP_IS_REACHED;
					SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
				}
				break;
			case COOL_TO_DELAY:
				tickOut = HeatCtrlOutput(Cool, targetTemp+bufferTemp, &reached);

				if(reached == HEAT_READY)
				{
					tskState = TEMP_IS_REACHED;
					SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
				}
				break;

			case HEAT_TO_TEMP_PID:
				tskState = HEAT_FINISH;
				if ((heatMsg.detail.mode == HEAT_MODE_HEAT_HOLD)
						|| (HEAT_MODE_COOL_HOLD == heatMsg.detail.mode))
				{
					tskState = HEAT_TO_TEMP_PID;
                    tickOut = HeatCtrlOutput(Heat, targetTemp, &reached);
				}
				else
				{
					SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
				}
				break;
			case TEMP_IS_REACHED_DELAY:
				if((heatMsg.detail.mode == HEAT_MODE_HEAT_HOLD) || (HEAT_MODE_COOL_HOLD == heatMsg.detail.mode))
				{
					tskState = HEAT_TO_TEMP_PID;
					if(localMsg.callBackUpdate)
					{
						 localMsg.callBackUpdate(OK,0x0);
						 localMsg.callBackUpdate = NULL;
					}
					tickOut = TICK_HEAT_TIME;
				}
				else
				{
					tskState = HEAT_FINISH;
					SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
				}

				break;
			default:
				//error
                tskState = HEAT_IDLE;
                tickOut = TICK_HEAT_TIME;
				break;

			}

		}
		else
		{
			localMsg = *TSK_MSG_CONVERT(event.value.p);
			const TSK_STATE mainTskState = localMsg.tskState;
			TracePrint(taskID, "%s, %d,\t%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);

			UnuseTskMsg( TSK_MSG_CONVERT(event.value.p) );
			tickOut = osWaitForever;
			if(mainTskState == TSK_FORCE_BREAK)
			{
				//do break action
				//todo
				heatMsg.heatMsg_Val = localMsg.val.value;
                targetTemp = 0;
				HeatCtrlOutput(AllStop, targetTemp, &reached);
				tskState = HEAT_FINISH;
				SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
				//do reset action
				//todo
                targetTemp = 0;
                heatMsg.heatMsg_Val = localMsg.val.value;
				HeatCtrlOutput(AllStop, targetTemp, &reached);
				tskState = HEAT_FINISH;
				SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
			}
			else if(TSK_RENEW_STATUS == mainTskState)
			{
				//renew status;
			}
			else if(mainTskState == TSK_INIT)
			{
				if(tskState != HEAT_IDLE)
				{
					//do nothing;

				}
				TraceMsg(taskID, "new tempctrl is called, %s, %d,\t%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);
				//this function could be trigger always by other task;
				{
					heatMsg.heatMsg_Val = localMsg.val.value;
					tskState = HEAT_INIT;
					reached = HEAT_NOT_READY;
                    targetTemp = heatMsg.detail.val;
                    SendTskMsgLOC(TEMPERATURE_ID, &localMsg);

				}
			}
			else if(mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
				case HEAT_IDLE:
					break;
				case HEAT_INIT:
                    PIDInit(heatMsg.detail.val, HEAT_NO_ADJUST);
                    Position_PID_Init(heatMsg.detail.val);
					TracePrint(taskID,"%3d, %s to temperature, %d\n", heatMsg.detail.mode, heat_mode_dsp[heatMsg.detail.mode], heatMsg.detail.val);
					if((heatMsg.detail.mode == HEAT_MODE_HEAT_HOLD) || (HEAT_MODE_COOL_HOLD == heatMsg.detail.mode))
					{
						if(GetTempChn(TARGET_CHN) < (heatMsg.detail.val + 400))
						{
							heatMsg.detail.mode = HEAT_MODE_HEAT_HOLD;
						}
					}
					TracePrint(taskID,"%3d, updated, %s to temperature, %d\n", heatMsg.detail.mode, heat_mode_dsp[heatMsg.detail.mode], heatMsg.detail.val);

					if((heatMsg.detail.mode == HEAT_MODE_HEAT_HOLD) || (heatMsg.detail.mode == HEAT_MODE_HEAT_TO) )
					{
						tskState = HEAT_TO_TEMP;
					}
					else if( (HEAT_MODE_COOL_HOLD == heatMsg.detail.mode) || (HEAT_MODE_COOL_TO == heatMsg.detail.mode) )
					{
						tskState = COOL_TO;
					}
					else
					{
						//stop cool fan,
						//stop heat;
                        targetTemp = 0;
						tskState = HEAT_FINISH;
						tickOut = HeatCtrlOutput(AllStop, 0, &reached);
					}
					delayForStable = 1000;
					bufferTemp = 300;
					if(HEAT_MODE_COOL_HOLD == heatMsg.detail.mode)
					{
						

                        if(GetTempChn(TARGET_CHN) > (1000 + heatMsg.detail.val))
                        {
                            bufferTemp = (bufferTemp * 2);
                            delayForStable = delayForStable*2;
                        }
                        else if( GetTempChn(TARGET_CHN) <= bufferTemp + heatMsg.detail.val)
                        {
                            tskState = TEMP_IS_REACHED_DELAY;
                        }
                        else if( GetTempChn(TARGET_CHN) < heatMsg.detail.val)
                        {
                            TracePrint(taskID,"%3d, %s to temperature, %d, Err, req temp is below target temp, change to use HEAT_MODE_HEAT_HOLD \n", heatMsg.detail.mode, heat_mode_dsp[heatMsg.detail.mode], heatMsg.detail.val);
                            heatMsg.detail.mode = HEAT_MODE_HEAT_HOLD;
                            
                            tskState = HEAT_TO_TEMP;
                        }
					}
					else if(HEAT_MODE_HEAT_TO == heatMsg.detail.mode)
					{
				//		delayForStable = delayForStable;
				//		bufferTemp = bufferTemp;
					}
					else if(heatMsg.detail.mode == HEAT_MODE_HEAT_HOLD)
					{
				//		delayForStable = delayForStable;
				//		bufferTemp = bufferTemp;
					}
					else
					{
						delayForStable = 0;
						bufferTemp = 0;
					}
					SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
					break;
				case COOL_TO:
                    heatCountId = 0;
					tskState = COOL_TO_DELAY;
					tickOut = 0;
					break;
				case HEAT_TO_TEMP:
                    heatCountId = 0;
					tskState = HEAT_TO_TEMP_DELAY;
					tickOut = 0;
					break;

				case TEMP_IS_REACHED:
                    heatCountId = 0;
					HeatCtrlOutput(AllStop, 0, &reached);
					tickOut = delayForStable;
					tskState = TEMP_IS_REACHED_DELAY;
                    PIDInit(heatMsg.detail.val, HEAT_NO_ADJUST);
                    Position_PID_Init(heatMsg.detail.val);
					break;
				case TEMP_IS_REACHED_DELAY:
					if((heatMsg.detail.mode == HEAT_MODE_HEAT_HOLD) || (HEAT_MODE_COOL_HOLD == heatMsg.detail.mode))
					{
						tskState = HEAT_TO_TEMP_PID;
						TSK_FINISH_ACT(&localMsg,taskID, OK, OK);
						tickOut = TICK_HEAT_TIME/2;
					}
					else
					{
						tskState = HEAT_FINISH;
						SendTskMsgLOC(TEMPERATURE_ID, &localMsg);
					}
                    heatCountId = 0;

					break;

				case HEAT_FINISH_ER:
					TSK_FINISH_ACT(&localMsg,taskID, FATAL_ERROR, OK);
					tskState = HEAT_IDLE;
					tickOut = TICK_HEAT_TIME;
					break;

				case HEAT_FINISH:
					TSK_FINISH_ACT(&localMsg,taskID, OK, OK);
					tskState = HEAT_IDLE;
					tickOut = TICK_HEAT_TIME;
					break;

				}
			}
		}

	}
}

