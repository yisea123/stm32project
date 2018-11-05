//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
#include <string.h>
#include "main.h"
#include "dev_eep.h"
#include "rtc.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "tsk_flow_all.h"

#include "dev_log_sp.h"
#include "unit_meas_cfg.h"
#include "unit_temp_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_flow_act.h"
#include "unit_rtc_cfg.h"

#include "unit_sch_cfg.h"
#include "unit_sys_info.h"
#include "unit_data_log.h"
#include "unit_meas_data.h"
#include "unit_io_cfg.h"
#include "unit_sys_diagnosis.h"
#include "unit_statistics_data.h"
#include "unit_hmi_setting.h"
#include "unit_cfg_ex.h"
#include "gpio.h"
#include "dev_spi.h"

void vApplicationMallocFailedHook(void);


#define TEST_BUFFER		1024
static uint8_t ram_test[TEST_BUFFER]			__attribute__ ((section (".ram_test")));

extern void MX_FREERTOS_Init(void);
uint16_t dummyRam = 0;

#pragma GCC diagnostic ignored "-Wpadded"

typedef struct {
	osMessageQId* ptrQid;
	uint16_t size;
}QueIDInit;

#pragma GCC diagnostic pop
osMessageQId MB_MICROSTEP			= NULL;
osMessageQId MB_MAINSTEP			= NULL;
osMessageQId MB_SUBSTEP				= NULL;
osMessageQId MB_PREMAIN				= NULL;
osMessageQId MB_PRESUB				= NULL;
osMessageQId MB_SPCTRL				= NULL;
osMessageQId MB_MOTOR				= NULL;
osMessageQId SHELL_TX_ID			= NULL;
osMessageQId SHELL_RX_ID			= NULL;
osMessageQId PRINT_ID				= NULL;
osMessageQId FLOW_SUBSTEP_ID		= NULL;
osMessageQId FLOW_MOTOR_CTL_ID		= NULL;
osMessageQId FLOW_MIX_CTL_ID		= NULL;
osMessageQId POLL_ID				= NULL;
osMessageQId TEMPERATURE_ID			= NULL;
osMessageQId SCH_ID					= NULL;
osMessageQId SCH_MEAS_ID			= NULL;
osMessageQId MEAS_CTL_ID			= NULL;
osMessageQId FLOW_VALVE_CTL_ID		= NULL;
osMessageQId CAN_POLL_ID			= NULL;
osMessageQId SPI_RDY_ID				= NULL;
osMessageQId SCH_CALI_ID			= NULL;
osMessageQId SCH_CLEAN_ID			= NULL;
osMessageQId SCH_IO_ID				= NULL;
osMessageQId SCH_DATALOG_ID			= NULL;
osMessageQId SCH_LB_ID				= NULL;
osMessageQId POLL_SCH_ID			= NULL;
osMessageQId MCU_STATUS_ID			= NULL;
osMessageQId SDO_Q_ID				= NULL;
osMessageQId CAN_IO_ID				= NULL;
osMessageQId ADC_CONVERT_ID			= NULL;
osMessageQId SCH_AUTORANGE_ID 		= NULL;


static const QueIDInit QID[]=
{
	{&SHELL_TX_ID, 			64},
	{&SHELL_RX_ID, 			8},
	{&PRINT_ID, 			64},
	{&SCH_ID,				32},
	{&FLOW_SUBSTEP_ID, 		16},
	{&FLOW_MOTOR_CTL_ID, 	16},
	{&FLOW_MIX_CTL_ID, 		16},
	{&FLOW_VALVE_CTL_ID, 	16},
	{&POLL_ID, 				16},
	{&TEMPERATURE_ID, 		16},
	{&MEAS_CTL_ID, 			8},
	{&CAN_POLL_ID, 			4},
	{&SPI_RDY_ID,			4},
	{&SCH_MEAS_ID,			4},
	{&SCH_CALI_ID,			4},
	{&SCH_CLEAN_ID,			4},
	{&SCH_DATALOG_ID,		16},
	{&SCH_LB_ID,			8},
	{&POLL_SCH_ID, 			4},
	{&MCU_STATUS_ID,		4},
	{&SCH_IO_ID,			4},
	{&SDO_Q_ID,				2},
	{&CAN_IO_ID,			8},
	{&ADC_CONVERT_ID,		4},
	{&SCH_AUTORANGE_ID,		4},
	{&MB_MICROSTEP,				4},
	{&MB_MAINSTEP,				4},
	{&MB_SUBSTEP,				4},
	{&MB_PREMAIN,				4},
	{&MB_PRESUB,				4},
	{&MB_SPCTRL,				4},
	{&MB_MOTOR,					4},
};


