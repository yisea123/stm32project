/*
 * dev_log_sp.c
 *
 *  Created on: 2016��11��17��
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
#include "unit_sys_diagnosis.h"
#include <string.h>

#define MAT(x)	pow(2,(int32_t)(log10((x))/log10(2)))

#define SIZE2POW(x) {(x)>MAT(x)? 2 * MAT(x) : MAT(x) }

static CaliData 		wrCaliData;
static MeasData 		wrMeasData;
static EventDataLog 		wrEventDataLog;
static MeasDataRawLog 		wrMeasDataRawLog;
static uint16_t 		dataUpdateCount[5] = {0,0,0,0,0};


#define EVENT_MAX_NUM	16
static 	EventDataLog eventLog[EVENT_MAX_NUM];

static uint16_t UpdateData_Log(const struct _LogData* inst);

static uint16_t	UpdateCrc_log(const LogData* data);
static uint16_t	CheckCrc_log(const LogData* data, uint8_t* rawData);


static uint16_t	CheckDataFilter_Measlog(const LogData* data, Filter* filter, uint8_t* rawData);
static uint16_t	CheckDataFilter_Calilog(const LogData* data, Filter* filter, uint8_t* rawData);
static uint16_t	CheckDataFilter_Eventlog(const LogData* data, Filter* filter, uint8_t* rawData);
static uint16_t	CheckDataFilter_MeasRawlog(const LogData* data, Filter* filter, uint8_t* rawData);

static uint16_t SaveEventToFlash(void);

static void LogLock(void);
static void LogUnLock(void);

static uint16_t CheckEventFilter(uint32_t event, uint32_t filter);
static SemaphoreHandle_t lock = NULL;
static SemaphoreHandle_t logLock = NULL;


static const LogData	logDataCali =
{
	HISTORY_CALIDATA_LOG,//get 1024
	128,//1024/(1024/sizeof(CaliData)),
	(sizeof(CaliData)-CRC_LEN),
	CALI_LOG_TIMES,
	.wrData.ptrCaliData = &wrCaliData,
	&dataUpdateCount[0],
	UpdateCrc_log,
	UpdateData_Log,
	CheckCrc_log,
	CheckDataFilter_Calilog,
	LogLock,
	LogUnLock,
};
static const LogData	logDataMeas =
{
	HISTORY_MEASDATA_LOG,
	64,//1024/(1024/sizeof(MeasData)),
	(sizeof(MeasData)-CRC_LEN),
	MEAS_LOG_TIMES,
	.wrData.ptrMeasData = &wrMeasData,
	&dataUpdateCount[1],
	UpdateCrc_log,
	UpdateData_Log,
	CheckCrc_log,
	CheckDataFilter_Measlog,
	LogLock,
	LogUnLock,
};
static const LogData	logDataEvent =
{
	HISTORY_EVENTDATA_LOG,
	16,//1024/(1024/sizeof(EventDataLog)),
	(sizeof(EventDataLog)-CRC_LEN),
	EVENT_LOG_TIMES,
	.wrData.ptrEventDataLog = &wrEventDataLog,
	&dataUpdateCount[2],
	UpdateCrc_log,
	UpdateData_Log,
	CheckCrc_log,
	CheckDataFilter_Eventlog,
	LogLock,
	LogUnLock,
};


static const LogData	logDataMeasRaw =
{
	HISTORY_RAW_MEASDATA_LOG,
	32,//1024/(1024/sizeof(EventDataLog)),
	(sizeof(MeasDataRawLog)-CRC_LEN),
	MEAS_RAW_LOG_TIMES,
	.wrData.ptrMeasDataRaw = &wrMeasDataRawLog,
	&dataUpdateCount[3],
	UpdateCrc_log,
	UpdateData_Log,
	CheckCrc_log,
	CheckDataFilter_MeasRawlog,
	LogLock,
	LogUnLock,
};


const LogData* allLogFIle[] =
{
	&logDataMeas,
	&logDataCali,
	&logDataEvent,
	&logDataMeasRaw,
};



static uint16_t	UpdateCrc_log(const LogData* data)
{
	uint16_t ret = OK;
	assert(data);
	uint16_t crc = 0x0;
	switch(data->fatType)
	{
		case HISTORY_MEASDATA_LOG:
			assert(data->dataLen >= sizeof(MeasData));
			crc = CalcCrc16Mem_COMMON((void*)data->wrData.ptrMeasData, InitCRC16_COMMON(), data->crcLen);
			data->wrData.ptrMeasData->crc = crc;
			break;
		case HISTORY_CALIDATA_LOG:
			assert(data->dataLen >= sizeof(CaliData));
			crc = CalcCrc16Mem_COMMON((void*)data->wrData.ptrCaliData, InitCRC16_COMMON(), data->crcLen);
			data->wrData.ptrCaliData->crc = crc;
			break;
		case HISTORY_EVENTDATA_LOG:
			assert(data->dataLen >= sizeof(EventDataLog));
			crc = CalcCrc16Mem_COMMON((void*)data->wrData.ptrEventDataLog, InitCRC16_COMMON(), data->crcLen);
			data->wrData.ptrEventDataLog->crc = crc;
			break;
		case HISTORY_RAW_MEASDATA_LOG:
			assert(data->dataLen >= sizeof(MeasDataRawLog));
			crc = CalcCrc16Mem_COMMON((void*)data->wrData.ptrMeasDataRaw, InitCRC16_COMMON(), data->crcLen);
			data->wrData.ptrMeasDataRaw->crc = crc;
			break;
		default:
			//todo extend for other log data;
			ret = FATAL_ERROR;
			break;
	}
	return ret;
}

static uint16_t	CheckCrc_log(const LogData* data, uint8_t* rawData)
{
	uint16_t ret = FATAL_ERROR;
	assert(data);
	uint16_t crc = 0x0;

	switch(data->fatType)
	{
		case HISTORY_MEASDATA_LOG:
			assert(data->dataLen >= sizeof(MeasData));
			crc = CalcCrc16Mem_COMMON(rawData, InitCRC16_COMMON(), data->crcLen);
			if(((MeasData*)rawData)->crc == crc)
			{
				ret = OK;
			}
			break;
		case HISTORY_CALIDATA_LOG:
			assert(data->dataLen >= sizeof(CaliData));
			crc = CalcCrc16Mem_COMMON(rawData, InitCRC16_COMMON(), data->crcLen);
			if(((CaliData*)rawData)->crc == crc)
			{
				ret = OK;
			}
			break;
		case HISTORY_EVENTDATA_LOG:
			assert(data->dataLen >= sizeof(EventDataLog));
			crc = CalcCrc16Mem_COMMON(rawData, InitCRC16_COMMON(), data->crcLen);
			if(((EventDataLog*)rawData)->crc == crc)
			{
				ret = OK;
			}
			break;
		case HISTORY_RAW_MEASDATA_LOG:
			assert(data->dataLen >= sizeof(MeasDataRawLog));
			crc = CalcCrc16Mem_COMMON(rawData, InitCRC16_COMMON(), data->crcLen);

			if(((MeasDataRawLog*)rawData)->crc == crc)
			{
				ret = OK;
			}
			break;
		default:
			//todo extend for other log data;
			ret = FATAL_ERROR;
			break;
	}
	return ret;
}

static uint16_t CheckTimeInRange(uint32_t checkTime, uint32_t startTime, uint32_t endTime)
{
	if(( checkTime >= startTime ) && (checkTime <= endTime))
	{
		return OK;
	}
	return FATAL_ERROR;
}

static uint16_t	CheckDataFilter_Measlog(const LogData* data, Filter* filter, uint8_t* rawData)
{
	uint16_t ret = FATAL_ERROR;
	assert(data);
	assert(filter);

	assert(rawData);
	MeasData* ptrData = ((MeasData*)rawData);

	ret = CheckTimeInRange(ptrData->measDataSt.startTimeST, filter->startTime, filter->endTime);

	return ret;
}

static uint16_t	CheckDataFilter_MeasRawlog(const LogData* data, Filter* filter, uint8_t* rawData)
{
	uint16_t ret = FATAL_ERROR;
	assert(data);
	assert(filter);
	assert(rawData);
	MeasDataRawLog* ptrData = ((MeasDataRawLog*)rawData);

	ret = CheckTimeInRange(ptrData->measDataRaw.startTimeST, filter->startTime, filter->endTime);

	return ret;
}


static uint16_t	CheckDataFilter_Calilog(const LogData* data, Filter* filter, uint8_t* rawData)
{
	uint16_t ret = FATAL_ERROR;
	assert(data);
	assert(filter);
	assert(rawData);
	CaliData* ptrData = ((CaliData*)rawData);

	ret = CheckTimeInRange(ptrData->caliDataSt.startTimeST, filter->startTime, filter->endTime);

	return ret;
}


static uint16_t	CheckDataFilter_Eventlog(const LogData* data, Filter* filter, uint8_t* rawData)
{
	uint16_t ret = FATAL_ERROR;
	assert(data);
	assert(filter);
	assert(rawData);

	EventDataLog* ptrData = ((EventDataLog*)rawData);
	assert(data->dataLen >= sizeof(EventDataLog));
	ret = CheckTimeInRange(ptrData->eventData.startTimeST, filter->startTime, filter->endTime);
	if(ret == OK)
	{
		switch(filter->filterType)
		{
			case ALL_EVENT:
				break;

			case WARNING_EVENTS:
			case ERROR_EVENTS:
			case OTHER_EVENTS:
				ret = CheckEventFilter(ptrData->eventData.eventType, filter->filterType);
				break;
			default:
				assert(0);
				break;
		}
	}

	return ret;
}


static uint16_t CheckEventFilter(uint32_t event, uint32_t filter)
{
	//todo
	(void)event;
	(void)filter;

	return OK;
}




static void LogLock(void)
{
	OS_Use(logLock);
}
static void LogUnLock(void)
{
	OS_Unuse(logLock);
}


static EventDataLog* GetEventDataLog(uint16_t st)
{
	static uint16_t idx = 0;
	static uint16_t getId = 0;
	EventDataLog* inst = NULL;
	OS_Use(lock);
	if(st)
	{
		if(getId != idx)
		{
			inst = &eventLog[(getId++)%EVENT_MAX_NUM];
		}
	}
	else
	{
		inst = &eventLog[(idx++)%EVENT_MAX_NUM];
	}
	OS_Unuse(lock);
	return inst;
}

static uint16_t SaveDataLog(void)
{
	uint16_t ret = SaveEventToFlash();
	ret |= Save2FF(HISTORY_MEASDATA_LOG);
	ret |= Save2FF(HISTORY_CALIDATA_LOG);
	ret |= Save2FF(HISTORY_RAW_MEASDATA_LOG);
	return ret;
}


static uint16_t UpdateData_Log(const struct _LogData* inst)
{
	assert(inst);

	inst->Lock();
	*inst->logSaveReq = 1;
	inst->UnLock();
	return OK;
}

static uint16_t SaveEventToFlash(void)
{
	uint16_t ret = OK;
	const EventDataLog* eventLog1 = NULL;
	for(;;)
	{
		eventLog1 = GetEventDataLog(1);
		if(eventLog1)
		{
			const LogData* dataInst = GetLogDataInst(HISTORY_EVENTDATA_LOG);
			if(dataInst)
			{
				dataInst->Lock();
				EventDataLog* logInst = dataInst->wrData.ptrEventDataLog;
				memcpy((void*)logInst, (void*)eventLog1,sizeof(EventDataLog));
				dataInst->UnLock();
				dataInst->UpdateData(dataInst);
			}
			ret = Save2FF(HISTORY_EVENTDATA_LOG);
		}
		else
		{
			ret = OK;
			break;
		}
	}
	return ret;
}




/////////////////////////////public interfaces//////////////////////////

uint16_t InitAllLogFiles(void)
{
	uint16_t ret = FF_Init();
	if(lock == NULL)
		lock = OS_CreateSemaphore();
	if(logLock == NULL)
		logLock = OS_CreateSemaphore();
	ret |= FF_Mount(allLogFIle, sizeof(allLogFIle)/sizeof(LogData *));//get all file include the address, size;..etc


	return ret;
}

uint16_t ResetLogFiles(uint16_t type)
{
	uint16_t result = OK;

	if(type == HISTORY_MEASDATA_LOG)
	{
		FileInfo* file = FF_Open(HISTORY_MEASDATA_LOG, OPEN_WRITE);//open file
		result = FF_InitFile(file);
		result |= FF_Close(file);//close file
	}
	else if (type == HISTORY_CALIDATA_LOG)
	{
		FileInfo* file = FF_Open(HISTORY_CALIDATA_LOG, OPEN_WRITE);//open file
		result = FF_InitFile(file);
		result |= FF_Close(file);//close file
	}
	else if (type == HISTORY_EVENTDATA_LOG)
	{
		FileInfo* file = FF_Open(HISTORY_EVENTDATA_LOG, OPEN_WRITE);//open file
		result = FF_InitFile(file);
		result |= FF_Close(file);//close file
	}
	else if (type == HISTORY_RAW_MEASDATA_LOG)
	{
		FileInfo* file = FF_Open(HISTORY_RAW_MEASDATA_LOG, OPEN_WRITE);//open file
		result = FF_InitFile(file);
		result |= FF_Close(file);//close file
	}
	else if(type == 0xFFFF)
	{
		FileInfo* file = FF_Open(HISTORY_MEASDATA_LOG, OPEN_WRITE);//open file
		result |= FF_InitFile(file);
		result |= FF_Close(file);//close file
		file = FF_Open(HISTORY_CALIDATA_LOG, OPEN_WRITE);//open file
		result |= FF_InitFile(file);
		result |= FF_Close(file);//close file
		file = FF_Open(HISTORY_EVENTDATA_LOG, OPEN_WRITE);//open file
		result |= FF_InitFile(file);
		result |= FF_Close(file);//close file

	}
	else
	{
		result = FATAL_ERROR;
	}

	return result;
}



const LogData* GetLogDataInst(uint16_t fatType)
{
	const LogData* inst = NULL;
	switch(fatType)
	{
		case HISTORY_MEASDATA_LOG:
			inst = &logDataMeas;
			break;
		case HISTORY_CALIDATA_LOG:
			inst = &logDataCali;
			break;
		case HISTORY_EVENTDATA_LOG:
			inst = &logDataEvent;
			break;
		case HISTORY_RAW_MEASDATA_LOG:
			inst = &logDataMeasRaw;
			break;
		default:
			//todo extend for other log data;
			assert(0);
			break;
	}
	return inst;
}

const LogData* GetLogInst(Filter* filter)
{
	const LogData* inst = NULL;
	switch(filter->filterType)
	{
		case ALL_EVENT:
		case WARNING_EVENTS:
		case ERROR_EVENTS:
		case OTHER_EVENTS:
			inst = &logDataEvent;
			break;
		case MEAS_DATA_LOG:
			inst = &logDataMeas;
			break;
		case CALI_DATA_LOG:
			inst = &logDataCali;
			break;
		case MEAS_RAW_DATA_LOG:
			inst = &logDataMeasRaw;
			break;
		default:
			assert(0);
			break;

	}
	return inst;
}


uint16_t NewEventLog(uint32_t event, uint8_t* data)
{
	EventDataLog* inst = GetEventDataLog(0);
	inst->eventData.eventType = event;
	if(data)
		memcpy((void*)&inst->eventData.rev[0], (void*)data, sizeof(inst->eventData.rev));
	(void)RTC_Get(IDX_RTC_ST,0,(void*)&(inst->eventData.startTimeST));
	if(data)
		TraceUser("New event is pushed: 0x%x - %x,%x,%x,%x,%x,%x\n", event, data[0],data[1],data[2],data[3],data[4],data[5]);
	else
		TraceUser("New event is pushed: 0x%x - null\n", event);
	Trigger_Save2FF(NULL);
	//store to the object which access by UI
	if(	(eventMask & event ) == 0)//EV_CMD_PUT_OBJ	!= event && EV_CMD_PUT_MEM	!= event)
	{

		if (((EV_DIA_SET | EV_DIA_CLR) & event) != 0)
		{
			EventData* buff = GetAvailableDiagBuff(GET_NEW);

			memcpy((void*) buff, (void*) &inst->eventData, sizeof(EventData));
		}
		else
		{
			EventData* buff = GetAvailableEventBuff(GET_NEW);
			TraceUser("New event is pushed: 0x%x\n", event);
			memcpy((void*)buff, (void*)&inst->eventData, sizeof(EventData));
		}
	}
	else
	{
		TraceMsg(TSK_ID_MCU_STATUS, "New event is not pushed: 0x%x\n", event);
	}
	return OK;
}


uint16_t Trigger_Save2FF(ptrTskCallBack ptrFunc)
{
	static  TSK_MSG inst;
	inst.tskState = TSK_INIT;
	inst.val.p = SaveDataLog;
	inst.callBack = ptrFunc;

	MsgPush (SCH_DATALOG_ID, (uint32_t)&inst, 0);

	return OK;
}



