/*
 * tsk_substep.h
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */

#ifndef INC_TSK_SUBSTEP_H_
#define INC_TSK_SUBSTEP_H_


#define MIX_DEFAULT			600
#define VALVE_DEFAUILT		0x0

#define VALVE_ALL			0xFFFF

#define CALC_PWM_REG(x)			(uint16_t)(200000/(x)-1)


typedef enum{
	IDX_PUMP1 = 0,
	IDX_PUMP2,
	IDX_PUMP3,
	IDX_PUMP_MAX,
	IDX_MIXING=3,
	IDX_MOTOR_MAX,
}MOTOR_IDX;



typedef enum
{
	MIX_IDLE,
	MIX_INIT,
	MIX_CYCL_RUN,
	MIX_CYCL_RUN_RDY,
	MIX_CYCL_RUN_DELAY,
	MIX_CYCL_STOP,
	MIX_CYCL_STOP_RDY,
	MIX_CYCL_STOP_DELAY,
	MIX_ACC,
	MIX_ACC_DELAY,
	MIX_RUN,
	MIX_STOP,
	MIX_FINISH,
} MIX_STATE;



void MixAction(const uint16_t);






#endif /* INC_TSK_SUBSTEP_H_ */