const T_UNIT* subSystem[IDX_SUB_MAX] =
{
	&flowCfg,//0
	&flowAct,//1
	&rtcCfg,//2
	&measCfg,//3
	&tempCfg,//4
	&STA_Data,//5
	&sysInfo,//6
//	&schCfg,//7
	&dataLog,//8
	&measData,//9
	&diagnosisCfg,//10
//	&IOCfg,//11
//	&HmiCfg,//12
//	&ExCfg,//13
};
//typedef uint16_t (*GetMem)(uint32_t adr, uint8_t* data, uint16_t len);
//typedef uint16_t (*PutMem)(uint32_t adr, uint8_t* data, uint16_t len);
typedef struct {
	uint32_t startAdr;
	uint32_t endAdr;
	uint32_t rwStatus;
	uint16_t (*GetMem)(uint32_t adr, uint8_t* data, uint16_t len);
	uint16_t (*PutMem)(uint32_t adr, uint8_t* data, uint16_t len);
}AdrInfo;

const char* mainTskStateDsp[] =
{
		TO_STR(TSK_IDLE),
		TO_STR(TSK_INIT),
		TO_STR(TSK_SUBSTEP),
		TO_STR(TSK_RESETIO),
		TO_STR(TSK_FINISH),
		TO_STR(TSK_FORCE_BREAK),
		TO_STR(TSK_FORCE_STOP),
		TO_STR(TSK_FORCE_DISABLE),
		TO_STR(TSK_FORCE_ENABLE),
		TO_STR(TSK_RENEW_STATUS),
};


const char* TskName[MAX_TASK_ID] =
{
		TO_STR(TSK_ID_ADC_T),
		TO_STR(TSK_ID_MEAS_),
		TO_STR(TSK_ID_FLOW_),
		TO_STR(TSK_ID_VALVE),//3
		TO_STR(TSK_ID_MIX_C),
		TO_STR(TSK_ID_SUBST),
		TO_STR(TSK_ID_M_CTR),//6
		TO_STR(TSK_ID_POLL_),
		TO_STR(TSK_ID_IDLE_),//8
		TO_STR(TSK_ID_TEMP_),
		TO_STR(TSK_ID_EEP__),//10
		TO_STR(TSK_ID_PRINT),//11
		TO_STR(TSK_SHELL_RX),
		TO_STR(TSK_SHELL_TX),//13
		TO_STR(TSK_ID_SCH__),
		TO_STR(TSK_SCH_CLEA),//15
		TO_STR(TSK_SCH_CALI),
		TO_STR(TSK_SCH_MEAS),//17
		TO_STR(TSK_SCH_IO__),
		TO_STR(TSK_DATA_LOG),//19
		TO_STR(TSK_LOCL_BUS),//20
		TO_STR(TSK_ID_SCH_P),//21
		TO_STR(TSK_MCU_STAT),//22
		TO_STR(TSK_CAN_TSK_),//23
		TO_STR(TSK_CAN_MAST),
		TO_STR(TSK___CAN_IO),//25
		TO_STR(TSK__MONITOR),//26
		TO_STR(TSK_AUTO_RAN),//27
		TO_STR(TSK_MICROSTP),
		TO_STR(TSK_MAINSTEP),
		TO_STR(TSK_SUB_STEP),
		TO_STR(TSK__PRE_SUB),
		TO_STR(TSK_PRE_MAIN),
		TO_STR(TSK_ID_MOTOR),
		TO_STR(TSK__POLLSCH),
		TO_STR(TSK_ID_SPCTL),
};

