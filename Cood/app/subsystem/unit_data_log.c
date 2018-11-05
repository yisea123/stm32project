/*
 * unit_data_log.c
 *
 *  Created on: 2016��11��18��
 *      Author: pli
 */
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_flowstep.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_act.h"
#include "unit_flow_cfg.h"
#include "unit_meas_cfg.h"
#include "unit_data_log.h"
#include "dev_log_sp.h"
#include "dev_spiflash.h"
#include "dev_logdata.h"
#include "dev_flash_fat.h"
#include "tsk_sch.h"
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

enum
{
	ATR_SET_FILTER,
	ATR_NEW_MEAS,
	ATR_NEW_CALI,
	ATR_NEW_EVENT,
	ATR_NEW_FILTER,
	ATR_MAX
}ATR_IDX;



CaliData	 		tempCaliData;
MeasData 			tempMeasData;
EventDataLog 		tempEventData;
MeasDataRawLog		tempRawMeasData;
Filter				tempFilter;
uint8_t 			tempReadBuff[FLASH_PAGE_SIZE];
static TimeCfg		filterStartTime;
static TimeCfg		filterEndTime;
//-------------------------------------------------------------------------------------------------
uint16_t			flashOperation[2];
uint16_t			dataLogFileReset[4];
uint16_t			filterOperation;
uint16_t			triggerSave[3];
static uint8_t*			copyAdr = NULL;
static uint16_t			copyLen = 0;
static uint32_t			fileInfo[4];

