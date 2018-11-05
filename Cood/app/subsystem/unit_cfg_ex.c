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



#define FILE_ID		0x18102408

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;


static uint32_t fileID1								__attribute__ ((section (".configbuf_EX")));

float  measLimitRange[MEAS_RANGE_MAX][2]			__attribute__ ((section (".configbuf_EX")));
float  failedMeasureVal								__attribute__ ((section (".configbuf_EX")));
uint16_t  retryIimesMax								__attribute__ ((section (".configbuf_EX")));
uint16_t  failedMeasureFlag							__attribute__ ((section (".configbuf_EX")));
uint16_t  failedPostEnable							__attribute__ ((section (".configbuf_EX")));
float     std1VeriDeviation							__attribute__ ((section (".configbuf_EX")));
uint16_t  std1VeriEnable							__attribute__ ((section (".configbuf_EX")));
uint16_t  measPostEnable							__attribute__ ((section (".configbuf_EX")));
uint16_t  measPostStep[	MEAS_RANGE_MAX]				__attribute__ ((section (".configbuf_EX")));
uint16_t  caliPostMeas 								__attribute__ ((section (".configbuf_EX")));
uint16_t  loadEPACfg 								__attribute__ ((section (".configbuf_EX")));
uint16_t	caliTimesMax							__attribute__ ((section (".configbuf_EX")));
static uint32_t fileID2								__attribute__ ((section (".configbuf_EX")));


static uint32_t	caliTime;

static Calibration_Sch caliFactor_Old;

static const float 	measLimitRange_Default[MEAS_RANGE_MAX][2] =
{
	{-1000.0f,15.0f},
	{-1000.0f,30.0f},
	{-1000.0f,100.0f},
	{-1000.0f,1000.0f},
};

static const float  failedMeasureVal_Default = 1555.0f;
static const uint16_t  retryIimesMax_Default = 3;
static const uint16_t failedMeasureFlag_Default = FLAG_RETRY_MAX;
static const uint32_t fileID_Default = FILE_ID;
static const uint16_t failedPostEnable_Default = 1;
static const float std1VeriDeviation_Default = 0.015f;
static const uint16_t std1VeriEnable_Default = 0;
static const uint16_t  measPostEnable_Default = 1;
static const uint16_t  caliPostMeas_Default = 0;
static const uint16_t  measPostStep_Default[MEAS_RANGE_MAX] = {0,0,0,0};
static const uint16_t  loadEPACfg_Default = 0;
static const uint16_t  caliTimesMax_Default = 4;
static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measLimitRange, measLimitRange_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(failedMeasureVal, failedMeasureVal_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(retryIimesMax, retryIimesMax_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(failedMeasureFlag, failedMeasureFlag_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measLimitRange, measLimitRange_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(failedPostEnable, failedPostEnable_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(std1VeriDeviation, std1VeriDeviation_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(std1VeriEnable, std1VeriEnable_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measPostStep, measPostStep_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measPostEnable, measPostEnable_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliPostMeas, caliPostMeas_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(loadEPACfg, loadEPACfg_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliTimesMax, caliTimesMax_Default),

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


	//5
	CONSTRUCT_SIMPLE_FLOAT(&std1VeriDeviation,NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&std1VeriEnable,NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&measPostEnable,NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&measPostStep, sizeof(measPostStep)/sizeof(uint16_t), NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&caliPostMeas,NON_VOLATILE),
	//10
	CONSTRUCT_SIMPLE_U16(&loadEPACfg,NON_VOLATILE),
	CONSTRUCT_STRUCT_CALIDATA(&caliFactor_Old,READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(&caliTimesMax,NON_VOLATILE),
	CONSTRUCT_SIMPLE_T32(&caliTime,READONLY_RAM),

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
	Get_T_UNIT,
	Put_ExCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};

uint16_t LoadOrStoreNewCaliFactor(Calibration_Sch* sch, uint16_t type)
{
	if(STORE_TMP_FACTOR == type)
	{
		caliTime = GetCurrentST();
		memcpy((void*)&caliFactor_Old, (void*)&sch, sizeof(caliFactor_Old));
	//	Trigger_EEPSave((void*)&caliFactor_Old, sizeof(caliFactor_Old));
	}
	else
	{
		memcpy((void*)&sch, (void*)&caliFactor_Old, sizeof(caliFactor_Old));
		Trigger_EEPSave((void*)&sch, sizeof(caliFactor_Old), SYNC_IM);
	}
	return OK;
}


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



//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put ex cfg
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
uint16_t Put_ExCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &ExCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);

	if(result == OK)
	{
		switch(objectIndex)
		{
		case OBJ_IDX_LOAD_EPA:
		//	LoadEPA_FlowCfg(loadEPACfg);
			if(loadEPACfg)
			{
				std1VeriEnable = 1;
				caliPostMeas = 1;
			}
			else
			{
				std1VeriEnable = 0;
				caliPostMeas = 0;
			}
			Trigger_EEPSave(&std1VeriEnable, sizeof(std1VeriEnable), SYNC_CYCLE);
			Trigger_EEPSave(&caliPostMeas, sizeof(caliPostMeas), SYNC_IM);
			break;
		default:
			break;
		}
	}

	return result;
}






