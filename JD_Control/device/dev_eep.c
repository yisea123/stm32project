/**
  ******************************************************************************
  * @file    dev eep
  * @author  Paul Li
  * @version V1.0.0
  * @date    2016-05-09
  * @brief   This file provides all the EEPROM emulation firmware functions.
  ******************************************************************************
 
  ******************************************************************************
  */ 

/** @addtogroup EEPROM_Emulation
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "dev_eep.h"

#include "main.h"
#include "t_unit_head.h"
#include "shell_io.h"
#include "string.h"
#include "unit_weld_cfg.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Global variable used to store variable value in read sequence */
#define PAGE_DATA_START			16
#define PAGE_NUM_USED			0x02

#define EEP_VERSION			0x0006u

static osMutexId eep_mutex = NULL;
uint16_t 	EEP_INIT_CRC = 0x22FF;
static osMutexDef(eep_mutex);

static uint8_t eepBuff[EEP_DATA_RESERV_LEN];
#define 	EEP_RAM_START_BUFF		(&eepBuff[0])

static SemaphoreHandle_t lock = NULL;
__IO uint16_t 	eepStatus[2];
static __IO uint8_t triggerSaveCnt = 0;
uint32_t adrIdx = 0;
static const uint32_t adler32Init = 0x00000001;


static const oneSeg eepSegTable[EEP_PAGE_MAX] =
{
		{
				(SegInfo*)PAGE0_BASE_ADDRESS,
				(uint8_t*)(PAGE0_BASE_ADDRESS+sizeof(SegInfo)),
				(uint32_t*)(PAGE0_BASE_ADDRESS+PAGE_SIZE - 4),
		},
		{
				(SegInfo*)PAGE1_BASE_ADDRESS,
				(uint8_t*)(PAGE1_BASE_ADDRESS+sizeof(SegInfo)),
				(uint32_t*)(PAGE1_BASE_ADDRESS+PAGE_SIZE - 4),
		},
};


static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
  {
    sector = FLASH_SECTOR_11;
  }

  return sector;
}

//hal driver


/**
  * @brief  Erases the required FLASH Sectors.
  * @param  Address: Start address for erasing data
  * @retval 0: Erase sectors done with success
  *         1: Erase error
  */
static uint16_t FLASH_If_EraseSector(uint32_t Address)
{
	uint16_t ret = 0;
	uint32_t SectorError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = GetSector(Address);;
	EraseInitStruct.NbSectors = 1;
	HAL_FLASH_Unlock();
	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	you have to make sure that these data are rewritten before they are accessed during code
	execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	DCRST and ICRST bits in the FLASH_CR register. */
	if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
		ret = FATAL_ERROR;

	HAL_FLASH_Lock();
	return ret;
}





static EEP_STATUS EE_Format(uint16_t pageId)
{
	EEP_STATUS ret = EEP_OK;
	uint32_t flashStatus = 0;
	uint16_t startId = pageId;
	uint16_t endId = (uint16_t)(pageId+1);
	if(EEP_PAGE_MAX <= pageId)
	{
		startId = 0;
		endId = EEP_PAGE_MAX;
	}
	for(uint16_t id = startId; id < endId; id++)
	{
		flashStatus = FLASH_If_EraseSector((uint32_t)(eepSegTable[id].ptrSeginfo));
	//	TracePrint(TSK_ID_EEP,"EEP Format %d\n",id);
		/* If erase operation was failed, a Flash error code is returned */
		if (flashStatus != 0)
		{
		//	TraceDBG(TSK_ID_EEP,"EEP Format error, %d", pageId);
			ret =  EEP_ERASE_ERR;
		}
	}
	return ret;
}


//checksum calculation
static uint32_t adler32(const uint8_t *data, uint32_t len)
/* where data is the location of the data in physical memory and
                                                      len is the length of the data in bytes */
{
    uint32_t a = 1, b = 0;
    uint32_t index;
	const uint32_t MOD_ADLER = 65521;
	adrIdx = (uint32_t)data;
    
    /* Process each byte of the data in order */
    for (index = 0; index < len; ++index)
    {
        a = (uint32_t)((uint32_t)(a + data[index]) % MOD_ADLER);
        b = (uint32_t)((uint32_t)(b + a) % MOD_ADLER);
        adrIdx++;
    }
    
    return (b << 16) | a;
}

