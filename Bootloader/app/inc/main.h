/*
 * main.h
 *
 *  Created on: 2016Äê8ÔÂ23ÈÕ
 *      Author: pli
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include <stdio.h>
#include "diag/Trace.h"
#include "bsp.h"
#include "assert.h"
#ifdef __cplusplus
 extern "C" {
#endif
#define VALID_PASSWORD_ID			6000


#define TIME_UNIT						100u

#define VALVE_OFF_DELAY_TIME			100u
#define VALVE_OPEN_DELAY				100u
#define MOTOR_ACT_DELAY					100u


#define TSK_SW_TIME_N					800u //buffer for other task
#define VALVE_DURING_TIME				(VALVE_OFF_DELAY_TIME+VALVE_OPEN_DELAY+TSK_SW_TIME_N)




 //#define DEBUG_TEST

 #define IS_ROM_ADR(x)	(((x)>=0x08000000)&&((x)<0x08200000) )
 #define IS_RAM_ADR(x)	((x)<0x001FFFFF)


 typedef enum
 {
 	TSK_IDLE,
 	TSK_INIT,
 	TSK_SUBSTEP,
 	TSK_RESETIO,
 	TSK_FINISH,
 	TSK_FORCE_BREAK,
	TSK_FORCE_DISABLE,
	TSK_FORCE_ENABLE,
	TSK_RENEW_STATUS,
	TSK_STATE_MAX,
 } TSK_STATE;

 enum
 {
	 GET_NEW=0,
	 GET_EXIST,
 };

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
 	uint16_t msgState;
 	uint32_t threadId;
 } TSK_MSG;
#define TO_STR(x)	(char*)#x

 #pragma GCC diagnostic ignored "-Wpadded"
#define TASK_LOOP_ST			1






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
 	TSK_ID_TEMP_MONITOR,//8
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


 	MAX_TASK_ID

 } TaskId_e;




extern CAN_HandleTypeDef 	hcan2;
#define ptrCanDevice		(&hcan2)

typedef enum
{
	READ_ACCESS = 0x01u,
	WRITE_ACCESS=0x02u,

}ADR_RW_STATUS;


typedef enum
{
    OK = 0,                 //!< operation was successful

    // this warnings are allowed to ignore, cause the subsystem isn't influenced
    WARNING=1,             							//!< ignoreable codes
    LESSTHAN_RANGEMIN=1,   						//!< at least one value is less than its minimum value
    GREATERTHAN_RANGEMAX,  		 			//!< at least one value is greater than its maximum
    RANGE_TOO_LOW,          						//!< one value of range parameter is less than the allowed minimum value
    RANGE_TOO_HIGH,      			   				//!< one value of range parameter is greater than the maximum
    UPPER_RANGE_TOO_LOW,    					//!< upper value of range parameter is less than the allowed minimum value
    UPPER_RANGE_TOO_HIGH,  			 			//!< upper value of range parameter is greater than the maximum
    LOWER_RANGE_TOO_LOW,   		 			//!< lower value of range parameter is less than the allowed minimum value
    LOWER_RANGE_TOO_HIGH,  		 			//!< lower value of range parameter is greater than the maximum
    SPAN_TO_SMALL,         			 				//!< then span between two values is too small
    NOT_ON_GRID,            							//!< at least one value is not a multiple of its increment
    RULE_VIOLATION,        		 					//!< this shows a violation of one or more businessrules
    READ_ONLY,             		 					//!< written parameter is read only
    SENSOR_INTERFACE_ARM_VIOLATION,
    WRONG_STATE,            							//!< written parameter is read only in current subsystem state

    // this errors occur only while writing a parameter, the value was not written to the parameter
    ERROR_CODES=32,               			    			//!< start of not ignoreable codes
    WRITE_ERROR=32,             						//!< write access denied
    LESSTHAN_RANGEMIN_ERR=32,  		 		//!< at least one value is less than its minimum value
    GREATERTHAN_RANGEMAX_ERR, 		  		//!< at least one value is greater than its maximum
    RANGE_TOO_LOW_ERR,        		  			//!< one value of range parameter is less than the allowed minimum value
    RANGE_TOO_HIGH_ERR,       		  			//!< one value of range parameter is greater than the maximum
    UPPER_RANGE_TOO_LOW_ERR,  			  		//!< upper value of range parameter is less than the allowed minimum value
    UPPER_RANGE_TOO_HIGH_ERR, 			  		//!< upper value of range parameter is greater than the maximum
    LOWER_RANGE_TOO_LOW_ERR,   				//!< lower value of range parameter is less than the allowed minimum value
    LOWER_RANGE_TOO_HIGH_ERR, 		 	 	//!< lower value of range parameter is greater than the maximum
    SPAN_TO_SMALL_ERR,          					//!< then span between two values is too small
    NOT_ON_GRID_ERR,            						//!< at least one value is not a multiple of its increment
    RULE_VIOLATION_ERR,        			 			//!< this shows a violation of one or more businessrules
    READ_ONLY_ERR,              						//!< written parameter is read only
    WRONG_STATE_ERR,            					//!< written parameter is read only in current subsystem state
    EXCESS_CORRECTION_ATTEMPED_ERR,
    APPLIED_PROCESS_TOO_LOW_ERR,
    APPLIED_PROCESS_TOO_HIGH_ERR,
    SENSOR_INTERFACE_COMMUNICATION_ERR,		//!< fe hardware is not available due to an errer
    NEW_LRV_URV_OUTSIDE_SENSOR,
    BOTH_LRV_URV_OUTSIDE_LIMIT,
	BUSY_TIME_OUT,

    // this errorcodes are impossible if the programm work properly. during debuging
    // this codes shold stop the program-executing immediately
    DEBUG_ERROR=64,  						       //!< beginning of implementation-errors
    DECLINE=64,             							//!< DEBUG, operation isn't permitted in the aktual unit-state
	ILLEGAL_SUB_IDX,        							//!< DEBUG, unknown Object
    ILLEGAL_OBJ_IDX,        							//!< DEBUG, unknown Object
    ILLEGAL_ATTRIB_IDX,     						//!< DEBUG, unknown Attribute
    METHOD_NOT_SUPPORTED,   					//!< DEBUG, this data_object does not support the called method
    STATE_DENIED,           							//!< DEBUG, actual state denied the requested state
    ILLEGAL_STATE,          							//!< DEBUG, the requestet state is illegal
    ILLEGAL_DATACLASS_INDEX,					//!< DEBUG, unknown Data-Class
	ERROR_SPI_DATA,
	DEVICE_BUSY_STATE,
    // this errorcodes occures if proper function of the subsystem ist guaranteed any longer
    // e.g. the subsystem attributes are corrupted.
    //@@ maybe an exception for fatal errors?
    FATAL_ERROR=96,         						//!< start of fatal-errors
    ERROR_NV_STORAGE=96,    						//!< attributes inside the nv-storage are corrupted
    ERROR_RAM_STORAGE,      						//!< attributes inseid the ram are corrupted
    ERROR_HW,                							//!< hardware used by the subsystem doesn't work
	ERROR_DATA_LENGTH,

} FRESULT;

typedef struct {
	uint32_t startAdr;
	uint32_t endAdr;
	uint32_t rwStatus;
	uint16_t* ptrState;
}AdrInfo;

#define CST_LEN 16
typedef struct
{
	uint16_t DeviceID;
	uint8_t compileDate[CST_LEN];
	uint8_t compileTime[CST_LEN];
	uint8_t DeviceType[CST_LEN];
	uint8_t Type_Info[CST_LEN];
	uint8_t SW_Info[3];
	uint8_t HW_Info[3];
	uint32_t VersionCode;
}SysInfoST;
extern const SysInfoST _sysInfo;
extern SysInfoST sysInfoApp;
extern uint32_t appVersion;
int main(int argc, char* argv[]);
void StartMCUStatusTask(void const * argument);
void StartEEPTask(void const * argument);
void ResetDevice(uint16_t type);
void AssertReaset(void);





extern const uint16_t   	burstLength;
extern uint16_t 			deviceStatus;


typedef void *		OS_RSEMA;


void OS_Use(OS_RSEMA);
void OS_Unuse(OS_RSEMA);
OS_RSEMA OS_CreateSemaphore(void);

#define  TraceMsg(...)		PrintChn(NULL,1,__VA_ARGS__)
#define  TraceDBG(...) 		PrintChn(NULL,2,__VA_ARGS__)
#define  TracePrint(...) 	PrintChn(NULL,0,__VA_ARGS__)
#define  TraceUser(...) 	PrintChn(NULL,3,0xFFFF,__VA_ARGS__)

uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus);
uint16_t EraseRequest(uint32_t adr, uint16_t len);
typedef enum
{
	INIT_STATE = 1,
	RX_STATE = 2,
	TX_STATE = 4,
	FLASH_STATE = 8,
	CYCLE_STATE = 16,
	RX_HANDLE = 32,
	TX_HANDLE = 64,
}ClassState;


#define TX_RX_STATE		(RX_STATE|TX_STATE)
extern uint32_t 			communicationState;
extern uint16_t 			deviceStatus;
#ifdef __cplusplus
 }
#endif
#endif /* INC_MAIN_H_ */
