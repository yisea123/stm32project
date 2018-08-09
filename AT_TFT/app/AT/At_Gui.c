/*
 * At_Gui.c
 *
 *  Created on: 2016��10��2��
 *      Author: pli
 */


#include <math.h>
#include "stdint.h"
#include "string.h"

#include "canprocess.h"

#include "t_data_obj.h"
#include "t_unit.h"
#include "unit_parameter.h"
#include "unit_rtc_cfg.h"
#include "parameter_idx.h"
#include "At_file.h"
#include "At_Gui.h"
#include "rtc.h"
#include "ff.h"

#include "tft_uart.h"
#include "shell_io.h"
#include "dev_eep.h"


static float Ratio_Curr1  = (float)(YSIZE1)/((float)(MaxCurr));
static float Ratio_Volt1  = (float)(YSIZE1)/((float)(MaxVolt));
static float Ratio_Speed1  = (float)(YSIZE1)/((float)(MaxSpeed));
static float Ratio_Energy1  = (float)(YSIZE1)/((float)(MaxLineEnergy));



static const uint8_t OK_TXT1[] = " Passed ";
static const uint8_t ING_TXT[] = " ING    ";
static const uint8_t ERR_TXT[] = " Failed ";
static const uint8_t IDL_TXT[] = " ...... ";
static const uint8_t TIME_OUT[]= "time out";
static const uint8_t WARNING_[]= "warning ";

static float LineEnergy = 0.0f ;
static SemaphoreHandle_t usartLock = NULL;
typedef struct {
	int16_t x;
	int16_t y;
}XYPOS;

static uint8_t dateStr[25] = "05:32:33 06/07/13 ";

static const int8_t StrIndex[][20] =
{
	"Idle    ",//0
	"Welding ",//1
	"SD Error",//2
	"SD Ok   ",//3
	"Copying ",//4
	"Copy Err",//5
	"Copy Ok ",//6
	"Invalid ",//7
	"No Copy ",//8
};


static uint16_t pointLine[4] = {0,0,0,0};
static uint8_t uartData[256];



static uint8_t* rxDataBuf = NULL;
static uint8_t  rxDataIdx = 0;
static uint8_t  rxDataLen = 0;

void HandleUartRx(uint8_t data)
{
	if(rxDataBuf)
	{
		rxDataBuf[rxDataIdx++] = data;
		if(rxDataIdx >= rxDataLen)
		{
			rxDataBuf = NULL;
			rxDataIdx = 0;
			MsgPush(USART_RX_EVENT, 0,0);
		}
	}
}



static void WriteTFT_TXT(const uint16_t adr,const uint8_t* data, const uint16_t len)
{
	uint16_t sendLen = 32+6;
	uint16_t bufLen = (uint16_t)(len + 6);
	OS_Use(usartLock);
	uartData[0] = 0x5A;
	uartData[1] = 0xA5;
	uartData[2] = (uint8_t)(len + 3);
	uartData[3] = CMD_WRITE_VALUE;
	uartData[4] = (uint8_t)(adr/256);
	uartData[5] = (uint8_t)(adr%256);

	memcpy((void*)&uartData[6],data,len);
	if(len < TXT_BUF_LEN)
		memset(&uartData[bufLen],0,32);
	shellSend((const char*)uartData,(int)sendLen, OTHER_USART);
	OS_Unuse(usartLock);
	osDelay(5);
}

static void WriteTFT_Scroll(const uint16_t adr, const uint8_t* data, const uint16_t len)
{
	uint16_t sendLen = (uint16_t)(len+6);
	uint16_t bufLen = (uint16_t)(len + 6);
	OS_Use(usartLock);
	uartData[0] = 0x5A;
	uartData[1] = 0xA5;
	uartData[2] = (uint8_t)(len + 3);
	uartData[3] = CMD_WRITE_VALUE;
	uartData[4] = (uint8_t)(adr/256);
	uartData[5] = (uint8_t)(adr%256);

	memcpy((void*)&uartData[6],data,len);
	if(len < TXT_BUF_LEN)
		memset(&uartData[bufLen],0,50);
	shellSend((const char*)uartData,(int)sendLen, OTHER_USART);
	OS_Unuse(usartLock);
	osDelay(5);
}



