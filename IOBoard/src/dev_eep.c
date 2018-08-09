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
//#include "flash_if.h"
#include "bsp.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Global variable used to store variable value in read sequence */
#define PAGE_DATA_START			16
#define ONE_SEG_SIZE			(512-16)//512 one seg; total 4 segs; //1 k
#define VALID_DATA_IN_ONE_SEG	(ONE_SEG_SIZE-4)
#define PAGE_NUM_USED			0x02				
#define RETRY_TIMES				0x03

#define EEP_VERSION			0x0006u

static osMutexId eep_mutex = NULL;
#define EEP_PRINT

static osMutexDef(eep_mutex);
typedef struct _segInfo
{
	uint16_t pageStatus;
	uint16_t eepVersion;
	uint32_t storeVersion;
	uint16_t validDataLen;
	uint16_t reserved;
	uint32_t checkSum;
}SegInfo;
typedef struct
{
	SegInfo* 	ptrSeginfo;
	uint8_t* 	dataAdr;
	uint32_t*	checkSum;
}oneSeg;

#define WHOLE_SEG_NUM		4
#define VALID_PAGE_STATUS		(0x5AA5)
#define SIZE_SEG_INFO			(sizeof(SegInfo))




static const oneSeg eepSegTable[WHOLE_SEG_NUM] =
{
		{
				(SegInfo*)PAGE0_BASE_ADDRESS,
				(uint8_t*)(PAGE0_BASE_ADDRESS+sizeof(SegInfo)),
				(uint32_t*)(PAGE0_BASE_ADDRESS+PAGE_SIZE/2 - 4),
		},
		{
				(SegInfo*)(PAGE0_BASE_ADDRESS+PAGE_SIZE/2),
				(uint8_t*)(PAGE0_BASE_ADDRESS+sizeof(SegInfo)+PAGE_SIZE/2),
				(uint32_t*)(PAGE0_BASE_ADDRESS + PAGE_SIZE - 4),
		},
		{
				(SegInfo*)PAGE1_BASE_ADDRESS,
				(uint8_t*)(PAGE1_BASE_ADDRESS+sizeof(SegInfo)),
				(uint32_t*)(PAGE1_BASE_ADDRESS+PAGE_SIZE/2 - 4),
		},
		{
				(SegInfo*)(PAGE1_BASE_ADDRESS+PAGE_SIZE/2),
				(uint8_t*)(PAGE1_BASE_ADDRESS+sizeof(SegInfo)+PAGE_SIZE/2),
				(uint32_t*)(PAGE1_BASE_ADDRESS + PAGE_SIZE - 4),
		}
};
//hal driver


/**
  * @brief  Erases the required FLASH Sectors.
  * @param  Address: Start address for erasing data
  * @retval 0: Erase sectors done with success
  *         1: Erase error
  */
uint16_t FLASH_If_EraseSector(uint32_t Address)
{
	  FLASH_EraseInitTypeDef EraseInitStruct;
	  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	  EraseInitStruct.PageAddress = Address;
	  EraseInitStruct.NbPages     = 1;
	  uint32_t PAGEError = 0;

		HAL_FLASH_Unlock();
	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register. */
	  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	  {
		  return 1;
	  }

		HAL_FLASH_Lock();
	  return 0;
}





static EEP_STATUS EE_Format(uint16_t adr)
{
	EEP_STATUS ret = EEP_OK;
	uint32_t FlashStatus = 0;
	if((adr == 0x0) || (adr > PAGE_NUM_USED ))
	{
		/* Erase Page0 */
		FlashStatus = FLASH_If_EraseSector(PAGE0_BASE_ADDRESS);
		EEP_PRINT("\nEEP Format 0x0");
		/* If erase operation was failed, a Flash error code is returned */
		if (FlashStatus != 0)
		{
			ret =  EEP_ERASE_ERR;
		}
	}
	if(adr >= 0x01)
	{
		/* Erase Page1 */
		FlashStatus = FLASH_If_EraseSector(PAGE1_BASE_ADDRESS);
		EEP_PRINT("\nEEP Format 0x1");
		/* If erase operation was failed, a Flash error code is returned */
		if (FlashStatus != 0)
		{
			ret =  EEP_ERASE_ERR;
		}
	}
  /* Return Page1 erase operation status */
  return ret;
}

