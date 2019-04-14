//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "main.h"
#include "dev_eep.h"
#include "t_unit_head.h"
#include "unit_parameter.h"
#include "unit_rtc_cfg.h"
#include "unit_sys_info.h"
#include "unit_weld_cfg.h"
#include "shell_io.h"
#include "dev_ad5689.h"
#include "dev_ad7190.h"
void vApplicationMallocFailedHook(void);

extern void MX_FREERTOS_Init(void);

#pragma GCC diagnostic ignored "-Wpadded"

typedef struct {
	osMessageQId* ptrQid;
	uint16_t size;
}QueIDInit;
typedef struct {
	uint32_t startAdr;
	uint32_t endAdr;
	uint32_t rwStatus;
	uint16_t (*GetMem)(uint32_t adr, uint8_t* data, uint16_t len);
	uint16_t (*PutMem)(uint32_t adr, uint8_t* data, uint16_t len);
}AdrInfo;
#pragma GCC diagnostic pop

uint32_t	freeRtosTskTick[MAX_TASK_ID];
uint8_t freeRtosTskState[MAX_TASK_ID];
osMessageQId SHELL_TX_ID			= NULL;
osMessageQId SHELL_RX_ID			= NULL;
osMessageQId FILE_TSK_ID			= NULL;
osMessageQId USART_RX_EVENT			= NULL;
osMessageQId PRINT_ID = NULL;
osMessageQId SCH_LB_ID = NULL;
osMessageQId ADC_MONITOR = NULL;
osMessageQId WELD_CTRL = NULL;
osMessageQId MOTOR_CTRL = NULL;
osMessageQId PWM_CTRL = NULL;
osMessageQId SCH_CTRL = NULL;
osMessageQId CURR_CALI = NULL;
uint16_t dummyRam = 0;
uint16_t printCtrlCfg[MSG_TYPE_MAX] = {0,1,1,1};


const char* mainTskStateDsp[] =
{
		TO_STR(TSK_IDLE),
		TO_STR(TSK_INIT),
		TO_STR(TSK_SUBSTEP),
		TO_STR(TSK_RESETIO),
		TO_STR(TSK_FINISH),
		TO_STR(TSK_FORCE_BREAK),
		TO_STR(TSK_FORCE_STOP),
};

static const QueIDInit QID[]=
{
	{&SHELL_TX_ID, 			32},
	{&SHELL_RX_ID, 			8},
	{&FILE_TSK_ID,			16},
	{&USART_RX_EVENT,		2},
	{&PRINT_ID,				4},
	{&SCH_LB_ID,			4},
	{&ADC_MONITOR,			4},
	{&WELD_CTRL,			4},
	{&MOTOR_CTRL,			4},
	{&PWM_CTRL,				4},
	{&SCH_CTRL,				8},
};

const T_UNIT*  subSystem[] =
{
		&rtcCfg,
		&weldCfg,
		&sysInfo,

};

uint8_t		printChnMap[MAX_TASK_ID];
uint8_t		printMsgMap[MAX_TASK_ID];
uint8_t 	dbgMsgMap[MAX_TASK_ID];

