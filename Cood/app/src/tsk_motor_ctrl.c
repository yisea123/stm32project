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






static 	TSK_MSG localMsg;
static volatile int16_t motorStatus = 0;
volatile int32_t motorSteps[IDX_MOTOR_MAX];
volatile uint32_t motorStepsCount[IDX_MOTOR_MAX];
volatile uint16_t motorPPS[IDX_MOTOR_MAX];
volatile uint16_t finalSpeed = 0;
volatile uint16_t currSpeed = 0;
volatile uint32_t motorPos_Pps = 0x7FFFFFFF;
volatile uint16_t motorPos_Status = PUMP_IDLE;



static uint8_t motorPosCmd;
static uint8_t motorDirCmd;
static uint16_t ledChkResult;


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

const uint8_t MotorPinsDefaults[IDX_MOTOR_MAX][5] =
{
{ 1, MOTOR_CURR_HOLD, 0, 0, 0, },
{ 1, MOTOR_CURR_HOLD, 0, 0, 0, },
{ 1, MOTOR_CURR_HOLD, 0, 0, 0, },
{ 1, MOTOR_CURR_HOLD, 0, 0, 0, },
};

#define MOTOR1_PINS			{	\
							{ MOTOR1_RST_N_GPIO_Port, MOTOR1_RST_N_Pin },		\
							{ MOTOR1_CURR_GPIO_Port, MOTOR1_CURR_Pin },			\
							{ MOTOR1_MODE0_GPIO_Port, MOTOR1_MODE0_Pin },		\
							{ MOTOR1_EN_N_GPIO_Port, MOTOR1_EN_N_Pin },			\
							{ MOTOR1_DIR_GPIO_Port, MOTOR1_DIR_Pin },			\
							{ MOTOR1_FAULT_N_GPIO_Port, MOTOR1_FAULT_N_Pin }, }

#define MOTOR2_PINS			{	\
							{ MOTOR2_RST_N_GPIO_Port, MOTOR2_RST_N_Pin },	\
							{ MOTOR2_CURR_GPIO_Port, MOTOR2_CURR_Pin },		\
							{ MOTOR2_MODE0_GPIO_Port, MOTOR2_MODE0_Pin },	\
							{ MOTOR2_EN_N_GPIO_Port, MOTOR2_EN_N_Pin },		\
							{ MOTOR2_DIR_GPIO_Port, MOTOR2_DIR_Pin },		\
							{ MOTOR2_FAULT_N_GPIO_Port, MOTOR2_FAULT_N_Pin }, }

#define MOTOR3_PINS			{	\
							{ MOTOR3_RST_N_GPIO_Port, MOTOR3_RST_N_Pin },	\
							{ MOTOR3_CURR_GPIO_Port, MOTOR3_CURR_Pin },		\
							{ MOTOR3_MODE0_GPIO_Port, MOTOR3_MODE0_Pin },	\
							{ MOTOR3_EN_N_GPIO_Port, MOTOR3_EN_N_Pin },		\
							{ MOTOR3_DIR_GPIO_Port, MOTOR3_DIR_Pin },		\
							{ MOTOR3_FAULT_N_GPIO_Port, MOTOR3_FAULT_N_Pin }, }

#define MOTOR4_PINS			{	\
							{ MOTOR4_RST_N_GPIO_Port, MOTOR4_RST_N_Pin },	\
							{ MOTOR4_CURR_GPIO_Port, MOTOR4_CURR_Pin },		\
							{ MOTOR4_MODE0_GPIO_Port, MOTOR4_MODE0_Pin },	\
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

void ResetStepMotor(uint16_t idx)
{
	if (idx >= IDX_MOTOR_MAX)
	{
		return;
	}
	else
	{
		const MotorPins* motor = &motorPins[idx];
		HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
				(GPIO_PinState)(GPIO_PinState)MotorPinsDefaults[idx][1]);
		HAL_GPIO_WritePin(motor->mode.port, motor->mode.pin,
				(GPIO_PinState)(GPIO_PinState)MotorPinsDefaults[idx][2]);

		SetMotorIdle(idx,MOTOR_WORK);

		HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin,
				(GPIO_PinState)(GPIO_PinState)MotorPinsDefaults[idx][4]);
		osDelay(100);
		HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin, GPIO_PIN_SET);
	}
}

