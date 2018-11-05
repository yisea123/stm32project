/*
 * dev_log_sp.h
 *
 *  Created on: 2016��11��17��
 *      Author: pli
 */

#ifndef DEV_LOG_SP_H_
#define DEV_LOG_SP_H_

#include "t_data_obj_measdata.h"


//definition of device specific;
#define FF_FILE_COUNT		0x04
//if this cersion is changed, the whole log is clean;
#define FF_FAT_VERSION		0x1111


// to extend by end user
typedef enum
{
	NULL_FILE_LOG	=	0,
	HISTORY_MEASDATA_LOG = 0x10,
#define MEAS_DATA_VERSION		0x1000
	HISTORY_CALIDATA_LOG = 0x20,
#define CALI_DATA_VERSION		0x1010
	HISTORY_EVENTDATA_LOG = 0x30,
#define EVENT_DATA_VERSION		0x1020
	HISTORY_RAW_MEASDATA_LOG = 0x40,
#define RAWMEAS_DATA_VERSION		0x1030

#define DATA_LOG_SIZE			3
}FILE_LOG_TYPE_INFO;

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	MeasDataSt	measDataSt;
	uint16_t	crc;
}MeasData ;
//64 bytes;


//312 bytes,
typedef struct
{
	CaliDataSt  caliDataSt;
	uint16_t	crc;
}CaliData;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef struct
{
	MeasDataRaw  measDataRaw;
	uint16_t	crc;
}MeasDataRawLog;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
//16bytes
typedef struct
{
	EventData	eventData;
	uint16_t	crc;
}EventDataLog ;

typedef struct
{
	uint32_t 	startTime;
	uint32_t 	endTime;
	uint16_t	filterType;
}Filter;


#pragma pack(pop)

//32 bytes;
typedef enum
{
	EV_RESET 		= 0x00100000,
	EV_DIAG_INFO 	= 0x00200000,
	EV_DIAG_WARN 	= 0x00400000,
	EV_DIAG_ERR 	= 0x00800000,
	EV_DIA_SET		= 0x00080000,
	EV_DIA_CLR		= 0x00040000,

	EV_SCH_STOP     = 0x00000100,
	EV_FLUSH		= 0x00000200,
	EV_PRIME		= 0x00000400,
	EV_DRAIN		= 0x00000800,
	EV_FLOWSTEP		= 0x00001000,
	EV_SCH_START	= 0x00002000,
	EV_RTC_UPDATE   = 0x00004000,
	EV_SW_UPGRADE   = 0x00008000,
	EV_CFG_RST  	= 0x00010000,

	EV_STOP_ACT  	= 0x00000001,
	EV_START_ACT  	= 0x00000002,
	EV_LOARD_FACT_SET = 0x00000004,

	EV_CALIBRATION 	= 0x01000000,
	EV_CLEANING 	= 0x02000000,
	EV_MEASURE		= 0x04000000,
	EV_CALC_GAIN 	= 0x08000000,

	EV_CMD_PUT_OBJ	= 0x10000000,
	EV_CMD_PUT_MEM	= 0x20000000,
	EV_AUTO_RANGE_REQ = 0x40000000,
	EV_FAKE_TEST	= 0x80000000,
}EVENT_ID;


typedef enum
{
	ALL_EVENT = 0,
	WARNING_EVENTS,//1
	ERROR_EVENTS,//2
	OTHER_EVENTS,//3
	FILTER_SRC_0 = OTHER_EVENTS,
	MEAS_DATA_LOG,//4
	FILTER_SRC_1 = MEAS_DATA_LOG,
	CALI_DATA_LOG,//5
	FILTER_SRC_2 = CALI_DATA_LOG,
	MEAS_RAW_DATA_LOG,//6
	FILTER_SRC_3 = MEAS_RAW_DATA_LOG,
}FilterType;



typedef struct _LogData
{
	uint16_t 	fatType;
	uint16_t	dataLen;
	uint32_t 	crcLen;
	uint32_t	logMaxTime;
	union
	{
		CaliData* 		ptrCaliData;
		MeasData* 		ptrMeasData;
		EventDataLog* 		ptrEventDataLog;
		MeasDataRawLog*    ptrMeasDataRaw;
		void*			ptrLogData;
	} wrData;
	uint16_t*	logSaveReq;
	uint16_t	(*UpdateCrc)(const struct _LogData*);
	uint16_t	(*UpdateData)(const struct _LogData*);
	uint16_t	(*CheckData)(const struct _LogData*, uint8_t* rawData);
	uint16_t	(*CheckDataFilter)(const struct _LogData*, Filter* filter,uint8_t* rawData);
	void 		(*Lock)(void);
	void 		(*UnLock)(void);
}LogData;




uint16_t InitAllLogFiles(void);
uint16_t ResetLogFiles(uint16_t);

const LogData* GetLogDataInst(uint16_t fatType);
const LogData* GetLogInst(Filter* filter);


uint16_t NewEventLog(uint32_t event, uint8_t* data);
uint16_t Trigger_Save2FF(ptrTskCallBack ptrFunc);


#endif /* DEV_LOG_SP_H_ */