static void WriteTFT_ToPage(const uint16_t  pageIdx)
{
	static uint16_t displayPage = 0xFFFF;
	static uint16_t count = 0;
	OS_Use(usartLock);
	if((displayPage != pageIdx) || (count < 0x04))
	{
		uint16_t bufLen = 7;

		uartData[0] = 0x5A;
		uartData[1] = 0xA5;
		uartData[2] = 4;
		uartData[3] = CMD_WRITE_REG;
		uartData[4] = PAGE_REG;
		uartData[5] = (uint8_t)(pageIdx/256);
		uartData[6] = (uint8_t)(pageIdx%256);
		shellSend((const char*)uartData,(int)bufLen, OTHER_USART);

		osDelay(5);
	}
	if(displayPage == pageIdx)
		count++;
	else
	{
		displayPage = pageIdx;
		count = 0;
	}
	OS_Unuse(usartLock);
}
static void WriteTFT_Wave(uint16_t p0,uint16_t p1,uint16_t p2,uint16_t p3)
{
	uint16_t bufLen = 13;
	OS_Use(usartLock);
	uartData[0] = 0x5A;
	uartData[1] = 0xA5;
	uartData[2] = 0x0A;
	uartData[3] = CMD_WRITE_WAVE;
	uartData[4] = 0x0F;
	uartData[5] = (uint8_t)(p0/256);
	uartData[6] = (uint8_t)(p0%256);
	uartData[7] = (uint8_t)(p1/256);
	uartData[8] = (uint8_t)(p1%256);
	uartData[9] = (uint8_t)(p2/256);
	uartData[10] = (uint8_t)(p2%256);
	uartData[11] = (uint8_t)(p3/256);
	uartData[12] = (uint8_t)(p3%256);
	shellSend((const char*)uartData,(int)bufLen, OTHER_USART);
	OS_Unuse(usartLock);
	osDelay(5);

//	5a a5 12 84 0f 20 10 00 90 10 a0 20 30 00 10 20 50 10 a0 00 f0
}
static void WriteTFT_U16(uint16_t adr, uint16_t data)
{
	uint16_t bufLen = 2 + 6;
	OS_Use(usartLock);
	uartData[0] = 0x5A;
	uartData[1] = 0xA5;
	uartData[2] = 2 + 3;
	uartData[3] = CMD_WRITE_VALUE;
	uartData[4] = (uint8_t)(adr/256);
	uartData[5] = (uint8_t)(adr%256);
	uartData[6] = (uint8_t)(data/256);
	uartData[7] = (uint8_t)(data%256);

	shellSend((const char*)uartData,(int)bufLen, OTHER_USART);
	OS_Unuse(usartLock);
	osDelay(5);
}
#define REG_HEAD_LEN		0x06


static uint16_t ReadTFT_Reg(uint8_t adr, uint8_t* ptrData,uint8_t len)
{
	uint16_t ret = FATAL_ERROR;
	uint16_t retryTime = 2;
	uint16_t bufLen = 6;
	OS_Use(usartLock);
	uartData[0] = 0x5A;
	uartData[1] = 0xA5;
	uartData[2] = 3;
	uartData[3] = CMD_READ_REG;
	uartData[4] = adr;
	uartData[5] = len;
	while(retryTime--)
	{
		rxDataBuf = ptrData;
		rxDataLen = (uint8_t)(len+REG_HEAD_LEN);
		rxDataIdx = 0;
		shellSend((const char*)uartData,(int)bufLen, OTHER_USART);

		osEvent event = osMessageGet(USART_RX_EVENT, 1000 );

		if(event.status == osEventMessage )
		{
			if((uartData[3] == CMD_READ_REG) && (adr == uartData[4]))
			{
				ret = OK;
				break;
			}

		}
	}

	OS_Unuse(usartLock);
	return ret;
}


