/*
 * usb_file.c
 *
 *  Created on: 2016��9��29��
 *      Author: pli
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "shell_io.h"
#include "ff.h"
#include "unit_parameter.h"
#include "parameter_idx.h"
#include "usb_file.h"
#include "rtc.h"

#define PRINTF1		shell_AddRTC

#define USB_PATH		"1:"


// file variables
//local variables
static 	FATFS 		fatfs_usb;
static 	FIL 		file_usb;

static uint16_t fatStatusOk = FATAL_ERROR;
static SemaphoreHandle_t usbFileLock = NULL;

uint16_t Mount_USB()
{
	if(usbFileLock == NULL)
		usbFileLock = OS_CreateSemaphore();
	fatStatusOk = f_mount( &fatfs_usb, USB_PATH,1);
	TraceMsg(TSK_ID_FILE,"Usb Mount : = %d (0==OK)\n", fatStatusOk);
	return fatStatusOk;
}
uint16_t DeleteFile_USB(const uint8_t *filename)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	if(fatStatusOk != OK)
	{
		ret = Mount_USB();
	}
	OS_Use(usbFileLock);
	if(ret == OK)
	{
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s",USB_PATH,filename);
		ret = f_unlink((const TCHAR*)pathname);
	}
	OS_Unuse(usbFileLock);
	TraceMsg(TSK_ID_FILE,"Usb unlink : %s= %d (0==OK)\n",pathname, ret);
	return ret;
}
uint16_t CreateDir_USB(const uint8_t *mainpath,const uint8_t *subpath)
{
	uint16_t ret = OK;
	int8_t pathname[40];
	if(fatStatusOk != OK)
	{
		Mount_USB();
	}
	OS_Use(usbFileLock);
	if(mainpath)
	{
		/*lint -e586*/
		snprintf((char*)pathname,25, "%s%s",USB_PATH,mainpath);
		ret = f_mkdir((const TCHAR*)pathname);
		if(FR_EXIST == ret)
			ret = OK;

		if(ret == OK)
		{
			if(subpath)
			{
				/*lint -e586*/
				snprintf((char*)pathname,35, "%s%s/%s",USB_PATH,mainpath,subpath);
				ret = f_mkdir((const TCHAR*)pathname);
				if(FR_EXIST == ret)
					ret = OK;
			}
		}
	}
	OS_Unuse(usbFileLock);
	TraceMsg(TSK_ID_FILE,"Create Usb Dir: %s/%s = %d\n",mainpath, subpath, ret);
	return ret;
}

uint16_t CreateNewFile_USB(const uint8_t *path,const uint8_t *filename, uint32_t psize)
{
	uint16_t ret = OK;
	int8_t pathname[40];
	if(fatStatusOk != OK)
	{
		Mount_USB();
	}
	OS_Use(usbFileLock);
	if(path)
	{
		/*lint -e586*/
		snprintf((char*)pathname,25, "%s%s",USB_PATH,path);
		ret = f_mkdir((const TCHAR*)pathname);
		if(FR_EXIST == ret)
			ret = OK;

	}
	if(ret == OK)
	{
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s/%s",USB_PATH,path,filename);
		ret = f_open(&file_usb, (const TCHAR*)pathname,FA_CREATE_ALWAYS | FA_WRITE);
		if(ret == FR_OK)
		{
			ret = f_lseek(&file_usb,psize);
			ret |= f_sync(&file_usb);
		}
		ret |= f_close(&file_usb);
	}
	OS_Unuse(usbFileLock);
	TraceMsg(TSK_ID_FILE,"Create Usb file: %s \\ %s = %d (0==OK)\n",path, filename, ret);
	return ret;

}

uint16_t WriteFileAdr_USB(const uint8_t *filename, uint8_t* buff, uint32_t size,uint32_t adr)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	if(fatStatusOk != OK)
	{
		ret = Mount_USB();
	}
	OS_Use(usbFileLock);
	if(ret == OK)
	{
		/*lint -e586*/
		snprintf((char*)pathname,35, "%s%s",USB_PATH,filename);
		ret = f_open(&file_usb, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_WRITE);
		if(ret == FR_OK)
		{
			if(adr)	ret = f_lseek(&file_usb,adr);
			if(ret == OK)
			{
				ret = f_write(&file_usb, buff,size, &bw);
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
			}
			else
			{

			}
		}
		ret |= f_close(&file_usb);
	}
	OS_Unuse(usbFileLock);
	TraceMsg(TSK_ID_FILE,"Write Usb file: %s = %d (0==OK)\n",filename, ret);
	return ret;
}

uint16_t ReadFileAdr_USB(const uint8_t *filename, uint8_t* buff, uint32_t size,uint32_t adr)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	if(fatStatusOk != OK)
	{
		ret = Mount_USB();
	}
	OS_Use(usbFileLock);
	if(ret == OK)
	{
		/*lint -e586*/
		snprintf((void*)pathname,35, "%s%s",USB_PATH,filename);
		ret = f_open(&file_usb, (const TCHAR*)pathname,FA_OPEN_ALWAYS | FA_READ);
		if(ret == FR_OK)
		{
			if(adr)	ret = f_lseek(&file_usb,adr);
			if(ret == OK)
			{
				ret = f_read(&file_usb, (char*)buff,size, &bw);
				if(bw != size)
				{
					if(ret == OK)
					{
						ret = WARNING;
					}
				}
			}
			else
			{

			}
		}
		ret |= f_close(&file_usb);
	}
	OS_Unuse(usbFileLock);
	TraceMsg(TSK_ID_FILE,"Read Usb file: %s = %d (0==OK)\n",filename, ret);
	return ret;
}

uint16_t RWTest_USB(uint8_t* filename)
{
	uint16_t ret = OK;
	uint8_t pathname[40];
	UINT bw = 0;
	uint32_t size = 0;
	uint8_t writeTest[50];
	uint8_t readTest[50] = "  ";
	uint16_t len = sprintf_rtc(writeTest,30);
	/*lint -e586*/
	snprintf((char*)&writeTest[len], (int)(50-len-2),"usb write test\r\n");

	if(fatStatusOk != OK)
	{
		ret = Mount_USB();
	}
	OS_Use(usbFileLock);
	if(ret == OK)
	{
		/*lint -e586*/
		snprintf((void*)pathname,35, "%s%s",USB_PATH,filename);
		ret = f_open(&file_usb, (const TCHAR*)pathname,FA_CREATE_NEW | FA_WRITE|FA_READ);
		if(FR_EXIST == ret)
			ret = f_open(&file_usb, (const TCHAR*)pathname,FA_OPEN_EXISTING | FA_WRITE|FA_READ);

		if(ret == FR_OK)
		{
			size = file_usb.fsize;
			ret = f_lseek(&file_usb, size);
			if(ret == OK)
			{
				len = strlen((void*)writeTest);
				ret |= f_write(&file_usb, (char*)writeTest,len, &bw);
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
				ret = f_lseek(&file_usb, size);
				ret |= f_read(&file_usb, (char*)readTest,len, &bw);
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
		ret |= f_close(&file_usb);
	}
	OS_Unuse(usbFileLock);
	TraceMsg(TSK_ID_FILE,"Usb Test file: %s = %d (0==OK)\n",filename, ret);
	return ret;
}
