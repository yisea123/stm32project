/*
 * tsk_valve_ctrl.c
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */



#include "main.h"
#include "dev_eep.h"

#include <string.h>
#include "unit_flow_cfg.h"
#include "tsk_substep.h"
#include "tim.h"
#include "unit_sys_diagnosis.h"
#include "unit_statistics_data.h"
#include "unit_flow_act.h"
#define FLOW_VALVE_QUEUE_SIZE			4
#define VALVE_ALL						0xFFFF

#define VALVE_MASK_IO					0xFFF0
#define VALVE_MASK_EX					0x7F

extern uint16_t ValveMask;
static const uint16_t PWM_MAX = 4500;
typedef enum
{
	VALVE_IDLE,
	VALVE_INIT,
	VALVE_OFF,
	VALVE_OFF_DECREASE,
	VALVE_OFF_DELAY,
	VALVE_ON,
	VALVE_ON_DELAY,

	VALVE_FINISH,
// 	VALVE_BREAK,
} VALVE_STATE;

static const char* taskStateDsp[] =
{
	TO_STR(VALVE_IDLE),
	TO_STR(VALVE_INIT),
	TO_STR(VALVE_OFF),
	TO_STR(VALVE_OFF_DELAY),
	TO_STR(VALVE_ON),
	TO_STR(VALVE_ON_DELAY),
	TO_STR(VALVE_FINISH),
};

volatile uint16_t valveIO_Status = VALVE_DEFAUILT;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
typedef struct

{
	GPIO_TypeDef* port;
	uint16_t pin;
	uint16_t alternate;
	TIM_HandleTypeDef* ptrTim;
	uint32_t channel;
} ValvePins;

const ValvePins valvPin[VALVE_PINS_NUM] =
{
//tim 2
		{ VALVE1_GPIO_Port, VALVE1_Pin, GPIO_AF2_TIM5, &htim5, TIM_CHANNEL_1 },
		{ VALVE2_GPIO_Port, VALVE2_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_2 },
		{ VALVE3_GPIO_Port, VALVE3_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_3 },
		{ VALVE4_GPIO_Port, VALVE4_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_4 },

		{ VALVE5_GPIO_Port, VALVE5_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_1 },
		{ VALVE6_GPIO_Port, VALVE6_Pin, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_3 },
		{ VALVE7_GPIO_Port, VALVE7_Pin, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_4 },
		{ VALVE8_GPIO_Port, VALVE8_Pin, GPIO_AF2_TIM5, &htim5, TIM_CHANNEL_2 },

		{ VALVE9_GPIO_Port,  VALVE9_Pin, GPIO_AF2_TIM5, &htim5, TIM_CHANNEL_3 },
		{ VALVE10_GPIO_Port, VALVE10_Pin, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_1 },
		{ GPIOI, 			 GPIO_PIN_7, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_2 },
};

void StartValveTask(void const * argument);
void CoolFanCtrl(uint16_t);


//begin-- valve control
static void SetPinOut(ValvePins pinConfig, uint8_t val)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	HAL_TIM_PWM_Stop(pinConfig.ptrTim, pinConfig.channel);

	GPIO_InitStructure.Pin = pinConfig.pin;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;

	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;

	HAL_GPIO_Init(pinConfig.port, &GPIO_InitStructure);

	if (val)
		HAL_GPIO_WritePin(pinConfig.port, pinConfig.pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(pinConfig.port, pinConfig.pin, GPIO_PIN_RESET);

	//initial pin reg then;
}


static void UpdatePwm(TIM_HandleTypeDef* ptrTim, uint32_t channel, uint16_t period)
{
	if(TIM_CHANNEL_1 == channel)
		ptrTim->Instance->CCR1 = period / 2;
	else if(TIM_CHANNEL_2 == channel)
		ptrTim->Instance->CCR2 = period / 2;
	else if(TIM_CHANNEL_3 == channel)
		ptrTim->Instance->CCR3 = period / 2;
	else if(TIM_CHANNEL_4 == channel)
		ptrTim->Instance->CCR4 = period / 2;
	else
	{}
	/* Generate an update event to reload the Prescaler
	 and the repetition counter(only for TIM1 and TIM8) value immediately */
	ptrTim->Instance->EGR = TIM_EGR_UG;
}