static const Filter				tempFilter_Default = {0,0,0,};
//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
	//0
	CONSTRUCT_ARRAY_SIMPLE_U16(&flashOperation[0],2, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&dataLogFileReset[0],4, RAM),
	CONSTRUCT_STRUCT_TIME(&filterStartTime, RAM),
	CONSTRUCT_STRUCT_TIME(&filterEndTime, RAM),
	CONSTRUCT_SIMPLE_U16(&tempFilter.filterType, RAM),
	//5
	CONSTRUCT_SIMPLE_U16(&filterOperation, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(&fileInfo[0],sizeof(fileInfo)/sizeof(uint32_t), RAM),
	NULL_T_DATA_OBJ,
	CONSTRUCT_ARRAY_SIMPLE_U8(&tempReadBuff[0],sizeof(tempReadBuff), RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(&tempReadBuff[0],sizeof(tempReadBuff), RAM),
	//10
	CONSTRUCT_ARRAY_SIMPLE_U8(&tempReadBuff[0],sizeof(tempReadBuff), RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(&tempReadBuff[0],sizeof(tempReadBuff), RAM),

	CONSTRUCT_STRUCT_MEASDATA(&tempMeasData.measDataSt,READONLY_RAM),
	CONSTRUCT_STRUCT_CALIDATA(&tempCaliData.caliDataSt,READONLY_RAM),

	CONSTRUCT_STRUCT_MEASDATARAW(&tempRawMeasData.measDataRaw,READONLY_RAM),
	//15
	CONSTRUCT_STRUCT_EVENTDATA(&tempEventData.eventData, READONLY_RAM),
	NULL_T_DATA_OBJ,//CONSTRUCT_SIMPLE_U32(&tempEventData.eventType, READONLY_RAM),
	NULL_T_DATA_OBJ,//CONSTRUCT_ARRAY_SIMPLE_U8(&tempEventData.rev[0],sizeof(tempEventData.rev)/sizeof(uint8_t), READONLY_RAM),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	//20
	CONSTRUCT_ARRAY_SIMPLE_U16(&triggerSave[0],sizeof(triggerSave)/sizeof(uint16_t), RAM),
};

static const  T_DATACLASS _ClassList[]=
{
	//lint -e545 -e651
	CONSTRUCTOR_DC_DYNAMIC(tempFilter,tempFilter_Default),
};

//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT dataLog =
{
	(uint8_t*)"dataLog",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_DataLog, // will be overloaded
	LoadRomDefaults_T_UNIT,
	Get_DataLog,
	Put_DataLog,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};


uint16_t Initialize_DataLog(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&dataLog);
	result = Initialize_T_UNIT(me,typeOfStartUp);
	if(result==OK)
	{
		/*if( (typeOfStartUp & INIT_HARDWARE) != 0)
		{
		}

		if( (typeOfStartUp & INIT_TASKS) != 0)
		{
		}
		*/
		if((typeOfStartUp & INIT_DATA) != 0)
		{
			(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
		}
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put data log
 \param  me = pointer to subsystem
 \param  objectIndex = object index.
 \param  attributeIndex = attribute index.
 \param  ptrValue = pointer to object value.
 \return T_UNIT error code.
 \warning
 \test	NA
 \n by:
 \n intention:
 \n result module test:
 \n result Lint Level 3:
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Put_DataLog(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &dataLog);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);


	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
		switch(objectIndex)
		{
			case IDX_TRIG_FAKESAVE:
				if(attributeIndex>=0)
				{
					if(triggerSave[attributeIndex])
					{
						triggerSave[attributeIndex] = 0;
						if(0 == attributeIndex)
						{
						//	Fake_TrigCaliStorage();
						}
						else if(1 == attributeIndex)
						{
						//	Fake_TrigMeasStorage();
						}
						else
						{
							NewEventLog(EV_FAKE_TEST, NULL);
						}
					}
				}
				break;
			case IDX_FLASH_OPERATION:
				if(attributeIndex == 0)
				{
					result = Dev_SpiFlash_EraseBlk(flashOperation[attributeIndex]);
				}
				else
				{
					copyAdr = NULL;
					copyLen = 0;
					result = Dev_SpiFlash_ReadWrite(NULL, tempReadBuff, (uint32_t)(flashOperation[attributeIndex]*FLASH_PAGE_SIZE), FLASH_PAGE_SIZE);
				}
				break;
			case IDX_INIT_DATALOG:
				if(attributeIndex == 0)
				{
					result |= ResetLogFiles(HISTORY_MEASDATA_LOG);

				}
				else if (attributeIndex == 1)
				{
					result |= ResetLogFiles(HISTORY_CALIDATA_LOG);
				}
				else if (attributeIndex == 2)
				{
					result |= ResetLogFiles(HISTORY_EVENTDATA_LOG);
				}
				else
				{
					result |= ResetLogFiles(0xFFFF);
				}
				break;
			case IDX_SET_FILTER_TYPE:
				if(FILTER_SRC_2 < tempFilter.filterType)
				{
					result = GREATERTHAN_RANGEMAX_ERR;
				}
				break;
			case IDX_FILTER_ACTION:
				if(filterOperation)
				{
					tempFilter.startTime = CalcTime_ST(&filterStartTime);
					tempFilter.endTime = CalcTime_ST(&filterEndTime);
					result |= SetFilter(&tempFilter);
					if(result == OK)
					{
						switch(tempFilter.filterType)
						{
							case ALL_EVENT:
							case WARNING_EVENTS:
							case ERROR_EVENTS:
							case OTHER_EVENTS:
								copyAdr = (void*)&tempEventData;
								copyLen = sizeof(tempEventData);
								result |= GetOneData_Filter(&tempFilter,(void*)&tempEventData,sizeof(tempEventData));
								break;

							case MEAS_DATA_LOG:
								copyAdr = (void*)&tempMeasData;
								copyLen = sizeof(tempMeasData);
								result |= GetOneData_Filter(&tempFilter,copyAdr,copyLen);
								break;
							case CALI_DATA_LOG:
								copyAdr = (void*)&tempCaliData;
								copyLen = sizeof(tempCaliData);
								result |= GetOneData_Filter(&tempFilter,copyAdr,copyLen);
								break;
							case MEAS_RAW_DATA_LOG:
								copyAdr = (void*)&tempRawMeasData;
								copyLen = sizeof(tempRawMeasData);
								result |= GetOneData_Filter(&tempFilter,copyAdr,copyLen);
								break;
							default:
								result = RULE_VIOLATION;
								break;
						}
					}
				}
				break;

		}
	}

	return result;
}



//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get data log
 \param  me = pointer to subsystem
 \param  objectIndex = object index.
 \param  attributeIndex = attribute index.
 \param  ptrValue = pointer to object value.
 \return T_UNIT error code.
 \warning
 \test	NA
 \n by:
 \n intention:
 \n result module test:
 \n result Lint Level 3:
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Get_DataLog(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &dataLog);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	switch(objectIndex)
	{
		case IDX_READ_BUF0:
			if((copyAdr) && (copyLen != 0))
			{
				uint16_t len = copyLen;
				if(len > FLASH_PAGE_SIZE)
				{
					len = FLASH_PAGE_SIZE;
				}
				memcpy((void*)tempReadBuff,(void*)copyAdr, len);
			}
			break;
		case IDX_READ_BUF1:
			if((copyAdr) && (copyLen != 0))
			{
				uint16_t len = copyLen;
				memset((void*)tempReadBuff,0, len);
				if(len > FLASH_PAGE_SIZE)
				{
					len -= FLASH_PAGE_SIZE;
					memcpy((void*)tempReadBuff,(void*)copyAdr, len);
				}
			}
			else
			{
			}
			break;
		case IDX_READ_BUF2:
			if((copyAdr) && (copyLen != 0))
			{
				uint16_t len = copyLen;
				memset((void*)tempReadBuff,0, len);
				if(len > FLASH_PAGE_SIZE*2)
				{
					len -= FLASH_PAGE_SIZE*2;
					memcpy((void*)tempReadBuff,(void*)copyAdr, len);
				}
			}
			break;
		case IDX_READ_BUF3:
			if((copyAdr) && (copyLen != 0))
			{
				uint16_t len = copyLen;
				memset((void*)tempReadBuff,0, len);
				if(len > FLASH_PAGE_SIZE*3)
				{
					len -= FLASH_PAGE_SIZE*3;
					memcpy((void*)tempReadBuff,(void*)copyAdr, len);
				}
			}
			break;
		case IDX_SET_FILTER_INFO:

			if(fileInfo[0] < FF_FILE_COUNT)
			{
				fileInfo[1] = fatFiles[ fileInfo[0] ].readAdr;
				fileInfo[2] = fatFiles[ fileInfo[0] ].readIdx;
				fileInfo[3] = fatFiles[ fileInfo[0] ].validCount;
			}
			else
			{
				fileInfo[1] = 0;
				fileInfo[2] = 0;
				fileInfo[3] = 0;
			}
			break;
		default:
			break;

	}
	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	return result;
}


