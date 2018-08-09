/*
 * tsk_temp_ctrl.c
 *
 *  Created on: 2016闁跨喐鏋婚幏锟�12闁跨喐鏋婚幏锟�1闁跨喐鏋婚幏锟�
 *      Author: pli
 */




#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "unit_meas_cfg.h"
#include "t_data_obj_pid.h"
#include "unit_flow_cfg.h"
#include "unit_temp_cfg.h"
#include "unit_sys_diagnosis.h"
#include "dev_temp.h"
#include "tsk_temp_ctrl.h"
#include "gpio.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim4;


static uint16_t temp_PWM = 0 ;
#define TARGET_TEMP			3850
#define DIFF_TARGET_TEMP	500
#define TEMP_PWM_MAX		10000
#define TEMP_PWM_MIN		0
#define TEMP_DIFF_MAX		40
#define TEMP_TOL_MAX		100


#define HEAT_NEEDS_STRONGER			0x00
#define HEAT_NEEDS_SMALLER			0x01

#define USE_PID					0x01
#define TEMP_TSK_CYCLE			50

#define MAX_DELAY_TIME			(uint16_t)(1500/TEMP_TSK_CYCLE)

#define MAX_DIFF_TIME		0


osThreadId tempTaskHandle = NULL;

typedef enum
{
	TEMP_IDLE,
	TEMP_INIT,
	TEMP_PREHEAT_START,
	TEMP_PREHEAT,//3
	TEMP_PREHEAT_CYCL,
	TEMP_PREHEAT_STOP,
	TEMP_STOP,
	TEMP_BREAK,
	TEMP_FINISH,
}TEMP_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(TEMP_IDLE),
	TO_STR(TEMP_INIT),
	TO_STR(TEMP_PREHEAT_START),
	TO_STR(TEMP_PREHEAT),
	TO_STR(TEMP_PREHEAT_CYCL),
	TO_STR(TEMP_PREHEAT_STOP),
	TO_STR(TEMP_STOP),
	TO_STR(TEMP_BREAK),
	TO_STR(TEMP_FINISH),
};


#pragma pack(push)
#pragma pack(2)

typedef struct PID
{
	double SetPoint; //闁跨喎锟界喎鐣鹃惄顕�鏁撻弬銈嗗 Desired Value
	double SumError; //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规總鍌炴晸閿燂拷
	double LastError; //Error[-1]
	double PrevError; //Error[-2]
	double sumInc;
	double offset;
	double sumOut;
	double sumCount;
	float Proportion; //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 Proportional Const
	float Integral; //闁跨喐鏋婚幏鐑芥晸鐞涙顒查幏鐑芥晸閺傘倖瀚� Integral Const
	float Derivative; //瀵邦噣鏁撶悰妤冾劜閹风兘鏁撻弬銈嗗 Derivative Const
	uint16_t timeCount;
	uint16_t timeDelay;
	uint16_t offsetIdx;
}PID;


#pragma pack(pop)


/*====================================================================================================
PID Function
The PID (闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗闁跨喕顢滈埥鍛瀵邦噣鏁撻弬銈嗗) function is used in mainly
control applications. PIDCalc performs one iteration of the PID
algorithm.
While the PID function works, main is just a dummy program showing
a typical usage.
=====================================================================================================*/

static PID_VAL* pidCfg = &sPID;
static PID pidLoc;
static PID *sptr = &pidLoc;
static const int16_t OFFSET_CHANGE = 13;

/*====================================================================================================
Initialize PID Structure PID
=====================================================================================================*/
static uint16_t tempEnv_Fix = 2700;

