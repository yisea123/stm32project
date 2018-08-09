/*
 * unit_temp_cfg.c
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_temp_cfg.h"
#include "dev_temp.h"
#include "dev_eep.h"
#include "dev_log_sp.h"
#define FILE_ID		0x18020110
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;


#define FILE_STABLE_ID		0x18042514



static uint32_t fileID1						__attribute__ ((section (".configbuf_temp")));
PID_VAL sPID								__attribute__ ((section (".configbuf_temp")));
int16_t PIDOffset[MAX_TEMP_IDX] 			__attribute__ ((section (".configbuf_temp")));
static PreHeatMode preHeatMode				__attribute__ ((section (".configbuf_temp")));
static PreHeat preHeat						__attribute__ ((section (".configbuf_temp")));
uint16_t	tempCtrlDebug[2]				__attribute__ ((section (".configbuf_temp")));
static uint32_t fileID2						__attribute__ ((section (".configbuf_temp")));
uint16_t pwmOutput = 0;

int16_t 	temperatureDisplay[4];

static int32_t tempValue[TEMP_MAX_ATR];
static const uint16_t tempCtrlDebug_Default[2] = {0,1};
static const uint32_t fileID_Default = FILE_ID;

static const  PreHeatMode preHeatMode_Default =
{
	1,
	2300,
	3500,
};
static const  PreHeat preHeat_Default =
{
	4300,
	5000,
};

static const PID_VAL pid_Default =
{
	3.1f,	0.7f, 0.25f, 16,
};

static const int16_t PIDOffset_Default[MAX_TEMP_IDX] =
{
	100, 130, 160, 190, 210, 240, 270, 310, 380, 480,
	600, 630, 660, 690, 710, 740, 770, 810, 880, 980,
	1000, 1130, 1160, 1190, 1210, 1240, 1270, 1310, 1380, 1480,
	1600, 1630, 1660, 1690, 1710, 1740, 1770, 1810, 1880, 1980,
	2100, 2130, 2160, 2190, 2210, 2240, 2270, 2310, 2380, 2480,
};



static const  T_DATACLASS _ClassList[]=
{

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(sPID,pid_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(PIDOffset,PIDOffset_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(preHeat,preHeat_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(preHeatMode,preHeatMode_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(tempCtrlDebug,tempCtrlDebug_Default),

};



//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
	CONSTRUCT_ARRAY_SIMPLE_I32(&tempValue[0],TEMP_MAX_ATR, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_I32(&tempValue[0],TEMP_MAX_ATR, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_I32(&tempValue[0],TEMP_MAX_ATR, READONLY_RAM),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,


	CONSTRUCT_ARRAY_SIMPLE_I16(&temperatureDisplay[0],sizeof(temperatureDisplay)/sizeof(int16_t), READONLY_RAM),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	CONSTRUCT_SIMPLE_U16(&pwmOutput, READONLY_RAM),


	NULL_T_DATA_OBJ,
	CONSTRUCT_ARRAY_SIMPLE_U16(&preHeatMode, sizeof(preHeatMode)/sizeof(int16_t), NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&preHeat, sizeof(preHeat)/sizeof(int16_t),  NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(PIDOffset,MAX_TEMP_IDX ,NON_VOLATILE),
	CONSTRUCT_STRUCT_PID(&sPID, NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&tempCtrlDebug[0],2, NON_VOLATILE),
};



//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT tempCfg =
{
	(uint8_t*)"tempCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_TempCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_TempCfg,
	Put_TempCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};

int16_t GetPreHeatTemp(void)
{
	static int16_t oldTemp = 4300;
	int16_t temp = preHeat.tempPreHeat_Low;
	if(preHeatMode.preHeat_Mode != 0)//enabled
	{
		int32_t tempEnv = GetTempChn(ENV_CHN);

		temp = oldTemp;
		if(tempEnv >= preHeatMode.preHeat_High)
		{
			temp = preHeat.tempPreHeat_High;
		}
		else if(tempEnv <= preHeatMode.preHeat_Low)
		{
			temp = preHeat.tempPreHeat_Low;
		}
	}
	else
	{
		temp = preHeat.tempPreHeat_Low;
	}
	oldTemp = temp;
	return temp;
}


uint16_t Initialize_TempCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&tempCfg);
	result = Initialize_T_UNIT(me,typeOfStartUp);
	if(result==OK)
	{
		/*if( (typeOfStartUp & INIT_HARDWARE) != 0)
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
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get temp cfg
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
uint16_t Get_TempCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &tempCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	extern int16_t hadc1080_Val[2];
	if(objectIndex < ADC_MAX_CHN)
	{
		tempValue[AD_DATA] = (int32_t)GetADCValueIdx(objectIndex);
		tempValue[AD_VOLT] = (int32_t)GetVoltageIdx(objectIndex);
		tempValue[TEMPERATURE] = (int32_t)(GetTempChn(objectIndex));
		tempValue[RESISTOR] = (int32_t)(GetResistor(objectIndex));
	}
	else if(ADC_IDX_ACT_TEMP == objectIndex)
	{
		temperatureDisplay[0] = GetTempChn(0);
		temperatureDisplay[1] = GetTempChn(1);
		temperatureDisplay[2] = GetTempChn(2);
		temperatureDisplay[3] = (int16_t)hadc1080_Val[0];
	}
	else
	{}
	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);

	return result;
}



//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put temp cfg
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
uint16_t Put_TempCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &tempCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	if(OBJ_IDX_TEMPCTL == objectIndex)
	{
		result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	//	PIDInit(tempCtrl.tempReq1,)
	}
	else
	{
		result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	}
	return result;
}