void StateFinishAct(TSK_MSG* ptrTask, uint16_t taskId, uint16_t ret, uint16_t result, uint32_t line)
{
	if(ptrTask)
	{
		if(ptrTask->callBackFinish)
		{
			ptrTask->callBackFinish(ret, result);
			ptrTask->callBackFinish = NULL;
			ptrTask->callBackUpdate = NULL;
		}
		else if(ptrTask->callBackUpdate)
		{
			ptrTask->callBackUpdate(ret, result);
			ptrTask->callBackFinish = NULL;
			ptrTask->callBackUpdate = NULL;
		}
		else
		{}
		TracePrint(taskId, "finish is called in line %d, result, %d , %d\n",line, ret,result);
	}
}


static uint16_t RamRdAccess(uint32_t adr, uint8_t* data, uint16_t len)
{
	memcpy((void*)data,(void*)adr,len);
	return OK;
}


static uint16_t RamWrAccess(uint32_t adr, uint8_t* data, uint16_t len)
{
	memcpy((void*)adr,(void*)data,len);
	return OK;
}

static uint16_t FlashRdAccess(uint32_t adr, uint8_t* data, uint16_t len)
{
	//static uint8_t buff[256];
	//todo
	memcpy((void*)data,(void*)adr,len);
	return OK;
}


static uint16_t FlashWrAccess(uint32_t adr, uint8_t* data, uint16_t len)
{
	memcpy((void*)adr,(void*)data,len);
	return OK;
}

#define MAX_G_MSG_QSIZE		128

static TSK_MSG gMsgQueue[MAX_G_MSG_QSIZE];

static TSK_MSG* GetNewTskMsg(uint16_t count)
{
	static uint16_t msgId = 0;
	TSK_MSG* msg = NULL;
	for(uint16_t i=0;i<MAX_G_MSG_QSIZE;i++)
	{
		if(gMsgQueue[msgId%MAX_G_MSG_QSIZE].msgState == 0)
		{
			msg = &gMsgQueue[msgId%MAX_G_MSG_QSIZE];
			msg->msgState = count;
			msgId++;
			break;
		}
		else
			msgId++;
	}
	assert(msg != NULL);
	return msg;
}
void SendTskMsg_LOC(osThreadId thread_id, TSK_MSG* msg,uint32_t lineNum  )
{
	msg->threadId = (uint32_t)thread_id;
	msg->tskState = TSK_SUBSTEP;
	msg->lineNum = lineNum;
	MsgPush(thread_id, (uint32_t) msg, 0);
}
void SendTskMsg_INST(osThreadId thread_id, TSK_STATE tskState, uint32_t val,ptrTskCallBack ptrCallFin, ptrTskCallBack ptrCallUpdate,uint32_t lineNum  )
{
	TSK_MSG* msg = GetNewTskMsg(1);
	msg->callBackFinish = ptrCallFin;
    msg->callBackUpdate = ptrCallUpdate;
	msg->val.value = val;
	msg->tskState = tskState;
	msg->threadId = (uint32_t)thread_id;
	msg->lineNum = lineNum;
	MsgPush(thread_id, (uint32_t) msg, 0);
}
void UnuseTskMsg(TSK_MSG* gMsg)
{
	if(gMsg && gMsg->msgState)
		gMsg->msgState--;
}



static const AdrInfo ST32Adr[] =
{
	//lint -e655
	{0x20000000, 0x20000000+0x30000, (uint32_t)(READ_ACCESS|WRITE_ACCESS), RamRdAccess, RamWrAccess},//ram
	//lint -e655
	{0x10000000, 0x10000000+0x10000, (uint32_t)(READ_ACCESS|WRITE_ACCESS), RamRdAccess, RamWrAccess},//ccram
	//lint -e655
	{0x08000000, 0x08000000+0x100000, (uint32_t)(READ_ACCESS), RamRdAccess, NULL},//rom
	{0,			0x800000, READ_ACCESS|WRITE_ACCESS, FlashRdAccess, FlashWrAccess},
};


//{0,			 0x800000},//flash

uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus)
{
	uint16_t ret = FATAL_ERROR;

	for(uint16_t idx = 0; idx < sizeof(ST32Adr)/sizeof(AdrInfo);idx++)
	{
		if(adr >=ST32Adr[idx].startAdr && (adr+len-1) < ST32Adr[idx].endAdr)
		{
			if(ST32Adr[idx].rwStatus & rwStatus )
			{
				ret = OK;
			}
			break;
		}
	}
	return ret;
}


