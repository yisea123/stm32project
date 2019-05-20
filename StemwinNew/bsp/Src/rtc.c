/*
 * dev_rtc.c
 *
 *  Created on: 2016ï¿½ï¿½8ï¿½ï¿½18ï¿½ï¿½
 *      Author: pli
 */
#include "main.h"
#include "rtc.h"
#include <string.h>


#define CFG_DEV_RTC_CLK_SOURCE_LSE 			0x00000100
#define CFG_DEV_RTC_CLK_SOURCE_LSI   		0x00000200
#define CFG_DEV_RTC_CLK_SOURCE 				CFG_DEV_RTC_CLK_SOURCE_LSE /* CFG_DEV_RTC_CLK_SOURCE_LSI */
#define RTC_DEFAULT_VAL						0x5089
#define CFG_DEV_RTC_SynchPrediv  			(0xFF)
#define CFG_DEV_RTC_AsynchPrediv 			(0x7F)

//static bool iRtcIsOk = false;
RTC_HandleTypeDef sRtcHandle;

static void prvRTC_EnableClk(void);
static void prvRTC_Config(void);


uint16_t RTC_GetCalibration(uint16_t* val)
{
	*val = (uint16_t)sRtcHandle.Instance->CALR;

	return OK;
}

uint16_t RTC_SetCalibration(uint16_t val)
{
	__HAL_RTC_WRITEPROTECTION_DISABLE(&sRtcHandle);

	/* check if a calibration is pending*/
	if ((sRtcHandle.Instance->ISR & RTC_ISR_RECALPF) != RESET) {
		/* Get tick */
		uint32_t tickstart = HAL_GetTick();

		/* check if a calibration is pending*/
		while ((sRtcHandle.Instance->ISR & RTC_ISR_RECALPF) != RESET) {
			if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE) {
				/* Enable the write protection for RTC registers */
				__HAL_RTC_WRITEPROTECTION_ENABLE(&sRtcHandle);


				return FATAL_ERROR;
			}
		}
	}

	/* Configure the Smooth calibration settings */
	sRtcHandle.Instance->CALR = (val & 0x9F);

	/* Enable the write protection for RTC registers */
	__HAL_RTC_WRITEPROTECTION_ENABLE(&sRtcHandle);
	return OK;
}


static void prvRTC_EnableClk(void)
{
	/* Enable the PWR clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Wait for RTC APB registers synchronisation */
	HAL_RTC_WaitForSynchro(&sRtcHandle);
}

static void prvRTC_Config(void)
{


	/* reset the RTC module, otherwise the RCC_RTCCLKConfig() has no effect.*/
	__HAL_RCC_BACKUPRESET_FORCE();

	delay_us(100);

	__HAL_RCC_BACKUPRESET_RELEASE();

	/* Allow access to RTC */
	HAL_PWR_EnableBkUpAccess(); //PWR_BackupAccessCmd(ENABLE);

	/* Enable the LSE OSC */
	__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	//lint -e506
	__HAL_RCC_RTC_CONFIG(CFG_DEV_RTC_CLK_SOURCE);

	/* Enable the RTC Clock */
	__HAL_RCC_RTC_ENABLE();

	/* Wait for RTC APB registers synchronisation */
	HAL_RTC_WaitForSynchro(&sRtcHandle);

}

/**
 * @brief  Reads data from the specified RTC Backup data Register.
 * @param  RTC_BKP_DR: RTC Backup data Register number.
 *          This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to
 *                          specify the register.
 * @retval None
 */
uint32_t prvReadBackupRegister(uint32_t RTC_BKP_DR)
{
	__IO uint32_t tmp = 0;

	/* Check the parameters */
	assert_param(IS_RTC_BKP(RTC_BKP_DR));

	tmp = RTC_BASE + 0x50;
	tmp += (RTC_BKP_DR * 4);

	/* Read the specified register */
	return (*(__IO uint32_t *) tmp);
}

/**
 * @brief  Writes a data in a specified RTC Backup data register.
 * @param  RTC_BKP_DR: RTC Backup data Register number.
 *          This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to
 *                          specify the register.
 * @param  Data: Data to be written in the specified RTC Backup data register.
 * @retval None
 */
void prvWriteBackupRegister(uint32_t RTC_BKP_DR, const uint32_t Data)
{
	__IO uint32_t tmp = 0;

	/* Check the parameters */
	assert_param(IS_RTC_BKP(RTC_BKP_DR));

	tmp = RTC_BASE + 0x50;
	tmp += (RTC_BKP_DR * 4);

	/* Write the specified register */
	*(__IO uint32_t *) tmp = (uint32_t) Data;
}


