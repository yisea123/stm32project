/*
 * unit_rtc_cfg.c
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_rtc_cfg.h"
#include "rtc.h"
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

#define rtcRegSize		16
static uint16_t rtcValue[IDX_RTC_MAX+1];
static uint32_t rtcReg[rtcRegSize];
static uint32_t rtcTime = 0;
static uint32_t rtcStartTime = 0;

//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
	CONSTRUCT_SIMPLE_U16(&rtcValue[0],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[1],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[2],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[3],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[4],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[5],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[6],	RAM),
	CONSTRUCT_SIMPLE_U16(&rtcValue[7],	RAM),
	CONSTRUCT_SIMPLE_T32(&rtcStartTime,		READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(&rtcReg[0],rtcRegSize,	RAM),
	CONSTRUCT_SIMPLE_T32(&rtcTime,	RAM),


};
static uint16_t rtcValue_Default[IDX_RTC_MAX+1] =
{
	10,10,10, 10,8,2016, 3,
};
static const  T_DATACLASS _ClassList[]=
{
	//lint -e545  -e651
	CONSTRUCTOR_DC_DYNAMIC(rtcValue,rtcValue_Default),
};


//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT rtcCfg =
{
	(uint8_t*)"rtcCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_RtcCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	Get_RtcCfg,
	Put_RtcCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};




static uint32_t CalcTime(RTC_DateTypeDef* ptrDate, RTC_TimeTypeDef* ptrTime)
{
	uint32_t val = 0;
	val = (uint32_t)(ptrDate->Year - T32_START_YEAR);
	val = (uint32_t)(val<<T32_YEAR) | (uint32_t)(ptrDate->Month<<T32_MONTH) | (uint32_t)(ptrDate->Date<<T32_DATE) |\
				(uint32_t)(ptrTime->Hours<<T32_HOUR) | (uint32_t)(ptrTime->Minutes<<T32_MINUTES) | (uint32_t)(ptrTime->Seconds<<T32_SECONDS);


	return val;
}

uint32_t CalcTime_ST(const TimeCfg* ptrTime)
{
	uint32_t val = 0;
	if(ptrTime->year >= (T32_START_YEAR+2000))
	{
		val = (uint32_t)(ptrTime->year - T32_START_YEAR - 2000u);
		val = (uint32_t)(val<<T32_YEAR) | (uint32_t)(ptrTime->month<<T32_MONTH) | (uint32_t)(ptrTime->date<<T32_DATE) |\
					(uint32_t)(ptrTime->hour<<T32_HOUR) | (uint32_t)(ptrTime->minute<<T32_MINUTES) | (uint32_t)(ptrTime->second<<T32_SECONDS);
	}
	else
	{
		val = 0;
	}


	return val;
}

void ConvertBack_U32Time(const uint32_t time, TimeCfg* ptrTime)
{
	ptrTime->year = (uint16_t)( 2000 + (int)(time>>T32_YEAR));
	ptrTime->month = (uint8_t)( (time>>T32_MONTH)&0x0F);
	ptrTime->date = (uint8_t)( (time>>T32_DATE)&0x1F);
	ptrTime->hour = (uint8_t)( (time>>T32_HOUR)&0x1F);
	ptrTime->minute = (uint8_t)( (time>>T32_MINUTES)&0x3F);
	ptrTime->second = (uint8_t)( (time>>T32_SECONDS)&0x3F);

}


uint16_t Initialize_RtcCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&rtcCfg);
	result = Initialize_T_UNIT(me,typeOfStartUp);
	if(result==OK)
	{
		if( (typeOfStartUp & INIT_HARDWARE) != 0)
		{
	//		result |= Init_RTC();
		}
/*
		if( (typeOfStartUp & INIT_TASKS) != 0)
		{
		}
		*/
		if((typeOfStartUp & INIT_DATA) != 0)
		{
			(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
		}
		(void)RTC_Get(IDX_RTC_ST,0,(void*)&rtcStartTime);
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}
extern RTC_HandleTypeDef sRtcHandle;