uint32_t adrIdx = 0;
static const uint32_t adler32Init = 0x00000001;
//checksum calculation
uint32_t adler32(const uint8_t *data, uint32_t len)
/* where data is the location of the data in physical memory and
                                                      len is the length of the data in bytes */
{
    uint32_t a = 1, b = 0;
    uint32_t index;
	const int MOD_ADLER = 65521;
	adrIdx = (uint32_t)data;
    
    /* Process each byte of the data in order */
    for (index = 0; index < len; ++index)
    {

        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
        adrIdx++;
    }
    
    return (b << 16) | a;
}

uint32_t adler32_N(const uint8_t *data, uint32_t len, uint32_t initVal)
/* where data is the location of the data in physical memory and
                                                      len is the length of the data in bytes */
{
    uint32_t a = 1, b = 0;
    uint32_t index;
	const int MOD_ADLER = 65521;

	a = (initVal & 0xFFFF);
	b = (initVal & 0xFFFF0000)>>16;


    /* Process each byte of the data in order */
    for (index = 0; index < len; ++index)
    {
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}

//find blank page
static EEP_STATUS FindBlankSeg(uint16_t pageNum, uint16_t* ptrSegIndex)
{
	uint16_t index =0;
	uint16_t end = 1;
	EEP_STATUS ret = EEP_SEG_ERROR;
	EEP_STATUS status;
	uint16_t i;	
	uint8_t* ptrData;
	if(pageNum == 0)
	{
		index = 0;
		end = 1;
	}
	else
	{
		index = 2;
		end = 3;
	}
	for(;index<=end; index++)
	{
		status = EEP_OK;
		if(eepSegTable[index].ptrSeginfo->pageStatus != VALID_PAGE_STATUS)
		{
			ptrData = (uint8_t*)eepSegTable[index].ptrSeginfo;
			for(i=0;i<SIZE_SEG_INFO;i++)
			{
				if(ptrData[i] != 0xFF)
				{
					status = EEP_SEG_ERROR;
					break;
				}
			}
			if(status == EEP_OK )
			{
				ret = EEP_OK;
				*ptrSegIndex = index;
				break;
			}
		}
	}
	return ret;
}

static EEP_STATUS FindLastValidSeg(uint16_t* ptrSegIndex)
{
	uint16_t index=0;
	EEP_STATUS ret = EEP_SEG_ERROR;
	uint8_t* ptrData;
	uint32_t checkSum;
	uint16_t validSeg[WHOLE_SEG_NUM] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,};
	uint16_t segIndex=0;
	
	for(index=0;index<WHOLE_SEG_NUM; index++)
	{
		if(eepSegTable[index].ptrSeginfo->pageStatus == VALID_PAGE_STATUS)
		{
			ptrData = (uint8_t*)eepSegTable[index].ptrSeginfo;
			checkSum = adler32(ptrData, SIZE_SEG_INFO-4);
			if(eepSegTable[index].ptrSeginfo->checkSum == checkSum)
			{
#if 0
				if(eepSegTable[index].ptrSeginfo->eepVersion != EEP_VERSION)
				{
					
				}
				else//not valid data in eeprom
#endif
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



static EEP_STATUS Flash2Rom(uint32_t adr, const uint8_t* data, uint16_t len)
{
	uint16_t index = 0;
	EEP_STATUS ret = EEP_OK;
	uint32_t adr1;
	if(len %4 != 0)
		return EEP_WRITE_ERROR;
	HAL_FLASH_Unlock();

	for(index= 0; index < len; )
	{
		adr1 = adr + index;

		ret |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr1, *(const uint32_t*)(data+index));
		index += 4;
	}
	HAL_FLASH_Lock();
	return ret;
	
}	
static EEP_STATUS Flash2Seg(uint16_t segIndex, const SegInfo* ptrSegInfo, uint8_t* data)	
{
	uint32_t checkSum = adler32((uint8_t*)data, ptrSegInfo->validDataLen);
	EEP_STATUS ret = Flash2Rom( (uint32_t)eepSegTable[segIndex].ptrSeginfo, (uint8_t*)ptrSegInfo, SIZE_SEG_INFO);
	ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].dataAdr, (uint8_t*)data, ptrSegInfo->validDataLen);
	ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].checkSum, (uint8_t*)&checkSum, 4);
	
	return ret;
	
}

