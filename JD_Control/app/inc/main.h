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
#include "shell_io.h"
#ifdef __cplusplus
 extern "C" {
#endif
#define VALID_PASSWORD_ID			6000
#define TASK_LOOP_ST			true
#define USE_DHCP       /* enable DHCP, if disabled static address is used */
#define USE_EXT_DEV			1

 extern uint32_t putValidNum;


extern osMessageQId SHELL_TX_ID;
extern osMessageQId SHELL_RX_ID;

extern osMessageQId FILE_TSK_ID;
extern osMessageQId USART_RX_EVENT;
extern osMessageQId PRINT_ID;
extern osMessageQId SCH_LB_ID;
extern osMessageQId ADC_MONITOR;
extern osMessageQId WELD_CTRL;
extern osMessageQId MOTOR_CTRL;
extern osMessageQId PWM_CTRL;
extern osMessageQId SCH_CTRL;
extern osMessageQId CURR_CALI;
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
	IDX_SUB_WELDCFG,
	IDX_SYS_INFO,
	IDX_SUB_MAX,
};




typedef struct
{
	const T_UNIT* 	ptrSub;
	const char 	subsystemName[32];
}T_COMMON_UNIT;


extern const T_UNIT*  subSystem[IDX_SUB_MAX];
void DeviceResetHandle(uint16_t id, uint16_t ret);

typedef enum
{
	TSK_IDLE,
	TSK_INIT,
	TSK_SUBSTEP,
	TSK_RESETIO,
	TSK_FINISH,
	TSK_FORCE_BREAK,
	TSK_FORCE_STOP,
	TSK_STATE_MAX,
} TSK_STATE;



extern const char* mainTskStateDsp[TSK_STATE_MAX];

typedef void (*ptrTskCallBack)(uint16_t ret, uint16_t val);

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

#define TSK_MSG_CONVERT(x)		((TSK_MSG*)(x))

#define TSK_MSG_RELASE		TracePrint(taskID, "owner: %d, %s: %x,\t%s\n",TSK_MSG_CONVERT(event.value.p)->lineNum, mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]); \
	UnuseTskMsg( TSK_MSG_CONVERT(event.value.p) )
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
	TSK_ID_ADC_MONITOR=0,
	TSK_ID_PRINT,
	TSK_ID_EEP,
	TSK_ID_SHELL_RX,
	TSK_ID_SHELL_TX,
	TSK_ID_LOCAL_BUS,
	TSK_ID_OUTPUT,
	TSK_ID_MOTOR,
	TSK_ID_WELD,
	TSK_ID_ETHERNET,
	TSK_ID_PWM,
	TSK_ID_SCH,
	TSK_ID_CURR_CALI,
	MAX_TASK_ID

} TaskId_e;

#define TIME_UNIT						1000u

extern uint8_t		printChnMap[MAX_TASK_ID];
extern uint8_t		printMsgMap[MAX_TASK_ID];
extern uint8_t 	dbgMsgMap[MAX_TASK_ID];

#define TO_STR(x)	(char*)#x
extern osThreadId taskThreadID[MAX_TASK_ID];
extern const char*		TskName[MAX_TASK_ID];



#define outputTaskHandle		taskThreadID[TSK_ID_OUTPUT]
#define eepTaskHandle			taskThreadID[TSK_ID_EEP]
#define monitorAdTaskHandle		taskThreadID[TSK_ID_ADC_MONITOR]

extern uint32_t	freeRtosTskTick[MAX_TASK_ID];
extern uint8_t freeRtosTskState[MAX_TASK_ID];
#define SHELL_RX_DATA			0x01



/************************* TSK message interfaces ***********************/

void UnuseTskMsg(TSK_MSG* gMsg);
void SendTskMsg_LOC(osThreadId thread_id, TSK_MSG* msg,uint32_t lineNum  );
void SendTskMsg_INST(osThreadId thread_id, TSK_STATE tskState, uint32_t val, ptrTskCallBack ptrCallFin, ptrTskCallBack ptrCallUpdate,uint32_t lineNum );
void StateFinishAct(TSK_MSG* ptrTask, uint16_t taskId, uint16_t ret, uint16_t result, uint32_t line);
osStatus MessagePush(uint32_t line, char* file, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
osStatus MessagePurge(uint32_t line, char* file, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);


void ResetDevice(uint16_t type);
void Trigger_DeviceReset(void);
#define SendTskMsg(x,y,z,w,u) SendTskMsg_INST(x,y,z,w,u,__LINE__)
#define SendTskMsgLOC(x,y) SendTskMsg_LOC(x,y,__LINE__)

#define MsgPush(x,y,z)		MessagePush(__LINE__,__FILE__,x,y,z)
#define MsgPurge(x,y,z)		MessagePurge(__LINE__,__FILE__,x,y,z)
osStatus SignalPush(osThreadId thread_id, int32_t signal);
#define SigPush(x,y)		SignalPush(x,y)
#define TSK_FINISH_ACT(x,y,z,w)		StateFinishAct(x,y,z,w,__LINE__)




void UpdateWeldSetting(void);
void CheckSimuAction(void );

int main(int argc, char* argv[]);
void InitTaskMsg(TSK_MSG* ptrMsg);

void RenitUsb_User(void);
extern __IO uint32_t kernelStarted;

#ifdef __cplusplus
 }
#endif
#endif /* INC_MAIN_H_ */
