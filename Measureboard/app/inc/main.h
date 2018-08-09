/*
 * main.h
 *
 *  Created on: 2016Äê8ÔÂ23ÈÕ
 *      Author: pli
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include <stdio.h>
#include "Trace.h"
#include "bsp.h"

#ifdef __cplusplus
 extern "C" {
#endif
#define VALID_PASSWORD_ID			6000


#define TIME_UNIT						100u

#define VALVE_OFF_DELAY_TIME			200u
#define VALVE_OPEN_DELAY				100u
#define MOTOR_ACT_DELAY					100u


#define TSK_SW_TIME_N					100u //buffer for other task
#define VALVE_DURING_TIME				(VALVE_OFF_DELAY_TIME+VALVE_OPEN_DELAY + TSK_SW_TIME_N)
#define TSK_SW_TIME_NEW					500u

#define FACTOR_UNIT_HOUR			3600u
#define FACTOR_UNIT_MINUTES			60u
#define TO_MS_UNIT					1000
#define TO_DAYS_UNIT				24u
 //#define DEBUG_TEST

 #define IS_ROM_ADR(x)	(((x)>=0x08000000)&&((x)<0x08200000) )
 #define IS_RAM_ADR(x)	((x)<0x001FFFFF)

extern uint16_t FVT_Test;
extern uint16_t  	masterState;
 #pragma GCC diagnostic ignored "-Wpadded"
#define TASK_LOOP_ST			true

 enum
 {
 	IDX_SUB_FLOW_CONFIG, IDX_SUB_FLOW_ACT, IDX_SUB_RTC,  IDX_SUB_MEAS,
 	IDX_SUB_PT100,IDX_SUB_STA_DATA,
 	IDX_SYS_INFO,IDX_SCH_CFG,IDX_DATA_LOG,IDX_MEAS_DATA,
	IDX_SYS_DIAGNOSIS,IDX_SUB_IOCFG,
	IDX_SUB_HMI,
	IDX_SUB_EX,
 	IDX_SUB_MAX,
 };


 extern const T_UNIT* subSystem[IDX_SUB_MAX];


 typedef enum
 {
 	TSK_IDLE,
 	TSK_INIT,
 	TSK_SUBSTEP,
 	TSK_RESETIO,
 	TSK_FINISH,
 	TSK_FORCE_BREAK,
	TSK_FORCE_STOP,
	TSK_FORCE_DISABLE,
	TSK_FORCE_ENABLE,
	TSK_RENEW_STATUS,
	TSK_STATE_MAX,
 } TSK_STATE;

extern const char* mainTskStateDsp[TSK_STATE_MAX];
 typedef enum
 {
 	MCU_IDLE = 0,
	MCU_VOLT_REFRESH,
 	MCU_FINISH,
 }MCU_STate;


 typedef void (*ptrTskCallBack)(uint32_t val);
 #define TSK_MSG_CONVERT(x)		((TSK_MSG*)(x))

#define TSK_MSG_RELASE		TracePrint(taskID, "owner: %d, %s: %x,\t%s\n",TSK_MSG_CONVERT(event.value.p)->lineNum, mainTskStateDsp[mainTskState], tskState, taskStateDsp[tskState]); \
	UnuseTskMsg( TSK_MSG_CONVERT(event.value.p) )


 enum
 {
 	SCH_TSK_IDLE,
 	SCH_TSK_MEAS,
 	SCH_TSK_CALI,
 	SCH_TSK_CLEAN,
	SCH_TSK_DRAIN,
	SCH_TSK_FLUSH,
	SCH_TSK_PRIME,
	SCH_TSK_OFFLINE_SAMPLE,
 	SCH_TSK_MANUAL=0x80000000,
 };

 enum
 {
	 ADC_12V_SUPPLY,
	 ADC_CURR_MONITOR,
	 ST_ADC_MAX_CHN,
 };


 typedef struct
 {
 	ptrTskCallBack callBack;

 	union
 	{
 		uint32_t value;
 		void* p;
 	} val; //
 	TSK_STATE tskState;
 	uint16_t msgState;
 	uint32_t threadId;
 	uint32_t lineNum;
 } TSK_MSG;
#define TO_STR(x)	(char*)#x

 #pragma GCC diagnostic pop
 typedef enum
 {
 	TSK_FLOW_IDX,
 	TSK_HEAT_IDX,
 	TSK_MEAS_IDX,
 	TSK_SUBSTEP_IDX,
 	TSK_VALVE_IDX,
 	TSK_MOTOR_IDX,
 	TSK_MIX_IDX,
 	TSK_SHELL_IDX,
 	TSK_CAN_IDX,
 	TSK_EEP_IDX,
 } TSK_IDX;


 enum
 {
	 GET_NEW=0,
	 GET_EXIST,
 };

 typedef enum
 {
 	TASK_IDLE = 0,
 } TSK_STATUS;

 #define FORCE_STOP		0xFFFFFF
 enum
 {
 	MAP_TRACE,
 	MAP_SHELL,
 	MAP_XPRINTFF,
	MAP_NONE,
	MAP_PRINT_MAX,
 };
#define MAX_IO_BOARD_NUM		16


 typedef enum
 {
 	TSK_ID_AD_DETECT =0,
 	TSK_ID_MEASURE,
 	TSK_ID_FLOW_CTRL,
 	TSK_ID_VALVE_CTRL,//3
 	TSK_ID_MIX_CTRL,
 	TSK_ID_SUBSTEP_CTRL,
 	TSK_ID_MOTOR_CTRL,//6



 	TSK_ID_FLOW_POLL,
	TSK_ID_IDLE,//8
 	TSK_ID_TEMP_CTRL,
 	TSK_ID_EEP,//10


 	TSK_ID_PRINT,//11
 	TSK_ID_SHELL_RX,
 	TSK_ID_SHELL_TX,//13
 	TSK_ID_SCH,
 	TSK_ID_SCH_CLEAN,//15
 	TSK_ID_SCH_CALI,
 	TSK_ID_SCH_MEAS,//17
 	TSK_ID_SCH_IO,
 	TSK_ID_DATA_LOG,//19
 	TSK_ID_LOCAL_BUS,//20


 	TSK_ID_POLL_SCH_POLL,//21
	TSK_ID_MCU_STATUS,//22
 	TSK_ID_CAN_TSK,//23
 	TSK_ID_CAN_MASTER,
 	TSK_ID_CAN_IO,//25
	TSK_ID_MONITOR,//26
	TSK_ID_AUTO_RANGE,//27
 	MAX_TASK_ID

 } TaskId_e;



 enum
 {
 	MEAS_RANGE_LOW = 0,
 	MEAS_RANGE_HIGH = 1,
 	MEAS_RANGE_HIGH2,
 	MEAS_RANGE_HIGH_DILUSION,

 	MEAS_RANGE_MAX,
 };

extern  __IO uint32_t	freeRtosTskTick[MAX_TASK_ID];
extern uint16_t dummyRam;
extern uint16_t AdcVolts[ST_ADC_MAX_CHN];
extern int16_t hadc1080_Val[2];


extern __IO uint16_t measDataFlag;

#define NULL_T_DATA_OBJ		CONSTRUCT_SIMPLE_U16(&dummyRam, RAM)

extern uint32_t putValidNum;


extern osMessageQId SHELL_TX_ID;
extern osMessageQId SHELL_RX_ID;
extern osMessageQId PRINT_ID;
extern osMessageQId FLOW_SUBSTEP_ID;
extern osMessageQId FLOW_TSK_ID;
extern osMessageQId FLOW_MOTOR_CTL_ID;
extern osMessageQId FLOW_MIX_CTL_ID;
extern osMessageQId POLL_ID;
extern osMessageQId POLL_SCH_ID;
extern osMessageQId TEMPERATURE_ID;
extern osMessageQId SCH_ID;
extern osMessageQId MEAS_CTL_ID;
extern osMessageQId FLOW_VALVE_CTL_ID;
extern osMessageQId CAN_POLL_ID;
extern osMessageQId SPI_RDY_ID;
extern osMessageQId SCH_CALI_ID;
extern osMessageQId SCH_MEAS_ID;
extern osMessageQId SCH_CLEAN_ID;
extern osMessageQId SCH_IO_ID;
extern osMessageQId SCH_DATALOG_ID;
extern osMessageQId SCH_LB_ID;
extern osMessageQId MCU_STATUS_ID;
extern osMessageQId SDO_Q_ID;
extern osMessageQId CAN_IO_ID;
extern osMessageQId ADC_CONVERT_ID;
extern osMessageQId SCH_AUTORANGE_ID;

extern const char*		TskName[MAX_TASK_ID];
extern uint8_t printChnMap[MAX_TASK_ID];
extern uint8_t printMsgMap[MAX_TASK_ID];
extern uint8_t dbgMsgMap[MAX_TASK_ID];
extern uint8_t freeRtosTskState[MAX_TASK_ID];

extern osThreadId taskThreadID[MAX_TASK_ID];



#define adDetTaskHandle			taskThreadID[TSK_ID_AD_DETECT]
#define eepTaskHandle			taskThreadID[TSK_ID_EEP]
#define tskCan					taskThreadID[TSK_ID_CAN_TSK]
#define rxShell					taskThreadID[TSK_ID_SHELL_RX]
#define tmrCan					taskThreadID[TSK_ID_CAN_TMR]
#define nmtMaster				taskThreadID[TSK_ID_CAN_NMT]
//#define rxShell					taskThreadID[TSK_ID_SHELL_RX]
#define monitorTempTaskHandle	taskThreadID[TSK_ID_TEMP_MONITOR]
#define canopenTaskId			taskThreadID[TSK_ID_CAN_MASTER]


#define FlowTskStatus freeRtosTskState[TSK_ID_FLOW_CTRL]

#define NMT_TRIGGER			0x01

#define NEW_MEAS_DATA		0x01
#define SHELL_RX_DATA		0x01
#define RX_CAN_BUF2			0x02
#define TX_CAN_BUF2			0x01
#define ERR_CAN_2			0x08
#define CAN_SIG_INFO		(ERR_CAN_2 | RX_CAN_BUF2 | TX_CAN_BUF2)
#define CAN_TMR_EV			0x01
extern CAN_HandleTypeDef 	hcan2;
#define ptrCanDevice		(&hcan2)

typedef enum
{
	READ_ACCESS = 0x01u,
	WRITE_ACCESS=0x02u,

}ADR_RW_STATUS;

#define WDI_SET(x)			HAL_GPIO_WritePin(WDI_EN_GPIO_Port,WDI_EN_Pin, x);
#define WDI_TRIGGER			HAL_GPIO_TogglePin(WDI_GPIO_Port,WDI_Pin)


int main(int argc, char* argv[]);
void StartMonitorTask(void const * argument);
void StartMCUStatusTask(void const * argument);
void StartEEPTask(void const * argument);
void ReInit_Canopen(void);
void UpdateIOState(uint8_t id, uint16_t state);
void UpdateHeartbeat(uint32_t duringTime);
void AssertReset(void);
void ResetDevice(uint16_t type);
void AssertReaset(void);
void HAL_TIM9_IT(TIM_HandleTypeDef *htim);
void StartCanopenTask(void const * argument);
void StartCanIOTask(void const * argument);

void UnuseTskMsg(TSK_MSG* gMsg);

void SendTskMsg_INST(osThreadId thread_id, TSK_STATE tskState, uint32_t val, ptrTskCallBack ptrCall,uint32_t lineNum );

uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus);

uint16_t GetObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		OBJ_DATA* inst);
uint16_t GetName(uint16_t subID,  uint16_t objectIndex,OBJ_DATA* inst);
uint16_t GetAdrInfo(uint16_t subId, uint16_t objId,int16_t atrId, uint32_t* adr, uint16_t* len);
uint16_t GetAdr(uint16_t subID,  uint16_t objectIndex,int16_t attributeIndex,OBJ_DATA* inst);
uint16_t GetObjectType(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		uint16_t* type);

uint16_t PutObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		void* data);

uint16_t LoadDefaultCfg(uint16_t id);

osStatus MessagePush(uint32_t line, char* file, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
osStatus MessagePurge(uint32_t line, char* file, osMessageQId queue_id, uint32_t info,
		uint32_t millisec);
#define SendTskMsg(x,y,z,w) SendTskMsg_INST(x,y,z,w,__LINE__)
#define MsgPush(x,y,z)		MessagePush(__LINE__,__FILE__,x,y,z)
#define MsgPurge(x,y,z)		MessagePurge(__LINE__,__FILE__,x,y,z)
osStatus SignalPush(osThreadId thread_id, int32_t signal);
#define SigPush(x,y)		SignalPush(x,y)

#ifdef __cplusplus
 }
#endif
#endif /* INC_MAIN_H_ */