static void DecreasePWM(uint16_t val, uint16_t pwm)
{
	uint16_t valIdx = 0;
	uint16_t valN = 0;
	if (val != 0)
	{
		for (valIdx = 0; valIdx < VALVE_PINS_NUM; valIdx++)
		{
			valN = (uint16_t) (0x01 << (15 - valIdx));
			if (val & valN)
			{
				UpdatePwm(valvPin[valIdx].ptrTim, valvPin[valIdx].channel, pwm);
			}
		}
	}
}
static void StartPwm(ValvePins pinConfig)
{
	if(FVT_Test)
	{
		//do nothing for FVT
	}
	else
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		GPIO_InitStructure.Pin = pinConfig.pin;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStructure.Alternate = pinConfig.alternate;
		HAL_GPIO_Init(pinConfig.port, &GPIO_InitStructure);
		UpdatePwm(pinConfig.ptrTim, pinConfig.channel, PWM_MAX);
		HAL_TIM_PWM_Start(pinConfig.ptrTim, pinConfig.channel);

		//initial pin reg then;
	}
}


static void CloseValve(const uint16_t val)
{
	uint16_t valIdx = 0;
	uint16_t valN = 0;
	if (val != 0)
	{
		for (valIdx = 0; valIdx < VALVE_PINS_NUM; valIdx++)
		{
			valN = (uint16_t) (0x01 << (15 - valIdx));
			if (val & valN)
			{
				SetPinOut(valvPin[valIdx], GPIO_PIN_RESET);
			}
		}
	}
}


void CoolFanCtrl(uint16_t status)
{
	uint16_t valIdx = 10;
	if(status)
		SetPinOut(valvPin[valIdx], GPIO_PIN_SET);
	else
		SetPinOut(valvPin[valIdx], GPIO_PIN_RESET);
}
//end-- valve control

//begin---step control

