/*
 * unit_meas_data.c
 *
 *  Created on: 2017骞�3鏈�8鏃�
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_flowstep.h"
#include "t_data_obj_measdata.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_act.h"
#include "unit_meas_cfg.h"

#include "unit_data_log.h"
#include "dev_log_sp.h"
#include "dev_spiflash.h"
#include "dev_logdata.h"
#include "dev_flash_fat.h"
#include "unit_meas_data.h"
#include "unit_sch_cfg.h"
#include "unit_sys_diagnosis.h"
#include "tsk_substep.h"
//! unit global attributes

#define FILE_ID			0x18020110
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

__IO uint16_t measDataSaturation	= NONE_SATURATION;

MeasDataSt 			measResultRealTime;
CaliDataSt	 		caliResultRealTime;
MeasDataRaw	 		measRawDataResult;
float 				absMeasConcentration[3];
float 				_absMeasFinal[3];

extern float 			absChnValue[MEA_DATA_MAX];
extern __IO uint16_t 	valveIO_Status;
extern __IO uint16_t 	motorPPS[IDX_MOTOR_MAX];
extern __IO uint16_t 	eepStatus[2];
extern __IO uint32_t 	osCPU_Usage;

__IO float    	realTimeConcentration;
__IO uint32_t 	resetFlag = 0xFFF;
__IO float 		blankRawAbsCali[BLANK_MAX]={0.0f,0.0f};

float 	blankRawAbs[BLANK_MAX]={0.0f,0.0f};
__IO 	BlankSetting blankSettingUsed;
float	lastMeasureForRetest = RETEST_DEFAULT_INIT + 10;



static uint16_t updateFlagUI[UPDATE_MAX_ID] = {UPDATE_ING,UPDATE_ING,UPDATE_ING,};
static uint16_t updateRequest[UPDATE_REQUEST_MAX_ID] = {UPDATE_ING,UPDATE_ING,UPDATE_ING,UPDATE_ING,UPDATE_ING};



static uint32_t fileID1									__attribute__ ((section (".configbuf_measdata")));
__IO BlankSetting 			blankSetting				__attribute__ ((section (".configbuf_measdata")));
static CaliDataSt	 		_caliResultRealTime			__attribute__ ((section (".configbuf_measdata")));
static MeasDataRaw	 		_measRawDataResult			__attribute__ ((section (".configbuf_measdata")));
static MeasDataSt 			_measResultRealTime			__attribute__ ((section (".configbuf_measdata")));
uint16_t					refreshTime					__attribute__ ((section (".configbuf_measdata")));
__IO uint16_t				blankEnable					__attribute__ ((section (".configbuf_measdata")));
static __IO float 			blankRawAbsMeas[BLANK_MAX]	__attribute__ ((section (".configbuf_measdata")));
static uint32_t fileID2									__attribute__ ((section (".configbuf_measdata")));

static const uint32_t 		fileID_Default	= FILE_ID;
static const uint16_t 		refreshTime_Default = 65;
static const MeasDataSt 	measResult_Default= {0};
static const float 			blankRawAbsMeas_Default[BLANK_MAX]={0.0f,0.0f};
static const BlankSetting 	blankSetting_Default = {10,8,8};
static const uint16_t 		blankEnable_Default = 1;
static const MeasDataRaw	measRawDataResult_Default = {0};
static const CaliDataSt 	caliResult_Default= {0};



static const  T_DATACLASS _ClassList[]=
{
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_measResultRealTime,measResult_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_caliResultRealTime,caliResult_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(_measRawDataResult,measRawDataResult_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(blankSetting,blankSetting_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(blankEnable,blankEnable_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(blankRawAbsMeas,blankRawAbsMeas_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(refreshTime,refreshTime_Default),

};

static const T_DATA_OBJ _ObjList[] =
{
//0
	CONSTRUCT_SIMPLE_U32(&resetFlag, READONLY_RAM),
	CONSTRUCT_STRUCT_MEASDATA(&_measResultRealTime,READONLY_NOV),
	CONSTRUCT_STRUCT_CALIDATA(&_caliResultRealTime,READONLY_NOV),
	CONSTRUCT_STRUCT_MEASDATARAW(&_measRawDataResult,READONLY_NOV),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&absMeasConcentration[0],sizeof(absMeasConcentration)/sizeof(float), READONLY_RAM),
//5
	CONSTRUCT_SIMPLE_BIT16(&valveIO_Status,READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&motorPPS[0], sizeof(motorPPS)/sizeof(uint16_t),READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&eepStatus[0], sizeof(eepStatus)/sizeof(uint16_t),READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&absChnValue[0],sizeof(absChnValue)/sizeof(float), READONLY_RAM),
	CONSTRUCT_SIMPLE_FLOAT(&realTimeConcentration, READONLY_RAM),

//10
	CONSTRUCT_SIMPLE_U32(&osCPU_Usage, READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(&measDataSaturation, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&blankSetting,sizeof(blankSetting)/sizeof(uint16_t),NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&blankRawAbsMeas[0],sizeof(blankRawAbsMeas)/sizeof(float),READONLY_RAM),
	CONSTRUCT_SIMPLE_FLOAT(&lastMeasureForRetest,READONLY_RAM),
//15
	CONSTRUCT_SIMPLE_U16(&blankEnable, NON_VOLATILE),
//	updateFlag;
	CONSTRUCT_ARRAY_SIMPLE_U16(&updateFlagUI[0],sizeof(updateFlagUI)/sizeof(uint16_t),RAM),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&blankRawAbsCali[0],sizeof(blankRawAbsCali)/sizeof(float),READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(&refreshTime, NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&updateRequest[0],sizeof(updateRequest)/sizeof(uint16_t),RAM),
//20
	CONSTRUCT_ARRAY_SIMPLE_U16(&hadc1080_Val[0],sizeof(hadc1080_Val)/sizeof(uint16_t),READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&_absMeasFinal[0],sizeof(_absMeasFinal)/sizeof(float), READONLY_RAM),



};


//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT measData =
{
	(uint8_t*)"measData",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_MeasData, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_MeasData,
	Put_T_UNIT,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};


static void UpdateResult2UI(uint32_t now, uint16_t type, uint16_t src)
{
	static uint16_t tempUpdateFlag[UPDATE_MAX_ID] = {UPDATE_ING,UPDATE_ING,UPDATE_ING,};
	static uint32_t tmpUpdateTimeSeconds[UPDATE_MAX_ID] = {0,0,0,};
	uint16_t idMax = UPDATE_MAX_ID;
	uint16_t idMin = 0;
	if(type < UPDATE_MAX_ID)
	{
		idMax = (uint16_t)(type + 1);
		idMin = type;
	//	updateFlag[idMin] = UPDATE_ING;
	}

	OS_Use_Unit(measData.ptrDataSemaphore);
	if(src == 0)
	{
		for(;idMin<idMax;idMin++ )
		{
			updateRequest[idMin] = UPDATE_ING;
			if(tempUpdateFlag[idMin] == UPDATE_FOR_READ )
			{
				updateRequest[idMin] = UPDATE_FIN;
				updateFlagUI[idMin] = UPDATE_FIN;
				tempUpdateFlag[idMin] = UPDATE_ING;
				tmpUpdateTimeSeconds[idMin] = GetCurrentSeconds();
			}
			else if(updateFlagUI[idMin] == UPDATE_FIN )
			{
				if((now - tmpUpdateTimeSeconds[idMin]) >= refreshTime)
				{
					updateFlagUI[idMin] = UPDATE_ING;
				}
			}

		}
	}
	else
	{
		if(updateFlagUI[idMin] == UPDATE_FIN)
		{
			//is replaced with new data
			TraceDBG(TSK_ID_MEASURE, "data is not read by UI, the old data is replaced with new data:old  time%d current:%d\n",tmpUpdateTimeSeconds[idMin],GetCurrentSeconds());
		}
		updateRequest[idMin] = UPDATE_ING;
		updateFlagUI[idMin] = UPDATE_ING;
		tempUpdateFlag[idMin] = UPDATE_FOR_READ;
		tmpUpdateTimeSeconds[idMin] = GetCurrentSeconds();
	}
	OS_Unuse_Unit(measData.ptrDataSemaphore);
}


void UpdateResult2UI_Pre(uint16_t type)
{
	uint16_t idMax = UPDATE_MAX_ID;
	uint16_t idMin = 0;
	if(type < UPDATE_MAX_ID)
	{
		idMax = (uint16_t)(type + 1);
		idMin = type;
	//	updateFlag[idMin] = UPDATE_ING;
	}
	OS_Use_Unit(measData.ptrDataSemaphore);
	for(;idMin<idMax;idMin++ )
	{


		if(idMin == UPDATE_MEASURE_SCH_ID )
		{
			memcpy((void*)&blankRawAbsMeas,(void*)&blankRawAbs,sizeof(blankRawAbsMeas));

			memcpy((void*)&_measResultRealTime,(void*)&measResultRealTime,sizeof(_measResultRealTime));
			Trigger_EEPSave((void*)&blankRawAbsMeas, sizeof(blankRawAbsMeas),SYNC_CYCLE);
			Trigger_EEPSave((void*)&_measResultRealTime, sizeof(_measResultRealTime),SYNC_IM);
		}
		else if(idMin == UPDATE_CALIBRATION_ID)
		{
			memcpy((void*)&_caliResultRealTime,(void*)&caliResultRealTime,sizeof(_caliResultRealTime));
			Trigger_EEPSave((void*)&_caliResultRealTime, sizeof(_caliResultRealTime),SYNC_CYCLE);
		}
		else if(idMin == UPDATE_MEASURE_RAW_ID)
		{
			memcpy((void*)&_measRawDataResult,(void*)&measRawDataResult,sizeof(_measRawDataResult));
			Trigger_EEPSave((void*)&_measRawDataResult, sizeof(_measRawDataResult),SYNC_CYCLE);
		}
		UpdateResult2UI(0,idMin,1);
	}
	OS_Unuse_Unit(measData.ptrDataSemaphore);

}

uint16_t Initialize_MeasData(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&measData);
	result = Initialize_T_UNIT(me,typeOfStartUp);
	if(result==OK)
	{
/*
 	 	 if( (typeOfStartUp & INIT_HARDWARE) != 0)
		{
		}

		if( (typeOfStartUp & INIT_TASKS) != 0)
		{
		}
		*/
		if((typeOfStartUp & INIT_DATA) != 0)
		{
			if((fileID1 != fileID_Default) || (fileID2 != fileID_Default))
			{
				(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
				TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);
				result = WARNING;
			}
		}
		resetFlag = RCC->CSR;
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get measData
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
uint16_t Get_MeasData(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &measData);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	if( objectIndex == OBJ_IDX_UPDATE_REQUEST )
	{
		uint16_t startIdx = 0;
		uint16_t endIdx = UPDATE_REQUEST_MAX_ID;

		if((uint16_t)attributeIndex < UPDATE_REQUEST_MAX_ID)
		{
			startIdx = (uint16_t)attributeIndex;
			endIdx = (uint16_t)(attributeIndex + 1);
		}

		uint32_t now = GetCurrentSeconds();
		for(uint16_t id = startIdx; id < endIdx; id++)
		{
			if(id < UPDATE_MAX_ID)
				UpdateResult2UI(now, (uint16_t)id, 0);
			else if(UPDATE_EVENT_ID == id)
			{
				updateRequest[id] = UPDATE_ING;
				if( OK == UpdateEvent2UI())
				{
					updateRequest[id] = UPDATE_FIN;
				}
			}
			else if(UPDATE_DIAG_ID == id)
			{
				updateRequest[id] = UPDATE_ING;
				if( OK == UpdateDiag2UI())
				{
					updateRequest[id] = UPDATE_FIN;
				}
			}
		}
	}


	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);


	if(result == OK && objectIndex == 0)
	{
		resetFlag = 0x0;
	}
	if(result == OK)
	{
		switch(objectIndex)
		{
			case OBJ_IDX_RSTFLAG:
				resetFlag = 0;
				break;
		}
	}
	return result;
}




#if 0

void GetCaliResult(CaliDataSt* data, uint32_t rangeSel)
{
	if(rangeSel < MEAS_RANGE_MAX)
	{
		(void)memcpy((void*)data, (void*)&caliResultRange[rangeSel], sizeof(CaliDataSt));
	}
}


void StoreCaliResult(const CaliDataSt* data, uint32_t rangeSel)
{
	if(data->caliStatus == OK)
	{
		(void)memcpy((void*)&caliResultRange[rangeSel], (void*)&data, sizeof(CaliDataSt));
		Trigger_EEPSave((void*)&caliResultRange[rangeSel], sizeof(CaliDataSt), SYNC_IM);
	}
}
#endif