static void UpdateWeekDay(RTC_DateTypeDef* RTC_DateStruct)
{
	uint16_t year =(uint16_t)( RTC_DateStruct->Year+2000U);

	uint32_t days = CalcDays(year,RTC_DateStruct->Month, RTC_DateStruct->Date );//from 2016.1.1, w5
	RTC_DateStruct->WeekDay = (uint8_t)((days+5)%7 + 1);
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put rtc
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
uint16_t Put_RtcCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &rtcCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;


	if(objectIndex == IDX_RTC_REG )
	{
		prvWriteBackupRegister( (uint32_t)(attributeIndex), *(uint32_t*)ptrValue);
		return OK;
	}
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
		(void)HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
		(void)HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
		RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_SET;
		uint16_t val = rtcValue[objectIndex];
		switch(objectIndex)
		{
		case IDX_RTC_SEC:
			if(val < 60)
			{
				RTC_TimeStruct.Seconds = (uint8_t)val;
				(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
			}
			else
				result = GREATERTHAN_RANGEMAX_ERR;
			break;

		case IDX_RTC_MIN:
			if(val < 60)
			{
				RTC_TimeStruct.Minutes = (uint8_t)val;
				(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
			}
			else
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case IDX_RTC_HOUR:
			if(val < 24)
			{
				RTC_TimeStruct.Hours = (uint8_t)val;
				(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
			}
			else
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case IDX_RTC_DAY:
			RTC_DateStruct.Date = (uint8_t)val;
			UpdateWeekDay(&RTC_DateStruct);
			(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
			break;
		case IDX_RTC_MON:
			if(val < 13)
			{
				RTC_DateStruct.Month = (uint8_t)val;
				UpdateWeekDay(&RTC_DateStruct);
				(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
			}
			else
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case IDX_RTC_YEAR:
			if(val < 2016)
			{
				result = LESSTHAN_RANGEMIN_ERR;
			}
			else
			{
				RTC_DateStruct.Year = (uint8_t)(val-2000);
				UpdateWeekDay(&RTC_DateStruct);
				(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
			}
			break;
		case IDX_RTC_WDAY:
			if((val < 8) && (val>0))
			{
				RTC_DateStruct.WeekDay = (uint8_t)val;
				(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
			}
			else
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case IDX_RTC_CAL:
			result = RTC_SetCalibration(val);
			break;

		case IDX_RTC_ST:
			{
				TimeCfg cfg;
				ConvertBack_U32Time(*(uint32_t*)ptrValue, &cfg);
				RTC_DateStruct.Date = cfg.date;
				RTC_DateStruct.Year = (uint8_t)(cfg.year-2000);
				RTC_DateStruct.Month = cfg.month;

				RTC_TimeStruct.Seconds = cfg.second;
				RTC_TimeStruct.Minutes = cfg.minute;
				RTC_TimeStruct.Hours = cfg.hour;

				(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
				(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
			}
			break;

		default:
			break;
		}
	}

	return result;
}


#define START_DATES		730000 //start from 2000,×÷ÓÃ

/* mktime from linux kernel code, since mdk mktime doesn't work */
uint32_t CalcDays(const uint32_t year0, const uint8_t mon0, const uint8_t day)
{
	int32_t mon = (int)mon0;
	uint32_t year = year0;
	if(year <= 2000)
	{
		year = 2000;
		mon = 1;
	}
	uint32_t dateTime = 0;
    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int) (mon -= 2))
    {
        mon = (int32_t)(mon + 12);  /* Puts Feb last since it has leap day */
        year--;
    }
    dateTime = (uint32_t)((year/4 - year/100 + year/400 + (uint32_t)(367*mon/12) + (uint32_t)day) +
              year*365 );
    assert(START_DATES<=dateTime);
    dateTime -= START_DATES;
    return dateTime;/* finally seconds */
}


/* mktime from linux kernel code, since mdk mktime doesn't work */
uint32_t CalcSeconds(const uint8_t hour, const uint8_t min, const uint8_t second)
{
    uint32_t seconds = (uint32_t)second + (uint32_t)min*60 + (uint32_t)hour*3600;

    return seconds;/* finally seconds */
}


uint32_t GetSeconds(const uint32_t days, const uint32_t seconds)
{
   return days*86400 + seconds;/* finally seconds */
}

uint16_t ValidChkT32(uint32_t* ptrT32, uint16_t typ)
{
	uint16_t ret = OK;
	TimeCfg cfg;
	ConvertBack_U32Time(*(uint32_t*)ptrT32, &cfg);
	if(cfg.year <= 2010)
	{
		ret = FATAL_ERROR;
		if(typ != 0)
		{
			cfg.year = 2019;
		}
	}
	if((cfg.month >= 13) || (cfg.month < 1))
	{
		ret = FATAL_ERROR;
		if(typ != 0)
		{
			cfg.month = 1;
		}
	}
	if((cfg.date >= 32) || (cfg.date < 1))
	{
		ret = FATAL_ERROR;
		if(typ != 0)
		{
			cfg.date = 1;
		}
	}
	if(cfg.hour >= 25)
	{
		ret = FATAL_ERROR;
		if(typ != 0)
		{
			cfg.hour = 0;
		}
	}
	if(cfg.minute >= 60)
	{
		ret = FATAL_ERROR;
		if(typ != 0)
		{
			cfg.minute = 0;
		}
	}
	if(cfg.second >= 60)
	{
		ret = FATAL_ERROR;
		if(typ != 0)
		{
			cfg.second = 0;
		}
	}
	if((ret == FATAL_ERROR ) && (typ != 0))
	{
		*ptrT32 = CalcTime_ST(&cfg);
		ret = WARNING;
	}
	return ret;
}

uint32_t GetCurrentSeconds(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);

	uint32_t days = CalcDays((uint32_t)(RTC_DateStruct.Year+2000), RTC_DateStruct.Month, RTC_DateStruct.Date);
	uint32_t seconds = CalcSeconds(RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);

	return GetSeconds(days,seconds);
}

uint32_t GetCurrentST(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);

	TimeCfg timeCfg;
	timeCfg.year = (uint16_t)(RTC_DateStruct.Year + 2000);
	timeCfg.month = RTC_DateStruct.Month;
	timeCfg.date = RTC_DateStruct.Date;

	timeCfg.hour = RTC_TimeStruct.Hours;
	timeCfg.minute = RTC_TimeStruct.Minutes;
	timeCfg.second = RTC_TimeStruct.Seconds;

	return CalcTime_ST(&timeCfg);
}


uint32_t GetSecondsFromST(uint32_t stTime)
{
	TimeCfg timeCfg;
	ConvertBack_U32Time(stTime, &timeCfg);

	uint32_t days = CalcDays(timeCfg.year, timeCfg.month, timeCfg.date);
	uint32_t seconds = CalcSeconds(timeCfg.hour, timeCfg.minute, timeCfg.second);

	return GetSeconds(days,seconds);

}

//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get rtc
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
uint16_t Get_RtcCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &rtcCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	if(objectIndex == IDX_RTC_REG )
	{
		*(uint32_t*)ptrValue = prvReadBackupRegister( (uint32_t)attributeIndex);
		return OK;
	}


	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
		HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
		uint16_t* val = ptrValue;
		switch(objectIndex)
		{
		case IDX_RTC_SEC:
			*val = (uint16_t)RTC_TimeStruct.Seconds;
			break;
		case IDX_RTC_MIN:
			*val = (uint16_t)RTC_TimeStruct.Minutes;
			break;
		case IDX_RTC_HOUR:
			*val = (uint16_t)RTC_TimeStruct.Hours;
			break;
		case IDX_RTC_DAY:
			*val = (uint16_t)RTC_DateStruct.Date;
			break;
		case IDX_RTC_MON:
			*val = (uint16_t)RTC_DateStruct.Month;
			break;
		case IDX_RTC_YEAR:
			*val = (uint16_t)(RTC_DateStruct.Year+2000);
			break;
		case IDX_RTC_WDAY:
			*val = (uint16_t)RTC_DateStruct.WeekDay;
			break;
		case IDX_RTC_CAL:
			result = RTC_GetCalibration(val);
			break;
		case IDX_RTC_ST:
			*(uint32_t*)ptrValue = CalcTime(&RTC_DateStruct,&RTC_TimeStruct);
			break;
		default:
			break;
		}
	}

	return result;
}


