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
#include "t_unit.h"

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

#define IS_ROM_ADR(x)	(((x)>=0x08000000)&&((x)<0x08200000) )
#define IS_RAM_ADR(x)	((x)<0x001FFFFF)


#pragma GCC diagnostic ignored "-Wpadded"
enum
{
	IDX_SUB_RTC, IDX_SUB_PARAMETER,IDX_SYS_INFO,
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



typedef void (*ptrTskCallBack)(uint32_t val);
#define TSK_MSG_CONVERT(x)		((TSK_MSG*)(x))

typedef struct
{
	ptrTskCallBack callBack;

	union
	{
		uint32_t value;
		void* p;
	} val; //
	TSK_STATE tskState;
} TSK_MSG __attribute__ ((aligned (1)));


#pragma GCC diagnostic pop


typedef enum
{
	READ_ACCESS = 0x01u,
	WRITE_ACCESS=0x02u,

}ADR_RW_STATUS;
typedef enum
{
	TASK_IDLE = 0, TASK_RUNNING = 1, TASK_STUCK = 2,
} TSK_STATUS;

#define FORCE_STOP		0xFFFFFF
typedef enum
{
	TSK_ID_AD_MONITOR=0,
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
	MAX_TASK_ID

} TaskId_e;
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



#define SHELL_RX_DATA			0x01
void CheckSimuAction(void );

int main(int argc, char* argv[]);
uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus);
void ResetDevice(uint16_t type);
uint16_t GetObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		OBJ_DATA* inst);

uint16_t GetObjectType(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		uint16_t* type);

uint16_t PutObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		void* inst);

uint16_t LoadDefaultCfg(void);

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
