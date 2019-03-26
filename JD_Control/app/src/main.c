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

uint16_t                printCtrlCfg[MSG_TYPE_MAX] = {0,1,1,1};

static const QueIDInit QID[]=
{
	{&SHELL_TX_ID, 			32},
	{&SHELL_RX_ID, 			8},
	{&FILE_TSK_ID,			16},
	{&USART_RX_EVENT,		2},
	{&PRINT_ID,				4},
	{&SCH_LB_ID,			4},
};

const T_UNIT*  subSystem[] =
{
		&rtcCfg,
	//	&parameter,
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


const T_UNIT* GetSubsystem(uint16_t subID)
{
	assert(subID < IDX_SUB_MAX);
	return subSystem[subID];
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

void Adc_Setup()
{

	uint16_t data = 0x1000;
	TraceUser("检测到  AD7190 !\n");
	weight_ad7190_conf();

	uint32_t weight_Zero_Data = weight_ad7190_ReadAvg(6);
	TraceUser("zero:%d\n",weight_Zero_Data);
	TraceUser("硬石DAC（AD5689）模块模拟量电压输出\n");

	AD5689_Init();
	AD5689_WriteUpdate_DACREG(DAC_A,data);
	AD5689_WriteUpdate_DACREG(DAC_B,0xFFFF-data);
	TraceUser("data:%d\n",data);
	if (AD7190_Init() == 0)
	{
		TraceUser("获取不到 AD7190 !\n");
		while (1)
		{
			HAL_Delay(1000);
			if (AD7190_Init())
				break;
		}
	}
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
	/* init code for LWIP */
	MX_LWIP_Init();

	/* init code for USB_HOST */
	MX_USB_HOST_Init();

	/* init code for FATFS */
	MX_FATFS_Init();
	Adc_Setup();
	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();
	kernelStarted = 1;


	/* Start scheduler */
	osKernelStart();
	return 1;
}


// ----------------------------------------------------------------------------