static void SyncTFT_RTC(void)
{
	static uint16_t syncRtc = 0;
	if(syncRtc == 0)
	{
		uint8_t rtcTime[20];
	#define RTC_REG		0x20
	#define RTC_REG_LEN	0x07
		memset(rtcTime, 0, sizeof(rtcTime));
		uint16_t ret = ReadTFT_Reg(RTC_REG,rtcTime, RTC_REG_LEN);
		if(ret == OK)
		{
			TimeCfg cfg;
			cfg.year = (uint16_t)(2000 + rtcTime[0+REG_HEAD_LEN]/16 *10 + rtcTime[0+REG_HEAD_LEN]%16);
			cfg.month = (uint8_t)(rtcTime[1+REG_HEAD_LEN]/16 *10 + rtcTime[1+REG_HEAD_LEN]%16);
			cfg.date = (uint8_t)(rtcTime[2+REG_HEAD_LEN]/16 *10 + rtcTime[2+REG_HEAD_LEN]%16);
			cfg.hour = (uint8_t)(rtcTime[4+REG_HEAD_LEN]/16 *10 + rtcTime[4+REG_HEAD_LEN]%16);
			cfg.minute = (uint8_t)(rtcTime[5+REG_HEAD_LEN]/16 *10 + rtcTime[5+REG_HEAD_LEN]%16);
			cfg.second = (uint8_t)(rtcTime[6+REG_HEAD_LEN]/16 *10 + rtcTime[6+REG_HEAD_LEN]%16);

			uint32_t timeNow = CalcTime_ST(&cfg);
			RTC_Put(IDX_RTC_ST, 0, &timeNow);
			if(cfg.year >= 2017)
				syncRtc = 1;
		}
	}

}



static void SendTestResult(uint16_t adr, uint8_t ret)
{
	if(ret == TEST_IDLE)
	{
		WriteTFT_TXT(adr, IDL_TXT,sizeof(IDL_TXT));
	}
	else if (ret == TEST_ING)
	{
		WriteTFT_TXT(adr, ING_TXT,sizeof(ING_TXT));
	}
	else if (ret == TEST_OK)
	{
		WriteTFT_TXT(adr, OK_TXT1,sizeof(OK_TXT1));
	}
	else  if (ret == TEST_ERROR)
	{
		WriteTFT_TXT(adr, ERR_TXT,sizeof(ERR_TXT));
	}
	else if (ret == TEST_TIMEOUT)
	{
		WriteTFT_TXT(adr, TIME_OUT,sizeof(TIME_OUT));
	}
	else if (ret == TEST_WARNING)
	{
		WriteTFT_TXT(adr, WARNING_,sizeof(WARNING_));
	}
}

static void InitialStartTest(void)
{
	uint8_t usbStatus = OK;
	uint8_t sdStatus = 0;
	uint8_t iicStatus = OK;
	uint8_t usbConnectStatus = 0;
	uint16_t maxCount = 25;
	uint32_t tickTime = HAL_GetTick();
	SyncTFT_RTC();
	WriteTFT_ToPage(PAGE_START_TEST);
	for(;;)
	{
		parameter_Get(PARA_IDX_IIC_Status,WHOLE_OBJECT,&iicStatus);
		SendTestResult(TXT_EEP_RESULT, iicStatus);

		parameter_Get(PARA_IDX_sd_status,WHOLE_OBJECT,&sdStatus);
		SendTestResult(TXT_SD_RESULT, sdStatus);


		parameter_Get(PARA_IDX_usbWriteStatus,WHOLE_OBJECT,&usbStatus);
		SendTestResult(TXT_USB_RESULT, usbStatus);

		SendTestResult(TXT_CAN_RESULT, TEST_OK);
		osDelay(200);

		//if( iicStatus != TEST_OK || TEST_WARNING != iicStatus)
		if( iicStatus != TEST_OK)
		{
			tickTime = HAL_GetTick();
		}
		else if(sdStatus != TEST_OK)
		{
			if(maxCount-- == 0)
			{
				break;
			}
			tickTime = HAL_GetTick();
		}
		else
		{
			osDelay(2000);
			if((usbStatus == TEST_OK) || (usbStatus == TEST_ERROR))
			{
				osDelay(2000);
				break;
			}
			else
			{
				parameter_Get(PARA_IDX_usbConnectedStatus,WHOLE_OBJECT,&usbConnectStatus);
				if(usbConnectStatus == USH_USR_FS_READY)
				{
					WriteTFT_TXT(TXT_NOTIFY_USB, (const uint8_t*)("U Disk Detected!"),sizeof("U Disk Detected!"));
					WriteTFT_ToPage(PAGE_WELD_NOTIFY);
					if(HAL_GetTick() - tickTime >= USB_TEST_TIME)
					{
						break;
					}
				}
				else if(USH_USR_DISCONNECT == usbConnectStatus)
				{
					WriteTFT_TXT(TXT_NOTIFY_USB, (const uint8_t*)("U Disk Unplug!"),sizeof("U Disk Unplug!"));
					WriteTFT_ToPage(PAGE_WELD_NOTIFY);
					if(HAL_GetTick() - tickTime >= USB_IDLE_TIME)
					{
						break;
					}
				}
			}
		}
	}
}

