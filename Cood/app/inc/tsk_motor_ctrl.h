/*
 * tsk_motor_ctrl.h
 *
 *  Created on: 2017Äê10ÔÂ24ÈÕ
 *      Author: pli
 */

#ifndef SRC_TSK_MOTOR_CTRL_H_
#define SRC_TSK_MOTOR_CTRL_H_

#include "bsp.h"
#include "tim.h"

#define MOTOR_WORK		1
#define MOTOR_STOP		0



extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim14;
extern uint32_t motorEnableStatus;

#define MOTOR_TIM1 &htim10
#define MOTOR_TIM2 &htim13
#define MOTOR_TIM3 &htim14
#define MOTOR_TIM4 &htim12

#define MOTOR1_TIM 	MOTOR_TIM1
#define MOTOR2_TIM 	MOTOR_TIM2
#define MOTOR3_TIM  MOTOR_TIM3
#define MOTOR4_TIM  MOTOR_TIM4


typedef struct
{
	PinInst rst_N; 	//when fault is 0: set rst to be 0; normal this is used when startup; the during time is 100ms?
	PinInst curr; 	//0: high current, 1: low current; default must be: 1!!
	PinInst mode; 	//0: full step; 1: half step;
	PinInst en_N; 	//0: enable(steps running, idle, startup); 1: only rst and fault
	PinInst dir; 	//0: CW; 1Â£Âº CCW
	PinInst fault_N; // input pins: 0 is fault;
} MotorPins;


typedef struct
{
	TIM_HandleTypeDef* ptrTim;
	uint32_t channel;
	volatile int32_t* ptrSteps;
	volatile uint32_t* ptrCount;
} Motor;

extern volatile uint16_t motorPos_Status;
extern volatile uint32_t motorPos_Pps;
extern volatile int32_t motorSteps[IDX_MOTOR_MAX];
extern volatile uint32_t motorStepsCount[IDX_MOTOR_MAX];
extern volatile uint16_t motorPPS[IDX_MOTOR_MAX];
extern const MotorPins motorPins[IDX_MOTOR_MAX];
extern const Motor motorConfig[IDX_MOTOR_MAX];

extern const uint8_t MotorPinsDefaults[IDX_MOTOR_MAX][5];

void UpdatePWMRegChn1(TIM_HandleTypeDef* ptrTim, uint16_t period);
void SetMotorIdle(uint16_t idx,uint16_t state);
void ResetStepMotor(uint16_t idx);

#endif /* SRC_TSK_MOTOR_CTRL_H_ */