static void UpdateMotor_IO(uint16_t idx, PumpStepCfg motor1, uint8_t status)
{
	if (idx >= IDX_PUMP_MAX)
	{
		return;
	}
	else
	{
		const MotorPins* motor = &motorPins[idx];
		SetMotorIdle(idx,status);
		if (status == MOTOR_WORK)
		{

			HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin,
					GPIO_PIN_SET);
			HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
			MOTOR_CURR_START);
			if (motor1.dir != CW)
				HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin, CW_DIR);
			else
				HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin, CCW_DIR);

			if (motor1.mode == FTP)
				HAL_GPIO_WritePin(motor->mode.port, motor->mode.pin,
				FULL_STEPS);
			else
				HAL_GPIO_WritePin(motor->mode.port, motor->mode.pin,
				HALF_FULL_STEPS);


		}
		else
		{
			HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin,
					(GPIO_PinState)MotorPinsDefaults[idx][0]);
			HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
					(GPIO_PinState)MotorPinsDefaults[idx][1]);
			HAL_GPIO_WritePin(motor->mode.port, motor->mode.pin,
					(GPIO_PinState)MotorPinsDefaults[idx][2]);
			HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin,
					(GPIO_PinState)MotorPinsDefaults[idx][4]);
			HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
					motorConfig[idx].channel);
		}
	}

}

