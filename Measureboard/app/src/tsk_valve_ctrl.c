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

typedef enum
{
	VALVE_IDLE,
	VALVE_INIT,
	VALVE_OFF,
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

__IO uint16_t valveIO_Status = VALVE_DEFAUILT;

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
	uint16_t* ptrState;
} ValvePins;


static uint16_t valPinState[VALVE_PINS_NUM] =
{0,0,0,0,0,};

static const uint16_t PWM_FULL_DUTY = 4500u;

const ValvePins valvPin[VALVE_PINS_NUM] =
{
//tim 2
		{ VALVE1_GPIO_Port, VALVE1_Pin, GPIO_AF2_TIM5, &htim5, TIM_CHANNEL_1, &valPinState[0]},
		{ VALVE2_GPIO_Port, VALVE2_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_2, &valPinState[1]},
		{ VALVE3_GPIO_Port, VALVE3_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_3, &valPinState[2]},
		{ VALVE4_GPIO_Port, VALVE4_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_4, &valPinState[3]},

		{ VALVE5_GPIO_Port, VALVE5_Pin, GPIO_AF1_TIM2, &htim2, TIM_CHANNEL_1, &valPinState[4]},
		{ VALVE6_GPIO_Port, VALVE6_Pin, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_3, &valPinState[5]},
		{ VALVE7_GPIO_Port, VALVE7_Pin, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_4, &valPinState[6]},
		{ VALVE8_GPIO_Port, VALVE8_Pin, GPIO_AF2_TIM5, &htim5, TIM_CHANNEL_2, &valPinState[7]},

		{ VALVE9_GPIO_Port,  VALVE9_Pin, GPIO_AF2_TIM5, &htim5, TIM_CHANNEL_3, &valPinState[8]},
		{ VALVE10_GPIO_Port, VALVE10_Pin, GPIO_AF2_TIM3, &htim3, TIM_CHANNEL_1, &valPinState[9]},
};

void StartValveTask(void const * argument);

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


static void StartPwm(ValvePins pinConfig, uint16_t pwm)
{
	if(FVT_Test)
	{
		//do nothing for FVT
	}
	else
	{
		if(*pinConfig.ptrState == 0)
		{
			GPIO_InitTypeDef GPIO_InitStructure;

			GPIO_InitStructure.Pin = pinConfig.pin;
			GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStructure.Pull = GPIO_NOPULL;
			GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStructure.Alternate = pinConfig.alternate;
			HAL_GPIO_Init(pinConfig.port, &GPIO_InitStructure);
		}
		*pinConfig.ptrState = 1;
		if(pinConfig.channel == TIM_CHANNEL_1)
			pinConfig.ptrTim->Instance->CCR1 = pwm;
		else if(pinConfig.channel == TIM_CHANNEL_2)
			pinConfig.ptrTim->Instance->CCR2 = pwm;
		else if(pinConfig.channel == TIM_CHANNEL_3)
			pinConfig.ptrTim->Instance->CCR3 = pwm;
		else if(pinConfig.channel == TIM_CHANNEL_4)
			pinConfig.ptrTim->Instance->CCR4 = pwm;
		else
		{}
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
				*valvPin[valIdx].ptrState = 0;
			}
		}
	}
}

//end-- valve control

//begin---step control