static void CreateAllQid(void)
{
	uint16_t qNum = sizeof(QID)/sizeof(QueIDInit);
	for(uint16_t idx = 0; idx<qNum; idx++)
	{
		osMessageQDef(TSK_Queue, QID[idx].size, uint32_t);
		*(QID[idx].ptrQid) = osMessageCreate(osMessageQ(TSK_Queue), NULL);
	}
}

uint16_t LoadDefaultCfg(uint16_t id)
{
	uint16_t ret = OK;
	uint32_t revData = 0;
	uint16_t subId = 0;
	uint16_t maxId = IDX_SUB_MAX;
	if(id < IDX_SUB_MAX)
	{
		subId = id;
		maxId = id+1;
	}
	for(; subId<maxId; subId++)
	{
		const T_UNIT* subPtr = subSystem[subId];
 		if(subPtr)
		{
			ret |= subPtr->LoadRomDefaults(subPtr, ALL_DATACLASSES);
			revData = (uint32_t)(revData | (1<<subId));
		}
	}
	NewEventLog(EV_CFG_RST, (void*)&revData);
	return ret;
}

uint16_t GetObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		OBJ_DATA* inst)
{
	if((inst) && (subID < IDX_SUB_MAX))
	{
		const T_UNIT* subPtr = subSystem[subID];
		T_DO_ATTRIB_RANGE atrDesp;
		if(subPtr)
		{
			subPtr->GetAttributeDescription(subPtr,objectIndex,(uint16_t)attributeIndex, &atrDesp);
			if(atrDesp.dataType)
			{
				inst->type = atrDesp.dataType;
				return subPtr->Get(subPtr,objectIndex,attributeIndex,inst->data);
			}
		}
	}
	return FATAL_ERROR;

}

uint16_t GetAdrInfo(uint16_t subId, uint16_t objId,int16_t atrId, uint32_t* adr, uint16_t* len)
{
	uint16_t ret = ILLEGAL_SUB_IDX;
	int16_t  atrId1 = atrId;

	if(subId < IDX_SUB_MAX)
	{
		const T_UNIT* subPtr = subSystem[subId];

		if(subPtr)
		{
			if(atrId == WHOLE_OBJECT)
			{
				T_DO_OBJECT_DESCRIPTION objDesp;
				ret = subPtr->GetObjectDescription(subPtr,objId,&objDesp);
				if(ret == OK)
					*len = objDesp.objectLength;
				atrId1 = 0;
			}
			else
			{
				atrId1 = atrId;
				ret = OK;
			}
			if(ret == OK)
			{
				T_DO_ATTRIB_RANGE atrDesp;
				ret = subPtr->GetAttributeDescription(subPtr,objId,(uint16_t)atrId1, &atrDesp);

				if(ret == OK)
				{
					*adr = atrDesp.adr;
					if(atrId != WHOLE_OBJECT)
					{
						*len = atrDesp.len;
					}
				}
			}
		}
	}
	return ret;
}
uint16_t GetName(uint16_t subID,  uint16_t objectIndex,OBJ_DATA* inst)
{
	if((inst) && (subID < IDX_SUB_MAX))
	{
		const T_UNIT* subPtr = subSystem[subID];
		if(subPtr)
		{
			return subPtr->GetObjectName(subPtr,objectIndex, inst->data,&inst->len);
		}
	}
	return FATAL_ERROR;

}


uint16_t GetObjectType(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		uint16_t* type)
{
	if(subID < IDX_SUB_MAX)
	{
		const T_UNIT* subPtr = subSystem[subID];
		T_DO_ATTRIB_RANGE atrDesp;
		if(subPtr)
		{
			subPtr->GetAttributeDescription(subPtr,objectIndex,(uint16_t)attributeIndex, &atrDesp);
			*type = atrDesp.dataType;
			return OK;
		}
	}
	return FATAL_ERROR;
}

