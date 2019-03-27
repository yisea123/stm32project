/*
 * main.h
 *
 *  Created on: 2016��8��23��
 *      Author: pli
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "diag/Trace.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "t_unit_head.h"

#ifdef __cplusplus
 extern "C" {
#endif
#define VALID_PASSWORD_ID			6000
#define TASK_LOOP_ST			true


 extern uint32_t putValidNum;


extern osMessageQId SHELL_TX_ID;
extern osMessageQId SHELL_RX_ID;

extern osMessageQId FILE_TSK_ID;
extern osMessageQId USART_RX_EVENT;
extern osMessageQId PRINT_ID;
extern osMessageQId SCH_LB_ID;
#define IS_ROM_ADR(x)	(((x)>=0x08000000)&&((x)<0x08200000) )
#define IS_RAM_ADR(x)	((x)<0x001FFFFF)
extern uint16_t dummyRam;
#define NULL_T_DATA_OBJ		CONSTRUCT_SIMPLE_U16(&dummyRam, RAM)
#define NMT_TRIGGER			0x01
#define NEW_MEAS_DATA		0x01
#define SHELL_RX_DATA		0x01


#pragma GCC diagnostic ignored "-Wpadded"
enum
{
	IDX_SUB_RTC,
	//IDX_SUB_PARAMETER,
	IDX_SYS_INFO,
	IDX_SUB_MAX,
};




typedef struct
{
	const T_UNIT* 	ptrSub;
	const char 	subsystemName[32];
}T_COMMON_UNIT;


extern const T_UNIT*  subSystem[IDX_SUB_MAX];


typedef enum
{
	TSK_IDLE,
	TSK_INIT,
	TSK_SUBSTEP,
	TSK_RESETIO,
	TSK_FINISH,
	TSK_FORCE_BREAK,
} TSK_STATE;




typedef void (*ptrTskCallBack)(uint16_t ret, uint16_t val);
#define TSK_MSG_CONVERT(x)		((TSK_MSG*)(x))


typedef struct
{
 	ptrTskCallBack callBackFinish;
 	ptrTskCallBack callBackUpdate;
 	union
 	{
 		uint32_t 	value;
 		void* 		p;
 	} val; //
 	TSK_STATE tskState;
 	uint16_t msgState;
 	uint32_t threadId;
 	uint32_t lineNum;
} TSK_MSG;

#pragma GCC diagnostic pop

enum
{
	 GET_NEW=0,
	 GET_EXIST,
};

typedef enum
{
	TASK_IDLE = 0, TASK_RUNNING = 1, TASK_STUCK = 2,
} TSK_STATUS;

#define FORCE_STOP		0xFFFFFF
typedef enum
{
	TSK_ID_AD_MONITOR=0,
	TSK_ID_PRINT,
	TSK_ID_EEP,
	TSK_ID_CAN_RX1,
	TSK_ID_CAN_TX1,
	TSK_ID_CAN_ERR,
	TSK_ID_FILE,
	TSK_ID_GUI,


	TSK_ID_SHELL_RX,
	TSK_ID_SHELL_TX,
	TSK_ID_GPIO,
	TSK_ID_TST,
	TSK_ID_CAN1_TSK,
	TSK_ID_LOCAL_BUS,
	MAX_TASK_ID

} TaskId_e;



extern uint8_t		printChnMap[MAX_TASK_ID];
extern uint8_t		printMsgMap[MAX_TASK_ID];
extern uint8_t 	dbgMsgMap[MAX_TASK_ID];

#define TO_STR(x)	(char*)#x
extern osThreadId taskThreadID[MAX_TASK_ID];
extern const char*		TskName[MAX_TASK_ID];

#define adDetTaskHandle			taskThreadID[TSK_ID_AD_DETECT]
#define eepTaskHandle			taskThreadID[TSK_ID_EEP]
#define errCanTaskHandle		taskThreadID[TSK_ID_CAN_ERR]

#define rxCan					taskThreadID[TSK_ID_CAN_RX1]
#define txCan					taskThreadID[TSK_ID_CAN_TX1]
#define rxShell					taskThreadID[TSK_ID_SHELL_RX]
#define monitorAdTaskHandle		taskThreadID[TSK_ID_AD_MONITOR]

extern uint32_t	freeRtosTskTick[MAX_TASK_ID];
extern uint8_t freeRtosTskState[MAX_TASK_ID];
#define SHELL_RX_DATA			0x01
void CheckSimuAction(void );

int main(int argc, char* argv[]);

osStatus MessagePush(uint32_t line, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
osStatus MessagePurge(uint32_t line, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
#define MsgPush(x,y,z)		MessagePush(__LINE__,x,y,z)
#define MsgPurge(x,y,z)		MessagePurge(__LINE__,x,y,z)
osStatus SignalPush(osThreadId thread_id, int32_t signal);
#define SigPush(x,y)		SignalPush(x,y)
void RenitUsb_User(void);
extern __IO uint32_t kernelStarted;

#ifdef __cplusplus
 }
#endif
#endif /* INC_MAIN_H_ */