void PIDInit(int32_t tempReq, uint8_t i)
{
	static uint16_t count_i = 0;
	assert(sptr);
	int32_t tempEnv1 = GetTempChn(PICK_CHN);


	int32_t tempEnv = GetTempChn(ENV_CHN);
	if(tempEnv > 11000)
		tempEnv = tempEnv_Fix;

	if(tempEnv1 < tempEnv)
	{
		tempEnv = tempEnv1;
	}

	sptr->SetPoint = tempReq;
	sptr->offsetIdx = 0;


	if(tempReq > tempEnv)
	{
		tempEnv = tempReq - tempEnv;
		sptr->offsetIdx = (uint16_t)(tempEnv/100);
		if(sptr->offsetIdx >= MAX_TEMP_IDX )
			sptr->offsetIdx = MAX_TEMP_IDX-1;
	}

	pidCfg = &sPID;

#if 0
	static int32_t pidChn = 0x8000;
	int32_t chn = tempEnv/500;

	int32_t absChn = abs(chn-pidChn);
	if(absChn >= 0x02)
	{
		pidChn = chn;
		if(chn < PID_ADJ_MAX)
		{
			pidCfg = &sPID[chn];
		}
		else
		{
			pidCfg = &sPID[PID_ADJ_MAX-1];
		}
	}
#endif

#if 1
	if(i==HEAT_NEEDS_SMALLER)//out cause final output bigger
	{
		if(sptr->sumCount > 3)
		{
			count_i = 0;
			assert(sptr->offsetIdx < MAX_TEMP_IDX);

			PIDOffset[sptr->offsetIdx] = (int16_t)(PIDOffset[sptr->offsetIdx] - OFFSET_CHANGE);
			if(PIDOffset[sptr->offsetIdx] >= sptr->sumOut/sptr->sumCount)
			{
				PIDOffset[sptr->offsetIdx] = (int16_t)(sptr->sumOut/sptr->sumCount);
			}
			Trigger_EEPSave((void*)&PIDOffset[sptr->offsetIdx], sizeof(PIDOffset[sptr->offsetIdx]), SYNC_CYCLE);
			TraceMsg(TSK_ID_TEMP_CTRL, "Adjust PID offset: PWM: %d %d, dir:%d;\t Offset: %d, %f,\n",temp_PWM,tempReq, i,sptr->offsetIdx,PIDOffset[sptr->offsetIdx]);
		}
	}
	else
	{

		if(sptr->sumCount > 3)
		{
			count_i = 0;
			assert(sptr->offsetIdx < MAX_TEMP_IDX);
			PIDOffset[sptr->offsetIdx] = (int16_t)(PIDOffset[sptr->offsetIdx] + OFFSET_CHANGE);
			if(PIDOffset[sptr->offsetIdx] <= sptr->sumOut/sptr->sumCount)
			{
				PIDOffset[sptr->offsetIdx] = (int16_t)(sptr->sumOut/sptr->sumCount);
			}
			Trigger_EEPSave((void*)&PIDOffset[sptr->offsetIdx], sizeof(PIDOffset[sptr->offsetIdx]), SYNC_CYCLE);
			TraceMsg(TSK_ID_TEMP_CTRL, "Adjust PID offset: PWM:%d; %d, dir:%d;\t Offset: %d, %f,\n",temp_PWM, tempReq, i,sptr->offsetIdx,PIDOffset[sptr->offsetIdx]);
		}
	}
#endif
	sptr->timeCount = 0;
	sptr->SumError = 0;
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2]
	sptr->sumInc = 0;
	sptr->sumOut = 0;
	sptr->sumCount = 0;
	sptr->timeDelay = pidCfg->timeDelay;
	sptr->offset = PIDOffset[sptr->offsetIdx];
	sptr->Proportion =  pidCfg->Proportion; //Proportional Const
	sptr->Integral = pidCfg->Integral; //Integral Const
	sptr->Derivative = pidCfg->Derivative; //Derivative Const
	if(count_i++ % 10 == 1)
		TraceMsg(TSK_ID_TEMP_CTRL, "Adjust Req: pwm:%d; %d, dir:%d;\t Offset: %.0f,\tP:%.3f, I:%.3f, D:%.3f\n",temp_PWM, tempReq, i,sptr->offset, sptr->Proportion, sptr->Integral, sptr->Derivative);



}



