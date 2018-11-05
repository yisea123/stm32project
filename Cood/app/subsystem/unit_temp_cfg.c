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


#define FILE_STABLE_ID		0x17082210

static uint32_t fileID3						__attribute__ ((section (".configbuf_stable")));
RTS100 tempCalibration[3] 					__attribute__ ((section (".configbuf_stable")));
static uint32_t fileID4						__attribute__ ((section (".configbuf_stable")));


static uint32_t fileID1						__attribute__ ((section (".configbuf_temp")));
static PreHeatMode preHeatMode				__attribute__ ((section (".configbuf_temp")));
static PreHeat preHeat						__attribute__ ((section (".configbuf_temp")));
uint16_t	tempCtrlDebug[2]				__attribute__ ((section (".configbuf_temp")));
static uint32_t fileID2						__attribute__ ((section (".configbuf_temp")));
uint16_t pwmOutput = 0;

int16_t 	temperatureDisplay[4];

static uint16_t tempValue[TEMP_MAX_ATR];
static const uint16_t tempCtrlDebug_Default[2] = {0,1};
static const uint32_t fileID_Default = FILE_ID;
static const uint32_t fileID_Stable_Default = FILE_STABLE_ID;

static const  PreHeatMode preHeatMode_Default =
{
	0,
	3200,
	3600,
};
static const  PreHeat preHeat_Default =
{
	17500,
	17500,
};

static const RTS100 tempCalibration_Default[3] =
{
	{{10000u,14000u},{10964u,60586u},9116.2f,0.0806094f,0},
	{{10000u,15000u},{4466u,28009u},9051.522461f,0.212377f,0},
	{{10000u,14000u},{10964u,60586u},9116.2f,0.0806094f,0},
};

static const T_DATACLASS _ClassList_Stable[]=
{
	//lint -e545 -e651
		//todo to restore the default value to defaults;
	CONSTRUCTOR_DC_STATIC_CONSTDEF(tempCalibration,tempCalibration_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID3,fileID_Stable_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID4,fileID_Stable_Default),
};

static const  T_DATACLASS _ClassList[]=
{

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
	CONSTRUCT_ARRAY_SIMPLE_U16(&tempValue[0],TEMP_MAX_ATR, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&tempValue[0],TEMP_MAX_ATR, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&tempValue[0],TEMP_MAX_ATR, READONLY_RAM),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,


	CONSTRUCT_ARRAY_SIMPLE_U16(&temperatureDisplay[0],sizeof(temperatureDisplay)/sizeof(int16_t), READONLY_RAM),
	CONSTRUCT_STRUCT_RST100(&tempCalibration[0], NON_VOLATILE),
	CONSTRUCT_STRUCT_RST100(&tempCalibration[1], NON_VOLATILE),
	CONSTRUCT_STRUCT_RST100(&tempCalibration[2], NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&pwmOutput, READONLY_RAM),


	NULL_T_DATA_OBJ,
	CONSTRUCT_ARRAY_SIMPLE_U16(&preHeatMode, sizeof(preHeatMode)/sizeof(int16_t), NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&preHeat, sizeof(preHeat)/sizeof(int16_t),  NON_VOLATILE),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
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
			}

			if((fileID3 != fileID_Stable_Default) || (fileID4 != fileID_Stable_Default))
			{
				for(uint16_t i=0;i<sizeof(_ClassList_Stable)/sizeof(T_DATACLASS); i++)
				{
					_ClassList_Stable[i].LoadRomDefaults(&_ClassList_Stable[i]);
				}
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
		tempValue[AD_DATA] = GetADCValueIdx(objectIndex);
		tempValue[AD_VOLT] = GetVoltageIdx(objectIndex);
		tempValue[TEMPERATURE] = (uint16_t)(GetTempChn(objectIndex));
		tempValue[RESISTOR] = (uint16_t)(GetResistor(objectIndex));
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
	if(objectIndex >= OBJ_IDX_CALI_0 && objectIndex < ADC_MAX_CALI_IDX)
	{
		result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
		if(RESIST_0 == attributeIndex)
		{
			uint16_t val = GetADCValueIdx( (uint16_t)(objectIndex-OBJ_IDX_CALI_0));
			tempCalibration[objectIndex-OBJ_IDX_CALI_0].voltage[0] = val;
			tempCalibration[objectIndex-OBJ_IDX_CALI_0].caliState &= 0xF0;
			tempCalibration[objectIndex-OBJ_IDX_CALI_0].caliState |= 0x01;
			CalcGainOffset(&tempCalibration[objectIndex-OBJ_IDX_CALI_0]);
		}
		else if(RESIST_1 == attributeIndex)
		{
			uint16_t val = GetADCValueIdx( (uint16_t)(objectIndex-OBJ_IDX_CALI_0));
			tempCalibration[objectIndex-OBJ_IDX_CALI_0].voltage[1] = val;
			tempCalibration[objectIndex-OBJ_IDX_CALI_0].caliState &= 0x0F;
			tempCalibration[objectIndex-OBJ_IDX_CALI_0].caliState |= 0x10;
			CalcGainOffset(&tempCalibration[objectIndex-OBJ_IDX_CALI_0]);
		}
		else
		{}
	}
	else if(OBJ_IDX_TEMPCTL == objectIndex)
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