static void InitialCopyFiles(void)
{
	uint16_t maxRetry = 30;
	uint8_t _copyStatus = 0;
	uint8_t sdStatus = 0;
	uint8_t usbConnectStatus = 0;

	uint16_t copiedNum = 0;
	uint16_t totalNum;

	parameter_Get(PARA_IDX_usbConnectedStatus,WHOLE_OBJECT,&usbConnectStatus);
	if(usbConnectStatus == USH_USR_DISCONNECT)
	{
		return;
	}
	WriteTFT_ToPage(PAGE_WELD_COPY);
	for(;;)
	{
		SyncTFT_RTC();
		parameter_Get(PARA_IDX_sd_status,WHOLE_OBJECT,&sdStatus);
		parameter_Get(PARA_IDX_usbConnectedStatus,WHOLE_OBJECT,&usbConnectStatus);
		if(sdStatus == TEST_OK)
		{
			maxRetry = 30;
			WriteTFT_ToPage(PAGE_WELD_COPY);
			if(usbConnectStatus != USH_USR_DISCONNECT)
			{
				parameter_Get(PARA_IDX_totalNum_Copy,WHOLE_OBJECT,&totalNum);
				WriteTFT_U16(U16_TOTAL_COPY, totalNum);
				parameter_Get(PARA_IDX_copy_filenum,WHOLE_OBJECT,&copiedNum);
				WriteTFT_U16(U16_COPIED_NUM, copiedNum);
				parameter_Get(PARA_IDX_copy_status,WHOLE_OBJECT,&_copyStatus);

				SendTestResult(TXT_COPIED_STATUS, _copyStatus);
				if(((_copyStatus != TEST_IDLE) && (_copyStatus != TEST_ING)) || (TEST_FINISHED == _copyStatus))
				{
					osDelay(2000);
					break;
				}
				else
				{

				}
			}
			else
			{
				break;
			}
		}
		else
		{
			if(maxRetry-- ==0)
			{
				break;
			}

		}
		osDelay(200);
	}
}


static void GetWeldData(void)
 {
 	uint16_t data_value = 0;

	parameter_Get(PARA_IDX_weldCurrent,WHOLE_OBJECT,&data_value);
	pointLine[0] = (uint16_t)(Ratio_Curr1 * data_value);
	parameter_Get(PARA_IDX_weldWorkPoint,WHOLE_OBJECT,&data_value);
	pointLine[2] = (uint16_t)(Ratio_Volt1 * data_value);
	parameter_Get(PARA_IDX_weldSpeed,WHOLE_OBJECT,&data_value);
	pointLine[1] = (uint16_t)(Ratio_Speed1 * data_value);
	pointLine[3] = (uint16_t)(Ratio_Energy1 * LineEnergy);
 }





static void ValueScreen(void)
{
    static uint8_t valueIndexClear = 0;
	valueIndexClear++;
	{
		float speed;
		uint16_t data_value = 0;
		uint16_t data_value1 = 0;
		uint16_t data_value2 = 0;
		float kj ;
		uint16_t valueInt = 0;

		parameter_Get(PARA_IDX_weldCurrent, WHOLE_OBJECT, &data_value1);
		WriteTFT_U16(U16_CURRENT_VALUE, data_value1);
		parameter_Get(PARA_IDX_weldWorkPoint, WHOLE_OBJECT, &data_value2);
		WriteTFT_U16(U16_VOLTAGE_VALUE, data_value2);
		parameter_Get(PARA_IDX_weldSpeed, WHOLE_OBJECT, &data_value);
		WriteTFT_U16(U16_SPEED_VALUE, data_value);


		if(data_value == 0)
		{
			speed = 8.0;
		}
		else
		{
			speed = data_value * 1.0f;
		}
		kj = ((float)data_value1 * (float)data_value2)*0.0006f/ speed;
		LineEnergy = kj;
		valueInt =(uint16_t)( LineEnergy * 100);
		WriteTFT_U16(U16_ENERGY_VALUE ,valueInt);
	}
}