static int16_t PIDCalc(int32_t NextPoint)
{
	double dError, Error;

	Error = sptr->SetPoint - NextPoint;          // 閸嬪繘鏁撻弬銈嗗
	sptr->SumError += Error;                      // 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�
	dError = sptr->LastError - sptr->PrevError;     // 闁跨喐鏋婚幏宄板瀵邦噣鏁撻弬銈嗗
	sptr->PrevError = sptr->LastError;
	sptr->LastError = Error;
	sptr->sumInc = (sptr->Proportion * Error              // 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
	+ sptr->Integral * sptr->SumError         // 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
	+ sptr->Derivative * dError             // 瀵邦噣鏁撻弬銈嗗闁跨喐鏋婚幏锟�
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
		if(abs(PIDOffset[sptr->offsetIdx] - offset)> 20)
		{
			PIDOffset[sptr->offsetIdx] = offset;
			Trigger_EEPSave((void*)&PIDOffset[sptr->offsetIdx], sizeof(PIDOffset[sptr->offsetIdx]), SYNC_CYCLE);
		}
		sptr->SumError = 0;
		sptr->LastError = 0; //Error[-1]
		sptr->PrevError = 0; //Error[-2]
		sptr->sumInc = 0;
		sptr->sumOut = 0;
		sptr->sumCount = 0;
		sptr->timeDelay = pidCfg->timeDelay;
		sptr->offset = PIDOffset[sptr->offsetIdx];
	}
	return (int16_t)(dError);
}




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


static int16_t PWMCalc(int16_t tempAct, int16_t tempReq)
{
	static int16_t 	pwmOut = TEMP_PWM_MIN;

#ifndef USE_PID
	if(temperature < (tempReq-TEMP_DIFF_MAX/2))
	{
		pwmOut = TEMP_PWM_MAX;
	}
	else
	{
		pwmOut = TEMP_PWM_MIN;
	}
#else
	sptr->SetPoint = tempReq;

	if(tempAct <= (tempReq-(int16_t)TEMP_TOL_MAX/2))
	{
		sptr->timeCount = 0;
		pwmOut = TEMP_PWM_MAX;
	//	PIDInit(tempReq, HEAT_NEEDS_STRONGER);
	}
	else if(tempAct >= (tempReq+(int16_t)TEMP_DIFF_MAX))
	{
		sptr->timeCount = 0;
		pwmOut = TEMP_PWM_MIN;
	//	PIDInit(tempReq, HEAT_NEEDS_SMALLER);
	}
	else
	{
		sptr->timeCount++;
		if(sptr->timeCount%sptr->timeDelay == 1)
		{
			pwmOut = PIDCalc((int32_t)tempAct);
		}
	}
#endif
	return pwmOut;
}
#define WARNING_OUT_CTRL		300//
#define MAX_HEAT_COUNT			3000//180s?
#define ERROR_SENSOR_VAL_BIG	8000
#define ERROR_SENSOR_VAL_SMALL	0
void StartTempCtrlTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	double temperature;
//	int32_t temperatureAvg;
	int16_t pwmOut = 0;
	int16_t actPwmOut = 0;
	osEvent event;

	TEMP_STATE tskState = TEMP_IDLE;

	tickOut = TEMP_TSK_CYCLE;
	int32_t allTemp[TEMP_MAX_CHN];
	const uint8_t taskID = TSK_ID_TEMP_CTRL;
	uint16_t printCountTimeout = 0;
	uint32_t stateOld = 0;
	TracePrint(taskID,"started\n");
	int16_t tempPreHeat = GetPreHeatTemp();
