//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
#include <stdarg.h>
#include <string.h>
#include "main.h"
#include "rtc.h"
#include "flash_if.h"
#include "gpio.h"
#include "trace.h"

static uint16_t romRWState[24] = {0,0,0,};

enum
{
	RAW_DATA,
	ERASED,
	FLASHING,
};



#define COMPUTE_BUILD_YEAR \
    ( \
        (pD[ 7] - '0') * 1000 + \
        (pD[ 8] - '0') *  100 + \
        (pD[ 9] - '0') *   10 + \
        (pD[10] - '0') \
    )


#define COMPUTE_BUILD_DAY \
    ( \
        ((pD[4] >= '0') ? (pD[4] - '0') * 10 : 0) + \
        (pD[5] - '0') \
    )
const uint8_t* pD = 0x8020000;
const uint8_t* pT = 0x8020000;


#define BUILD_MONTH_IS_JAN (pD[0] == 'J' && pD[1] == 'a' && pD[2] == 'n')
#define BUILD_MONTH_IS_FEB (pD[0] == 'F')
#define BUILD_MONTH_IS_MAR (pD[0] == 'M' && pD[1] == 'a' && pD[2] == 'r')
#define BUILD_MONTH_IS_APR (pD[0] == 'A' && pD[1] == 'p')
#define BUILD_MONTH_IS_MAY (pD[0] == 'M' && pD[1] == 'a' && pD[2] == 'y')
#define BUILD_MONTH_IS_JUN (pD[0] == 'J' && pD[1] == 'u' && pD[2] == 'n')
#define BUILD_MONTH_IS_JUL (pD[0] == 'J' && pD[1] == 'u' && pD[2] == 'l')
#define BUILD_MONTH_IS_AUG (pD[0] == 'A' && pD[1] == 'u')
#define BUILD_MONTH_IS_SEP (pD[0] == 'S')
#define BUILD_MONTH_IS_OCT (pD[0] == 'O')
#define BUILD_MONTH_IS_NOV (pD[0] == 'N')
#define BUILD_MONTH_IS_DEC (pD[0] == 'D')


#define COMPUTE_BUILD_MONTH \
    ( \
        (BUILD_MONTH_IS_JAN) ?  1 : \
        (BUILD_MONTH_IS_FEB) ?  2 : \
        (BUILD_MONTH_IS_MAR) ?  3 : \
        (BUILD_MONTH_IS_APR) ?  4 : \
        (BUILD_MONTH_IS_MAY) ?  5 : \
        (BUILD_MONTH_IS_JUN) ?  6 : \
        (BUILD_MONTH_IS_JUL) ?  7 : \
        (BUILD_MONTH_IS_AUG) ?  8 : \
        (BUILD_MONTH_IS_SEP) ?  9 : \
        (BUILD_MONTH_IS_OCT) ? 10 : \
        (BUILD_MONTH_IS_NOV) ? 11 : \
        (BUILD_MONTH_IS_DEC) ? 12 : \
        /* error default */  0 \
    )

#define COMPUTE_BUILD_HOUR ((pT[0] - '0') * 10 + pT[1] - '0')
#define COMPUTE_BUILD_MIN  ((pT[3] - '0') * 10 + pT[4] - '0')
#define COMPUTE_BUILD_SEC  ((pT[6] - '0') * 10 + pT[7] - '0')


#define BUILD_DATE_IS_BAD (pD[0] == '?')

#define BUILD_YEAR  ((BUILD_DATE_IS_BAD) ? 99 : COMPUTE_BUILD_YEAR)
#define BUILD_MONTH ((BUILD_DATE_IS_BAD) ? 99 : COMPUTE_BUILD_MONTH)
#define BUILD_DAY   ((BUILD_DATE_IS_BAD) ? 99 : COMPUTE_BUILD_DAY)

#define BUILD_TIME_IS_BAD (pT[0] == '?')

#define BUILD_HOUR  ((BUILD_TIME_IS_BAD) ? 99 :  COMPUTE_BUILD_HOUR)
#define BUILD_MIN   ((BUILD_TIME_IS_BAD) ? 99 :  COMPUTE_BUILD_MIN)
#define BUILD_SEC   ((BUILD_TIME_IS_BAD) ? 99 :  COMPUTE_BUILD_SEC)



