/*
 * tsk_motor_ctrl.c
 *
 *  Created on: 2016骞�12鏈�1鏃�
 *      Author: pli
 */





#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "tsk_substep.h"
#include "tsk_flow_all.h"

#include "unit_sys_diagnosis.h"
#include "unit_statistics_data.h"
#include "tsk_motor_ctrl.h"





static __IO int16_t motorStatus = 0;
__IO int32_t motorSteps[IDX_MOTOR_MAX];
__IO uint32_t motorStepsCount[IDX_MOTOR_MAX];
__IO uint16_t motorPPS[IDX_MOTOR_MAX];




typedef enum
{
	MOTOR_IDLE,
	MOTOR_INIT,
	MOTOR_RUN,
	MOTOR_RUN_DELAY,
	MOTOR_FINISH,
} MOTOR_STATE;





// logical -> physical
//motor 1 -> motor 1
//motor 2 -> motor 2
//mix -> motor 3
//motor 3 -> motor 5

const uint8_t MotorPinsDefaults[IDX_MOTOR_MAX][8] =
{
{ 1, MOTOR_CURR_HOLD, 0,0,0, 0, 0, },
{ 1, MOTOR_CURR_HOLD, 0,0,0, 0, 0, },
{ 1, MOTOR_CURR_HOLD, 0,0,0, 0, 0, },
{ 1, MOTOR_CURR_START, 0,0,0, 0, 0, },
};

#define MOTOR1_PINS			{	\
							{ MOTOR1_RST_N_GPIO_Port, MOTOR1_RST_N_Pin },		\
							{ MOTOR1_CURR_GPIO_Port, MOTOR1_CURR_Pin },			\
							{ MOTOR1_MODE0_GPIO_Port, MOTOR1_MODE0_Pin },		\
							{ MOTOR1_MODE1_GPIO_Port, MOTOR1_MODE1_Pin },		\
							{ MOTOR1_MODE2_GPIO_Port, MOTOR1_MODE2_Pin },		\
							{ MOTOR1_EN_N_GPIO_Port, MOTOR1_EN_N_Pin },			\
							{ MOTOR1_DIR_GPIO_Port, MOTOR1_DIR_Pin },			\
							{ MOTOR1_FAULT_N_GPIO_Port, MOTOR1_FAULT_N_Pin }, }

#define MOTOR2_PINS			{	\
							{ MOTOR2_RST_N_GPIO_Port, MOTOR2_RST_N_Pin },	\
							{ MOTOR2_CURR_GPIO_Port, MOTOR2_CURR_Pin },		\
							{ MOTOR2_MODE0_GPIO_Port, MOTOR2_MODE0_Pin },	\
							{ MOTOR2_MODE1_GPIO_Port, MOTOR2_MODE1_Pin },	\
							{ MOTOR2_MODE2_GPIO_Port, MOTOR2_MODE2_Pin },	\
							{ MOTOR2_EN_N_GPIO_Port, MOTOR2_EN_N_Pin },		\
							{ MOTOR2_DIR_GPIO_Port, MOTOR2_DIR_Pin },		\
							{ MOTOR2_FAULT_N_GPIO_Port, MOTOR2_FAULT_N_Pin }, }

#define MOTOR3_PINS			{	\
							{ MOTOR3_RST_N_GPIO_Port, MOTOR3_RST_N_Pin },	\
							{ MOTOR3_CURR_GPIO_Port, MOTOR3_CURR_Pin },		\
							{ MOTOR3_MODE0_GPIO_Port, MOTOR3_MODE0_Pin },	\
							{ MOTOR3_MODE1_GPIO_Port, MOTOR3_MODE1_Pin },	\
							{ MOTOR3_MODE2_GPIO_Port, MOTOR3_MODE2_Pin },	\
							{ MOTOR3_EN_N_GPIO_Port, MOTOR3_EN_N_Pin },		\
							{ MOTOR3_DIR_GPIO_Port, MOTOR3_DIR_Pin },		\
							{ MOTOR3_FAULT_N_GPIO_Port, MOTOR3_FAULT_N_Pin }, }

