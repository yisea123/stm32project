/*
 * sd_file.c
 *
 *  Created on: 2016锟斤拷9锟斤拷29锟斤拷
 *      Author: pli
 */



#include <stdio.h>
#include <string.h>
#include "main.h"
#include "shell_io.h"
#include "ff.h"
#include "unit_parameter.h"
#include "parameter_idx.h"
#include "sd_file.h"
#include "rtc.h"
#define PRINTF1		shell_AddRTC
#define PAGE_SIZE_MAX		512

#define SD_PATH		"0:"


// file variables
//local variables
static 	FATFS 		fatfs_sd;
static 	FIL 		file_sd;
static 	uint8_t		currentFile[40];
static 	uint16_t	openFile = 0;

static uint16_t fatStatusOk = FATAL_ERROR;
static SemaphoreHandle_t sdFileLock = NULL;
static uint16_t	mkfsSet = 0;


uint16_t Mount_SD()
{
	if(sdFileLock == NULL)
		sdFileLock = OS_CreateSemaphore();
	fatStatusOk = f_mount( &fatfs_sd,SD_PATH,1);
	TraceMsg(TSK_ID_FILE,"SD Mount := %d (0==OK)\n",  fatStatusOk);

	if(mkfsSet)
	{
		fatStatusOk = f_mkfs(SD_PATH,0,0);
		fatStatusOk |= f_mount( &fatfs_sd,(const TCHAR*)SD_PATH,1);
		TraceMsg(TSK_ID_FILE,"SD format and mount := %d (0==OK)\n",  fatStatusOk);
	}
	return fatStatusOk;
}

uint16_t CreateNewFile_SD(const uint8_t *path,const uint8_t *filename, uint32_t size)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	if(fatStatusOk != OK)
	{
		Mount_SD();
	}
	OS_Use(sdFileLock);
	if(path)
	{
		/*lint -e586*/
		snprintf((char*)pathname,25, "%s%s",SD_PATH,path);
		ret = f_mkdir((const TCHAR*)pathname);
		if(FR_EXIST == ret)
			ret = OK;
	}
	if(ret == OK)
	{
		/*lint -e586*/
		if(openFile)
		{
			f_close(&file_sd);
			openFile = 0;
		}
		snprintf((char*)pathname,35, "%s%s/%s",SD_PATH,path,filename);
		ret = f_open(&file_sd, (const TCHAR*)pathname,FA_CREATE_ALWAYS | FA_WRITE);
		if((ret == FR_OK) && (size))
		{
			ret = f_lseek(&file_sd,size);
			ret |= f_sync(&file_sd);
		}
		openFile = 1;
		memcpy(currentFile,pathname,strlen(pathname));
		//ret |= f_close(&file_sd);
	}
	OS_Unuse(sdFileLock);

	TraceMsg(TSK_ID_FILE,"SD Create File :%s/%s= %d (0==OK)\n", path,filename, ret);
	return ret;

}




uint16_t WriteFile_SD(const uint8_t *filename, const uint8_t* buff, uint32_t size)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	if(fatStatusOk != OK)
	{
		ret = Mount_SD();
	}
	OS_Use(sdFileLock);
	if(ret == OK)
	{
		if(openFile)
		{
			f_close(&file_sd);
			openFile = 0;
		}
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s",SD_PATH,filename);
		ret = f_open(&file_sd, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_WRITE);
		if(ret == FR_OK)
		{
			ret = f_write(&file_sd, buff,size, &bw);
			if(ret == OK)
			{
				if(bw != size)
				{
					ret = DISK_FULL_WARNING;
				}
				else
				{

				}
			}
			else
			{

			}
		}
		ret |= f_close(&file_sd);
	}
	OS_Unuse(sdFileLock);
	TraceMsg(TSK_ID_FILE,"SD Write File : %s = %d (0==OK)\n", filename, ret);
	return ret;
}

