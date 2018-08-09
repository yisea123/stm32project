//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "main.h"
#include "dev_eep.h"
#include "t_dataclass.h"

#include "unit_parameter.h"
#include "unit_rtc_cfg.h"
#include "unit_sys_info.h"
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



osMessageQId SHELL_TX_ID			= NULL;
osMessageQId SHELL_RX_ID			= NULL;
osMessageQId FILE_TSK_ID			= NULL;
osMessageQId USART_RX_EVENT			= NULL;
static const QueIDInit QID[]=
{
	{&SHELL_TX_ID, 			32},
	{&SHELL_RX_ID, 			8},
	{&FILE_TSK_ID,			16},
	{&USART_RX_EVENT,		2},
};

const T_UNIT*  subSystem[] =
{
		&rtcCfg,
		&parameter,
		&sysInfo,
};



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

};

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
		if(systemReset == 1)
		{
			NVIC_SystemReset();
		}
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
uint16_t LoadDefaultCfg(void)
{
	uint16_t ret = OK;
	for(uint16_t subID=0;	subID<IDX_SUB_MAX;	subID++)
	{
		const T_UNIT* subPtr = subSystem[subID];
 		if(subPtr)
		{
			ret |= subPtr->LoadRomDefaults(subPtr, ALL_DATACLASSES);
		}

	}
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
		//	subPtr->GetAttributeDescription(subPtr,objectIndex,attributeIndex, &atrDesp);
			return subPtr->Put(subPtr,objectIndex,attributeIndex,data);
		}
	}
	return FATAL_ERROR;
}





void vApplicationMallocFailedHook(void)
{

	dbg_printf("\n\nheap exhausted..");

	for (;;)
		;
}



__IO uint32_t kernelStarted = 0;
osMessageQId usbQueue;
int main(int argc, char* argv[])
{
  	bsp_init();
	// Send a greeting to the trace device (skipped on Release).
	trace_puts("Hello ARM World!\n");
	osMessageQDef(USBH_Queue, 10, uint16_t);
	usbQueue = osMessageCreate (osMessageQ(USBH_Queue), NULL);

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

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();
	kernelStarted = 1;

	/* Start scheduler */
	osKernelStart();
	return 1;
}


// ----------------------------------------------------------------------------