/* StartValveTask function */
void StartValveTask(void const * argument)
{
	(void)argument; // pc lint
	uint16_t currValvePwm = 0;
	uint16_t currValveDecrease = 100;
	uint16_t decCount = 0;
	uint32_t tickOut = osWaitForever;
	osEvent event;
	uint16_t valveIOReq = 0x0;
	uint16_t valveOff = 0x0;
	TSK_MSG localMsg;
	uint16_t locValvReq = 0x0;
	VALVE_STATE tskState = VALVE_IDLE;
	const uint8_t taskID = TSK_ID_VALVE_CTRL;
	ValveCmd valveCmd;
	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	CloseValve(VALVE_ALL);
	TracePrint(taskID,"started  \n");

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_VALVE_CTL_ID, tickOut);
//		tickOut = osWaitForever;
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout, %d,valveoff,%x, %s, Time,%d\n",tskState,valveOff, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
				case VALVE_OFF_DECREASE:
					tickOut = VALVE_OFF_DECREASE_TIME;
					if(currValvePwm > currValveDecrease)
					{
						currValvePwm = currValvePwm - currValveDecrease;
						DecreasePWM(valveOff, currValvePwm);
					}
					else
					{
						tskState = VALVE_OFF_DELAY;
						tickOut = 0;
						CloseValve(valveOff);
					}
					if(decCount)
					{
						decCount--;
					}
					else
					{
						tskState = VALVE_OFF_DELAY;
						tickOut = 0;
						CloseValve(valveOff);
					}
					break;
				case VALVE_OFF_DELAY:
					tskState = VALVE_ON;
					SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
					break;
				case VALVE_ON_DELAY:
					for (uint16_t valIdx = 0; valIdx < VALVE_PINS_NUM-1; valIdx++)
					{
						uint16_t valN1 = (uint16_t) (0x01 << (15 - valIdx));
						if (valN1 & valveIOReq)
						{
							StartPwm(valvPin[valIdx]);
						}
					}
					tskState = VALVE_FINISH;
					SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
					break;
				default:
					tskState = VALVE_IDLE;
					break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TracePrint(taskID, "%s, %d,\t%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);
			UnuseTskMsg( TSK_MSG_CONVERT(event.value.p) );
			tickOut = osWaitForever;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				locValvReq = 0x0;
				tskState = VALVE_INIT;
				localMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
				Valve_Motor(close_cera_v,NULL,NULL);
				//force state change to be break;
			}
			else if ( mainTskState == TSK_INIT)
			{

				if (tskState != VALVE_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is,%d, %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{
					localMsg = *(TSK_MSG_CONVERT(event.value.p));
					valveCmd = *(ValveCmd*)(TSK_MSG_CONVERT(event.value.p)->val.p);
					if(valveCmd.valveId < 15)
					{
						if(valveCmd.valveStatus == 0)
							locValvReq &= ~(1<<(15-valveCmd.valveId));
						else
							locValvReq |= (1<<(15-valveCmd.valveId));
					}
					else
					{
						TraceDBG(taskID, "send error cmd, 0x%04x, %d,%d\n", valveIOReq, valveCmd.valveId, valveCmd.valveStatus);
					}
					tskState = VALVE_INIT;
					localMsg.tskState = TSK_SUBSTEP;
					SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
					//change to init
					//send msg to be running;
				}
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
					case VALVE_IDLE:
					break;
					case VALVE_INIT:
					valveIOReq = (uint16_t)((locValvReq & VALVE_MASK_IO));
					TraceMsg(taskID, "valve out, 0x%04x, %d,%d\n", valveIOReq, valveCmd.valveId, valveCmd.valveStatus);
					tskState = VALVE_OFF;
					SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
					break;
					case VALVE_OFF:
					{
						uint16_t rev = (uint16_t) (~valveIOReq);
						//todo add protection
						uint16_t valN = rev & valveIO_Status;
					//	CloseValve(valN);

						if(valN)
						{
							valveOff = valN;
							currValvePwm = (uint16_t)((PWM_MAX*35)/50);
							decCount = VALVE_OFF_DELAY_COUNT-1;
							currValveDecrease = currValvePwm/decCount;
							tskState = VALVE_OFF_DECREASE;
							tickOut = 0;
						}
						else
						{
							valveOff = 0;
							tskState = VALVE_ON;
							SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
						}
					}
					break;
					case VALVE_OFF_DECREASE:
					case VALVE_OFF_DELAY:
					case VALVE_ON:
					{
						//todo add protection
						uint16_t valN = (uint16_t) (~valveIO_Status);
						valN &= valveIOReq;

						if (valN != 0x0)
						{
							uint16_t valIdx = 0;
							uint16_t valN1 = 0;
							for (valIdx = 0; valIdx < VALVE_PINS_NUM; valIdx++)
							{
								valN1 = (uint16_t) (0x01 << (15 - valIdx));
								if (valN1 & valN)
								{
									SetPinOut(valvPin[valIdx], GPIO_PIN_SET);
								}
							}

							Sta_AddValveCount(valN);
							tskState = VALVE_ON_DELAY;
							tickOut = VALVE_OPEN_DELAY;
						}
						else
						{
							tskState = VALVE_FINISH;
							SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);

						}
					}
					break;
					case VALVE_ON_DELAY:
					for (uint16_t valIdx = 0; valIdx < VALVE_PINS_NUM-1; valIdx++)
					{
						uint16_t valN1 = (uint16_t) (0x01 << (15 - valIdx));
						if (valN1 & valveIOReq)
						{
							StartPwm(valvPin[valIdx]);
						}
					}
					tskState = VALVE_FINISH;
					SendTskMsgLOC(FLOW_VALVE_CTL_ID, &localMsg);
					break;
					case VALVE_FINISH:
					tskState = VALVE_IDLE;
					valveIO_Status = valveIOReq;
					TSK_FINISH_ACT(&localMsg,taskID,OK,OK);
					//MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&localMsg, 0);
					break;
				}
				//when finish->call back;
			}
			else
			{
				//todo
			}
		}
	}
}


