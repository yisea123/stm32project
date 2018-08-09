/*
 * At_File.c
 *
 *  Created on: 2016閿熸枻鎷�9閿熸枻鎷�23閿熸枻鎷�
 *      Author: Paul
 */


#include "main.h"
#include "can.h"
#include <string.h>
#include "dev_can.h"
#include "usb_file.h"
#include "sd_file.h"
#include "weld_data.h"
#include "dev_eep.h"
#include "unit_rtc_cfg.h"
#include "unit_parameter.h"
#include "parameter_idx.h"
#include "shell_io.h"
#include "canprocess.h"
#include "fatfs.h"
#include "At_File.h"
#include "common_crc.h"
#include <math.h>

#define SD_TEST_NUM		20
enum
{
	SAVE_APP =0,
	SAVE_NEW,

};


enum
{
	COPY_REQ_FILES =0,
	COPY_ALL_FILES,
};

#define MAX_FILE_NUM_PER_FOLD		50
#define MAX_FILE_NUM				384
#define COPY_SIZE_MAX				2048//10K
#define USB_TEST_NUM				10


static uint8_t copyBuff[COPY_SIZE_MAX];

static const uint8_t WELD_PATH[]	=	"WELD";
static uint8_t weldFileName[50];


#define RETRY_COUNT_TIMES		5
#define RETRY_COUNT_DELAY		50


static FileAction* GetOneNewFileTskMsg(void)
{
#define FILE_TSK_MSG_SIZE		16
	static FileAction	fileAction[FILE_TSK_MSG_SIZE];
	static uint16_t idxLoc= 0;
	static SemaphoreHandle_t filTskLock = NULL;
	FileAction* ret = NULL;
	if(NULL == filTskLock)
		filTskLock = OS_CreateSemaphore();
	OS_Use(filTskLock);
	if(idxLoc >= FILE_TSK_MSG_SIZE)
	{
		idxLoc = 0;
	}
	ret = &fileAction[idxLoc++];
	OS_Unuse(filTskLock);
	return ret;
}

static void Tsk_Usb_Test(void)
{
	uint16_t 	ret = OK;
	uint32_t 	FileTestOKNum = 0;
	uint32_t 	FileTestErrNum = 0;
	uint8_t 	filename[20] = "testFile.txt";
	uint8_t 	testRet = TEST_ING;

	parameter_Put(PARA_IDX_usbWriteStatus,WHOLE_OBJECT,&testRet) ;
	for (;;)
	{
		uint32_t tickOut = 4;

		ret = RWTest_USB(filename);

		if (ret == OK)
		{
			FileTestOKNum++;
		}
		else
		{
			osDelay(5);
			FileTestErrNum++;
		}
		if (FileTestOKNum + FileTestErrNum > USB_TEST_NUM)
		{
			tickOut = osWaitForever;
			if(FileTestErrNum == 0)
				testRet = TEST_OK;
			else
				testRet = TEST_ERROR;

			parameter_Put(PARA_IDX_usbWriteStatus,WHOLE_OBJECT,&testRet) ;
			break;
		}
		else
		{
			osDelay(tickOut);
		}
	}

}

static void Tsk_AD_Test(void)
{
	uint16_t 	ret = OK;
	uint32_t 	FileTestOKNum = 0;
	uint32_t 	FileTestErrNum = 0;
	uint8_t 	filename[20] = "testFile.txt";
	uint8_t 	testRet = TEST_ING;

	parameter_Put(PARA_IDX_sd_status,WHOLE_OBJECT,&testRet) ;
	for (;;)
	{
		uint32_t tickOut = 4;
		ret = RWTest_SD(filename);
		if (ret == OK)
		{
			FileTestOKNum++;
		}
		else
		{
			FileTestErrNum++;
		}
		if (FileTestOKNum + FileTestErrNum > SD_TEST_NUM)
		{
			tickOut = osWaitForever;
			if(FileTestErrNum == 0)
				testRet = TEST_OK;
			else
				testRet = TEST_ERROR;

			parameter_Put(PARA_IDX_sd_status,WHOLE_OBJECT,&testRet) ;
			break;
		}
		else
		{
			osDelay(tickOut);
		}
	}

}
uint16_t fileSaveOkCount = 0;
uint16_t fileSaveErrorCount = 0;

