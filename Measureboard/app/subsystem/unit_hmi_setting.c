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
#include "unit_sys_diagnosis.h"
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
uint8_t 	hmi_U8_Ram[16] = {0,0,0,};

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
		120,3,1440,0,0,
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
		1000.0f, 2000.0f, 0.001f, 0, 0,
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
		//0
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
	//5
	CONSTRUCT_ARRAY_SIMPLE_U8(
			&hmi_U8_Ram[0],
			sizeof(hmi_U8_Ram)/sizeof(uint8_t),
			RAM),

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


uint16_t AlignCfgtoHMI(void)
{
	uint16_t ret = OK;
	uint8_t val_8 = 0;
	uint16_t val_16 = 0;
	uint32_t val_32 = 0;

	HMI_Get(0, 0, &val_8);   // synchronize measurement interval
	switch (val_8)
	{
	case (0):
		val_16 = 0;
		SCH_Put(24, 0, &val_16);   //schedule off, put 7 24 0 0
		break;
	case (1):
		val_16 = 1;
		SCH_Put(24, 0, &val_16);   //schedule on continuous mode, put 7 24 0 1
		SCH_Put(26, 0, &val_16);   //put 7 26 0 1
		break;

	case (2):
		val_16 = 1;
		SCH_Put(24, 0, &val_16);   //schedule on customize mode, put 7 24 0 1
		HMI_Get(2, 0, &val_32);   //get customize interval in HMI system
		val_16 = (uint16_t)val_32;
		SCH_Put(26, 0, &val_16); //put 7 26 0 x, put customize interval in schedule
		break;

	case (3):
		val_16 = 1;
		SCH_Put(24, 0, &val_16);   //schedule on, 30min, put 7 24 0 1
		val_16 = 30;
		SCH_Put(26, 0, &val_16);   //put 7 26 0 30
		break;

	case (4):
		val_16 = 1;
		SCH_Put(24, 0, &val_16);   //schedule on, 1hr, put 7 24 0 1
		val_16 = 60;
		SCH_Put(26, 0, &val_16);   //put 7 26 0 60
		break;

	case (5):
		val_16 = 1;
		SCH_Put(24, 0, &val_16);   //schedule on, 2hr, put 7 24 0 1
		val_16 = 120;
		SCH_Put(26, 0, &val_16);   //put 7 26 0 120
		break;

	case (6):
		val_16 = 1;
		SCH_Put(24, 0, &val_16);   //schedule on, 2hr, put 7 24 0 1
		val_16 = 240;
		SCH_Put(26, 0, &val_16);   //put 7 26 0 240
		break;
	}

	HMI_Get(0, 1, &val_8);   // synchronize measurement mode

	switch (val_8)
	{
	case (0):   //standard mode
		val_16 = 0;
		SCH_Put(30, 0, &val_16);   //disable average mode, put 7 30 0 0
		break;

	case (1):   //multiple mode
		val_16 = 1;
		SCH_Put(30, 0, &val_16);   //enable average mode, put 7 30 0 1
		val_16 = 0;
		SCH_Put(23, 0, &val_16);   //disable retest mode, put 7 23 0 0
		break;

	}

	HMI_Get(0, 2, &val_8);   // synchronize calibration interval
	switch (val_8)
	{
	case (0):
		val_16 = 0;
		SCH_Put(36, 0, &val_16);   //schedule off, put 7 36 0 0
		break;

	case (1):
		val_16 = 1;
		SCH_Put(36, 0, &val_16);   //schedule on customize mode, put 7 36 0 1
		HMI_Get(2, 1, &val_32);   //get customize interval in HMI system
		val_16 = (uint16_t)(24 * val_32);
		SCH_Put(37, 0, &val_16); //put 7 37 0 x, put customize interval in schedule
		break;

	case (2):
		val_16 = 1;
		SCH_Put(36, 0, &val_16);   //schedule on, 24hr(1day), put 7 36 0 1
		val_16 = 24;
		SCH_Put(37, 0, &val_16);   //put 7 37 0 24
		break;

	case (3):
		val_16 = 1;
		SCH_Put(36, 0, &val_16);   //schedule on, 72hr(3days), put 7 36 0 1
		val_16 = 72;
		SCH_Put(37, 0, &val_16);   //put 7 37 0 72
		break;

	case (4):
		val_16 = 1;
		SCH_Put(36, 0, &val_16);   //schedule on, 120hr(5days), put 7 36 0 1
		val_16 = 120;
		SCH_Put(37, 0, &val_16);   //put 7 37 0 120
		break;

	case (5):
		val_16 = 1;
		SCH_Put(36, 0, &val_16);   //schedule on, 168hr(7days), put 7 36 0 1
		val_16 = 168;
		SCH_Put(37, 0, &val_16);   //put 7 37 0 168
		break;
	}

	HMI_Get(0, 3, &val_8);   // synchronize cleaning interval

	switch (val_8)
	{
	case (0):
		val_16 = 0;
		SCH_Put(65, 0, &val_16);   //schedule off, put 7 65 0 0
		break;

	case (1):
		val_16 = 1;
		SCH_Put(65, 0, &val_16);   //schedule on customize mode, put 7 65 0 1
		HMI_Get(2, 2, &val_32);   //get customize interval in HMI system
		val_16 = (uint16_t)(val_32*60);
		SCH_Put(66, 0, &val_16); //put 7 66 0 x, put customize interval in schedule
		break;

	case (2):
		val_16 = 1;
		SCH_Put(65, 0, &val_16);   //schedule on, 6hr, put 7 65 0 1
		val_16 = 360;
		SCH_Put(66, 0, &val_16);   //put 7 66 0 360
		break;

	case (3):
		val_16 = 1;
		SCH_Put(65, 0, &val_16);   //schedule on, 12hr, put 7 65 0 1
		val_16 = 720;
		SCH_Put(66, 0, &val_16);   //put 7 66 0 720
		break;

	case (4):
		val_16 = 1;
		SCH_Put(65, 0, &val_16);   //schedule on, 24hr(1day), put 7 65 0 1
		val_16 = 1440;
		SCH_Put(66, 0, &val_16);   //put 7 66 0 1440
		break;

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

	AlignCfgtoHMI();
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
	case OBJ_IDX_U8_RAM:
		if(attributeIndex == ATR_ID_RTC)
		{
			Dia_UpdateRTCAlarm();
		}
		break;

		case OBJ_IDX_FACTORYSET:
			NewEventLog(EV_LOARD_FACT_SET, NULL);
			break;
	}
	return result;
}