uint16_t PutObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		void* data)
{
	if((data) && (subID < IDX_SUB_MAX))
	{

		const T_UNIT* subPtr = subSystem[subID];
	//	T_DO_ATTRIB_RANGE atrDesp;
		if(subPtr)
		{
			uint8_t dataEvent[6];
			dataEvent[0] = (uint8_t)objectIndex;
			dataEvent[1] = (uint8_t)attributeIndex;
			dataEvent[2] = ((uint8_t*)data)[0];
			dataEvent[3] = ((uint8_t*)data)[1];
			dataEvent[4] = ((uint8_t*)data)[2];
			dataEvent[5] = ((uint8_t*)data)[3];
			if((subID !=8) && (subID !=9))
				NewEventLog((EV_CMD_PUT_OBJ|(uint32_t)subID), dataEvent);
		//	subPtr->GetAttributeDescription(subPtr,objectIndex,attributeIndex, &atrDesp);
			return subPtr->Put(subPtr,objectIndex,attributeIndex,data);
		}
	}
	return FATAL_ERROR;
}







void vApplicationMallocFailedHook(void)
{
	/* The malloc failed hook is enabled by setting
	 configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	 Called if a call to pvPortMalloc() fails because there is insufficient
	 free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	 internally by FreeRTOS API functions that create tasks, queues, software
	 timers, and semaphores.  The size of the FreeRTOS heap is set by the
	 configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	dbg_printf("\n\nheap exhausted..");

	for (;;)
		;
}


void AssertReset(void)
{
	if(autoReset)
	{
		ResetDevice(1);
	}
}

void ResetDevice(uint16_t type)
{
	extern uint16_t			systemReset;
	if(type == 0)
	{
		if(systemReset != 0)
		{
			NVIC_SystemReset();
		}
	}
	else
	{
		NVIC_SystemReset();
	}
}

static void DeviceReset(uint32_t id)
{
	(void)id;
	//send msg to shell to let it reset
	MsgPush ( SHELL_TX_ID, (uint32_t)0, 0);
}

void StartEEPTask(void const * argument)
{
	(void)argument; // pc lint
	const uint8_t taskID = TSK_ID_EEP;

	TracePrint(taskID,"EEP task started  \n");
	osEvent evt;

	while(TASK_LOOP_ST)
	{
		//first save when load rom defaults is called!

		freeRtosTskTick[taskID]++;
		Save_EEPData();
		if (evt.status == osEventSignal)
		{
			//save flash data
			if (evt.value.v & EEP_SIGNAL_RESET_SAVE)
				Trigger_Save2FF(DeviceReset);
		}
		evt = osSignalWait(EEP_SIGNAL, EEP_STORAGE_TIME);
	}
}


void Trigger_DeviceReset(void)
{
	SigPush(eepTaskHandle, EEP_SIGNAL_RESET_SAVE);
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	uint32_t revData = 0;
 	__enable_irq();

	SPI_SemInit();
	CreateAllQid();
	Init_PrintSem();
 	MX_GPIO_Init();
 	WDI_SET(GPIO_PIN_SET);
 	WDI_TRIGGER;
	bsp_init();

	for(uint16_t i=0;i<TEST_BUFFER;i++)
	{
		ram_test[i] = (uint8_t)i;
	}

//	delayms(30);
 	WDI_TRIGGER;
 	Init_RTC();
  	WDI_TRIGGER;

//  MX_CAN1_Init();
//	delayms(30);
//  Send a greeting to the trace device (skipped on Release).
	trace_puts("Hello ARM World!\n");
//	shell_printf("Hello ARM World!");
//	xprintf("Hello ARM World!");

	// At this stage the system clock should have already been configured
	// at high speed.
	trace_printf("System clock, %u Hz\n", SystemCoreClock);
	rtcCfg.Initialize(&rtcCfg, INIT_HARDWARE|INIT_TASKS|INIT_CALCULATION|INIT_DATA);
	//WDI_SET(GPIO_PIN_SET);
//	xprintf("System clock, %u Hz\n", SystemCoreClock);
//	Timer timer;
//	timer.start();
//	portENABLE_INTERRUPTS();
  	Init_EEPData();
  	WDI_TRIGGER;

  	for(uint16_t i = 0; i< sizeof(subSystem)/sizeof(const T_UNIT*);i++)
	{
		assert(subSystem[i]);
		//lint -e655 -e655 -e655
		if(&rtcCfg != subSystem[i])
		{
			uint16_t ret = subSystem[i]->Initialize(subSystem[i], INIT_HARDWARE|INIT_TASKS|INIT_CALCULATION|INIT_DATA);
			if( ret == WARNING)
			{
				revData = (uint32_t)(revData | (1<<i));
			}
		}
	}
	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();
	if(revData != 0)
		NewEventLog(EV_CFG_RST, (void*)&revData);
	printMsgMap[TSK_ID_TEMP_CTRL] = MAP_SHELL;
//	memset(printChnMap,0,sizeof(printChnMap));
//	memset(printMsgMap,0,sizeof(printMsgMap));
//	printMsgMap[TSK_ID_VALVE_CTRL] = MAP_SHELL;
//	printChnMap[TSK_ID_VALVE_CTRL] = MAP_SHELL;
//	printMsgMap[TSK_ID_TEMP_CTRL] = MAP_SHELL;
//	printChnMap[TSK_ID_MICROSTEP] = MAP_SHELL;
//	printMsgMap[TSK_ID_MICROSTEP] = MAP_SHELL;
//	printMsgMap[TSK_ID_MAINSTEP] = MAP_SHELL;
//	printMsgMap[TSK_ID_SUBSTEP] = MAP_SHELL;
//	printMsgMap[TSK_ID_PRESUB] = MAP_SHELL;
//	printMsgMap[TSK_ID_PREMAIN] = MAP_SHELL;
#if 0
	memset(printChnMap,0xFF,sizeof(printChnMap));
	memset(printMsgMap,0xFF,sizeof(printMsgMap));
//	memset(dbgMsgMap,0xFF,sizeof(dbgMsgMap));


	printMsgMap[TSK_ID_SCH_IO] = MAP_SHELL;
	printChnMap[TSK_ID_SCH_IO] = MAP_SHELL;

//	printMsgMap[TSK_ID_CAN_MASTER] = MAP_SHELL;
//	printMsgMap[TSK_ID_CAN_IO] = MAP_SHELL;

//	dbgMsgMap[TSK_ID_CAN_MASTER] = MAP_SHELL;
//	dbgMsgMap[TSK_ID_CAN_IO] = MAP_SHELL;

//	printChnMap[TSK_ID_LOCAL_BUS] = MAP_SHELL;
//	printMsgMap[TSK_ID_LOCAL_BUS] = MAP_SHELL;
//	dbgMsgMap[TSK_ID_LOCAL_BUS] = MAP_SHELL;

#endif

#if 0
	memset(printChnMap,0xFF,sizeof(printChnMap));
	memset(printMsgMap,0xFF,sizeof(printMsgMap));

	printChnMap[TSK_ID_LOCAL_BUS] = MAP_TRACE;
	printMsgMap[TSK_ID_LOCAL_BUS] = MAP_TRACE;
	dbgMsgMap[TSK_ID_LOCAL_BUS] = MAP_TRACE;

#endif
#if 0
	printChnMap[TSK_ID_DATA_LOG] = MAP_SHELL;
	printMsgMap[TSK_ID_DATA_LOG] = MAP_SHELL;
	dbgMsgMap[TSK_ID_DATA_LOG] = MAP_SHELL;
#endif


#if 0
	memset(printChnMap,0xFF,sizeof(printChnMap));
	memset(printMsgMap,0xFF,sizeof(printMsgMap));
	printChnMap[TSK_ID_SCH] = MAP_TRACE;
	printChnMap[TSK_ID_SCH_CLEAN] = MAP_TRACE;
	printChnMap[TSK_ID_SCH_CALI] = MAP_TRACE;
	printChnMap[TSK_ID_SCH_MEAS] = MAP_TRACE;
	printChnMap[TSK_ID_FLOW_CTRL] = MAP_TRACE;
	printChnMap[TSK_ID_SUBSTEP_CTRL] = MAP_TRACE;
	printChnMap[TSK_ID_VALVE_CTRL] = MAP_TRACE;
	printChnMap[TSK_ID_MIX_CTRL] = MAP_TRACE;
	printChnMap[TSK_ID_MOTOR_CTRL] = MAP_TRACE;

#endif
	/* Start scheduler */
	osKernelStart();
	return 1;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