uint16_t AppendFile_SD(const uint8_t *filename, uint8_t* buff, uint32_t size)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	if(fatStatusOk != OK)
	{
		ret = Mount_SD();
	}
	OS_Use(sdFileLock);
	if(ret == OK)
	{
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s",SD_PATH,filename);
		if(openFile)
		{
			if(0 != memcmp(currentFile,pathname,strlen(pathname)))
			{
				ret = f_open(&file_sd, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_WRITE);
				TraceMsg(TSK_ID_FILE,"SD Append Error File : %s = %d (0==OK)\n", filename, ret);
			}
		}
		else
		{
			ret = f_open(&file_sd, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_WRITE);
			TraceMsg(TSK_ID_FILE,"SD Append new File : %s = %d (0==OK)\n", filename, ret);
		}
		if(ret == FR_OK)
		{
			openFile = 1;
			uint32_t adr = file_sd.fsize;

			if(adr)	ret = f_lseek(&file_sd,adr);
			if(ret == OK)
			{
				ret = f_write(&file_sd, buff,size, &bw);
				if(ret == OK)
				{
					if(bw != size)
					{
						ret = DISK_FULL_WARNING;
					}
					else
					{
					}
				}
				ret |= f_sync(&file_sd);
			}
			else
			{

			}
			//ret |= f_close(&file_sd);
		}

	}
	OS_Unuse(sdFileLock);
	TraceMsg(TSK_ID_FILE,"SD Append File : %s = %d (0==OK)\n", filename, ret);
	return ret;
}


uint16_t IsFileReady_SD(const uint8_t *filename)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	if(fatStatusOk != OK)
	{
		ret = Mount_SD();
	}
	OS_Use(sdFileLock);
	if(ret == OK)
	{
		if(openFile)
		{
			f_close(&file_sd);
			openFile = 0;
		}
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s",SD_PATH,filename);
		ret = f_open(&file_sd, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_READ);
		if(ret == OK)
		{
			if(file_sd.fsize < 10)
			{
				ret |= 0x100;
			}
		}
		ret |= f_close(&file_sd);
	}
	OS_Unuse(sdFileLock);
	return ret;
}
uint16_t ReadFileAdr_SD(const uint8_t *filename, uint8_t* buff, uint32_t *psize,uint32_t adr)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	if(fatStatusOk != OK)
	{
		ret = Mount_SD();
	}

	OS_Use(sdFileLock);

	if(ret == OK)
	{
		if(openFile)
		{
			f_close(&file_sd);
			openFile = 0;
		}
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s",SD_PATH,filename);
		ret = f_open(&file_sd, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_READ);
		if(ret == FR_OK)
		{
			if(adr)	ret = f_lseek(&file_sd,adr);
			if(ret == OK)
			{
				ret = f_read(&file_sd, buff,*psize, &bw);
				if(bw != *psize)
				{
					if(ret == OK)
					{
						*psize = bw;
						ret = WARNING;
					}
				}

			}
			else
			{

			}
		}
		ret |= f_close(&file_sd);
	}
	OS_Unuse(sdFileLock);
	TraceMsg(TSK_ID_FILE,"SD Read File : %s = %d (0==OK) adr=%d, len=%d\n", filename, ret,adr,*psize);
	return ret;
}

uint16_t RWTest_SD(uint8_t* filename)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	uint32_t size = 0;
	uint8_t writeTest[50];
	uint8_t readTest[50] = "  ";
	uint16_t len = sprintf_rtc(writeTest,30);
	/*lint -e586*/
	snprintf((char*)&writeTest[len], (int)(50-len-2),"%s\r\n","SD write test");

	if(fatStatusOk != OK)
	{
		ret = Mount_SD();
	}
	OS_Use(sdFileLock);
	if(ret == OK)
	{
		if(openFile)
		{
			f_close(&file_sd);
			openFile = 0;
		}
		/*lint -e586*/
		snprintf((void*)pathname,35, "%s%s",SD_PATH,filename);
		ret = f_open(&file_sd, (const TCHAR*)pathname,FA_CREATE_NEW | FA_WRITE|FA_READ);
		if(FR_EXIST == ret)
			ret = f_open(&file_sd, (const TCHAR*)pathname,FA_OPEN_EXISTING | FA_WRITE|FA_READ);

		if(ret == FR_OK)
		{
			size = file_sd.fsize;
			ret = f_lseek(&file_sd, size);
			if(ret == OK)
			{
				len = strlen((char*)writeTest);
				ret = f_write(&file_sd, (char*)writeTest,len, &bw);
				if(bw != len)
				{
					if(ret == OK)
					{
						ret = WARNING;
					}
				}
			}
			if(ret == OK)
			{
				ret = f_lseek(&file_sd, size);
				ret = f_read(&file_sd, (char*)readTest,len, &bw);
				if(bw != len)
				{
					if(ret == OK)
					{
						ret = WARNING;
					}
				}
			}
			//compare
			if(ret == OK)
			{
				if(memcmp((void*)readTest, (void*)writeTest, len) != 0)
				{
					ret = FATAL_ERROR;
				}
			}
		}
		ret |= f_close(&file_sd);
	}
	OS_Unuse(sdFileLock);
	TraceMsg(TSK_ID_FILE,"SD Test :%s = %d (0==OK)\n", filename, ret);
	return ret;
}