static uint16_t SaveDatatoSD(uint16_t type, uint8_t* buf, uint32_t len)
{
	uint16_t ret = OK;
	uint16_t crccheck;
	uint8_t  filename[20];
	uint8_t  path[20];
	uint8_t  testRet;


	if(type == SAVE_NEW)
	{
		uint32_t fileID = GetNewFileID();

		//fold name
		/*lint -e586*/
		snprintf((char*)path,18,"%s%d", WELD_PATH, fileID/MAX_FILE_NUM_PER_FOLD );
		//file name
		/*lint -e586*/
		snprintf((char*)filename,18,"W%04d.b", fileID%MAX_FILE_NUM_PER_FOLD );
		/*lint -e586*/
		snprintf((char*)weldFileName,40,"%s/%s",path,filename);
		uint16_t counter = RETRY_COUNT_TIMES;
		do
		{
			ret = CreateNewFile_SD(path,filename,0);
			if(ret != OK)
				osDelay(RETRY_COUNT_DELAY);
		}while((ret != OK) && (counter--));
		if(ret == OK)
		{
			UpdateNewFileID();
			fileSaveOkCount++;
		}
		else
		{
			fileSaveErrorCount++;
		}
	}
	if(ret == OK)
	{
		for(uint32_t locIdx=0; locIdx<len;locIdx+=ONE_PAGE_SIZE)
		{
			crccheck = CalcCrc16Mem_COMMON( (uint8_t*)&buf[locIdx],InitCRC16_COMMON(),ONE_PAGE_DATA_SIZE);
			buf[locIdx+ONE_PAGE_DATA_SIZE] = crccheck/256;
			buf[locIdx+ONE_PAGE_DATA_SIZE + 1] = crccheck%256;
		}
		uint16_t counter = RETRY_COUNT_TIMES;
		do
		{
			ret = AppendFile_SD(weldFileName, buf, len);
			if(ret != OK)
				osDelay(RETRY_COUNT_DELAY);
		}while((ret != OK) && (counter--));

		if(ret == OK)
		{
			fileSaveOkCount++;
		}
		else
		{
			fileSaveErrorCount++;
		}
	}

	if(ret == OK)
		testRet = TEST_OK;
	else
		testRet = TEST_ERROR;
	TraceMsg(TSK_ID_FILE,"SD write Status: %d \n",testRet);
	parameter_Put(PARA_IDX_sd_status,WHOLE_OBJECT,&testRet) ;
	return ret;
}

