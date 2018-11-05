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
#include "t_data_obj_flowstep.h"
#include "t_data_obj_test.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_hmi_setting.h"
#include "dev_log_sp.h"
#include "unit_sch_cfg.h"


#define FILE_ID		0x18042514

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;


static uint32_t fileID1								__attribute__ ((section (".configbuf_HMI")));

uint8_t 	hmi_U8[128] 							__attribute__ ((section (".configbuf_HMI")));
uint16_t 	hmi_U16[32]								__attribute__ ((section (".configbuf_HMI")));
uint32_t 	hmi_U32[16]								__attribute__ ((section (".configbuf_HMI")));
float 		hmi_Float[16]							__attribute__ ((section (".configbuf_HMI")));

static uint32_t fileID2								__attribute__ ((section (".configbuf_HMI")));

uint16_t    hmi_loadFactoryDefault = 0;


static const uint32_t fileID_Default = FILE_ID;

static const uint8_t hmi_U8_Default[128] =
{
		//0
		5,0,3,0,0,
		//5
		0,0,0,0,0,
		//10
		0,0,0,0,0,
		//15
		0,0,0,0,0,
		//20
		0,0,0,0,0,
		//25
		0,0,0,0,0,
		//30
		0,0,0,0,0,
		//35
		0,0,0,0,1,
		//40
		0,0,0,0,0,
		//45
		0,0,0,0,0,
		//50
		0,0,0,0,0,
		//55
		0,0,0,0,0,
};
static const uint16_t hmi_U16_Default[32] =
{
		//0
		0,0,0,0,0,
		//5
		0,0,0,0,0,
		//10
		0,0,0,0,0,
		//15
		0,0,0,0,0,
};
static const uint32_t hmi_U32_Default[16] =
{
		//0
		120,72,1440,0,0,
		//5
		0,0,0,0,0,
		//10
		0,0,0,0,0,
		//15
		0,
};
static const float hmi_Float_Default[16] =
{
		//0
		970.0f,970.0f,1000.0f,1000.0f,1000.0f,
		//5
		1000.0f,2000.0f,0,0,0,
		//10
		0,0,0,0,0,
		//15
		0,
};

static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(hmi_U8,hmi_U8_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(hmi_U16,hmi_U16_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(hmi_U32,hmi_U32_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(hmi_Float,hmi_Float_Default),
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
	CONSTRUCT_ARRAY_SIMPLE_U8(
			&hmi_U8[0],
			sizeof(hmi_U8)/sizeof(uint8_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&hmi_U16[0],
			sizeof(hmi_U16)/sizeof(uint16_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U32(
			&hmi_U32[0],
			sizeof(hmi_U32)/sizeof(uint32_t),
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(
			&hmi_Float[0],
			sizeof(hmi_Float)/sizeof(float),
			NON_VOLATILE),

	CONSTRUCT_SIMPLE_U16(&hmi_loadFactoryDefault, RAM),
};



//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT HmiCfg =
{
	(uint8_t*)"HmiCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_HmiCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_T_UNIT,
	Put_HmiCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};


//IO_Get
//IO_Put
//SCH_Get
//SCH_Put
//HMI_Get
//HMI_Put

//uint8_t
//uint16_t
//float
//uint32_t


//{
	//sub,obj,atrid,m_subid,m_obj,m_atr,{0,120},{20,43},{},{}
	//{12,0,0},{7,26,0},{5,120},{4,30}
//}

uint16_t AlignCfgtoHMI(void)
{
	uint16_t ret = OK;
	uint8_t val_8 = 0;

	HMI_Get(0, 0, &val_8);

	if(val_8 == 5)
	{
		//7, 26,0 =>120
		uint16_t val = 120;
		SCH_Put(26, 0, &val);
		//同步另一个
		val = 203;
		SCH_Put(8, 5, &val);
	}

	HMI_Get(0, 1, &val_8);

	if(val_8 == 1)
	{
		//7, 27,0 =>12
		uint16_t val = 12;
		SCH_Put(26, 0, &val);
	}

	return ret;
}

uint16_t Initialize_HmiCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&HmiCfg);
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
 \brief	Perform the put _HmiCfg
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
uint16_t Put_HmiCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &HmiCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	switch(objectIndex)
	{
		case OBJ_IDX_FACTORYSET:
			NewEventLog(EV_LOARD_FACT_SET, NULL);
			break;
	}
	return result;
}