const char* TskName[MAX_TASK_ID] =
{
	TO_STR(TSK_ID_AD_MONITOR),
	TO_STR(TSK_ID_EEP),
	TO_STR(TSK_ID_CAN_RX1),
	TO_STR(TSK_ID_CAN_TX1),
	TO_STR(TSK_ID_CAN_ERR),
	TO_STR(TSK_ID_FILE),
	TO_STR(TSK_ID_GUI),//6

	TO_STR(TSK_ID_SHELL_RX),
	TO_STR(TSK_ID_SHELL_TX),
	TO_STR(TSK_ID_GPIO),
	TO_STR(TSK_ID_TST),

	TO_STR(TSK_ID_CAN1_TSK),
	TO_STR(TSK_ID_LOCAL_BUS),
	TO_STR(TSK_ID_ADC_MONITOR),

};
void AssertReset(void)
{
	if(autoReset)
	{
		ResetDevice(1);
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


static const AdrInfo ST32Adr[] =
{
	//lint -e655
	{0x20000000, 0x20000000+0x30000, (uint32_t)(READ_ACCESS|WRITE_ACCESS), RamRdAccess, RamWrAccess},//ram
	//lint -e655
	{0x10000000, 0x10000000+0x10000, (uint32_t)(READ_ACCESS|WRITE_ACCESS), RamRdAccess, RamWrAccess},//ccram
	//lint -e655
	{0x08000000, 0x08000000+0x100000, (uint32_t)(READ_ACCESS), RamRdAccess, NULL},//rom
//	{0,			0x800000, READ_ACCESS|WRITE_ACCESS, FlashRdAccess, FlashWrAccess},
};

void ResetDevice(uint16_t type)
{
	extern uint16_t			systemReset;
	if(type == 0)
	{
		NVIC_SystemReset();
	}
	else
	{
		NVIC_SystemReset();
	}
}

//{0,			 0x800000},//flash

uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus)
{
	uint16_t ret = FATAL_ERROR;

	for(uint16_t idx = 0; idx < sizeof(ST32Adr)/sizeof(AdrInfo);idx++)
	{
		if(adr >=ST32Adr[idx].startAdr && (adr+len) < ST32Adr[idx].endAdr)
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

void vApplicationMallocFailedHook(void)
{

	dbg_printf("\n\nheap exhausted..");

	for (;;)
		;
}

void InitTaskMsg(TSK_MSG* ptrMsg)
{
	ptrMsg->callBackFinish = NULL;
	ptrMsg->callBackUpdate = NULL;
	ptrMsg->val.value = 0;
	ptrMsg->tskState = TSK_SUBSTEP;
	ptrMsg->msgState = 0xFFFF;
	ptrMsg->threadId = 0xFFFF;
}


const T_UNIT* GetSubsystem(uint16_t subID)
{
	assert(subID < IDX_SUB_MAX);
	return subSystem[subID];
}
#define MAX_G_MSG_QSIZE		180

static TSK_MSG gMsgQueue[MAX_G_MSG_QSIZE];

static TSK_MSG* GetNewTskMsg(uint16_t count)
{
	static uint32_t msgId = 0;
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

uint16_t LoadDefaultCfg(uint16_t id)
{
	uint16_t ret = OK;
	uint32_t revData = 0;
	uint16_t subId = id;
	uint16_t maxId = (uint16_t)(id + 1);
	if(id >= IDX_SUB_MAX)
	{
		subId = 0;
		maxId = IDX_SUB_MAX;
	}
	for(; subId<maxId; subId++)
	{
		const T_UNIT* subPtr = GetSubsystem(subId);
 		if(subPtr)
		{
			ret |= subPtr->LoadRomDefaults(subPtr, ALL_DATACLASSES);
			revData = (uint32_t)((long int)revData | (long int)(1<<subId));
		}
 		else
 		{
 			ret = FATAL_ERROR;
 			break;
 		}
	}
	return ret;
}


__IO uint32_t kernelStarted = 0;
uint16_t tt = 120;
uint8_t* ptrVal = &tt;
osMessageQId usbQueue;
int main(int argc, char* argv[])
{
  	bsp_init();
	// Send a greeting to the trace device (skipped on Release).
	trace_puts("Hello ARM World!\n");
	osMessageQDef(USBH_Queue, 10, uint16_t);
	usbQueue = osMessageCreate (osMessageQ(USBH_Queue), NULL);
	ptrVal[1] = 0x30;
  //	Init_RTC();
	CreateAllQid();
	// At this stage the system clock should have already been configured
	// at high speed.
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	Init_EEPData();


	for(uint16_t i = 0; i< sizeof(subSystem)/sizeof(const T_UNIT*);i++)
	{
		assert(subSystem[i]);
		//lint -e655 -e655 -e655
		subSystem[i]->Initialize(subSystem[i], INIT_HARDWARE|INIT_TASKS|INIT_CALCULATION|INIT_DATA);
	}


	/* init code for USB_HOST */
//	MX_USB_HOST_Init();

	/* init code for FATFS */
//	MX_FATFS_Init();

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();
	kernelStarted = 1;


	/* Start scheduler */
	osKernelStart();
	return 1;
}


// ----------------------------------------------------------------------------
