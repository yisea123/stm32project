/*
 * tsk_sch.h
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */

#ifndef INC_TSK_SCH_H_
#define INC_TSK_SCH_H_

#include "t_data_obj_measdata.h"
#include"t_data_obj_Time.h"

typedef enum
{
	IO_IDLE,
	RO_PRETREATMENT_SET,//1
	RO_PRETREATMENT_CLR,
	RO_ALARMLOW_SET,//3
	RO_ALARMHIGH_SET,
	RO_ALARMLOW_CLR,//5
	RO_ALARMHIGH_CLR,
	RO_EVENT_SET,//7
	RO_EVENT_CLR,
	RO_POLL_SET_TIMEOUT,//9
	RO_POLL_CLR_TIMEOUT,
	IO_POLL_CHECK_TIMEOUT,//11
	AO_OUTPUT,
	REFRESH_OUTPUT,//13
	IO_FINISH,
	IO_POLL_INIT,
}IO_State;


typedef enum
{
	SCH_MEAS_IDLE,
	SCH_MEAS_PRETREATMENT,//delay
	SCH_MEAS_SAMPLEFLOW_DELAY,
	SCH_MEAS_PRETREATMENT_DELAY,
	SCH_MEAS_MEASURE,
	SCH_MEAS_MEASURE_DELAY,
	//deep flush or standard implemented in the steps;
	//different measure range, call different steps
	SCH_MEAS_FINISH,//issue data to UI
	SCH_MEAS_BREAK,

}SCH_MEAS_STATE;




typedef enum
{
	SCH_CALI_IDLE,
	SCH_CALI_PRE,
	SCH_CALI_STD0_PRE,
	SCH_CALI_STD0_PRE_DELAY,
	SCH_CALI_STD0,
	SCH_CALI_STD0_DELAY,
	SCH_CALI_STD0_FINISH,//issue data to UI
	SCH_CALI_STD1_PRE,
	SCH_CALI_STD1_PRE_DELAY,
	SCH_CALI_STD1,
	SCH_CALI_STD1_DELAY,
	SCH_CALI_RETRY,
	SCH_CALI_FINISH,//issue data to UI
	SCH_CALI_BREAK,
}SCH_CALI_STATE;


typedef enum
{
	SCH_IDLE,
	SCH_MEASURE, //1
	SCH_MEASURE_DELAY,
	//deep flush or standard implemented in the steps;
	//different measure range, call different steps

	SCH_CALIBRATION,//3
	SCH_CALIBRATION_DELAY,
	//deep flush or standard implemented in the steps;
	//different measure range, call different steps


	SCH_CLEANING, //5
	SCH_CLEANING_DELAY,

	SCH_DRAIN,//7
	SCH_DRAIN_DELAY,

	SCH_FLUSH,//9
	SCH_FLUSH_DELAY,
	SCH_PRIME,//11
	SCH_PRIME_DELAY,

	SCH_FLOW,//13

	SCH_FLOW_DELAY, //14
	SCH_STOP,
	//
	SCH_BREAK,
	SCH_RECOVERY,//17
	SCH_FINISH,//18
	SCH_FLOW_1,
	SCH_AR_MEASURE,
}SCH_STATE;





typedef enum
{
	SCH_AR_IDLE,
	SCH_AR_PRE,
	SCH_AR_MEAS,
	SCH_AR_MEAS_DELAY,
	SCH_AR_CALI_CHK,
	SCH_AR_CALI_DELAY,
	SCH_AR_MEAS_RETRY_CHK,//issue data to UI
	SCH_AR_FINISH,//issue data to UI
	SCH_AR_BREAK,
	SCH_AR_POST,
	SCH_AR_POST_DELAY,
}SCH_AUTO_RANGE_STATE;




//#define MSK_MEAS_OFFLINE 	0x10000000
#define MSK_POST_STD1	 	0x10000000
#define MSK_MEAS_ONLINE 	0x20000000
#define MSK_MEAS_STD0  		0x40000000
#define MSK_MEAS_STD1  		0x80000000
#define MSK_MEAS_SCH		0x08000000
#define MSK_MEAS_TRIG		0x04000000

#define MSK_CLEAN_TRIG		0x02000000
#define MSK_CLEAN_SCH		0x01000000
#define MSK_CALI_TRIG		0x00800000
#define MSK_CALI_SCH		0x00400000
#define MSK_CALI_AR			0x00200000
#define MSK_MEAS_CALI_TYPE	0xFFF00000
#define MSK_RANGE_SEL  		0x0000FFFF



#define SCH_TSK_CYCLE			60000u//1 minutes


extern float				measRangeAlarmLimit[2];



uint32_t GetCleanDuringTime_Ms(uint32_t rangeIdx);
uint32_t GetCaliDuringTime_Ms(uint32_t rangeIdx, uint16_t* ptrCaliNum);
uint32_t GetMeasDuringTime_Ms(uint32_t rangeIdx);
uint16_t ChkCalibrationSCH(void);
void IssueFinalResult(void);
float CalcConcentration(const MeasDataRaw* measResult, uint16_t rangeIdx, uint16_t type);

void StartAutoRangeTask(void const * argument);

void StartSchTask(void const * argument);
void StartSchMeasTask(void const * argument);
void StartCleanTask(void const * argument);
void StartCalibrationTask(void const * argument);
void StartIOTask(void const * argument);
void WakeUpSchedule(void);
uint16_t CheckValidCalibrationData(uint16_t rangeIdx);
void CheckMeasRange(uint16_t rangeidx, float measValue);
uint32_t GetSeconds_TimeCfg(TimeCfg* ptrStartTime);
void Fake_TrigCaliStorage(void);
void Fake_TrigMeasStorage(void);
void UpdateLedDiag(uint16_t ledLow, uint16_t val);
void UpdateTimeCostVal(void);
#endif /* INC_TSK_SCH_H_ */