#define MOTOR4_PINS			{	\
							{ MOTOR4_RST_N_GPIO_Port, MOTOR4_RST_N_Pin },	\
							{ MOTOR4_CURR_GPIO_Port, MOTOR4_CURR_Pin },		\
							{ MOTOR4_MODE0_GPIO_Port, MOTOR4_MODE0_Pin },	\
							{ MOTOR4_MODE1_GPIO_Port, MOTOR4_MODE1_Pin },	\
							{ MOTOR4_MODE2_GPIO_Port, MOTOR4_MODE2_Pin },	\
							{ MOTOR4_EN_N_GPIO_Port, MOTOR4_EN_N_Pin },		\
							{ MOTOR4_DIR_GPIO_Port, MOTOR4_DIR_Pin },		\
							{ MOTOR4_FAULT_N_GPIO_Port, MOTOR4_FAULT_N_Pin }, }


const MotorPins motorPins[IDX_MOTOR_MAX] =
{
		MOTOR1_PINS,
		MOTOR2_PINS,
		MOTOR3_PINS,
		MOTOR4_PINS,
};



const Motor motorConfig[IDX_MOTOR_MAX] =
{
	{ MOTOR1_TIM, TIM_CHANNEL_1, &motorSteps[0], &motorStepsCount[0] },
	{ MOTOR2_TIM, TIM_CHANNEL_1, &motorSteps[1], &motorStepsCount[1] },
	{ MOTOR3_TIM, TIM_CHANNEL_1, &motorSteps[2], &motorStepsCount[2] },
	{ MOTOR4_TIM, TIM_CHANNEL_1, &motorSteps[3], &motorStepsCount[3] },
};



//begin---step control

void UpdatePWMRegChn1(TIM_HandleTypeDef* ptrTim, uint16_t period)
{
	ptrTim->Instance->ARR = (uint32_t)(period - 1);
	ptrTim->Instance->CCR1 = period / 2;
	/* Generate an update event to reload the Prescaler
	 and the repetition counter(only for TIM1 and TIM8) value immediately */
	ptrTim->Instance->EGR = TIM_EGR_UG;
}



void SetMotorIdle(uint16_t idx,uint16_t state)
{
	if(motorEnableStatus && (state == MOTOR_STOP))
	{
		HAL_GPIO_WritePin(motorPins[idx].en_N.port,
						motorPins[idx].en_N.pin,
						MOTOR_DISABLE);
	}
	else
	{
		HAL_GPIO_WritePin(motorPins[idx].en_N.port,
						motorPins[idx].en_N.pin,
						MOTOR_ENABLE);
	}
}

void InitMotorPins(uint16_t idx, uint16_t status)
{
	if (idx >= IDX_MOTOR_MAX)
	{
		return;
	}
	else
	{
		const MotorPins* motor = &motorPins[idx];
		HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin,
							(GPIO_PinState)MotorPinsDefaults[idx][0]);
		HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
				(GPIO_PinState)MotorPinsDefaults[idx][1]);
		HAL_GPIO_WritePin(motor->mode0.port, motor->mode0.pin,
				(GPIO_PinState)(GPIO_PinState)MotorPinsDefaults[idx][2]);
		HAL_GPIO_WritePin(motor->mode1.port, motor->mode1.pin,
				(GPIO_PinState)(GPIO_PinState)MotorPinsDefaults[idx][3]);
		HAL_GPIO_WritePin(motor->mode2.port, motor->mode2.pin,
				(GPIO_PinState)(GPIO_PinState)MotorPinsDefaults[idx][4]);
		SetMotorIdle(idx,status);
		HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin,
				(GPIO_PinState)MotorPinsDefaults[idx][6]);
	}
}
void SetMotorSpeed(uint16_t idx, uint16_t speedId)
{
	if(speedId >= SPEED_MAX_CFG )
		speedId = FTP2;
	if (idx >= IDX_MOTOR_MAX)
	{
		return;
	}
	const MotorPins* motor = &motorPins[idx];
	HAL_GPIO_WritePin(motor->mode0.port, motor->mode0.pin,
			MotorSpeed_Cfg[speedId][0]);
	HAL_GPIO_WritePin(motor->mode1.port, motor->mode1.pin,
			MotorSpeed_Cfg[speedId][1]);
	HAL_GPIO_WritePin(motor->mode2.port, motor->mode2.pin,
			MotorSpeed_Cfg[speedId][2]);
}


