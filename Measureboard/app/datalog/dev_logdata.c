/*
 * dev_logdata.c
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
#include "unit_rtc_cfg.h"
#include "dev_flash_fat.h"
#include <string.h>




static FilterData		filterData =
{
		{0,0,0},
		0,
		0,
		0,

};


uint16_t Save2FF(uint16_t type)
{
	uint16_t ret = FATAL_ERROR;
	const LogData* inst = GetLogDataInst(type);
	if(inst)
	{
		inst->Lock();
		if(*(inst->logSaveReq) != 0)
		{
			*(inst->logSaveReq) = 0;
			inst->UpdateCrc(inst);
			FileInfo* file = FF_Open(inst->fatType, OPEN_WRITE);
			assert(file);
			if(file)
			{
				ret = FF_Append_Write(file,inst->wrData.ptrLogData, inst->dataLen);

				if(ret == OK)
				{
				}
				else
				{
					TraceDBG(TSK_ID_DATA_LOG, "append write to flash failed!\n");
				}

				ret |= FF_Close(file);
			}
			if(ret != OK)
			{
				TraceDBG(TSK_ID_DATA_LOG, "data save to flash failed!\n");
			}
		}
		else
		{
			ret = OK;
		}
		inst->UnLock();
	}

	return ret;
}


uint16_t SetFilter(Filter* filter)
{
	uint16_t ret = OK;

	uint16_t filterType = filter->filterType;
	if(( (filterType <= FILTER_SRC_0) && (filterData.filter.filterType <= FILTER_SRC_0) ) || \
			( (filterType <= FILTER_SRC_1) && (filterData.filter.filterType <= FILTER_SRC_1) ) || \
			( (filterType <= FILTER_SRC_2) && (filterData.filter.filterType <= FILTER_SRC_2) )
			)
	{
		if(filterData.filter.endTime == filter->endTime )
		{
		}
		else
		{
			filterData.startAdr = 0;
			//todo
			filterData.currentAdr = 0;
		}
		if(filterData.filter.startTime == filter->startTime)
		{
		}
		else
		{
			filterData.endAdr = 0;
			//todo
			filterData.currentAdr = 0;
		}
		if(filterData.filter.filterType != filterType)
			filterData.currentAdr = filterData.endAdr;
	}
	else
	{
		filterData.endAdr = 0;
		filterData.startAdr = 0;
		filterData.currentAdr = 0;
	}

	filterData.filter = *filter;

	return ret;
}

uint16_t GetOneData_Filter(Filter* filter,uint8_t* rawData, uint16_t len)
{
	uint16_t ret = FATAL_ERROR;
	const LogData* inst = GetLogInst(filter);
	inst->Lock();
	FileInfo* file = FF_Open(inst->fatType, OPEN_READ);
	ret = FF_ReadSeek(file,&filterData.currentAdr);
	//this function will not stuck in while loop
	while(1)
	{
		assert(len >= (inst->crcLen+CRC_LEN));

		ret = FF_Append_Read(file, rawData,
				len,&filterData.currentAdr,READ_OLD );

		if(ret == OK)
		{
			ret = inst->CheckData(inst, rawData);
			if(ret == OK)
			{
				ret = inst->CheckDataFilter(inst, filter, rawData);
				if(ret == OK)
				{
					if(filterData.endAdr == 0)
					{
						filterData.endAdr = filterData.currentAdr;
					}
					else
					{
						filterData.startAdr = filterData.currentAdr;
					}
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	if(ret != OK)
		FF_Close(file);

	inst->UnLock();
	return ret;
}



