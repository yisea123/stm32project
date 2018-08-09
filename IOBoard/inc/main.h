/*
 * main.h
 *
 *  Created on: 2016��8��23��
 *      Author: pli
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

//#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "shell_io.h"


#ifdef __cplusplus
 extern "C" {
#endif

 typedef enum
 {
 	READ_ACCESS = 0x01u,
 	WRITE_ACCESS=0x02u,

 }ADR_RW_STATUS;



#define		CAN_ENABLED

#pragma GCC diagnostic ignored "-Wpadded"



typedef enum
{
	TSK_IDLE,
	TSK_INIT,
	TSK_SUBSTEP,
	TSK_RESETIO,
	TSK_FINISH,
	TSK_FORCE_BREAK,
} TSK_STATE;


#pragma GCC diagnostic pop



typedef enum
{
	TASK_IDLE = 0, TASK_RUNNING = 1, TASK_STUCK = 2,
} TSK_STATUS;

#define FORCE_STOP		0xFFFFFF
typedef enum
{
	TSK_ID_CAN_OPEN = 0,
	TSK_ID_CAN_IO,
	TSK_ID_LOGIC,
	TSK_ID_SHELL,
	MAX_TASK_ID

} TaskId_e;
#define 	MAX_PRINT_SIZE		0x100//512;
#define SHELL_RX_DATA		0x01
#define rxShell					taskThreadID[TSK_ID_SHELL]
extern osThreadId taskThreadID[MAX_TASK_ID];


#define canopenTaskId		taskThreadID[TSK_ID_CAN_OPEN]


extern CAN_HandleTypeDef 	hcan;
#define ptrCanDevice		(&hcan)


int main(int argc, char* argv[]);
void StartLogicTask(void const * argument);
void InitEEP(void);
uint16_t CheckValidation(float gain, float offset);
void ResetCalibration(uint8_t chn);

uint16_t CalcGainOffset(const uint16_t setValue, const uint16_t measValue, uint8_t idx);
void OutputAOWithClibration(int16_t ao, uint8_t idx);
void StartDefaultTask(void const * argument);

typedef void * OS_RSEMA;
void vApplicationMallocFailedHook(void);
void OS_Use(OS_RSEMA);
void OS_Unuse(OS_RSEMA);
OS_RSEMA OS_CreateSemaphore(void);


osStatus MessagePush(uint32_t line, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
osStatus MessagePurge(uint32_t line, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
#define MsgPush(x,y,z)		MessagePush(__LINE__,x,y,z)
#define MsgPurge(x,y,z)		MessagePurge(__LINE__,x,y,z)
osStatus SignalPush(osThreadId thread_id, int32_t signal);
#define SigPush(x,y)		SignalPush(x,y)

#ifdef __cplusplus
 }
#endif
#endif /* INC_MAIN_H_ */