static uint32_t UpdateMotorIdx(const MOTOR_IDX idx, PumpStepCfg pump)
{
	uint32_t timeN = 0;
	if(idx == 0)
	{
		finalSpeed = pump.pps;
		if(pump.pps > 200)
			pump.pps = 200;
	}

	if (idx < IDX_PUMP_MAX)
	{

		if ((pump.pps != 0) && (pump.steps != 0))
		{
			motorPPS[idx] = pump.pps;
			motorStatus = (int16_t) (motorStatus |(0x01 << idx));
			timeN = (uint32_t)((pump.steps * 1000) / pump.pps);
			UpdateMotor_IO(idx, pump, MOTOR_WORK);
			*(motorConfig[idx].ptrSteps) = (int32_t)pump.steps;
			currSpeed = pump.pps;
			UpdatePWMRegChn1(motorConfig[idx].ptrTim, CALC_PWM_REG(pump.pps));
			SetMotorIdle(idx, MOTOR_WORK);
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

static uint32_t MotorAction(PumpCmd pumpCmd)
{
	PumpStepCfg pump1;
	uint16_t speed = 0;
	uint32_t timeReq = 0;
	uint32_t timeN = 0;
	if((DELAY_CFG_EN  & pumpCmd.pumpCfg.speed) == 0)
		speed = (uint16_t)(pumpCmd.pumpCfg.speed+1);
	else
		speed = (uint16_t)((pumpCmd.pumpCfg.speed & DELAY_CFG_EN_MSK) + 1);

	MonitorMotorStatus();
	pump1.dir = (motorDirCmd == 0x0)?CW:CCW;
	pump1.mode = MTP;
	pump1.pps = (uint16_t)(2*motorHighSpeed/(speed));
  	pump1.steps = pumpCmd.pumpCfg.steps;
	if(pump1.pps < 50)
		pump1.pps = 50;
	ledChkResult = FATAL_ERROR;

	timeN = UpdateMotorIdx(pumpCmd.pumpId, pump1);
	if (timeReq < timeN)
		timeReq = timeN;

	if (timeReq)
		timeReq = timeReq + MOTOR_ACT_DELAY + MOTOR_ACC_DELAY;
	TraceMsg( TSK_ID_MOTOR_CTRL,"T%d,id, %d pump information, %d,%d,%d,%d,pos, %d,cmd, %d\n",\
			timeReq, pumpCmd.pumpId, \
			pump1.dir,pump1.mode,pump1.pps,pump1.steps,
			motorPos_Pps, motorPosCmd);
	return timeReq;
}



static uint16_t LedPosChk(uint16_t id)
{
#define POS_CHK_CNT 1
	//todo
	//if() // if bubble if found
	static uint8_t posChkCnt = 0;
	if((id == 0) && (led0Pos < LED_POS_CHK_LIMIT))
	{
		posChkCnt++;
	}
	else if((id == 1) && (led1Pos < LED_POS_CHK_LIMIT))
	{
		posChkCnt++;

	}
	else if((id == 2) && ((led1Pos < LED_POS_CHK_LIMIT) || (led0Pos < LED_POS_CHK_LIMIT)))
	{
		posChkCnt++;

	}
	else
	{
		posChkCnt = 0;
	}
	if(posChkCnt >= POS_CHK_CNT)
	{
		posChkCnt = POS_CHK_CNT;
		ledChkResult = OK;
	}
	else
	{
		ledChkResult = FATAL_ERROR;
	}
	return ledChkResult;
}
#define TIMES_END_POS 8

static uint16_t EndPos_Chk(uint16_t cnt)
{
	static uint16_t count = 0;

	uint16_t idx = 0xFFFF;
	GPIO_PinState st2 = GPIO_PIN_RESET;
	if(motorDirCmd == PULL)
	{
	//	st2 = HAL_GPIO_ReadPin(SW_SNS2_GPIO_Port, SW_SNS2_Pin);
	//	if(st2 == GPIO_PIN_RESET)
		{
			if(motorPos_Pps >= LIMIT_STEPS_PUMP0 )
				st2 = GPIO_PIN_SET;
		}
	}
	else
	{
		st2 = HAL_GPIO_ReadPin(SW_SNS1_GPIO_Port, SW_SNS1_Pin);
	}
	if(cnt > count)
		count = cnt;
	if(st2 != GPIO_PIN_RESET)
	{
		if(count >= TIMES_END_POS)
			idx = 0;
		else
			count++;
	}
	else
	{
		count = 0;
	}
	if((motorDirCmd == PUSH) && (idx == 0))
		motorPos_Pps = 0;
	return idx;
}

static uint16_t CheckCounter(uint16_t id)
{
	uint16_t ret = 0xFFFF;
	if(motorSteps[id])
		motorSteps[id]--;
	if(!motorSteps[id])
	{
		ret = id;
	}
	return ret;
}
uint16_t lstRet = 0;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t idx = IDX_MOTOR_MAX +1;
	if(htim == MOTOR1_TIM)
	{
		//motor 1
		if(motorDirCmd == PUSH)
			motorPos_Pps--;
		else
			motorPos_Pps++;

		switch(motorPosCmd)
		{
		case END_POS:
			idx = EndPos_Chk(TIMES_END_POS);
		//	if(idx != 0)
		//		idx = CheckCounter(0);
			break;
		case LED1_CHK:
			if( OK != LedPosChk(1) )
			{
				idx = 0;
			}
			else
			{
				idx = EndPos_Chk(0);
				if(idx > IDX_MOTOR_MAX)
				{
					idx = CheckCounter(0);
				}
			}
			break;
		case LED0_CHK:
			lstRet = ledChkResult;

			if( OK != LedPosChk(0) )
			{
				idx = 0;
			}
			else
			{
				idx = EndPos_Chk(0);
				if(idx > IDX_MOTOR_MAX)
				{
					idx = CheckCounter(0);
				}
			}
			break;
		case LED0_POS:
			lstRet = ledChkResult;
			if( OK == LedPosChk(0) )
			{
				idx = 0;
			}
			else
			{
				idx = EndPos_Chk(0);

			}
			break;
		case LED1_POS:
			if( OK == LedPosChk(1) )
			{
				idx = 0;
			}
			else
			{
				idx = EndPos_Chk(0);

			}
			break;
		case FREE_POS:
			idx = EndPos_Chk(0);
			if(idx > IDX_MOTOR_MAX)
			{
				idx = CheckCounter(0);
			}
			break;
		case STOP_ACT:
			idx = 0;
			break;
		}
		if(idx == 0)
			motorPos_Status = PUMP_IDLE;
		else
			motorPos_Status = PUMP_RUNNING;
		if(finalSpeed > currSpeed)
		{
			currSpeed++;
			UpdatePWMRegChn1(htim, CALC_PWM_REG(currSpeed));
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
	if(idx < IDX_MOTOR_MAX)
	{
		HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
							motorConfig[idx].channel);
		HAL_GPIO_WritePin(motorPins[idx].curr.port,
							motorPins[idx].curr.pin, MOTOR_CURR_HOLD);
		SetMotorIdle(idx,MOTOR_STOP);
		motorStatus = (int16_t) ((uint16_t)motorStatus & (uint16_t)(~(0x01U << idx)));
		if (motorStatus == 0)
		{
			SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
		}
	}

}

static void InitStepMotors(void)
{
	for (uint8_t idx = 0; idx < IDX_PUMP_MAX; idx++)
	{
		HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim, motorConfig[idx].channel);
		const MotorPins* motor = &motorPins[idx];
		HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin,
				(GPIO_PinState)MotorPinsDefaults[idx][0]);
		HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
				(GPIO_PinState)MotorPinsDefaults[idx][1]);
		HAL_GPIO_WritePin(motor->mode.port, motor->mode.pin,
				(GPIO_PinState)MotorPinsDefaults[idx][2]);
		SetMotorIdle(idx,MOTOR_STOP);
		HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin,
				(GPIO_PinState)MotorPinsDefaults[idx][4]);
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

uint16_t Valve_Motor(uint16_t subStepId, ptrTskCallBack ptrCallFin, ptrTskCallBack ptrCallUpdate)
{
	uint16_t ret = OK;
	static 	PumpCmd pmpCmd = {0,
	                        				{0,	0,	0,	0,0,	0,},
	                        		};
	if(open_cera_v == subStepId)
	{
		//dianji
		pmpCmd.pumpId = IDX_PUMP2;
		pmpCmd.pumpCfg = pumpCfg2;
		pmpCmd.pumpCfg.dir = PULL|FREE_POS;
		SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_INIT, (uint32_t)&pmpCmd, ptrCallFin,ptrCallUpdate);
	}
	else if(close_cera_v == subStepId)
	{
		//close cera
		pmpCmd.pumpId = IDX_PUMP2;
		pmpCmd.pumpCfg = pumpCfg2;
		pmpCmd.pumpCfg.dir = PUSH|FREE_POS;
		SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_INIT, (uint32_t)&pmpCmd, ptrCallFin,ptrCallUpdate);
	}
	else
	{
		ret = FATAL_ERROR;
	}
	return ret;
}