uint16_t Init_RTC(void)
{
	uint16_t ret = FATAL_ERROR;
	sRtcHandle.Instance = RTC;
	/* Configure the RTC data register and RTC prescaler */
	sRtcHandle.Init.AsynchPrediv = CFG_DEV_RTC_AsynchPrediv;
	sRtcHandle.Init.SynchPrediv = CFG_DEV_RTC_SynchPrediv;
	sRtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	sRtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	sRtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	sRtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	prvRTC_EnableClk();
	if (prvReadBackupRegister(RTC_BKP_DR0) != RTC_DEFAULT_VAL)
	{
		RTC_DateTypeDef RTC_DateStruct;

		/* RTC configuration  */
		prvRTC_Config();



		if (HAL_RTC_Init(&sRtcHandle) == 0)
		{
			/* Configure the time register */

			RTC_DateStruct.WeekDay = 1;
			RTC_DateStruct.Month = 10;
			RTC_DateStruct.Date = 16;
			RTC_DateStruct.Year = 2017-2000;

			HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
			ret = OK;
			//iRtcIsOk = true;
		}
		prvWriteBackupRegister(RTC_BKP_DR0, RTC_DEFAULT_VAL);
	}
	else
	{
		/* Allow access to RTC */
		HAL_PWR_EnableBkUpAccess(); //PWR_BackupAccessCmd(ENABLE);
		//iRtcIsOk = true;
		ret = OK;
	}
	return ret;
}