static EEP_STATUS Flash2Seg_Relative(uint16_t segIndex, const SegInfo* ptrSegInfo, uint8_t* data, uint16_t validSegIndex, uint16_t adr, uint16_t len)
{

	EEP_STATUS ret = Flash2Rom( (uint32_t)eepSegTable[segIndex].ptrSeginfo, (uint8_t*)ptrSegInfo, SIZE_SEG_INFO);


	if( ret == EEP_OK)
	{
		uint32_t checkSum;
		if(validSegIndex < WHOLE_SEG_NUM)
		{
			ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].dataAdr, eepSegTable[validSegIndex].dataAdr, adr);
			ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].dataAdr+adr, data, len);
			ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].dataAdr+adr+len, eepSegTable[validSegIndex].dataAdr+adr+len, (ptrSegInfo->validDataLen-len-adr));

			checkSum = adler32_N(eepSegTable[validSegIndex].dataAdr, adr, adler32Init);
			checkSum = adler32_N(data, len, checkSum);
			checkSum = adler32_N(eepSegTable[validSegIndex].dataAdr+adr+len, (ptrSegInfo->validDataLen-len-adr), checkSum);
		}
		else
		{
			ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].dataAdr+adr, data, len);
			checkSum = adler32((uint8_t*)eepSegTable[segIndex].dataAdr, ptrSegInfo->validDataLen);
		}

		ret |= Flash2Rom( (uint32_t)eepSegTable[segIndex].checkSum, (uint8_t*)&checkSum, 4);
	}
	else
	{
	}
	return ret;
}

#if 0
static EEP_STATUS WriteToPage(uint16_t pageNum,uint32_t storeVersion,uint8_t* data, uint16_t len)	
{
	uint16_t segIndex = 0;
	EEP_STATUS ret = EEP_WRITE_ERROR;
	SegInfo  info;
	info.eepVersion = EEP_VERSION;
	info.pageStatus = VALID_PAGE_STATUS;
	info.reserved = 0x0101;
	info.storeVersion = storeVersion+1;
//	info.validDataLen = len;
	info.validDataLen = VALID_DATA_IN_ONE_SEG;
	info.checkSum = adler32((uint8_t*)&info.pageStatus, SIZE_SEG_INFO-4);

	ret = FindBlankSeg(pageNum, &segIndex);
	if(ret == EEP_OK)
	{
		ret = Flash2Seg(segIndex, &info, data);
	}
	else
	{
		ret = EE_Format(pageNum);
		if(ret == EEP_OK)
		{							
			segIndex = pageNum*2;
			ret = Flash2Seg(segIndex, &info, data);
			if(ret != EEP_OK)
			{
				segIndex = pageNum*2 +1;
				ret = Flash2Seg(segIndex, &info, data);
			}
		}
		else
		{
		}
	}
	return ret;
}

#endif
static EEP_STATUS WriteToPageAdr(uint16_t pageNum,uint32_t validSegIndex,uint8_t* data, uint16_t adr, uint16_t len)
{
	uint16_t segIndex = 0;

	EEP_STATUS ret = EEP_WRITE_ERROR;
	SegInfo  info;
	info.eepVersion = EEP_VERSION;
	info.pageStatus = VALID_PAGE_STATUS;
	info.reserved = 0x0101;
	info.storeVersion = eepSegTable[validSegIndex].ptrSeginfo->storeVersion+1;
//	info.validDataLen = len;
	info.validDataLen = VALID_DATA_IN_ONE_SEG;
	info.checkSum = adler32((uint8_t*)&info.pageStatus, SIZE_SEG_INFO-4);

	ret = FindBlankSeg(pageNum, &segIndex);

	if(ret == EEP_OK)
	{
		ret = Flash2Seg_Relative(segIndex, &info, data,validSegIndex, adr,len);
	}
	else
	{
		ret = EE_Format(pageNum);
		if(ret == EEP_OK)
		{
			segIndex = pageNum*2;
			ret = Flash2Seg_Relative(segIndex, &info, data, validSegIndex, adr,len);
			if(ret != EEP_OK)
			{
				segIndex = pageNum*2 +1;
				ret = Flash2Seg_Relative(segIndex, &info, data,validSegIndex, adr,len);
			}
		}
		else
		{
		}
	}
	return ret;
}
EEP_STATUS EEP_Init(void)		
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
		ret = EE_Format(0x03);//format all	
		if(ret == EEP_OK)
		{
			ret = EEP_ERASED;
		}
		if(eep_mutex)
			osMutexRelease(eep_mutex);
	}
	return ret;
}

