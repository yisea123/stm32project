/*
 * unit_meas_data.h
 *
 *  Created on: 2017年3月8日
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_MEAS_DATA_H_
#define SUBSYSTEM_UNIT_MEAS_DATA_H_


#define MAX_AVG_TIMES_MEAS		5
#define MAX_AVG_TIMES_CALI		5

#include "t_data_obj_measdata.h"

enum
{
	BLANK_NONE = 0,
	BLANK_END = 0x01,
	BLANK_DIABLE = 0x0F,
	BLANK_ENABLE1 = 0xF0,
	BLANK_ENABLE2 = 0xF5,
};
enum{
	UPDATE_ING = 0,
	UPDATE_FIN = 1,
	UPDATE_START = 2,
	UPDATE_FOR_READ = 3,
};

enum{
	UPDATE_MEASURE_RAW_ID,
	UPDATE_CALIBRATION_ID,
	UPDATE_MEASURE_SCH_ID,
	UPDATE_MAX_ID,
	UPDATE_EVENT_ID = UPDATE_MAX_ID,
	UPDATE_DIAG_ID,
	UPDATE_REQUEST_MAX_ID,
};

typedef struct
{
	uint16_t duringTime;
	uint16_t delayTimeStart;
	uint16_t delayTimeContinue;
}BlankSetting;
extern __IO uint16_t 	blankEnable;
extern float 			blankRawAbs[BLANK_MAX];
extern const T_UNIT 	measData;
extern __IO BlankSetting 	blankSettingUsed;
extern __IO BlankSetting 	blankSetting;
extern MeasDataSt 		measResultRealTime;
extern CaliDataSt 		caliResultRealTime;
extern MeasDataRaw	 	measRawDataResult;
extern int16_t 			temperatureDisplay[6];
extern __IO uint16_t 	measDataSaturation;
extern float 			absMeasConcentration[3];
#define LONG_SATURATION 		0x0F
#define SHORT_SATURATION 		0xF0
#define NONE_SATURATION			0x00
#define ABS_SATUATION_VALUE			10.00f

enum
{
	OBJ_IDX_RSTFLAG = 0,
	OBJ_IDX_UPDATEFLAG = 16,
	OBJ_IDX_UPDATE_REQUEST = 19,
};
void UpdateResult2UI_Pre(uint16_t type);
uint16_t Initialize_MeasData(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Get_MeasData(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Put_MeasData(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t GetNewRawMeasDataResult(MeasDataRaw* val);
uint16_t GetNewRawCaliDataResult(CaliDataRaw* val);
void GetCaliResult(CaliDataSt* data, uint32_t rangeSel);
void StoreCaliResult(const CaliDataSt* data, uint32_t rangeSel);

#endif /* SUBSYSTEM_UNIT_MEAS_DATA_H_ */