//	TestPinSet(0,0x0);
//	uint16_t dia = 0;
	uint16_t tickCount = 0;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(TEMPERATURE_ID, tickOut);
	//	GetAllTemperature();
		pwmOut = 0;
	//	TestTempPrint();
		GetAllTemperature();
		tickOut = TEMP_TSK_CYCLE;
		allTemp[TARGET_CHN] = GetTempChn(TARGET_CHN);
		allTemp[ENV_CHN] = GetTempChn(ENV_CHN);
		allTemp[PICK_CHN] = GetTempChn(PICK_CHN);
		temperature = allTemp[TARGET_CHN];

		if(tskState == TEMP_PREHEAT)
		{
			tickCount++;
		}
		else if( tskState == TEMP_PREHEAT_CYCL)
		{
			//do nothing
			tickCount = MAX_HEAT_COUNT;
		}
		else
		{
			tickCount = 0;
		}
		if(tickCount >= MAX_HEAT_COUNT)
		{
			tickCount = MAX_HEAT_COUNT;
			Dia_UpdatePreHeatCtrl((uint16_t)tempPreHeat);
		}

	//	TracePrint(taskID, "T: %d,%d,%d,%d,\n",allTemp[0],allTemp[1],allTemp[2],allTemp[3]);
		if (event.status != osEventMessage)
		{
			if(stateOld != tskState)
				printCountTimeout = 30;
			else
			{
				printCountTimeout++;
			}
			stateOld = tskState;
			if(printCountTimeout >= 25)
			{
				printCountTimeout = 0;
				TracePrint(taskID, "Timeout: %d,pwmOut: %d\t%s, Time%d\n",tskState, actPwmOut, taskStateDsp[tskState], tickOut);
			}
			switch(tskState)
			{
			case TEMP_BREAK:
			case TEMP_IDLE:
				pwmOut = TEMP_PWM_MIN;
				break;
			case TEMP_INIT:
			case TEMP_PREHEAT_START:
				pwmOut = TEMP_PWM_MIN;
				tickOut = 0;
				tskState = TEMP_PREHEAT;
				tempPreHeat = GetPreHeatTemp();
				PIDInit(tempPreHeat, HEAT_NEEDS_STRONGER);
				break;

			case TEMP_PREHEAT:
				//temperatureAvg = GetAvgTemperature(temperature);
				if(temperature < (tempPreHeat-TEMP_DIFF_MAX/2))
				{
					pwmOut = TEMP_PWM_MAX;
				}
				else
				{
					pwmOut = TEMP_PWM_MIN;
					tskState = TEMP_PREHEAT_CYCL;
				}
				break;
			case TEMP_PREHEAT_CYCL:
				//temperatureAvg = GetAvgTemperature(temperature);
				pwmOut = PWMCalc((int16_t)(temperature), tempPreHeat);
				break;
			case TEMP_PREHEAT_STOP:
				pwmOut = TEMP_PWM_MIN;
				tskState = TEMP_STOP;
				break;
			case TEMP_STOP:
			case TEMP_FINISH:
				pwmOut = TEMP_PWM_MIN;
				tskState = TEMP_IDLE;
				break;
			}

		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;

			tickCount = 0;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = TEMP_BREAK;
				pwmOut = TEMP_PWM_MIN;

			}
			else if ( mainTskState == TSK_INIT)
			{
				uint32_t state = TSK_MSG_CONVERT(event.value.p)->val.value;
				if(state == TEMP_PREHEAT_CMD)
				{
					tskState = TEMP_PREHEAT_START;
				}
				else if(TEMP_CTRL_CMD == state)
				{
					tskState = TEMP_PREHEAT;
				}
				else
				{
					tskState = TEMP_STOP;
				}
				pwmOut = TEMP_PWM_MIN;
				tickOut = 0;
			}
			else
			{
				tskState = TEMP_BREAK;
				pwmOut = TEMP_PWM_MIN;
			}
		}

		if(pwmOut >= TEMP_PWM_MAX)
			pwmOut = TEMP_PWM_MAX-1;

		if(actPwmOut != pwmOut)
		{
			TraceMsg(taskID,"pwm: %04d,\t req:%.0f temperature: %04d,\t%04d,\t%04d,\t%04d,\n", \
							pwmOut,sptr->SetPoint, allTemp[0],allTemp[1],allTemp[2],allTemp[3]  );
		}
		TempCtrlPwmOut((uint16_t)pwmOut);
		actPwmOut = pwmOut;
		pwmOutput = (uint16_t)pwmOut;
	}


}