/* StartValveTask function */
void StartValveTask(void const * argument)
{
	(void)argument; // pc lint

	uint32_t tickOut = osWaitForever;
	osEvent event;
	uint16_t valveIOReq = 0x0;
	TSK_MSG localMsg;
	uint16_t locValvReq = 0x0;
	VALVE_STATE tskState = VALVE_IDLE;
	const uint8_t taskID = TSK_ID_VALVE_CTRL;

	localMsg.callBack = NULL;
	CloseValve(VALVE_ALL);
	TracePrint(taskID,"started  \n");
	ValveCtrl valCtrl = valveCtrl;
	uint16_t pwmCyc = 0;
	uint16_t pwmCtrl = 0;
	uint16_t close_Valve = 0x0;
	uint16_t open_Valve = 0x00;
	uint32_t offTickTime = 0;
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_VALVE_CTL_ID, tickOut);
//		tickOut = osWaitForever;
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
				case VALVE_OFF_DELAY:
					valCtrl.offDelay_Cyc --;

					if(valCtrl.offDelay_Cyc)
					{
						tickOut = offTickTime ;
						if(pwmCtrl > pwmCyc)
							pwmCtrl = (uint16_t)(pwmCtrl - pwmCyc);
						else
							pwmCtrl = 0;
						for (uint16_t valIdx = 0; valIdx < VALVE_PINS_NUM; valIdx++)
						{
							uint16_t valN = (uint16_t) (0x01 << (15 - valIdx));
							if (close_Valve & valN)
							{
								StartPwm(valvPin[valIdx], pwmCtrl);
							}
						}
						TracePrint(taskID, "Timeout: %d,\t%s, PWM:%d\n",tskState, taskStateDsp[tskState], pwmCtrl);

					}
					else
					{
						pwmCtrl = 0;
						TracePrint(taskID, "Timeout: %d,\t%s, PWM:%d\n",tskState, taskStateDsp[tskState], pwmCtrl);
						CloseValve(close_Valve);
						tskState = VALVE_ON;
						MsgPush(FLOW_VALVE_CTL_ID, (uint32_t )&localMsg, 0);
					}
					break;
				case VALVE_ON_DELAY:
					pwmCtrl = (uint16_t)(PWM_FULL_DUTY/100*valCtrl.onDuty);

					for (uint16_t valIdx = 0; valIdx < VALVE_PINS_NUM; valIdx++)
					{
						uint16_t valN1 = (uint16_t) (0x01 << (15 - valIdx));
						if (valN1 & open_Valve)
						{
							StartPwm(valvPin[valIdx], pwmCtrl);
						}
					}
					tskState = VALVE_FINISH;
					MsgPush(FLOW_VALVE_CTL_ID, (uint32_t )&localMsg, 0);
					break;
				default:
					tskState = VALVE_IDLE;
					break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				locValvReq = 0x0;
				tskState = VALVE_INIT;
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_VALVE_CTL_ID, (uint32_t )&localMsg, 0);

				//force state change to be break;
			}
			else if ( mainTskState == TSK_INIT)
			{

				if (tskState != VALVE_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
					//not finish;
				}
				//this function could be trigger always by other task;
				//idle
				{
					localMsg = *(TSK_MSG_CONVERT(event.value.p));
					locValvReq = ((StepConfig*)(TSK_MSG_CONVERT(event.value.p)->val.p))->valvStatus;
					tskState = VALVE_INIT;
					localMsg.tskState = TSK_SUBSTEP;
					MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&localMsg, 0);
					valCtrl = valveCtrl;
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
					{
						valveIOReq = (uint16_t)((locValvReq & VALVE_MASK_IO) | (ValveMask & VALVE_MASK_EX));

						tskState = VALVE_OFF;

						//todo add protection
						close_Valve =  (uint16_t)(valveIO_Status & (~valveIOReq) );
						open_Valve = (uint16_t)(valveIOReq & (~valveIO_Status) );
						TraceMsg(taskID, "valve out: 0x%04x-> old %x; act on %x, off %x \n", valveIOReq,valveIO_Status, open_Valve, close_Valve);
						MsgPush (FLOW_VALVE_CTL_ID, (uint32_t)&localMsg, 0);
					}
					break;
					case VALVE_OFF:
					{
						pwmCtrl = (uint16_t)(PWM_FULL_DUTY/100*valCtrl.onDuty);
						if(close_Valve)
						{
							tskState = VALVE_OFF_DELAY;
							tickOut = valCtrl.offDelay;
							pwmCyc = PWM_FULL_DUTY;
							if(tickOut > VALVE_OFF_DELAY_TIME)
							{
								tickOut = VALVE_OFF_DELAY_TIME;
							}
							if(valCtrl.offDelay_Cyc)
							{
								tickOut = tickOut/valCtrl.offDelay_Cyc;
								pwmCyc = (uint16_t)(PWM_FULL_DUTY/100*valCtrl.onDuty/valCtrl.offDelay_Cyc);
							}
							else
							{
								valCtrl.offDelay_Cyc = 1;
							}
							offTickTime = tickOut;
						}
						else
						{
							tskState = VALVE_ON;
							MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&localMsg, 0);
						}
					}
					break;
					case VALVE_OFF_DELAY:
					case VALVE_ON:
					{

						if (open_Valve != 0x0)
						{
							uint16_t valIdx = 0;
							uint16_t valN1 = 0;
							for (valIdx = 0; valIdx < VALVE_PINS_NUM; valIdx++)
							{
								valN1 = (uint16_t) (0x01 << (15 - valIdx));
								if (valN1 & open_Valve)
								{
									SetPinOut(valvPin[valIdx], GPIO_PIN_SET);
								}
							}
							Sta_AddValveCount(open_Valve);
							tskState = VALVE_ON_DELAY;
							tickOut = VALVE_OPEN_DELAY;
							if(tickOut > valCtrl.onDelay)
								tickOut = valCtrl.onDelay;
						}
						else
						{
							tskState = VALVE_FINISH;
							MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&localMsg, 0);

						}
					}
					break;

					case VALVE_FINISH:
					tskState = VALVE_IDLE;
					valveIO_Status = valveIOReq;
					if(localMsg.callBack)
					{
						localMsg.callBack(taskID);

					}
					//MsgPush ( FLOW_VALVE_CTL_ID, (uint32_t)&localMsg, 0);
					break;
					default:
						TraceDBG(taskID,"the valve task has bugs, original state is:%d- %s!\n",tskState, taskStateDsp[tskState]);
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