static void Display_Scroll(void)
{
	static uint8_t idx = 0;
	static uint8_t displayStr[50];
	uint8_t 	data = 0;
	uint8_t 	status = 0;
#define ST_SCROLL_ID 	6u
	sprintf_rtc((uint8_t*)dateStr,25);
	uint8_t lenId = (uint8_t)strlen((const char*)dateStr);
	memcpy((void*)&displayStr[ST_SCROLL_ID],dateStr,lenId);

	lenId = (uint8_t)(lenId + ST_SCROLL_ID);
	parameter_Get(PARA_IDX_weldStatus,WHOLE_OBJECT, &data  );

	if(idx <= 1)
	{
		if(data == PARA_WELD_IDLE)
			memcpy((void*)&displayStr[lenId],&StrIndex[0][0],STR_INDEX_LEN);
		else
			memcpy((void*)&displayStr[lenId],&StrIndex[1][0],STR_INDEX_LEN);
		lenId = (uint8_t)(lenId + STR_INDEX_LEN);
	}
	else if(idx <=4)
	{
		parameter_Get(PARA_IDX_sd_status,WHOLE_OBJECT,&status) ;
		if(status == TEST_OK)
			memcpy((void*)&displayStr[lenId],&StrIndex[3][0],STR_INDEX_LEN);
		else
			memcpy((void*)&displayStr[lenId],&StrIndex[2][0],STR_INDEX_LEN);
		lenId =  (uint8_t)(lenId + STR_INDEX_LEN);
	}
	else if(idx <=6)
	{
		if(data == PARA_WELD_IDLE)
		{
			parameter_Get(PARA_IDX_copy_status,WHOLE_OBJECT,&status) ;
			if(status == TEST_ING)
				memcpy((void*)&displayStr[lenId],&StrIndex[4][0],STR_INDEX_LEN); //copying
			else if(status == TEST_OK)
				memcpy((void*)&displayStr[lenId],&StrIndex[6][0],STR_INDEX_LEN); //end ok
			else if(status == TEST_ERROR)
				memcpy((void*)&displayStr[lenId],&StrIndex[5][0],STR_INDEX_LEN);  //end error
			else
			{
				memcpy((void*)&displayStr[lenId],&StrIndex[8][0],STR_INDEX_LEN);
				idx = 0xff;
			}

			lenId =  (uint8_t)(lenId + STR_INDEX_LEN);
		}
		else
			idx = 0xff;
	}
	else
		idx = 0xff;
	idx++;
	WriteTFT_Scroll(TXT_SCROLL_BAR, displayStr,lenId);
}


static void Display_Wave(void)
{
	if(eepStatus == OK)
	{
		GetWeldData();
		WriteTFT_Wave(pointLine[0], pointLine[1], pointLine[2], pointLine[3]);
	}
}
void SetCopyFileName(uint8_t* fileName)
{
	WriteTFT_TXT(TXT_COPIED_NAME,fileName,(uint16_t)strlen(fileName));
}

