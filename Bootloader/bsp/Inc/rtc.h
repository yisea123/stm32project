#ifndef DEV_RTC__H
#define DEV_RTC__H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif



uint16_t RTC_GetCalibration(uint16_t* val);
uint16_t RTC_SetCalibration(uint16_t val);

/*
    stucture tm is defined in standard header file time.h
    struct tm
    {
      int	tm_sec;
      int	tm_min;
      int	tm_hour;
      int	tm_mday;
      int	tm_mon;
      int	tm_year;
      int	tm_wday;
      int	tm_yday;
      int	tm_isdst;
    };

    time_t is also defined in standard header file, it is smaller than tm.
    so please use time_t format to store time in order to save storage space.
*/
uint16_t Dev_Rtc_Get(struct tm* o_pNewSetting);
uint16_t Dev_Rtc_Get_CurrentTime(time_t* ptrTime);
uint16_t Dev_Rtc_Set(const struct tm* pNewSetting);

uint32_t prvReadBackupRegister(uint32_t RTC_BKP_DR);
void prvWriteBackupRegister(uint32_t RTC_BKP_DR, const uint32_t Data);
uint16_t Init_RTC(void);
extern void xprintf_rtc(void);
extern void xprintf_tick(void);
uint8_t sprintf_rtc(uint8_t* buff, uint8_t len);
#ifdef __cplusplus
}
#endif

#endif

