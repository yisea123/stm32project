/*
 * unit_sys_info.c
 *
 *  Created on: 2016��11��1��
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_const.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_sys_info.h"


#define FILE_ID		0x112435
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

#define MAJOR_VERSION			1
#define SUB_VERSION				0
#define MINOR_VERSION			0
#define CSV_VERSION				0x05


#define SW_GIT_VERSION			1704180000




static const uint8_t 	SW_Info[3] = {MAJOR_VERSION, SUB_VERSION, MINOR_VERSION};
static uint32_t 	SW_Version = SW_GIT_VERSION;
static const uint8_t 	HW_Info[3] = {1, 0, 0};
const uint8_t 		Type_Info[INFO_LEN] = "Measure Board";
const uint16_t 		DeviceID = 0x00000001;
const uint8_t 		DeviceType[INFO_LEN] = "AT_LCD";
const uint8_t 		CompileDate[INFO_LEN] = __DATE__;
const uint32_t   	VersionCode = MAJOR_VERSION*256*256*256+SUB_VERSION*256*256+MINOR_VERSION*256+CSV_VERSION;
const uint16_t   	burstLength = BURST_LEN_CFG;
uint16_t 			deviceStatus = 0x0;


uint8_t		printChnMap[MAX_TASK_ID];
uint8_t		printMsgMap[MAX_TASK_ID];
uint8_t 	dbgMsgMap[MAX_TASK_ID];
static const uint32_t deviceStatus_default = 0x000;

static const uint8_t		printChnMap_Default[MAX_TASK_ID] =
{
	MAP_TRACE,	//TSK_ID_AD_MONITOR=0,
	MAP_TRACE,	//	TSK_ID_EEP,
	MAP_TRACE,	//	TSK_ID_CAN_RX1,
	MAP_TRACE,	//	TSK_ID_CAN_TX1,
	MAP_TRACE,	//	TSK_ID_CAN_ERR,
	MAP_TRACE,	//	TSK_ID_FILE,
	MAP_TRACE,	//	TSK_ID_GUI,


	MAP_TRACE,	//	TSK_ID_SHELL_RX,
	MAP_TRACE,	//	TSK_ID_SHELL_TX,
	MAP_TRACE,	//	TSK_ID_GPIO,
	MAP_TRACE,	//	TSK_ID_TST,

};



static const  T_DATACLASS _ClassList[]=
{
		//lint -e545
		CONSTRUCTOR_DC_STATIC_CONSTDEF(deviceStatus,deviceStatus_default),
		//lint -e545
		CONSTRUCTOR_DC_STATIC_CONSTDEF(printChnMap,printChnMap_Default),
		//lint -e545
		CONSTRUCTOR_DC_STATIC_CONSTDEF(printMsgMap,printChnMap_Default),
		//lint -e545
		CONSTRUCTOR_DC_STATIC_CONSTDEF(dbgMsgMap,printChnMap_Default),

};

enum
{
	IDX_INFO = 0,
	IDX_SW_VERSION=1,
};


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
	CONSTRUCT_ARRAY_SIMPLE_U8(SW_Info, 3, ROM),
	CONSTRUCT_SIMPLE_U32(&SW_Version, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(HW_Info, 3, ROM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(Type_Info, INFO_LEN, ROM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(CompileDate,INFO_LEN, ROM),
	CONSTRUCT_SIMPLE_U32(&DeviceID, ROM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(DeviceType, INFO_LEN, ROM),
	CONSTRUCT_SIMPLE_U32(&VersionCode, ROM),
	CONSTRUCT_SIMPLE_U32(&deviceStatus, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(printChnMap, MAX_TASK_ID, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(printMsgMap, MAX_TASK_ID, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(dbgMsgMap, MAX_TASK_ID, RAM),

};





//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT sysInfo =
{
	"sys info",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_SysInfo, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_SysInfo,
	Put_T_UNIT,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT,
};

uint16_t Get_SysInfo(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &sysInfo);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	switch(objectIndex)
	{
		case IDX_SW_VERSION:
			SW_Version = (((BUILD_YEAR-2000)*100000000)+(BUILD_MONTH*1000000)+(BUILD_DAY*10000)+(BUILD_HOUR*100)+ BUILD_MIN);
			break;
		default:
			break;

	}
	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	return result;
}


uint16_t Initialize_SysInfo(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&sysInfo);
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
			(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
		}
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}