void ResetStepMotor(uint16_t idx)
{
	if (idx >= IDX_MOTOR_MAX)
	{
		return;
	}
	else
	{
		const MotorPins* motor = &motorPins[idx];
		HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
							motorConfig[idx].channel);
		HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin, GPIO_PIN_RESET);
		osDelay(100);
		InitMotorPins(idx, MOTOR_WORK);
	}
}

static void UpdateMotor_IO(uint16_t idx, PumpConfig motor1, uint8_t status)
{
	if (idx >= IDX_PUMP_MAX)
	{
		return;
	}
	else
	{
		const MotorPins* motor = &motorPins[idx];
		
		if (status == MOTOR_WORK)
		{
			SetMotorIdle(idx,status);
			HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin,
					GPIO_PIN_SET);
			HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
			MOTOR_CURR_START);
			if (motor1.dir != CW)
				HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin, CW_DIR);
			else
				HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin, CCW_DIR);
			SetMotorSpeed(idx, motor1.mode);
		}
		else
		{
			HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
					motorConfig[idx].channel);
			InitMotorPins(idx, status);
		}
	}

}

static uint32_t UpdateMotorIdx(const MOTOR_IDX idx, const PumpConfig pump)
{
	uint32_t timeN = 0;

	if (idx < IDX_PUMP_MAX)
	{

		if ((pump.pps != 0) && (pump.steps != 0))
		{
			motorPPS[idx] = pump.pps;
			motorStatus = (int16_t) (motorStatus |(0x01 << idx));
			timeN = (uint32_t)((pump.steps * 1000) / pump.pps);
			UpdateMotor_IO(idx, pump, MOTOR_WORK);
			*(motorConfig[idx].ptrSteps) = (int32_t)pump.steps;
			UpdatePWMRegChn1(motorConfig[idx].ptrTim, CALC_PWM_REG(pump.pps));
			SetMotorIdle(idx,MOTOR_WORK);
			if (IDX_MIXING != idx)
				HAL_TIM_PWM_Start_IT(motorConfig[idx].ptrTim,
						motorConfig[idx].channel);
		}
		else
		{
			motorPPS[idx] = 0;
			UpdateMotor_IO(idx, pump, MOTOR_STOP);
			motorStatus = (int16_t) ((uint16_t)motorStatus & (uint16_t)(~(0x01U << idx)));
			if (IDX_MIXING != idx)
			{
				HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
						motorConfig[idx].channel);
				SetMotorIdle(idx,MOTOR_STOP);
			}
		}
	}
	return timeN;

}

uint16_t MonitorMotorStatus(void)
{
	for(uint16_t idx=0;idx<IDX_MOTOR_MAX;idx++)
	{
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(motorPins[idx].fault_N.port, motorPins[idx].fault_N.pin))
		{
			TraceDBG( TSK_ID_MOTOR_CTRL,"Motor Id: %d fault is detected\n",idx+1);
			ResetStepMotor(idx);
		}
	}
	return OK;

}

