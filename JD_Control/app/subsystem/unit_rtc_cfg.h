/*
 * unit_rtc_cfg.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_RTC_CFG_H_
#define SUBSYSTEM_UNIT_RTC_CFG_H_
#include "t_data_obj_time.h"
enum {
	IDX_RTC_0=0,
	IDX_RTC_SEC = IDX_RTC_0,
	IDX_RTC_MIN,
	IDX_RTC_HOUR,
	IDX_RTC_DAY,
	IDX_RTC_MON,
	IDX_RTC_YEAR,
	IDX_RTC_WDAY,
	IDX_RTC_CAL,
	IDX_RTC_MAX = IDX_RTC_CAL,
	IDX_RTC_START_TIME,
	IDX_RTC_REG,
	IDX_RTC_ST,

};
#ifdef __cplusplus
 extern "C" {
#endif



#define T32_START_YEAR		0u //2000

#define T32_YEAR			26
#define T32_MONTH			22
#define T32_DATE			17
#define T32_HOUR			12
#define T32_MINUTES			6
#define T32_SECONDS			0

#define T32_DEFAULT_TIME	0x420000//(1<<T32_MONTH | 1<<T32_DATE)

extern const T_UNIT rtcCfg;

uint16_t Initialize_RtcCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_RtcCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_RtcCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

uint32_t CalcDays(const uint32_t year0, const uint8_t mon0, const uint8_t day);
uint32_t CalcSeconds(const uint8_t hour, const uint8_t min, const uint8_t second);
uint32_t GetSeconds(const uint32_t days, const uint32_t seconds);
void ConvertBack_U32Time(const uint32_t time, TimeCfg* ptrTime);
uint32_t GetCurrentST(void);
uint32_t GetCurrentSeconds(void);
uint32_t GetSecondsFromST(const uint32_t stTime);
uint32_t CalcTime_ST(const TimeCfg* ptrTime);
uint16_t ValidChkT32(uint32_t* ptrT32, uint16_t typ);
#define RTC_Get(objId,attributeIndex,ptrValue)		Get_RtcCfg(&rtcCfg,objId,attributeIndex,ptrValue)
#define RTC_Put(objId,attributeIndex,ptrValue)		Put_RtcCfg(&rtcCfg,objId,attributeIndex,ptrValue)
#ifdef __cplusplus
 }
#endif

#endif /* SUBSYSTEM_UNIT_RTC_CFG_H_ */
