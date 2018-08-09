/*
 * dev_rtc.c
 *
 *  Created on: 2016��8��18��
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

			RTC_DateStruct.WeekDay = 5;
			RTC_DateStruct.Month = 4;
			RTC_DateStruct.Date = 7;
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



uint8_t sprintf_rtc(uint8_t* buff, uint8_t len1)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	uint8_t len = 0;
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