static void DisplayWaves(void)
{
	const uint8_t USB_TXT7[] = "USB Copy Passed ";
	const uint8_t USB_TXT8[] = "USB Copy Failed ";
	int i = 0;
	uint16_t file_num = 0;
	uint16_t file_total = 0;

	int16_t num = 0;
	uint8_t sdStatus = 0;
	uint8_t oldSdStatus = 0;
	uint8_t usbConnectStatus = USH_USR_DISCONNECT;
	uint8_t usbConOld = USH_USR_DISCONNECT;
	uint16_t usbCount = 0;

	Ratio_Curr1  = (float)(YSIZE1)/((float)(parameterStaticRare.currentMax))/10.5f;
	Ratio_Volt1  = (float)(YSIZE1)/((float)(parameterStaticRare.voltMax))/1.1f;
	Ratio_Speed1  = (float)(YSIZE1)/((float)(parameterStaticRare.speedMax))*9.8f;
	Ratio_Energy1  = (float)(YSIZE1)/((float)(parameterStaticRare.energyMax));


	parameter_Get(PARA_IDX_usbConnectedStatus,WHOLE_OBJECT,&usbConOld);
	CheckSimuAction();
	WriteTFT_ToPage(PAGE_WELD_WAVE);

	while(1)
	{
		SyncTFT_RTC();
		ValueScreen();
		Display_Wave();
		if(i++%2==1)
		{
			Display_Scroll();
		}


		parameter_Get(PARA_IDX_sd_status,WHOLE_OBJECT,&sdStatus);
		parameter_Get(PARA_IDX_usbConnectedStatus,WHOLE_OBJECT,&usbConnectStatus);
		if(usbConOld != usbConnectStatus)
		{
			usbConOld = usbConnectStatus;
			usbCount = 35;
		}

		if(usbCount!= 0)
		{
			usbCount--;
			if(usbConnectStatus == USH_USR_FS_READY)
				WriteTFT_TXT(TXT_NOTIFY_USB, (const uint8_t*)("U Disk Detected!"),sizeof("U Disk Detected!"));
			else if(usbConnectStatus == USH_USR_DISCONNECT)
				WriteTFT_TXT(TXT_NOTIFY_USB, (const uint8_t*)("U Disk Unplugged!"),sizeof("U Disk Unplugged!"));
			WriteTFT_ToPage(PAGE_WELD_NOTIFY);
		}

		if((oldSdStatus != sdStatus) || (sdStatus ==  TEST_ERROR))
		{
			switch(sdStatus)
			{
			case TEST_OK:
				WriteTFT_TXT(TXT_NOTIFY_SD, (const uint8_t*)("SD Card OK!"),sizeof("SD Card OK!"));
				break;
			case TEST_ERROR:
				WriteTFT_TXT(TXT_NOTIFY_SD, (const uint8_t*)("SD Card Error!"),sizeof("SD Card Error!"));

				WriteTFT_ToPage(PAGE_WELD_NOTIFY);

				break;
			}
			oldSdStatus = sdStatus;
		}
		else if(usbCount == 0)
		{
			uint8_t _copyStatus = 0;
			parameter_Get(PARA_IDX_copy_status,WHOLE_OBJECT,&_copyStatus);
			//copy
			if(_copyStatus != TEST_IDLE)
			{
				WriteTFT_ToPage(PAGE_WELD_COPY);
				if(_copyStatus == TEST_ING)
				{
					num = 40;
					SendTestResult(TXT_COPIED_STATUS, _copyStatus);
				}
				else if(_copyStatus == TEST_OK)
				{
					num--;
					SendTestResult(TXT_COPIED_STATUS, _copyStatus);
					WriteTFT_TXT(TXT_COPY_RESULT,USB_TXT7,sizeof(USB_TXT7));
				}
				else if(_copyStatus == TEST_ERROR)
				{
					num--;
					SendTestResult(TXT_COPIED_STATUS, _copyStatus);
					WriteTFT_TXT(TXT_COPY_RESULT,USB_TXT8,sizeof(USB_TXT8));
				}
				parameter_Get(PARA_IDX_copy_filenum,WHOLE_OBJECT,&file_num );
				WriteTFT_U16(U16_COPIED_NUM, file_num);

				parameter_Get(PARA_IDX_totalNum_Copy,WHOLE_OBJECT,&file_total);
				WriteTFT_U16(U16_TOTAL_COPY, file_total);

				if(num <= 0)
				{
					_copyStatus = TEST_IDLE;
					parameter_Put(PARA_IDX_copy_status,WHOLE_OBJECT,&_copyStatus);
					CheckSimuAction();
				}
			}
			else
			{
				WriteTFT_ToPage(PAGE_WELD_WAVE);
			}

		}

		Actual_SaveData();
		osDelay(50);
	}
}


void StartGuiTask(void const * argument)
{
	(void)argument;

	if(NULL == usartLock)
		usartLock = OS_CreateSemaphore();
 	InitialStartTest();

 	InitialCopyFiles();

 	DisplayWaves();
}