static const AdrInfo ST32Adr[] =
{
	{ADDR_FLASH_SECTOR_0, ADDR_FLASH_SECTOR_1, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[0]},
	{ADDR_FLASH_SECTOR_1, ADDR_FLASH_SECTOR_2, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[1]},
	{ADDR_FLASH_SECTOR_2, ADDR_FLASH_SECTOR_3, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[2]},
	{ADDR_FLASH_SECTOR_3, ADDR_FLASH_SECTOR_4, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[3]},
	{ADDR_FLASH_SECTOR_4, ADDR_FLASH_SECTOR_5, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[4]},
	{ADDR_FLASH_SECTOR_5, ADDR_FLASH_SECTOR_6, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[5]},
	{ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_7, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[6]},
	{ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_8, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[7]},
	{ADDR_FLASH_SECTOR_8, ADDR_FLASH_SECTOR_9, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[8]},
	{ADDR_FLASH_SECTOR_9, ADDR_FLASH_SECTOR_10, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[9]},
	{ADDR_FLASH_SECTOR_10, ADDR_FLASH_SECTOR_11, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[10]},
	{ADDR_FLASH_SECTOR_11, ADDR_FLASH_SECTOR_12, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[11]},
	{ADDR_FLASH_SECTOR_12, ADDR_FLASH_SECTOR_13, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[12]},
	{ADDR_FLASH_SECTOR_13, ADDR_FLASH_SECTOR_14, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[13]},
	{ADDR_FLASH_SECTOR_14, ADDR_FLASH_SECTOR_15, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[14]},
	{ADDR_FLASH_SECTOR_15, ADDR_FLASH_SECTOR_16, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[15]},
	{ADDR_FLASH_SECTOR_16, ADDR_FLASH_SECTOR_17, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[16]},
	{ADDR_FLASH_SECTOR_17, ADDR_FLASH_SECTOR_18, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[17]},
	{ADDR_FLASH_SECTOR_18, ADDR_FLASH_SECTOR_19, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[18]},
	{ADDR_FLASH_SECTOR_19, ADDR_FLASH_SECTOR_20, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[19]},
	{ADDR_FLASH_SECTOR_20, ADDR_FLASH_SECTOR_21, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[20]},
	{ADDR_FLASH_SECTOR_21, ADDR_FLASH_SECTOR_22, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[21]},
	{ADDR_FLASH_SECTOR_22, ADDR_FLASH_SECTOR_23, (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[22]},
	{ADDR_FLASH_SECTOR_23, ADDR_FLASH_SECTOR_E , (uint32_t)(READ_ACCESS|WRITE_ACCESS), &romRWState[23]},
};

static uint16_t EraseFlashAdr(uint32_t adr, uint16_t len)
{
	uint16_t state = FATAL_ERROR;

	for(uint16_t idx = 0; idx < sizeof(ST32Adr)/sizeof(AdrInfo);idx++)
	{
		if(adr ==ST32Adr[idx].startAdr)
		{
			if(*(ST32Adr[idx].ptrState) < ERASED)
			{
				//erase
				deviceStatus = DEVICE_BUSY_STATE;
				trace_printf("Erase one %d\n", HAL_GetTick());
				FLASH_If_EraseSector(adr);
				trace_printf("Erase End %d\n", HAL_GetTick());
				deviceStatus = OK;
				state = OK;
				*(ST32Adr[idx].ptrState) = ERASED;
			}
			break;
		}
		else if((adr+len-1) <= ST32Adr[idx].endAdr)
		{
			//flash directly
			*(ST32Adr[idx].ptrState) = RAW_DATA;
			state = OK;
			break;
		}
	}
	return state;
}

uint16_t EraseRequest(uint32_t adr, uint16_t len)
{
	uint16_t state = FATAL_ERROR;

	for(uint16_t idx = 0; idx < sizeof(ST32Adr)/sizeof(AdrInfo);idx++)
	{
		if(adr == ST32Adr[idx].startAdr)
		{
			if(*(ST32Adr[idx].ptrState) < ERASED)
			{
				//erase
				deviceStatus = DEVICE_BUSY_STATE;
			}
			break;
		}
		else if((adr+len-1) <= ST32Adr[idx].endAdr)
		{
			//flash directly
			state = OK;
			break;
		}
	}
	return state;
}

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


#define MAX_DATA_LEN_ROM		512
typedef struct
{
	uint32_t adrWrite;
	uint16_t lenWrite;
	uint16_t status;
	uint8_t  data[MAX_DATA_LEN_ROM];
}MemBuffer;


#define MEM_BUF_SIZE	64

static MemBuffer memBuff[MEM_BUF_SIZE];

MemBuffer* GetMemBuff(uint16_t type)
{
	static uint16_t newIdx = 0;
	static uint16_t getIdx = 0;
	MemBuffer* ptrBuff = NULL;
	if(type==GET_NEW)
	{
		ptrBuff = &memBuff[(newIdx++)%MEM_BUF_SIZE];

		if(ptrBuff->status == 0)
		{
			ptrBuff->status = 0x10;
		}
		else
		{
			newIdx--;
			return NULL;
		}
	}
	else
	{
		if(getIdx != newIdx)
		{
			ptrBuff = &memBuff[(getIdx++)%MEM_BUF_SIZE];
		}
	}
	return ptrBuff;

}


static volatile uint32_t flashRequest = 0;


void Flash2Rom(uint32_t adr,uint16_t len,uint8_t* ptrData)
{
	if((len >0) &&(len <= MAX_DATA_LEN_ROM))
	{
		MemBuffer* msg = GetMemBuff(GET_NEW);
		msg->adrWrite = adr;
		msg->lenWrite = len;
		memcpy(msg->data, ptrData, msg->lenWrite);
		EraseRequest(msg->adrWrite, msg->lenWrite);

		flashRequest++;
		trace_printf("flash request:%d;\t %d\n", flashRequest, HAL_GetTick());
	}
	else
	{
		trace_printf("flash request Error???:%d;\t %d\n", flashRequest, HAL_GetTick());
	}

}

void AssertReset(void)
{
	ResetDevice(1);
}

void ResetDevice(uint16_t type)
{
	NVIC_SystemReset();
}

void PrintChn(uint8_t* msgTable,uint8_t enabled, uint16_t chn, const char* str, ...)
{
#define MAX_PRINT_SIZE  256
	static uint8_t printBuff[MAX_PRINT_SIZE];
	{
		va_list arp;
		uint16_t len = 0;
		//lint -e586
		va_start(arp, str);
		//lint -e586
		len += (uint8_t)vsnprintf((char*)printBuff[len], MAX_PRINT_SIZE-len , str,arp);
		//lint -e586
		va_end(arp);

		assert(len < MAX_PRINT_SIZE);

		trace_write((const char*)&printBuff[0],len+1,chn);
	}
}

typedef struct
{
	uint32_t adr;
	uint32_t len;
	uint32_t status;
}RomSector;



uint32_t JumpAddress;
pFunction Jump_To_Application;


static const uint32_t	segAdr[] =
{
		ADDR_FLASH_SECTOR_0 ,
		ADDR_FLASH_SECTOR_1 ,
		ADDR_FLASH_SECTOR_2 ,
		ADDR_FLASH_SECTOR_3 ,
		ADDR_FLASH_SECTOR_4 ,
		ADDR_FLASH_SECTOR_5 ,
		ADDR_FLASH_SECTOR_6 ,
		ADDR_FLASH_SECTOR_7 ,
		ADDR_FLASH_SECTOR_8 ,
		ADDR_FLASH_SECTOR_9 ,
		ADDR_FLASH_SECTOR_10,
		ADDR_FLASH_SECTOR_11,
		ADDR_FLASH_SECTOR_12,
		ADDR_FLASH_SECTOR_13,
		ADDR_FLASH_SECTOR_14,
		ADDR_FLASH_SECTOR_15,
		ADDR_FLASH_SECTOR_16,
		ADDR_FLASH_SECTOR_17,
		ADDR_FLASH_SECTOR_18,
		ADDR_FLASH_SECTOR_19,
		ADDR_FLASH_SECTOR_20,
		ADDR_FLASH_SECTOR_21,
		ADDR_FLASH_SECTOR_22,
		ADDR_FLASH_SECTOR_23,
		ADDR_FLASH_SECTOR_E,
};

typedef struct
{
	uint32_t size;
	uint32_t revert;
	uint32_t checkSumAdr;
	uint16_t rev;
	uint16_t checkSum;
}CheckSumAdr;

#define CheckSumLen		sizeof(CheckSumAdr)
SysInfoST sysInfoApp;
uint32_t appVersion = 0;

static uint32_t GetValidRom(uint32_t appAdr)
{
	uint32_t endAdr = ADDR_FLASH_SECTOR_9;
	uint16_t size = sizeof(segAdr)/sizeof(uint32_t);

	for(uint16_t i=0; i < size;i++)
	{
		if(segAdr[i] > appAdr)
		{
			uint32_t len = segAdr[i] - appAdr;
			const CheckSumAdr* ptrCheck = (CheckSumAdr*)(segAdr[i] - CheckSumLen);

			if(ptrCheck->size == len)
			{
				if(ptrCheck->revert == ~len)
				{
					if(ptrCheck->checkSumAdr == segAdr[i]-2)
					{
						endAdr = segAdr[i];
						memcpy((void*)&sysInfoApp, (void*)(endAdr-256), sizeof(sysInfoApp));

						pD = sysInfoApp.compileDate;
						pT = sysInfoApp.compileTime;


						appVersion = (uint32_t)((BUILD_YEAR-2000)*100000000);
						appVersion += (BUILD_MONTH*1000000);
						appVersion += (BUILD_DAY*10000);
						appVersion += (BUILD_HOUR*100);
						appVersion += (BUILD_MIN);

						break;
					}
				}
			}
		}
	}
	return endAdr;
}




static void StartApp(uint32_t endAdr)
{


	//todo: check Rom CRC Ok;
	uint16_t crc = (uint16_t)crc16_ccitt((void*)APPLICATION_ADDRESS, (endAdr - APPLICATION_ADDRESS-2));
	uint16_t crcCheck = *(uint16_t*)(endAdr-2);
	if(crcCheck == crc)
	{
		if ((((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0xFF000000 ) == 0x20000000) || \
				  (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0xFF000000 ) == 0x10000000))
		{
		  /* Jump to user application */
		  JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		  Jump_To_Application = (pFunction) JumpAddress;
		  __disable_irq();
		  /* Initialize user application's Stack Pointer */
		  __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		  Jump_To_Application();
		}
	}
	else
	{
		trace_printf("APP checksum Error\n");
	}
}





int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	__enable_irq();
	MX_GPIO_Init();
	WDI_TRIGGER;
	uint32_t endAdr = GetValidRom(APPLICATION_ADDRESS);
	if(validPrintMsg != VALID_RST_MSG)
	{
		StartApp(endAdr);
	}
	FLASH_If_FlashUnlock();
	validPrintMsg = 0;
  	bsp_init();
  	WDI_TRIGGER;
	trace_puts("Hello ARM World - Bootloader!\n");

	trace_printf("Bootloader System clock: %u Hz\n", SystemCoreClock);
	LB_Init();

	for(int i =0; i<MEM_BUF_SIZE; i++)
	{
		memBuff[i].status = 0;
	}
	while(1)
	{
		WDI_TRIGGER;
		uint32_t state = communicationState & RX_HANDLE;
		HandleCanMsg_Bootloader(RX_HANDLE);
		HandleCanMsg_Bootloader(TX_HANDLE);

		if(flashRequest)
		{
			flashRequest--;
			MemBuffer* ptrMsg = GetMemBuff(GET_EXIST);
			if(ptrMsg)
			{

				uint16_t ret = EraseFlashAdr(ptrMsg->adrWrite,ptrMsg->lenWrite);
				WDI_TRIGGER;
				if(ret == OK)
				{
					for(uint32_t idx = 0; idx<ptrMsg->lenWrite;idx+=4)
					{
						FLASH_If_Write(ptrMsg->adrWrite+idx, *(uint32_t*)(&ptrMsg->data[idx]));
					}
				}
				WDI_TRIGGER;
				ptrMsg->status = 0;
			}
			else
			{
				trace_printf("Flash no memory %d\n", HAL_GetTick());
			}
		}

	//	if(RX_STATE )
	}
//	MX_FREERTOS_Init();


	/* Start scheduler */
//	osKernelStart();
	return 1;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