static uint16_t FCopySdtoUsb(uint8_t* sdFile,uint8_t* usbFile)
{

	uint16_t ret = OK;
	uint32_t idx = 0;

	for(;;)
	{
		uint32_t readSize = COPY_SIZE_MAX;
		uint16_t counter = RETRY_COUNT_TIMES;
		do
		{
			ret = ReadFileAdr_SD(sdFile,&copyBuff[0],&readSize,idx*COPY_SIZE_MAX);
			if(ret != OK)
				osDelay(RETRY_COUNT_DELAY);
		}while((ret > WARNING) && (counter--));
		if(ret <= WARNING)
		{
			ret = OK;
			if(readSize)
				ret = WriteFileAdr_USB(usbFile,&copyBuff[0],readSize,idx*COPY_SIZE_MAX);
			else
				break;
			if(ret != OK)
			{
				//usb needs to reboot;
				//todo:
				RenitUsb_User();
				break;
			}
		}

		if(ret == OK)
			idx++;
		else
			break;
	}
	TraceMsg(TSK_ID_FILE,"Copy to Usb Path: %s -> %s = %d\n",sdFile,usbFile, ret);

	return ret;
}
static uint16_t CopyFiles(uint16_t type)
{
	uint32_t 	startFileIdx = 0;
	uint32_t 	endFileIdx = GetEndCopyFileID();
	uint8_t 	filenameSD[50];
	uint8_t 	filenameUsb[50];
	uint8_t 	path[20];
	uint16_t 	ret = OK;
	uint8_t 	testRet = COPY_ING;
	uint32_t    savedCopyID = 0;



	if(endFileIdx >= 0xFFFFFFF0)
	{
		endFileIdx = 0;
	}

	//MAX_FILE_NUM_PER_FOLD;
	switch(type)
	{
		case COPY_REQ_FILES:
			startFileIdx = GetStartCopyFileID();
			break;
		case COPY_ALL_FILES:
			startFileIdx = GetEndCopyFileID();
			if(startFileIdx > MAX_FILE_NUM)
			{
				startFileIdx -= MAX_FILE_NUM;
			}
			else
				startFileIdx = 0;
			break;
		default:
			break;
	}
	uint32_t totalNum = endFileIdx - startFileIdx;
	uint32_t copyFileID = startFileIdx;
	uint32_t copyPathID = startFileIdx;
	savedCopyID = GetStartCopyFileID();

	startFileIdx = startFileIdx%MAX_FILE_NUM;
	endFileIdx = endFileIdx%MAX_FILE_NUM;


	uint16_t copyNum = 0;
	parameter_Put(PARA_IDX_copy_status,WHOLE_OBJECT,&testRet) ;


	parameter_Put(PARA_IDX_totalNum_Copy,WHOLE_OBJECT, &totalNum);
	parameter_Put(PARA_IDX_copy_filenum,WHOLE_OBJECT, &copyNum);


	copyPathID = copyPathID/MAX_FILE_NUM_PER_FOLD ;
	/*lint -e586*/
	snprintf((char*)path,18,"%s%d", WELD_PATH, copyPathID );

	ret = CreateDir_USB(WELD_PATH,path);
	if(ret != OK)
	{
		testRet = COPY_ERROR;
		parameter_Put(PARA_IDX_copy_status,WHOLE_OBJECT,&testRet) ;
		return ret;
	}
	for(copyNum=0; copyNum<totalNum;)
	{
		/*lint -e586*/
		snprintf((char*)filenameSD,45,"%s/W%04d.b",path, copyFileID%MAX_FILE_NUM_PER_FOLD );
		/*lint -e586*/
		snprintf((char*)filenameUsb,45,"%s/%s/W%04d.b",WELD_PATH, path,copyFileID%MAX_FILE_NUM_PER_FOLD );
		if( OK != IsFileReady_SD(filenameSD))
		{
			ret = OK;
			//no need to copy
		}
		else
		{
			DeleteFile_USB(filenameUsb);
			SetCopyFileName(filenameUsb);
			uint16_t counter = RETRY_COUNT_TIMES;
			do
			{
				ret = FCopySdtoUsb(filenameSD, filenameUsb);
				if(ret != OK)
					osDelay(RETRY_COUNT_DELAY);
			}while((ret != OK) && (counter--));
		}


		if(ret == OK)
		{
			copyFileID++;
			copyNum++;
			if(savedCopyID < copyFileID)
			{
				savedCopyID = copyFileID;
				SetStartCopyFileID(savedCopyID);
			}
			parameter_Put(PARA_IDX_copy_filenum,WHOLE_OBJECT, &copyNum);

			uint32_t newPathID = copyFileID/MAX_FILE_NUM_PER_FOLD ;
			if(newPathID != copyPathID)
			{
				//fold name
				copyPathID = copyFileID/MAX_FILE_NUM_PER_FOLD ;
				/*lint -e586*/
				snprintf((char*)path,18,"%s%d", WELD_PATH, copyPathID );

				ret = CreateDir_USB(WELD_PATH,path);

				if(ret != OK)
				{
					break;
				}
			}
			//osDelay(5);
		}
		else
		{
			break;
		}
	}
	testRet = COPY_ERROR;
	if(ret == OK)
	{
		testRet = COPY_OK;
	}
	parameter_Put(PARA_IDX_copy_status,WHOLE_OBJECT,&testRet) ;
	return ret;
}

void SendFileMsg(const FileAction* act)
{
	FileAction* tempact;
	tempact = GetOneNewFileTskMsg();
	*tempact = *act;
	MsgPush(FILE_TSK_ID, (uint32_t)tempact,0);
}