char* GetRTCStr()
{
	static char buff[20];
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	int len = 0;
	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
	//lint -e586
	len = snprintf((void*)buff, 20, "%04d-%02d-%02d %02d:%02d:%02d", (uint16_t)(RTC_DateStruct.Year+2000),\
			RTC_DateStruct.Month,RTC_DateStruct.Date,\
			RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
	return buff;
}


#define START_DATES		(730000+456) //start from 2000,×÷ÓÃ

/* mktime from linux kernel code, since mdk mktime doesn't work */
uint32_t CalcDays(const uint32_t year0, const uint8_t mon0, const uint8_t day)
{
	int32_t mon = (int)mon0;
	uint32_t year = year0;
	if(year < 2000)
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
    if(START_DATES<=dateTime)
    	dateTime -= START_DATES;
    return dateTime;/* finally seconds */
}

static void UpdateWeekDay(RTC_DateTypeDef* RTC_DateStruct)
{
	uint16_t year =(uint16_t)( RTC_DateStruct->Year+2000U);

	uint32_t days = CalcDays(year,RTC_DateStruct->Month, RTC_DateStruct->Date );//from 2016.1.1, w5
	RTC_DateStruct->WeekDay = (uint8_t)((days+5)%7 + 1);
}


void UpdateRTC(uint16_t eTp,uint16_t value)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
	switch(eTp)
	{
	case e_year:
		RTC_DateStruct.Year = (uint8_t)(value-2000);
		UpdateWeekDay(&RTC_DateStruct);
		(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
		break;
	case e_month:
		RTC_DateStruct.Month = value;
		UpdateWeekDay(&RTC_DateStruct);
		(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
		break;
	case e_date:
		RTC_DateStruct.Date = value;
		UpdateWeekDay(&RTC_DateStruct);
		(void)HAL_RTC_SetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
		break;
	case e_hour:
		//not sure why,

		RTC_TimeStruct.Hours = (value);
		if(RTC_TimeStruct.Hours == 0)
			RTC_TimeStruct.Hours = 24;
		RTC_TimeStruct.Hours = RTC_TimeStruct.Hours - 1;
		(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
		break;
	case e_min:

		if(RTC_TimeStruct.Hours == 0)
			RTC_TimeStruct.Hours = 24;
		RTC_TimeStruct.Hours = RTC_TimeStruct.Hours - 1;
		RTC_TimeStruct.Minutes = value;
		(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
		break;
	case e_sec:

		if(RTC_TimeStruct.Hours == 0)
			RTC_TimeStruct.Hours = 24;
		RTC_TimeStruct.Hours = RTC_TimeStruct.Hours - 1;
		RTC_TimeStruct.Seconds = value;
		(void)HAL_RTC_SetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
		break;
	}
}


uint16_t GetRTC(uint16_t eTp)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	uint16_t value = 0;
	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
	switch(eTp)
	{
	case e_year:
		value = RTC_DateStruct.Year + 2000;
		break;
	case e_month:
		value = RTC_DateStruct.Month;
		break;
	case e_date:
		value = RTC_DateStruct.Date;
		break;
	case e_hour:
		value = RTC_TimeStruct.Hours;
		break;
	case e_min:
		value = RTC_TimeStruct.Minutes;
		break;
	case e_sec:
		value = RTC_TimeStruct.Seconds;
		break;
	}
	return value;
}

int sprintf_rtc(uint8_t* buff, uint8_t len1)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	int len = 0;
	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);
	//lint -e586
	len = snprintf((void*)buff, len1, "%04d-%d-%d %02d:%02d:%02d\t", (uint16_t)(RTC_DateStruct.Year+2000),\
			RTC_DateStruct.Month,RTC_DateStruct.Date,\
			RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
	return len;

}

void xprintf_rtc()
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	HAL_RTC_GetTime(&sRtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&sRtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN);

	xprintf("%04d-%d-%d %02d:%02d:%02d\t", RTC_DateStruct.Year+2000,RTC_DateStruct.Month,RTC_DateStruct.Date,\
			RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);

}





void xprintf_tick()
{
	uint32_t tickstart = HAL_GetTick() ;

	xprintf("%08d\t", tickstart);

}
// End of file


void ConvertBack_U32Time(const uint32_t time, TimeCfg* ptrTime)
{
	ptrTime->year = (uint16_t)( 2000 + (int)(time>>T32_YEAR));
	ptrTime->month = (uint8_t)( (time>>T32_MONTH)&0x0F);
	ptrTime->date = (uint8_t)( (time>>T32_DATE)&0x1F);
	ptrTime->hour = (uint8_t)( (time>>T32_HOUR)&0x1F);
	ptrTime->minute = (uint8_t)( (time>>T32_MINUTES)&0x3F);
	ptrTime->second = (uint8_t)( (time>>T32_SECONDS)&0x3F);

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

uint16_t ValidChkT32(uint32_t* ptrT32, uint16_t typ)
{
	uint16_t ret = OK;
	TimeCfg cfg;
	ConvertBack_U32Time(*(uint32_t*)ptrT32, &cfg);
	if(cfg.year < 2019)
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
		ret = 1;
	}
	return ret;
}



static uint16_t GetDateFromDays(uint32_t _days,TimeCfg* timeCfg)
{
    int day1[]  = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int day2[]  = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int i = 0;
	_days = _days + 1;
	while(1)
	{
		if(_days <=366)
		{
			break;
		}
		if ((i+2000) % 400 == 0 || ((i+2000) % 4 == 0 && (i+2000) % 100 != 0))
		{
			_days -=366;
		}
		else
		{
			_days -=365;
		}
		i++;
	}
	if(_days == 366)
	{
		if((i + 2000) % 400 == 0 || ((i + 2000) % 4 == 0 && (i + 2000) % 100 != 0))
		{
			timeCfg->year = (i + 2000);
			timeCfg->month = 12;
			timeCfg->date = 31;
		}
		else
		{
			timeCfg->year = (i + 2000 + 1);
			timeCfg->month = 1;
			timeCfg->date = 1;
		}
	}
	else
	{
		timeCfg->year = (i + 2000);
		int* mothDays = &day1[0];
		if(timeCfg->year % 400 == 0 || (timeCfg->year % 4 == 0 && timeCfg->year% 100 != 0))
		{
			mothDays = &day2[0];
		}
		for(i=0; i< 12;i++)
		{
			if(_days >=mothDays[i])
			{
				_days -= mothDays[i];
			}
			else
			{
				break;
			}
		}
		timeCfg->month = i+1;
		timeCfg->date = _days;
	}
	return OK;
}

uint16_t GetTimeFromSeconds(uint32_t seconds, TimeCfg* timeCfg)
{
	uint32_t days = seconds/(24*3600);
	uint32_t sec = seconds%(24*3600);

	timeCfg->hour = sec/3600;
	timeCfg->minute = (sec/60)%60;
	timeCfg->second = sec%60;
	GetDateFromDays(days,timeCfg);

	return OK;
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

uint32_t GetST_SecLater(uint32_t timeSt, uint32_t seconds)
{
	TimeCfg timeCfg;
	ConvertBack_U32Time(timeSt, &timeCfg);
	uint32_t days = CalcDays(timeCfg.year, timeCfg.month, timeCfg.date);
	uint32_t secs = CalcSeconds(timeCfg.hour, timeCfg.minute, timeCfg.second);

	uint32_t secN =  GetSeconds(days,secs);
	seconds += secN;
	GetTimeFromSeconds(seconds, &timeCfg);
	return CalcTime_ST(&timeCfg);
}

char* GetSTStr(uint32_t timeSt)
{
	TimeCfg timeCfg;
	ConvertBack_U32Time(timeSt, &timeCfg);
	static char buff[20];
		//lint -e586
	snprintf((void*)buff, 20, "%04d-%02d-%02d %02d:%02d:%02d", timeCfg.year,\
			timeCfg.month,timeCfg.date,\
			timeCfg.hour,timeCfg.minute,timeCfg.second);
	return buff;

}
uint32_t tstSt = 0;
uint16_t TestCaseRTC()
{
#define SEC_TEST  3820
#define SEC_START (START_DATES*24*3600U)
	TimeCfg timeCfg;
	uint32_t days;
	uint32_t seconds;
	uint32_t secN;
	uint32_t sec;
	for(uint32_t st = 0; st < 2110000; st++)
	{
		sec = SEC_TEST*st + 31536000;

		if(tstSt == st)
		{
			sec+=1;
		}
		GetTimeFromSeconds(sec, &timeCfg);
		days = CalcDays(timeCfg.year, timeCfg.month, timeCfg.date);
		seconds = CalcSeconds(timeCfg.hour, timeCfg.minute, timeCfg.second);

		secN =  GetSeconds(days,seconds);
		if(sec != secN)
		{
			timeCfg.year = 2018;
			return FATAL_ERROR;
		}
	}

	return OK;
}