void StartMotorTask(void const * argument)
{
	(void)argument; // pc lint

	uint32_t tickOut = osWaitForever;
	osEvent event;
	PumpCmd pmpCmd = {0,
			{0,	0,	0,	0,0,	0,}
	};
	MOTOR_STATE tskState = MOTOR_IDLE;
	const uint8_t taskID = TSK_ID_MOTOR_CTRL;
	uint32_t lastTick = 0;
	const char* taskStateDsp[] =
	{
		TO_STR(MOTOR_IDLE),
		TO_STR(MOTOR_INIT),
		TO_STR(MOTOR_RUN),
		TO_STR(MOTOR_RUN_DELAY),
		TO_STR(MOTOR_FINISH),
	};

	localMsg.callBackFinish = NULL;
	localMsg.callBackUpdate = NULL;
	//init step motor;
	InitStepMotors();
	TracePrint(taskID,"started  \n");
	Valve_Motor(close_cera_v,NULL,NULL);
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(FLOW_MOTOR_CTL_ID, tickOut);

		if (event.status != osEventMessage)
		{
			lastTick = tickOut;
			TracePrint(taskID, "Timeout, %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case MOTOR_RUN_DELAY:
				if (motorStatus)
				{
					TraceDBG(taskID,"motor finished error, %d, tick, %d, M, %d,%d \n", \
							motorStatus,lastTick, motorSteps[0],motorSteps[1]);
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
				SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
				break;
			default:
				tskState = MOTOR_IDLE;
				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TracePrint(taskID, "%s, %d,%s\n",mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]);
			UnuseTskMsg( TSK_MSG_CONVERT(event.value.p) );
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

				SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
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

				SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
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
				SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);

				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = MOTOR_FINISH;
				ResetMotors();
				localMsg = *(TSK_MSG_CONVERT(event.value.p));
				SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
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
					pmpCmd =
					*(PumpCmd*) (TSK_MSG_CONVERT(event.value.p)->val.p);

					tskState = MOTOR_INIT;
					SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
	//				Sta_UpdateStep(&pmpCmd);
					//change to init
					//send msg to be running;
				}
	//			if(pmpCmd.pumpId == IDX_PUMP1)
				{
					motorPosCmd = (uint8_t)(pmpCmd.pumpCfg.dir & E_POS_MASK);
					motorDirCmd = (uint8_t)(pmpCmd.pumpCfg.dir & E_DIR_MASK);
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
						const MotorPins* motor = &motorPins[idx];
						HAL_GPIO_WritePin(motor->rst_N.port, motor->rst_N.pin,
								(GPIO_PinState)MotorPinsDefaults[idx][0]);
						HAL_GPIO_WritePin(motor->curr.port, motor->curr.pin,
								(GPIO_PinState)MotorPinsDefaults[idx][1]);
						HAL_GPIO_WritePin(motor->mode.port, motor->mode.pin,
								(GPIO_PinState)MotorPinsDefaults[idx][2]);
						SetMotorIdle(idx,MOTOR_STOP);
						HAL_GPIO_WritePin(motor->dir.port, motor->dir.pin,
								(GPIO_PinState)MotorPinsDefaults[idx][4]);
						HAL_TIM_PWM_Stop_IT(motorConfig[idx].ptrTim,
								motorConfig[idx].channel);
					}
					tskState = MOTOR_RUN;
					SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
					break;
					case MOTOR_RUN:
					{
						uint32_t time = MotorAction(pmpCmd);
						if (time)
						{
							tickOut = time;
							tskState = MOTOR_RUN_DELAY;
						}
						else
						{
							tskState = MOTOR_FINISH;
							SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
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
					SendTskMsgLOC(FLOW_MOTOR_CTL_ID, &localMsg);
					break;
					case MOTOR_FINISH:
					TracePrint(taskID,"motor finished status! --%d \n",
							ledChkResult);
					tskState = MOTOR_IDLE;
					TSK_FINISH_ACT(&localMsg,taskID,OK,ledChkResult);
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