#if 0
static EEP_STATUS EEP_Read(uint8_t* data, uint16_t len)
{
	uint16_t validSegIndex = 0xFFFF;

	EEP_STATUS ret;
	if(eep_mutex)
		osMutexWait(eep_mutex, osWaitForever);

	ret = FindLastValidSeg(&validSegIndex);
	if(ret == EEP_OK)
	{
		if( len > eepSegTable[validSegIndex].ptrSeginfo->validDataLen )
		{
			ret = EEP_DATA_CONSISTANT_ERR;
		}
		else
		{
			memcpy(data, eepSegTable[validSegIndex].dataAdr, len);
		}
	}
	else
	{
		ret = EE_Format(0x03);//format all
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



static EEP_STATUS EEP_Write(uint8_t* data, uint16_t len)
{
	uint16_t validSegIndex = 0;
	uint16_t retry = 0;
	uint16_t pageWr = 0;
	EEP_STATUS ret ;
	
	if(VALID_DATA_IN_ONE_SEG < len)
		return EEP_ERROR_PARA;
	if(eep_mutex)
		osMutexWait(eep_mutex, osWaitForever);
	ret = FindLastValidSeg(&validSegIndex);
	if(ret != EEP_OK)
	{		
		ret = EE_Format(0x03);//format all
	}
	if(ret == EEP_OK)
	{
		if(validSegIndex >= 2)//write to page 0
		{
			pageWr = 0;
		}
		else
		{
			pageWr = 1;
		}
		do
		{
			ret = WriteToPage(pageWr, eepSegTable[validSegIndex].ptrSeginfo->storeVersion, data, len);
			retry++;
		}
		while((ret != EEP_OK) && (retry < RETRY_TIMES));
	}
	if(eep_mutex)
		osMutexRelease(eep_mutex);
	return ret;
}


#endif
EEP_STATUS EEP_ReadAdr(uint16_t adrRelative,uint8_t* data, uint16_t len)
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
			memcpy(data, eepSegTable[validSegIndex].dataAdr+adrRelative , len);
		}
	}
	else
	{
		ret = EE_Format(0x03);//format all
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


EEP_STATUS EEP_WriteAdr(uint16_t adrRelative, uint8_t* data, uint16_t len)
{
	uint16_t validSegIndex = 0;
	uint16_t retry = 0;
	uint16_t pageWr = 0;
	EEP_STATUS ret ;

	if(VALID_DATA_IN_ONE_SEG < len)
		return EEP_ERROR_PARA;
	if(eep_mutex)
		osMutexWait(eep_mutex, osWaitForever);
	ret = FindLastValidSeg(&validSegIndex);
	if(ret != EEP_OK)
	{
		ret = EE_Format(0x03);//format all
	}
	if(ret == EEP_OK)
	{
		if(validSegIndex >= 2)//write to page 0
		{
			pageWr = 0;
		}
		else
		{
			pageWr = 1;
		}
		do
		{
			ret = WriteToPageAdr(pageWr, validSegIndex, data, adrRelative, len);
			EEP_PRINT("\nEEP Write Adr: %d, Len: %d ",adrRelative, len);
			retry++;
		}
		while((ret != EEP_OK) && (retry < RETRY_TIMES));
	}
	if(eep_mutex)
		osMutexRelease(eep_mutex);
	return ret;
}

#if 0 

EEP_STATUS EEP_WriteAdr(uint16_t adrRelative, uint8_t* data, uint16_t len)		
{
	uint16_t validSegIndex = 0;
	uint16_t retry = 0;
	uint16_t pageWr = 0;
	EEP_STATUS ret ;
	
	if(VALID_DATA_IN_ONE_SEG < (adrRelative+len))
		return EEP_ERROR_PARA;
	ret = FindLastValidSeg(&validSegIndex);
	if(ret != EEP_OK)
	{		
		ret = EE_Format(0x03);//format all
	}
	if(ret == EEP_OK)
	{
		if(validSegIndex >= 2)//write to page 0
		{
			pageWr = 0;
		}
		else
		{
			pageWr = 1;
		}
		do
		{
			ret = WriteToPageRelative(pageWr, validSegIndex, adrRelative, data, len);
			retry++;
		}
		while((ret != EEP_OK) && (retry < RETRY_TIMES));
	}
	return ret;
}


#endif

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

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