static uint32_t adler32_N(const uint8_t *data, uint32_t len, uint32_t initVal)
/* where data is the location of the data in physical memory and
                                                      len is the length of the data in bytes */
{
    uint32_t a = 1, b = 0;
    uint32_t index;
	const uint32_t MOD_ADLER = 65521;

	a = (initVal & 0xFFFF);
	b = (initVal & 0xFFFF0000)>>16;


    /* Process each byte of the data in order */
    for (index = 0; index < len; ++index)
    {
        a = (uint32_t)((a + data[index]) % MOD_ADLER);
        b = (uint32_t)((b + a) % MOD_ADLER);
    }

    return (b << 16) | a;
}


static EEP_STATUS FindLastValidSeg(uint16_t* ptrSegIndex)
{
	uint16_t index=0;
	EEP_STATUS ret = EEP_SEG_ERROR;
	uint8_t* ptrData;
	uint32_t checkSum;
	uint16_t validSeg[EEP_PAGE_MAX] = {0xFFFF,0xFFFF,};
	uint16_t segIndex=0;
	
	for(index=0;index<EEP_PAGE_MAX; index++)
	{
		if(eepSegTable[index].ptrSeginfo->pageStatus == VALID_PAGE_STATUS)
		{
			ptrData = (uint8_t*)eepSegTable[index].ptrSeginfo;
			checkSum = adler32(ptrData, SIZE_SEG_INFO-4);
			if(eepSegTable[index].ptrSeginfo->checkSum == checkSum)
			{
				ptrData = (uint8_t*)eepSegTable[index].dataAdr;
				checkSum = adler32(ptrData, eepSegTable[index].ptrSeginfo->validDataLen);
				if(*(eepSegTable[index].checkSum) == checkSum)
				{
					validSeg[segIndex++] = index;
					ret = EEP_OK;
				}
			}
		}
	}
	
	if(EEP_OK == ret)
	{
		uint32_t oldVersion = eepSegTable[ validSeg[0] ].ptrSeginfo->storeVersion;
		*ptrSegIndex = validSeg[0];
		for(index = 1; index< segIndex;index++)
		{
			if( oldVersion < eepSegTable[ validSeg[index] ].ptrSeginfo->storeVersion )
			{
				*ptrSegIndex = validSeg[index];
				oldVersion = eepSegTable[ validSeg[index] ].ptrSeginfo->storeVersion;
			}
		}
	}
	
	return ret;
}



static EEP_STATUS Flash2Rom(uint32_t adr, const uint8_t* data, uint32_t len)
{
	uint32_t index = 0;
	EEP_STATUS ret = EEP_OK;
	if(len %4 != 0)
		return EEP_WRITE_ERROR;
	HAL_FLASH_Unlock();

	for(index= 0; index < len; index+=4)
	{
		ret |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr + index, *(const uint32_t*)(data+index));
	}
	HAL_FLASH_Lock();
	return ret;
	
}	


static EEP_STATUS Flash2Seg(uint16_t segIndex, const SegInfo* ptrSegInfo, uint8_t* data, uint32_t len)
{

	EEP_STATUS ret = Flash2Rom( (uint32_t)eepSegTable[segIndex].ptrSeginfo, (uint8_t*)ptrSegInfo, SIZE_SEG_INFO);

	if( ret == EEP_OK)
	{
		uint32_t checkSum;
		if(segIndex < EEP_PAGE_MAX)
		{
			ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].dataAdr, data, len);
			checkSum = adler32_N(eepSegTable[segIndex].dataAdr, len, adler32Init);
		}
		ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].checkSum, (uint8_t*)&checkSum, 4);
	}
	return ret;
}


static EEP_STATUS WriteToPage(uint16_t segIndex,uint32_t storeVersion, uint8_t* data, uint32_t len)
{
	EEP_STATUS ret = EEP_WRITE_ERROR;
	SegInfo  info;
	info.eepVersion = EEP_VERSION;
	info.pageStatus = VALID_PAGE_STATUS;
	info.storeVersion = storeVersion+1;
	info.validDataLen = len;
	info.checkSum = adler32((uint8_t*)&info.pageStatus, SIZE_SEG_INFO-4);
	ret = EE_Format(segIndex);
	if(ret == EEP_OK)
	{
		ret = Flash2Seg(segIndex, &info, data, len);
	}
	return ret;
}


static EEP_STATUS EEP_Init(void)
{
	uint16_t validSegIndex = 0xFFFF;
	EEP_STATUS ret = FindLastValidSeg(&validSegIndex);
	if(eep_mutex == NULL)
		eep_mutex = osMutexCreate(osMutex(eep_mutex));
	if(ret == EEP_OK)
	{
	}
	else
	{
		if(eep_mutex)
			osMutexWait(eep_mutex, osWaitForever);
		ret = EE_Format(EEP_PAGE_MAX);//format all
		if(ret == EEP_OK)
		{
			ret = EEP_ERASED;
		}
		if(eep_mutex)
			osMutexRelease(eep_mutex);
	}
	return ret;
}