static uint32_t MotorAction(const PumpConfig pump1, const PumpConfig pump2,
		const PumpConfig pump3)
{

	uint32_t timeReq = 0;
	uint32_t timeN = 0;

	MonitorMotorStatus();

	timeN = UpdateMotorIdx(IDX_PUMP1, pump1);
	if (timeReq < timeN)
		timeReq = timeN;
	timeN = UpdateMotorIdx(IDX_PUMP2, pump2);
	if (timeReq < timeN)
		timeReq = timeN;
	timeN = UpdateMotorIdx(IDX_PUMP3, pump3);
	if (timeReq < timeN)
		timeReq = timeN;

	if (timeReq)
		timeReq = timeReq + MOTOR_ACT_DELAY;

	TraceMsg( TSK_ID_MOTOR_CTRL,"T%d,pump information: %d,%d,%d,%d; %d,%d,%d,%d; %d,%d,%d,%d \n",\
			timeReq, \
			pump1.dir,pump1.mode,pump1.pps,pump1.steps,\
			pump2.dir,pump2.mode,pump2.pps,pump2.steps,\
			pump3.dir,pump3.mode,pump3.pps,pump3.steps);
	return timeReq;
}


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t idx = IDX_MOTOR_MAX +1;
	if(htim == MOTOR1_TIM)
	{
		//motor 1
		if(motorSteps[0])
			motorSteps[0]--;
		if(!motorSteps[0])
		{
			idx = 0;
		}
	}
	else if(htim == MOTOR2_TIM)
	{
		if(motorSteps[1])
			motorSteps[1]--;
		if(!motorSteps[1])
		{
			idx = 1;
		}
	}
	else if(htim == MOTOR3_TIM)
	{
		if(motorSteps[2])
			motorSteps[2]--;
		if(!motorSteps[2])
		{
			idx = 2;
		}
	}
	if(idx < IDX_PUMP_MAX)
	{
		HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
							motorConfig[idx].channel);
		HAL_GPIO_WritePin(motorPins[idx].curr.port,
							motorPins[idx].curr.pin, MOTOR_CURR_HOLD);
		SetMotorIdle(idx,MOTOR_STOP);
		motorStatus = (int16_t) ((uint16_t)motorStatus & (uint16_t)(~(0x01U << idx)));
		if (motorStatus == 0)
		{
			SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_SUBSTEP, 0, NULL);
		}
	}

}

static void InitStepMotors(void)
{
	for (uint8_t idx = 0; idx < IDX_PUMP_MAX; idx++)
	{
		HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim, motorConfig[idx].channel);
		InitMotorPins(idx, MOTOR_STOP);
	}
	motorStatus = 0x0;
	for(uint16_t id=0;id<IDX_MOTOR_MAX;id++)
	{
		motorSteps[id] = 0;
	}
}


static void ResetMotors(void)
{
	//reset pump
	ResetStepMotor(IDX_PUMP1);
	ResetStepMotor(IDX_PUMP2);
	ResetStepMotor(IDX_PUMP3);
	motorStatus = 0x0;
}