void Actual_SaveData(void)
{
	static uint32_t tickTime = 0;
	uint32_t tickDiff = HAL_GetTick();
	uint32_t tickNow  = tickDiff;
	tickDiff = abs(tickTime - tickDiff);
	if(tickDiff >= 1000)
	{
		tickTime = tickNow;
		uint8_t weldStatus = PARA_WELD_IDLE;
		parameter_Get(PARA_IDX_weldStatus,WHOLE_OBJECT,&weldStatus);
		if(parameterDynamic.simulateSave[0])
		{
			for(uint8_t id = 0; id<30;id++)
				AddOneData(weldStatus);
		}
		else
			AddOneData(weldStatus);
	}
}

void SimulateWeldData(uint16_t num,uint8_t type)
{
	float data = num;
	for(int16_t idx = 0;idx<num;idx++)
	{
		uint16_t voltfeedback = 300+ (uint16_t)(50*sin(idx/data));
		parameter_Put(PARA_IDX_weldWorkPoint,WHOLE_OBJECT,&voltfeedback);
		voltfeedback = 400+ (uint16_t)(50*cos(idx/data));
		parameter_Put(PARA_IDX_weldCurrent,WHOLE_OBJECT,&voltfeedback);
		AddOneData(1);
	}
	if(type)
	{
		uint16_t voltfeedback = 0;
		parameter_Put(PARA_IDX_weldWorkPoint,WHOLE_OBJECT,&voltfeedback);
		parameter_Put(PARA_IDX_weldCurrent,WHOLE_OBJECT,&voltfeedback);
		AddOneData(0);
	}
}


void StartFileTask(void const * argument)
{
//	FileTask();
	uint32_t tickOut = osWaitForever;
	osEvent event;

	FileAction localAction;

	/* init code for FATFS */
	MX_FATFS_Init();
//	localAction.action = IIC_DATA_TEST;
//	SendFileMsg(&localAction);


	localAction.action = SD_FILE_TEST;
	SendFileMsg(&localAction);

	MX_USB_HOST_Init();

	while (1)
	{
		event = osMessageGet(FILE_TSK_ID, tickOut);
		if (event.status == osEventMessage)
		{
			localAction = (*(FileAction*)(event.value.p));
			switch(localAction.action)
			{
				case USB_FILE_TEST:
					Tsk_Usb_Test();
					break;
				case SD_FILE_TEST:
					Tsk_AD_Test();
					break;
#if 0
				case IIC_DATA_TEST:
				{
					uint8_t testRet = TEST_OK;
					if(eepStatus != OK)
						testRet = TEST_ERROR;
					parameter_Put(PARA_IDX_IIC_Status,WHOLE_OBJECT,&testRet) ;
				}
					break;
#endif
				case CAN_TEST:
					//todo
					//Tsk_AD_Test(fileName);
					break;
				case WELD_DATA_SAVE_APPEND:
					SaveDatatoSD(SAVE_APP, localAction.buff, localAction.len);
					break;
				case WELD_DATA_SAVE_NEW:
					SaveDatatoSD(SAVE_NEW, localAction.buff, localAction.len);
					break;
				case COPY_FILES_REQ:
					CopyFiles(COPY_REQ_FILES);
					break;
				case COPY_FILES_ALL:
					CopyFiles(COPY_ALL_FILES);

					break;

			}

		}

	}
}



void StartGpioTask(void const * argument)
{
	uint32_t 	inCount1 = 0;
	uint32_t	inCount2 = 0;
	uint32_t 	canCommunication = 0;
	uint32_t	validCommunication = 0;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_0);
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);
		osDelay(100);
		if(parameterDynamic.can_validatenum != validCommunication)
		{
			validCommunication = parameterDynamic.can_validatenum;
			inCount1 = 0;
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);

		}
		else if(inCount1++ > 10)
		{
			inCount1 = 20;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
		}


		if(canCommunication != parameterDynamic.can_innum)
		{
			canCommunication = parameterDynamic.can_innum;
			inCount2 = 0;
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
		}
		else if(inCount2++ > 10)
		{
			inCount2 = 20;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
		}
		//pe2 & pe3 if for can communication
		osDelay(100);
	}
}



