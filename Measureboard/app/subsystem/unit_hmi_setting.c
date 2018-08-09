/*
 * unit_hmi_setting.c
 *
 *  Created on: 2017Äê7ÔÂ12ÈÕ
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



#define FILE_ID		0x18042514

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;


static uint32_t fileID1								__attribute__ ((section (".configbuf_HMI")));

uint8_t 	hmi_U8[128] 							__attribute__ ((section (".configbuf_HMI")));
uint16_t 	hmi_U16[32]								__attribute__ ((section (".configbuf_HMI")));
uint32_t 	hmi_U32[16]								__attribute__ ((section (".configbuf_HMI")));
float 		hmi_Float[16]							__attribute__ ((section (".configbuf_HMI")));

static uint32_t fileID2								__attribute__ ((section (".configbuf_HMI")));




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
	Put_T_UNIT,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};



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