static EEP_STATUS EEP_ReadAdr(uint16_t adrRelative,uint8_t* data, uint16_t len)
{
	uint16_t validSegIndex = 0xFFFF;
	EEP_STATUS ret;
	if(eep_mutex)
		osMutexWait(eep_mutex, osWaitForever);
	ret = FindLastValidSeg(&validSegIndex);
	if(ret == EEP_OK)
	{
		if( (len + adrRelative) > eepSegTable[validSegIndex].ptrSeginfo->validDataLen )
		{
			ret = EEP_DATA_CONSISTANT_ERR;
		}
		else
		{
			memcpy((void*)data, eepSegTable[validSegIndex].dataAdr+adrRelative , len);
		}
	}
	else
	{
		ret = EE_Format(EEP_PAGE_MAX);//format all
		if(ret == EEP_OK)
		{
			//not retrn EEP_OK, as there is no valid data in EEP;
			ret = EEP_NO_VALID_DATA;
		}
	}
	if(eep_mutex)
		osMutexRelease(eep_mutex);
	return ret;
}


static EEP_STATUS EEP_Write(uint8_t* data, uint32_t len)
{
	uint16_t validPageIndex = 0;
	uint16_t pageWr = 0;
	EEP_STATUS ret ;
	uint32_t storeVersion = 0;

	if(EEP_DATA_RESERV_LEN < len)
		return EEP_ERROR_PARA;
	if(eep_mutex)
		osMutexWait(eep_mutex, osWaitForever);
	ret = FindLastValidSeg(&validPageIndex);

	if(ret != EEP_OK)
	{
		validPageIndex = 0;
		storeVersion = 0;
		ret = EE_Format(EEP_PAGE_MAX);//format all
	}
	else
	{
		storeVersion = eepSegTable[validPageIndex].ptrSeginfo->storeVersion;
	}

	if(ret == EEP_OK)
	{

		if(validPageIndex >= 1)//write to page 0
		{
			pageWr = 0;
		}
		else
		{
			pageWr = 1;
		}
		ret = WriteToPage(pageWr, storeVersion, data, len);
		TracePrint(TSK_ID_EEP,"\nEEP Write v,%d, Len, %d ",storeVersion, len);
	}
	if(eep_mutex)
		osMutexRelease(eep_mutex);
	return ret;
}


#ifdef TEST_EEP_ROM


static uint16_t TestResult[50];
static uint16_t testIdx = 0;
#define TEST_SIZE		500
static uint8_t testBuffR[TEST_SIZE];
static uint8_t testBuff[TEST_SIZE];
void TestCase()
{
	uint16_t idx = 0;
	//EEP Init;
	for(idx = 0;idx<TEST_SIZE;idx++)
	{
		testBuff[idx] = idx;
	}
	TestResult[testIdx++] = EEP_Init();
	TestResult[testIdx++] = EEP_Init();
	TestResult[testIdx++] = EEP_Init();
	testIdx = 10;
	TestResult[testIdx++] = EEP_Read(testBuffR, TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Read(testBuffR, TEST_SIZE);
	TestResult[testIdx++] = EEP_Read(testBuffR, TEST_SIZE/3);
	testIdx = 20;
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/3);	
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/3);
	//26
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/3);	
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/3);
	//32
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/3);	
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/2);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE/3);
	//38
	TestResult[testIdx++] = EEP_Write(testBuff,TEST_SIZE);
}


#endif






static uint16_t WriteAllData(uint8_t* data)
{
	uint16_t crccheck = 0;
	memcpy(eepBuff, data, EEP_DATA_SIZE);
	crccheck = CalcCrc16Mem_COMMON((uint8_t*) &(eepBuff[0]),
			EEP_INIT_CRC, EEP_DATA_SIZE);
	eepBuff[EEP_DATA_RESERV_LEN - 2] = (uint8_t)(crccheck / 256);
	eepBuff[EEP_DATA_RESERV_LEN - 1] = (uint8_t)(crccheck % 256);
	return EEP_Write(eepBuff,EEP_DATA_RESERV_LEN);
}


uint16_t ResetNVData(void)
{
	uint8_t* adr = (uint8_t*) EEP_RAM_START_ORIG;
	//todo the data is not restored when iic is error;
	uint16_t ret = LoadDefaultCfg(0xFFFF);
	//todo
	EE_Format(EEP_PAGE_MAX);
	WriteAllData(adr);
	eepStatus[0] = ret;
	//new cmd shall be implemented to restore the NV data;
	return ret;
}




