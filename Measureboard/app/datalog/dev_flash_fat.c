/*
 * dev_flash_fat.c
 *
 *  Created on: 2016��11��14��
 *      Author: pli
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"
#include "dev_log_sp.h"
#include "dev_logdata.h"
#include "dev_flash_fat.h"
#include "dev_spiflash.h"
#include "unit_rtc_cfg.h"
#include "dev_flash.h"
#include <string.h>

#define FATLIST_MAX_LEN		64


//used for the temp data storage; read/write
static uint8_t tempData[FF_SECTOR_SZ];
static FatList 	fatList[FF_FILE_COUNT];
static uint32_t fatListAdr[FF_FILE_COUNT];
FileInfo	fatFiles[FF_FILE_COUNT];



static FatInfo fatInfo =
{
	FF_HEAD_START_ADR,
	1,
	0,
	{0,0,0},
	NULL,
};




uint16_t FF_Init(void)
{
	uint16_t ret = OK;
	if(fatInfo.lock == NULL)
		fatInfo.lock = OS_CreateSemaphore();

	OS_Use(fatInfo.lock);
	ret = Flash_Init();
	fatInfo.fileEndAdr = FF_HEAD_START_ADR;
	fatInfo.isFlashRaw = true;
	fatInfo.isMount = true;
	OS_Unuse(fatInfo.lock);

	return ret;
}

static uint16_t CheckFileIntegrity(FatList* newFile, FatList* oldFile)
{
	uint16_t ret = OK;
	if(newFile->fatStartAdr != oldFile->fatStartAdr)	ret++;

	if(newFile->fatVersion != oldFile->fatVersion)	ret++;
	if(newFile->fatType != oldFile->fatType)	ret++;
	if(newFile->fatSegSize != oldFile->fatSegSize)	ret++;
	if(newFile->fatEndAdr != oldFile->fatEndAdr)	ret++;
	//??
	//todo
	if(newFile->fatCurrentAdr > oldFile->fatEndAdr)	ret++;

	return ret;
}

static uint16_t CheckValidData(uint8_t* data, uint16_t len)
{
	uint16_t ret = OK;
	uint16_t crc = *(uint16_t*)&data[len];
	if(crc != CalcCrc16Mem_COMMON(data, InitCRC16_COMMON(), len))
	{
		ret = FATAL_ERROR;
	}
	return ret;
}

static uint16_t UpdateFileList(uint8_t* data, uint16_t size, uint32_t adrStart)
{
	uint16_t ret = OK;
	uint16_t valid = 0;
	uint16_t newFileInfo = 0;

	for(uint16_t idx =0; idx<size; idx+= FF_FATLIST_SIZE)
	{
		if(OK == CheckValidData(&data[idx], FF_FATLIST_CRC_LEN))
		{
			FatList* tempFile = (FatList*)(&data[idx]);
			if(tempFile->fatVersion == FF_FAT_VERSION)
			{
				for (uint16_t i = 0; i < FF_FILE_COUNT; i++)
				{
					if (tempFile->fatType != NULL_FILE_LOG)
					{
						if (tempFile->fatType == fatList[i].fatType)
						{
							if(OK == CheckFileIntegrity(tempFile, &fatList[i]))
							{
								valid = 1;
								fatInfo.isFlashRaw  = false;

								if (tempFile->writeCount > fatList[i].writeCount)
								{
									newFileInfo = 1;
									memcpy((void*)&fatList[i], (void*)tempFile,
											sizeof(FatList));
									fatListAdr[i] = adrStart + idx;
									//fatInfo.fileEndAdr = adrStart + idx;
								}
							}
							break;
						}
						else
						{
						}
					}
				}

				if(valid == 0)
				{
					for (uint16_t i = 0; i < FF_FILE_COUNT; i++)
					{
						if (fatList[i].fatType == NULL_FILE_LOG)
						{
							memcpy((void*) &fatList[i], (void*) tempFile,
																	sizeof(FatList));
							valid = 1;
							break;
						}
					}
					//assert(valid == 0);
				}

			}
		}

	}
	//just use the next sector as the new file;
	if(newFileInfo )
		fatInfo.fileEndAdr = adrStart + size - FF_FATLIST_SIZE;

	return ret;
}
uint16_t FF_Mount(const LogData** inst, uint16_t num)
{
	//mount and init file list;
	uint16_t ret = OK;
	uint32_t startAdr = 0x0;
	uint32_t tempAdr;
	assert(num <= FF_FILE_COUNT);

	memset((void*)&fatList[0],0, sizeof(fatList));
	memset((void*)fatListAdr, 0, sizeof(fatListAdr));

	for(uint16_t i =0; i< num; i++)
	{
		fatList[i].dataCount = 0;
		fatList[i].writeCount = 0;
		fatList[i].fatVersion = FF_FAT_VERSION;
		fatList[i].fatType = inst[i]->fatType;
		fatList[i].fatStartAdr = startAdr;
		fatList[i].fatSegSize = inst[i]->dataLen;
		fatList[i].fatCurrentAdr = startAdr;
		fatList[i].time = 0;
		tempAdr = (uint32_t)(inst[i]->dataLen * inst[i]->logMaxTime);
		startAdr += (tempAdr+FF_SECTOR_SZ-1)/FF_SECTOR_SZ*FF_SECTOR_SZ + FF_REV_SECTORS * FF_SECTOR_SZ;
		fatList[i].fatEndAdr = startAdr;
		assert(startAdr >= FF_HEAD_SECTORS);
		fatFiles[i].logInst = inst[i];
		fatFiles[i].ptrFile = &fatList[i];
	}


	OS_Use(fatInfo.lock);
	for(uint16_t idx = 0; idx<FF_HEAD_SECTORS; )
	{
		ret |= Flash_ReadAdr((uint32_t)(FF_HEAD_START_ADR + idx*FF_SECTOR_SZ), tempData, FF_SECTOR_SZ);
		if(ret == OK)
		{
			UpdateFileList(tempData, FF_SECTOR_SZ,(uint32_t)( FF_HEAD_START_ADR + idx*FF_SECTOR_SZ));
		}
		else
		{
			break;
		}
		idx++;
	}
	OS_Unuse(fatInfo.lock);
	return ret;
}


//open, close
FileInfo* FF_Open(uint16_t fileType, uint16_t openType)
{
	FileInfo* ptrFile = NULL;
	OS_Use(fatInfo.lock);
	for(uint16_t i=0;i<FF_FILE_COUNT;i++)
	{
		if(fileType == fatFiles[i].ptrFile->fatType)
		{
			ptrFile = &fatFiles[i];
			ptrFile->openType = openType;
			ptrFile->readAdr = 0;
			break;
		}
	}
	OS_Unuse(fatInfo.lock);
	return ptrFile;
}
uint16_t FF_Close(FileInfo* ptrFile)//close file
{
	uint16_t ret = OK;
	assert(ptrFile);
	OS_Use(fatInfo.lock);
	if(ptrFile->openType)
	{
		ptrFile->openType = 0;
		ptrFile->readAdr = 0;
	}
	OS_Unuse(fatInfo.lock);
	return ret;
}


//write
uint16_t FF_InitFile(FileInfo* ptrFile)//init file storage,delete the old data;
{
	uint16_t ret = FATAL_ERROR;
	uint16_t fileListLen = sizeof(FatList);
	assert(ptrFile);
	OS_Use(fatInfo.lock);
	if(ptrFile->openType & OPEN_WRITE)
	{
		ptrFile->readAdr = 0;
		ptrFile->ptrFile->writeCount += 1;
		ptrFile->ptrFile->dataCount = 0;
		ptrFile->ptrFile->fatCurrentAdr = ptrFile->ptrFile->fatStartAdr;
		ptrFile->ptrFile->fatVersion = FF_FAT_VERSION;
		(void)RTC_Get(IDX_RTC_ST,0,(void*)&(ptrFile->ptrFile->time));
		//refresh all file list;

		if(!fatInfo.isFlashRaw )
		{
			fatInfo.fileEndAdr += FF_FATLIST_SIZE;
		}
		memcpy( (void*)&fatInfo.tempBuffer[0],(void*)ptrFile, sizeof(FatList));
		*(uint16_t*)&fatInfo.tempBuffer[fileListLen] = CalcCrc16Mem_COMMON(fatInfo.tempBuffer, InitCRC16_COMMON(), fileListLen);
		ret = Flash_WriteAdr(fatInfo.fileEndAdr,fatInfo.tempBuffer, (uint16_t)(fileListLen + CRC_LEN));
		fatInfo.isFlashRaw  = false;


	}
	OS_Unuse(fatInfo.lock);
	return ret;
}
uint16_t FF_Append_Write(FileInfo* file, uint8_t* data, uint16_t len)//get all file include the address, size;..etc
{
	uint16_t ret = OK;
	uint16_t fileListLen = sizeof(FatList);

	FatList* ptrFile = file->ptrFile;

	assert(ptrFile);
	assert(ptrFile->fatSegSize >= len );
	OS_Use(fatInfo.lock);
	if(!fatInfo.isFlashRaw)
		fatInfo.fileEndAdr += FF_FATLIST_SIZE;

	if(fatInfo.fileEndAdr >= FF_HEAD_END_ADR)
	{
		//dump all file list to the start adr;
		fatInfo.fileEndAdr = FF_HEAD_START_ADR;
	}


	if(ptrFile->dataCount>=1)
		ptrFile->fatCurrentAdr += ptrFile->fatSegSize;
	else
		ptrFile->fatCurrentAdr = ptrFile->fatStartAdr;


	if(ptrFile->fatCurrentAdr >= ptrFile->fatEndAdr)
	{
		ptrFile->fatCurrentAdr = ptrFile->fatStartAdr;
	}

	(void)RTC_Get(IDX_RTC_ST,0,(void*)&(ptrFile->time));
	ptrFile->writeCount++;
	ptrFile->dataCount++;
	memcpy( (void*)&fatInfo.tempBuffer[0],(void*)ptrFile, sizeof(FatList));
	*(uint16_t*)&fatInfo.tempBuffer[fileListLen] = CalcCrc16Mem_COMMON(fatInfo.tempBuffer, InitCRC16_COMMON(), fileListLen);

	//write head list;
	ret = Flash_WriteAdr(fatInfo.fileEndAdr,fatInfo.tempBuffer, (uint16_t)(fileListLen + CRC_LEN) );

	//write raw data
	ret |= Flash_WriteAdr(ptrFile->fatCurrentAdr,data, len);
	assert((uint16_t)(fileListLen + CRC_LEN) <= FF_FATLIST_SIZE);


	fatInfo.isFlashRaw  = false;
	//dump all file list
	if(fatInfo.fileEndAdr % FF_SECTOR_SZ == 0)
	{
		//dump all file list to this secotr;
		for(uint16_t i = 0; i<FF_FILE_COUNT; i++ )
		{
			if((fatList[i].fatType != NULL_FILE_LOG) && (&fatList[i] != ptrFile))
			{
				fatInfo.fileEndAdr += FF_FATLIST_SIZE;
				memcpy( (void*)&fatInfo.tempBuffer[0],(void*)&fatList[i], sizeof(FatList));
				*(uint16_t*)&fatInfo.tempBuffer[fileListLen] = CalcCrc16Mem_COMMON(fatInfo.tempBuffer, InitCRC16_COMMON(), fileListLen);
				ret |= Flash_WriteAdr(fatInfo.fileEndAdr,fatInfo.tempBuffer, (uint16_t)(fileListLen + CRC_LEN));
			}
		}
	}

	OS_Unuse(fatInfo.lock);
	return ret;
}


static uint16_t GetValidDataCount(FileInfo* file)
{

	assert(file);
	uint32_t validCount = file->ptrFile->dataCount;
	uint32_t maxCount = (file->ptrFile->fatEndAdr - file->ptrFile->fatStartAdr) / file->ptrFile->fatSegSize;
	if(maxCount < file->ptrFile->dataCount)
	{
		validCount -= ((FF_SECTOR_SZ - file->ptrFile->fatCurrentAdr%FF_SECTOR_SZ)/file->ptrFile->fatSegSize - 1);
	}
	file->validCount = validCount;
	return OK;
}




//start from 0;
uint16_t FF_ReadSeek(FileInfo* file, uint32_t* ptrIdx)//get all file include the address, size;..etc
{
	uint16_t ret = OK;
	assert(file);
	assert(ptrIdx);
	OS_Use(fatInfo.lock);
	GetValidDataCount(file);
	if(*ptrIdx < file->validCount)
	{
		ret = OK;
	}
	else
	{
		*ptrIdx = file->validCount-1;
		ret = WARNING;
	}
	file->readAdr = file->ptrFile->fatCurrentAdr - (*ptrIdx) * file->ptrFile->fatSegSize;
	if(file->readAdr < file->ptrFile->fatStartAdr)
	{
		file->readAdr = file->ptrFile->fatEndAdr - file->readAdr;
	}
	OS_Unuse(fatInfo.lock);
	return ret;
}


uint16_t FF_Append_Read(FileInfo* file, uint8_t* data, uint16_t len, uint32_t* ptrIdx, uint16_t direction)//get all file include the address, size;..etc
{
	uint16_t ret = OK;
	assert(file);
	assert(data);
	assert(len <= file->ptrFile->fatSegSize);

	OS_Use(fatInfo.lock);
	if(file->readAdr == 0)
	{
		file->readAdr  = file->ptrFile->fatCurrentAdr;
		file->readIdx = 0;
	}

	ret = Flash_ReadAdr(file->readAdr, data, (uint16_t)len);
	if(ret == OK)
	{
		if(READ_OLD == direction)
		{
			file->readAdr -= file->ptrFile->fatSegSize;
			file->readIdx ++;
			if(file->readAdr <file->ptrFile->fatStartAdr)
			{
				file->readAdr = file->ptrFile->fatEndAdr - file->ptrFile->fatSegSize;
			}
		}
		else
		{
			if(file->readIdx > 0)
			{
				file->readAdr += file->ptrFile->fatSegSize;
				file->readIdx--;
				if(file->readAdr >=file->ptrFile->fatEndAdr)
				{
					file->readAdr = file->ptrFile->fatCurrentAdr;
				}
			}
		}
	}

	*ptrIdx = file->readIdx;

	OS_Unuse(fatInfo.lock);
	return ret;
}