void StartMotorTask(void const * argument)
{
	(void)argument; // pc lint
	TSK_MSG localMsg;
	uint32_t tickOut = osWaitForever;
	osEvent event;
	StepConfig localVal = {0,\
			{0,	0,	0,	0},	\
			{0,	0,	0,	0}, \
			{0,	0,	0,	0}, \
	};
	MOTOR_STATE tskState = MOTOR_IDLE;
	const uint8_t taskID = TSK_ID_MOTOR_CTRL;

	const char* taskStateDsp[] =
	{
		TO_STR(MOTOR_IDLE),
		TO_STR(MOTOR_INIT),
		TO_STR(MOTOR_RUN),
		TO_STR(MOTOR_RUN_DELAY),
		TO_STR(MOTOR_FINISH),
	};

	localMsg.callBack = NULL;
	//init step motor;
	InitStepMotors();
	TracePrint(taskID,"started  \n");

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_MOTOR_CTL_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case MOTOR_RUN_DELAY:
				if (motorStatus)
				{
					TraceDBG(taskID,"motor finished error! --%d,M  %d,%d,%d,%d \n", \
							motorStatus,motorSteps[0],motorSteps[1],motorSteps[2],motorSteps[3]);
					motorStatus = 0;
					for (uint16_t idx = 0; idx < IDX_PUMP_MAX; idx++)
					{
						HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
								motorConfig[idx].channel);
						HAL_GPIO_WritePin(motorPins[idx].curr.port,
								motorPins[idx].curr.pin,
								MOTOR_CURR_HOLD);
					}
				}
				tskState = MOTOR_FINISH;
				MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t ) &localMsg, 0);
				break;
			default:
				tskState = MOTOR_FINISH;
				MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t ) &localMsg, 0);
				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if(TSK_FORCE_DISABLE == mainTskState)
			{
				tskState = MOTOR_FINISH;
				motorStatus = 0;
				for (uint16_t idx = 0; idx < IDX_PUMP_MAX; idx++)
				{
					HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
							motorConfig[idx].channel);
					HAL_GPIO_WritePin(motorPins[idx].curr.port,
							motorPins[idx].curr.pin,
							MOTOR_CURR_HOLD);
					SetMotorIdle(idx,MOTOR_STOP);
				}
				localMsg = *(TSK_MSG_CONVERT(event.value.p));

				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t ) &localMsg, 0);
			}
			else if(TSK_FORCE_ENABLE == mainTskState)
			{
				tskState = MOTOR_FINISH;
				motorStatus = 0;
				for (uint16_t idx = 0; idx < IDX_PUMP_MAX; idx++)
				{
					HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
							motorConfig[idx].channel);
					HAL_GPIO_WritePin(motorPins[idx].curr.port,
							motorPins[idx].curr.pin,
							MOTOR_CURR_HOLD);
					SetMotorIdle(idx,MOTOR_WORK);
				}
				localMsg = *(TSK_MSG_CONVERT(event.value.p));

				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t ) &localMsg, 0);
			}
			else if ( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = MOTOR_FINISH;
				motorStatus = 0;
				for (uint16_t idx = 0; idx < IDX_PUMP_MAX; idx++)
				{
					HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
							motorConfig[idx].channel);
					HAL_GPIO_WritePin(motorPins[idx].curr.port,
							motorPins[idx].curr.pin,
							MOTOR_CURR_HOLD);
				}
				localMsg = *(TSK_MSG_CONVERT(event.value.p));

				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t ) &localMsg, 0);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = MOTOR_FINISH;
				ResetMotors();
				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				localMsg.tskState = TSK_SUBSTEP;
				MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t ) &localMsg, 0);
			}
			else if (mainTskState == TSK_INIT)
			{
				if (tskState != MOTOR_IDLE)
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
					localVal =
					*(StepConfig*) (TSK_MSG_CONVERT(event.value.p)->val.p);
					Sta_UpdateVolumeUsage(&localVal);

					tskState = MOTOR_INIT;
					localMsg.tskState = TSK_SUBSTEP;
					MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t) &localMsg, 0);
					Sta_UpdateStep(&localVal);
					//change to init
					//send msg to be running;
				}
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
					case MOTOR_IDLE:
					break;
					case MOTOR_INIT:
					for (uint8_t idx = 0; idx < IDX_PUMP_MAX; idx++)
					{
						
						
						HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
								motorConfig[idx].channel);
						InitMotorPins(idx, MOTOR_STOP);
					}
					tskState = MOTOR_RUN;
					MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t) &localMsg, 0);
					break;
					case MOTOR_RUN:
					{
						uint32_t time = MotorAction(localVal.pump1,
								localVal.pump2, localVal.pump3);
						if (time)
						{
							tickOut = time;
							tskState = MOTOR_RUN_DELAY;
						}
						else
						{
							tskState = MOTOR_FINISH;
							MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t) &localMsg, 0);
						}
					}
					break;
					case MOTOR_RUN_DELAY:
					if (motorStatus)
					{
						TraceDBG(taskID,"motor finished error! --%d \n",
								motorStatus);

						motorStatus = 0;
						for (uint16_t idx = 0; idx < IDX_PUMP_MAX; idx++)
						{
							HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
									motorConfig[idx].channel);
							HAL_GPIO_WritePin(motorPins[idx].curr.port,
									motorPins[idx].curr.pin,
									MOTOR_CURR_HOLD);
						}
					}
					tskState = MOTOR_FINISH;
					MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t) &localMsg, 0);
					break;
					case MOTOR_FINISH:
					for(uint16_t i=0;i<IDX_PUMP_MAX;i++)
						motorPPS[i] = 0;
					tskState = MOTOR_IDLE;
					if (localMsg.callBack)
					{
						localMsg.callBack(taskID);
					}
					//	MsgPush(FLOW_MOTOR_CTL_ID, (uint32_t) &localMsg, 0);
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