static uint16_t GetLockCode(void)
{
	uint32_t CpuID[3];
	CpuID[0]=*(uint32_t*)(0x1ffff7e8);
	CpuID[1]=*(uint32_t*)(0x1ffff7ec);
	CpuID[2]=*(uint32_t*)(0x1ffff7f0);
	return (uint16_t) ((CpuID[0]>>5)+(CpuID[1]>>8)+(CpuID[2])) ^ ((CpuID[0]<<3)+(CpuID[2]>>8)+(CpuID[1]));
}



uint8_t NVRestore = 0;

uint16_t Init_EEPData(void)
{
	uint8_t* adr = (uint8_t*) EEP_RAM_START_ORIG;
	lock = OS_CreateSemaphore();
	EEP_STATUS ret = EEP_Init();
    devLock = EEP_INIT_CRC = InitCRC16_IIC( GetLockCode() );
	eepStatus[1] = OK;
	if(ret != EEP_OK)
	{
		eepStatus[0] = ERROR_NV_STORAGE;
	}
	else
	{
		EEP_ReadAdr(0, (void*)&eepBuff[0],EEP_DATA_RESERV_LEN);
		uint16_t crccheck = CalcCrc16Mem_COMMON((uint8_t*) &(eepBuff[0]),
					EEP_INIT_CRC, EEP_DATA_RESERV_LEN - 2);
		if ((eepBuff[EEP_DATA_RESERV_LEN - 2] == crccheck / 256)
				&& (eepBuff[EEP_DATA_RESERV_LEN - 1] == crccheck % 256))
		{
			memcpy(adr, eepBuff, EEP_DATA_SIZE);
			eepStatus[0] = ret = OK;
			devLock = 0;
		}
		else
		{
			eepStatus[0] = ret = FATAL_ERROR;
		}
	}

	if (ret != EEP_OK)
	{
		eepStatus[0] = ERROR_NV_STORAGE;
		LoadDefaultCfg(0xFFFF);
		TraceMsg(TSK_ID_EEP,"EEP storage is wrong \n");
	//	NVRestore = 1;
	}
	else
	{
		eepStatus[0] = OK;
		TraceMsg(TSK_ID_EEP,"EEP init OK! \n");
	}
	if(NVRestore)
	{
		ResetNVData();
		eepStatus[0] = OK;
	}
	return ret;
}


uint16_t Trigger_EEPSaveInst(uint8_t* adr, uint16_t len, uint8_t sync, uint32_t _line)
{
	if(eepStatus[1] != OK)
	{
		devLock = 100;
		return OK;
	}
	if (len != 0)
	{
		uint32_t adrSegIdx = ((uint32_t)adr - EEP_RAM_START_ORIG);

		if(adrSegIdx > EEP_DATA_SIZE)
		{
			TraceDBG(TSK_ID_EEP,"EEP storage Overrun: 0x%x- \n",adrSegIdx, _line);
		}
		
		
		OS_Use(lock);
		uint32_t adrDiff = ((uint32_t) adr - EEP_RAM_START_ORIG);
		if (adrDiff <= EEP_DATA_SIZE)
		{
			void* dst = (void*) (EEP_RAM_START_BUFF + adrDiff);
			if (0 != memcmp(dst, (void*)adr, len))
			{
				memcpy(dst, adr, len);
				triggerSaveCnt++;
				if ((SYNC_IM == sync) && (eepTaskHandle!= NULL))
					SigPush(eepTaskHandle, EEP_SIGNAL_SAVE);
			}
		}
		OS_Unuse(lock);

	}
	return OK;
}

#define EEP_STORAGE_TIME	(60000)//1 minutes

void StartEEPTask(void const * argument)
{
	(void)argument;
	void* dst = (void*) EEP_RAM_START_ORIG;
	osEvent evt;
	TraceMsg(TSK_ID_EEP,"EEP task started  \n");
	while (1)
	{
		//first save when load rom defaults is called!
		if( (triggerSaveCnt) && (eepStatus[1] == OK))
		{
			TraceMsg(TSK_ID_EEP,"EEP task saved once  \n");
			OS_Use(lock);
			triggerSaveCnt = 0;
			OS_Unuse(lock);
			eepStatus[1] = WriteAllData(dst);
		}
		if (evt.status == osEventSignal)
		{
			//save flash data
			if (evt.value.v & EEP_SIGNAL_RESET_SAVE)
				DeviceResetHandle(0,0);
				//Trigger_Save2FF(DeviceResetHandle);
		}
		evt = osSignalWait(EEP_SIGNAL, EEP_STORAGE_TIME);

	}
}

