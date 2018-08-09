/*
 * unit_cfg_ex.c
 *
 *  Created on: 2018年4月20日
 *      Author: pli
 */

/*
 * unit_hmi_setting.c
 *
 *  Created on: 2017年7月12日
 *      Author: pli
 */



#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "main.h"

#include "unit_meas_cfg.h"
#include "unit_flow_cfg.h"
#include "unit_sys_info.h"
#include "unit_rtc_cfg.h"
#include "t_data_obj_measdata.h"
#include "unit_statistics_data.h"
#include "dev_eep.h"
#include "dev_log_sp.h"
#include "tsk_sch.h"
#include "unit_sch_cfg.h"
#include "unit_cfg_ex.h"



#define FILE_ID		0x18071210

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;


static uint32_t fileID1								__attribute__ ((section (".configbuf_EX")));

float  measLimitRange[MEAS_RANGE_MAX][2]			__attribute__ ((section (".configbuf_EX")));
float  failedMeasureVal								__attribute__ ((section (".configbuf_EX")));
uint16_t  retryIimesMax								__attribute__ ((section (".configbuf_EX")));
uint16_t  failedMeasureFlag							__attribute__ ((section (".configbuf_EX")));
uint16_t  failedPostEnable							__attribute__ ((section (".configbuf_EX")));
static uint32_t fileID2								__attribute__ ((section (".configbuf_EX")));

static const float 	measLimitRange_Default[MEAS_RANGE_MAX][2] =
{
	{-1000.0f,15.0f},
	{-1000.0f,30.0f},
	{-1000.0f,100.0f},
	{-1000.0f,1000.0f},
};

static const float  failedMeasureVal_Default = 1555.0f;
uint16_t  retryIimesMax_Default = 3;
uint16_t failedMeasureFlag_Default = FLAG_RETRY_MAX;
static const uint32_t fileID_Default = FILE_ID;
static const uint16_t failedPostEnable_Default = 1;


static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measLimitRange, measLimitRange_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(failedMeasureVal, failedMeasureVal_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(retryIimesMax, retryIimesMax_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(failedMeasureFlag, failedMeasureFlag_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measLimitRange, measLimitRange_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(failedPostEnable, failedPostEnable_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
};

//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
								&measLimitRange,
								sizeof(measLimitRange)/sizeof(float),
								NON_VOLATILE),
	CONSTRUCT_SIMPLE_FLOAT(&failedMeasureVal,NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&failedMeasureFlag,NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&retryIimesMax,NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&failedPostEnable,NON_VOLATILE),

};



//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT ExCfg =
{
	(uint8_t*)"ExCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_ExCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_T_UNIT,
	Put_T_UNIT,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};



uint16_t Initialize_ExCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&ExCfg);
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
		if((fileID1 != fileID_Default) || (fileID2 != fileID_Default))
		{
			(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
			TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);

			result = WARNING;
		}
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}





