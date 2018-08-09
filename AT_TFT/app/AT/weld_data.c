/*
 * weld_data.c
 *
 *  Created on: 2016Äê9ÔÂ29ÈÕ
 *      Author: pli
 */


#include <At_File.h>
#include <string.h>
#include "main.h"
#include "ff.h"
#include "unit_rtc_cfg.h"
#include "unit_parameter.h"
#include "parameter_idx.h"
#include "sd_file.h"
#include "weld_data.h"


#define MAX_BUFF_IXD		4

static	uint8_t		bufWeld[MAX_BUFF_IXD][MAX_BUF_LEN+4];//the write-read buffer
static  uint8_t*	buffToStore = &bufWeld[0][0];
static  uint16_t	buffStoreIndex = 0;

enum{
	SAVE_NEW,
	SAVE_APPEND,
	SAVE_IM,
	SAVE_IDLE,
};


static void GetWeldingData(OneWeldData* data)
{
	parameter_Get(PARA_IDX_weldWorkPoint,WHOLE_OBJECT,&data->volt);
	parameter_Get(PARA_IDX_weldCurrent,WHOLE_OBJECT,&data->curr);
	parameter_Get(PARA_IDX_weldSpeed,WHOLE_OBJECT,&data->speed);
	(void)RTC_Get(IDX_RTC_ST,0,(void*)&data->time);
}


uint16_t AddOneData(uint16_t welding)
{
	static uint32_t		bufWeldIdx = 0;//the write-read buffer
	static uint32_t		saveCount = 0;
	uint16_t	ret    = WARNING;
	uint32_t    saveAct = 0;
	uint16_t	newData = 0;

	OneWeldData	 weldData;
	if(welding)
	{
		GetWeldingData(&weldData);
		newData = 1;
	}
	else
	{
		GetWeldingData(&weldData);
		weldData.curr = 1;
		weldData.speed = 1;
		weldData.volt = 1;
		if(saveCount != 0)
		{
			saveAct = 1;
			newData = 1;
		}
	}
	if(newData)
	{
		(void)memcpy(buffToStore+bufWeldIdx,&weldData ,ONE_WELD_DATA_SIZE);
		bufWeldIdx += ONE_WELD_DATA_SIZE ;
		if( (bufWeldIdx % ONE_PAGE_SIZE) == ONE_PAGE_DATA_SIZE)
		{
			bufWeldIdx += ONE_PAGE_CRC_SIZE;
		}
		saveCount++;
	}


	if( (bufWeldIdx >= MAX_BUF_LEN) || (saveAct))
	{
		FileAction act;
		if(saveCount <= SAVE_COUNT_MIN)
			act.action = WELD_DATA_SAVE_NEW;
		else
			act.action = WELD_DATA_SAVE_APPEND;

		act.buff = buffToStore;
		act.len = MAX_BUF_LEN;
		SendFileMsg(&act);
		if(!welding)
		{
			//send message to copy;
			act.action = COPY_FILES_REQ;
			SendFileMsg(&act);
		}
		bufWeldIdx = 0;
		buffStoreIndex++;
		buffStoreIndex = buffStoreIndex % MAX_BUFF_IXD;
		buffToStore = &bufWeld[buffStoreIndex][0];
		memset(buffToStore,0x0, MAX_BUF_LEN);
		ret = OK;
	}
	if(!welding)
		saveCount = 0;
	return ret;
}

