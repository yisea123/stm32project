/*
 * unit_parameter.c
 *
 *  Created on: 2016��9��22��
 *      Author: paul
 */


#include <At_File.h>
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_const.h"
#include "t_dataclass.h"
#include "unit_parameter.h"
#include "unit_rtc_cfg.h"
#include "Parameter_idx.h"
#include "main.h"
#include "canprocess.h"
#include "dev_eep.h"

//-------------------------------------------------------------------------------------------------
//! unit global attributes
uint16_t parameterState;     // Subsystem state
OS_RSEMA parameterDataSemaphore;

//dynamic
T_PARAMETER_WELD_SET_PARAMETER parameterWeldSetParameter ;
T_PARAMETER_DYNAMIC parameterDynamic;


//nv parameter

T_PARAMETER_STATIC_RARE parameterStaticRare 			__attribute__ ((section (".configbuf_can")));
static uint32_t 		fileID[MAX_ID] 					__attribute__ ((section (".configbuf_can")));


//-------------------------------------------------------------------------------------------------

uint16_t systemReset = 0;

//-------------------------------------------------------------------------------------------------
//! definition of constant block
const T_PARAMETER_CONSTANT parameterConstant=
{
    {0,0,0,0,0,0,0,0,0,0,0,0},    // CONST_U8 getWireSize[12]
    {0,0,0,0,0,0,0,0,0,0,0,0},    // CONST_U8 getWireType[12]
    {0,0,0,0,0,0,0,0,0,0,0,0},    // CONST_U8 getProcessType[12]
    {0,0,0,0,0,0,0,0,0,0,0,0},    // CONST_U8 getGasType[12]
    {0,0,0,0,0,0,0,0,0,0,0,0}    // CONST_U8 getUnitCode[12]
};
//-------------------------------------------------------------------------------------------------


const uint32_t fileID_Default[MAX_ID] =
{0,0,0,
};




const T_PARAMETER_WELD_SET_PARAMETER parameterWeldSetParameterInitializer=
{
    65,    // SIMPLE_U16 setMode
    4500,    // SIMPLE_U16 setCurrent
    286,    // SIMPLE_U16 setWorkPoint
    651,    // SIMPLE_U16 setWeldSpeed
    0,    // SIMPLE_U16 setWeldMask
    100,    // SIMPLE_U16 strikeSpeed
    10,    // SIMPLE_U16 startTime
    1,    // SIMPLE_U16 strikeTime
    3,    // SIMPLE_U16 upslopeTime
    1,    // SIMPLE_U16 downSlopeTime
    3,    // SIMPLE_U16 craterTime
    1,    // SIMPLE_U16 startMoveDelay
    4,    // SIMPLE_U16 stopMoveDelay
    350,    // SIMPLE_U16 strikeTrim
    3000,    // SIMPLE_U16 strikeWorkpoint
    340,    // SIMPLE_U16 startTrim
    300,    // SIMPLE_U16 startWorkpoint
    280,    // SIMPLE_U16 craterTrim
    350,    // SIMPLE_U16 craterWorkpoint
    270,    // SIMPLE_U16 strikeInch
    200    // SIMPLE_U16 codeInchSpeed
};
//-------------------------------------------------------------------------------------------------


const T_PARAMETER_STATIC_RARE parameterStaticRareInitializer=
{
    {2,0,5,5},    // SIMPLE_U8 swVersion[4]
    {0,0,0,0},    // SIMPLE_U8 regData[4]
    {'L','P','Y','@','1','6','3','.','C','O','M',0},    // SIMPLE_U8 company[12]
    {'P','a','u','l',' ','L','i',0},    // SIMPLE_U8 owner[8]
    0,    // SIMPLE_U32 deviceID
    0,    // SIMPLE_U32 factoryTime
    0,    // SIMPLE_U8 deviceType
    {0,0,0,0x71,0x122,32770},    // SIMPLE_U32 adjustRTCFactor[6]
    3200,    // SIMPLE_U16 speedMax
    1300,//uint16_t currentMax;
    500,//uint16_t voltMax;
    50,//uint16_t energyMax;
};
//-------------------------------------------------------------------------------------------------



const T_PARAMETER_DYNAMIC parameterDynamicInitializer=
{
    0,    // SIMPLE_U16 usbWrite_retry
    0,    // SIMPLE_U8 usbWriteStatus
    0,    // SIMPLE_U8 copy_status
    0,    // SIMPLE_U16 copy_filenum
    0,    // SIMPLE_U16 totalNum_Copy
    0,    // SIMPLE_U32 resetFileID
    0,    // SIMPLE_U8 loadRomDefault
    0,    // SIMPLE_U8 initializeNV
    0,    // SIMPLE_U8 saveNV
    0,    // SIMPLE_U16 weldCurrent
    0,    // SIMPLE_U16 weldWorkPoint
    0,    // SIMPLE_U16 weldSpeed
    0,    // SIMPLE_U16 weldEnergy
    0,    // SIMPLE_U16 inchSpeed
    0,    // SIMPLE_U16 inchCurrent
    0,    // SIMPLE_U16 shortCount
    0,    // SIMPLE_U16 loseArcNum
    0,    // SIMPLE_U16 weldMode
    0,    // SIMPLE_U16 can_innum
    0,    // SIMPLE_U16 can_validatenum
    0,    // SIMPLE_U32 setDifferenceTime
    0,    // SIMPLE_U16 controlKey
    0,    // SIMPLE_U8 weldStatus
    0,    // SIMPLE_U32 copyFilePath
    0,    // SIMPLE_U8 InitIIC_Data
    0,    // SIMPLE_U16 IIC_Status
    {0,300}    // SIMPLE_U16 simulateSave[2]
};
//-------------------------------------------------------------------------------------------------


static const T_DATACLASS parameterDataClassList[]=
{
    CONSTRUCTOR_DC_DYNAMIC(parameterWeldSetParameter,parameterWeldSetParameterInitializer),
    //lint -e{708} union initialization is OK
    CONSTRUCTOR_DC_DYNAMIC(parameterDynamic,parameterDynamicInitializer),
    //lint -e{708} union initialization is OK
	CONSTRUCTOR_DC_STATIC_CONSTDEF(parameterStaticRare,parameterStaticRareInitializer),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID,fileID_Default),
 };




//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ parameterObjectList[] =
{

    //usbConnectedStatus
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.usbConnectedStatus,
        RAM),

    //sd_status
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.sd_status,
        RAM),

    //usbInit_retry
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.usbInit_retry,
        RAM),

    //usbWrite_retry
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.usbWrite_retry,
        RAM),

    //usbWriteStatus
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.usbWriteStatus,
        RAM),

    //copy_status
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.copy_status,
        RAM),

    //copy_filenum
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.copy_filenum,
        RAM),

    //totalNum_Copy
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.totalNum_Copy,
        RAM),

    //resetFileID
    CONSTRUCT_SIMPLE_U32(
        &parameterDynamic.resetFileID,
        RAM),

    //loadRomDefault
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.loadRomDefault,
        RAM),

    //saveNV
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.saveNV,
        RAM),

    //weldCurrent
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.weldCurrent,
        RAM),

    //weldWorkPoint
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.weldWorkPoint,
        RAM),

    //weldSpeed
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.weldSpeed,
        RAM),

    //weldEnergy
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.weldEnergy,
        RAM),

    //inchSpeed
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.inchSpeed,
        RAM),

    //inchCurrent
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.inchCurrent,
        RAM),

    //shortCount
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.shortCount,
        RAM),

    //loseArcNum
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.loseArcNum,
        RAM),

    //weldMode
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.weldMode,
        RAM),

    //can_innum
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.can_innum,
        RAM),

    //can_validatenum
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.can_validatenum,
        RAM),

    //setDifferenceTime
    CONSTRUCT_SIMPLE_U32(
        &parameterDynamic.setDifferenceTime,
        RAM),

    //swVersion
    CONSTRUCT_ARRAY_SIMPLE_U8(
        parameterStaticRare.swVersion,
        4,
        NON_VOLATILE),

    //regData
    CONSTRUCT_ARRAY_SIMPLE_U8(
        parameterStaticRare.regData,
        4,
        NON_VOLATILE),

    //company
    CONSTRUCT_ARRAY_SIMPLE_U8(
        parameterStaticRare.company,
        12,
        NON_VOLATILE),

    //owner
    CONSTRUCT_ARRAY_SIMPLE_U8(
        parameterStaticRare.owner,
        8,
        NON_VOLATILE),

    //deviceID
    CONSTRUCT_SIMPLE_U32(
        &parameterStaticRare.deviceID,
        NON_VOLATILE),

    //factoryTime
    CONSTRUCT_SIMPLE_U32(
        &parameterStaticRare.factoryTime,
        NON_VOLATILE),

    //deviceType
    CONSTRUCT_SIMPLE_U8(
        &parameterStaticRare.deviceType,
        NON_VOLATILE),

    //adjustRTCFactor
    CONSTRUCT_ARRAY_SIMPLE_U32(
        parameterStaticRare.adjustRTCFactor,
        6,
        NON_VOLATILE),

    //speedMax
    CONSTRUCT_SIMPLE_U16(
        &parameterStaticRare.speedMax,
        NON_VOLATILE),

    //setMode
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.setMode,
        RAM),

    //setCurrent
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.setCurrent,
        RAM),

    //setWorkPoint
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.setWorkPoint,
        RAM),

    //setWeldSpeed
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.setWeldSpeed,
        RAM),

    //setWeldMask
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.setWeldMask,
        RAM),

    //getWireSize
    CONSTRUCT_ARRAY_CONST_U8(
        parameterConstant.getWireSize,
        12),

    //getWireType
    CONSTRUCT_ARRAY_CONST_U8(
        parameterConstant.getWireType,
        12),

    //getProcessType
    CONSTRUCT_ARRAY_CONST_U8(
        parameterConstant.getProcessType,
        12),

    //getGasType
    CONSTRUCT_ARRAY_CONST_U8(
        parameterConstant.getGasType,
        12),

    //getUnitCode
    CONSTRUCT_ARRAY_CONST_U8(
        parameterConstant.getUnitCode,
        12),

    //strikeSpeed
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.strikeSpeed,
        RAM),

    //startTime
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.startTime,
        RAM),

    //strikeTime
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.strikeTime,
        RAM),

    //upslopeTime
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.upslopeTime,
        RAM),

    //downSlopeTime
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.downSlopeTime,
        RAM),

    //craterTime
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.craterTime,
        RAM),

    //startMoveDelay
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.startMoveDelay,
        RAM),

    //stopMoveDelay
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.stopMoveDelay,
        RAM),

    //strikeTrim
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.strikeTrim,
        RAM),

    //strikeWorkpoint
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.strikeWorkpoint,
        RAM),

    //startTrim
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.startTrim,
        RAM),

    //startWorkpoint
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.startWorkpoint,
        RAM),

    //craterTrim
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.craterTrim,
        RAM),

    //craterWorkpoint
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.craterWorkpoint,
        RAM),

    //strikeInch
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.strikeInch,
        RAM),

    //codeInchSpeed
    CONSTRUCT_SIMPLE_U16(
        &parameterWeldSetParameter.codeInchSpeed,
        RAM),

     //weldStatus
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.weldStatus,
        RAM),
    //InitIIC_Data
    CONSTRUCT_SIMPLE_U8(
        &parameterDynamic.InitIIC_Data,
        RAM),

    //IIC_Status
    CONSTRUCT_SIMPLE_U16(
        &parameterDynamic.IIC_Status,
        RAM),

    //simulateSave
    CONSTRUCT_ARRAY_SIMPLE_U16(
        parameterDynamic.simulateSave,
        2,
        RAM),
		 CONSTRUCT_SIMPLE_U16(
		        &systemReset,
		        RAM),
		CONSTRUCT_ARRAY_SIMPLE_U32(
				fileID,
			sizeof(fileID)/sizeof(uint32_t),
			NON_VOLATILE),
};

//-------------------------------------------------------------------------------------------------
//! define the PARAMETER-Class object
const T_UNIT parameter =
{
		"parameter",
	&parameterState,
	&parameterDataSemaphore,

	parameterObjectList,
	//lint -e{778}
	sizeof(parameterObjectList)/sizeof(parameterObjectList[0])-1,

	parameterDataClassList,
	sizeof(parameterDataClassList)/sizeof(parameterDataClassList[0]),

	Initialize_PARAMETER, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_PARAMETER,
	Put_PARAMETER,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT,
};



extern MODEINFO modeinfo;


static volatile uint32_t weldTime = 0;
static volatile uint32_t usbStartState = 0;
//-------------------------------------------------------------------------------------------------
/*!
 \brief  initialize the subsystem; (hardware, tasks, data, calculation)
 \author
 \date
 \param  typeOfStartUp
 <pre>
   INIT_HARDWARE   =0x80  initialize hardware used by the subsystem
   INIT_TASKS      =0x40  initialize tasks, semaphores, timer ....
   INIT_DATA       =0x20  initialize data and data structures
   INIT_CALCULATION=0x10  initialize calculations used by the subsystem
 </pre>
 \return error-code
 <pre>
   OK                      operation was successful
   ERROR_NV_STORAGE        attributes inside the nv-storage are corrupted
   ERROR_RAM_STORAGE       attributes inside the ram are corrupted
   ERROR_HW                hardware used by the subsystem doesn't work
 </pre>
 \test
 \n by:
 \n intention:
 \n result module test:
 \n result Lint Level 3:
 \bug
*/
//-------------------------------------------------------------------------------------------------
uint16_t Initialize_PARAMETER(const T_UNIT *me, uint8_t typeOfStartUp)
{
    uint16_t result = FATAL_ERROR;

    //lint -e{746}
    assert(me==&parameter);
    result = Initialize_T_UNIT(me,typeOfStartUp);

	result |= parameter.LoadRomDefaults(&parameter, (int16_t)PARAMETER_DC_IDX_DYNAMIC);
	result |= parameter.LoadRomDefaults(&parameter, (int16_t)PARAMETER_DC_IDX_weldSetParameter);

	if((eepStatus == OK) && (parameterStaticRare.swVersion[3] < parameterStaticRareInitializer.swVersion[3]))
	{
		ResetNVData();
	}
    return result;
}


static int32_t TickTimeDiff(const uint32_t oldTick)
{
	uint32_t newTick = HAL_GetTick();
	if(oldTick > newTick)
	{
		newTick = newTick + (0xFFFFFFFF-oldTick) + 1;
	}
	else
	{
		newTick = newTick - oldTick;
	}
	return (int32_t)(newTick);
}
//--------------------------------------------------------------------------------------------------
/*!
\brief  gets attributeIndex of the data-object objectIndex; only in "INITIALIZED"
\author
\date
\param  me				this pointer
\param  objectIndex    object index
\param  attributeIndex attribute index; attributeIndex<0 --> get whole object
\param  ptrValue  pointer for return-Value
\return error-code
<pre>
OK                      operation was successful
DECLINE                 DEBUG, operation isn't permitted in the actual unit-state
ILLEGAL_OBJ_IDX         DEBUG, unknown Object
ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
</pre>
\test
\n by: Heiko Kresse
\n
\n environment: C-SPY module-test is part of the framework, the module testing is part of the framework
\n
\n intention: 1. complete instruction-coverage
\n result module test: 2006-02-14 OK
\n result Lint Level 3: free // approved warnings and informations are suppressed
*/
//--------------------------------------------------------------------------------------------------
static uint16_t numbit = 0;
uint16_t Get_PARAMETER(const T_UNIT *me,
					  uint16_t objectIndex, int16_t attributeIndex, void * ptrValue)
{
	uint16_t result = OK;
	uint32_t timeLen = 0;

	// check for zeropointer
	VIP_ASSERT(me==&parameter);
	VIP_ASSERT(ptrValue);
	assert(me->ptrObjectList);

	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
		return (uint16_t)result;
	}
	else
	{}
	switch(objectIndex)
	{
		case PARA_IDX_IIC_Status:
			{
				uint8_t testRet = TEST_OK;
				if(eepStatus != OK)
					testRet = TEST_WARNING;
				parameterDynamic.IIC_Status =  testRet;
				result = Get_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
			}
			break;

		case PARA_IDX_can_innum:

			if(0 != parameterDynamic.can_innum)
			{
				*(uint16_t*) (ptrValue) = 1;
				parameterDynamic.can_innum = 0;
			}
			else
			{
				*(uint16_t*)(ptrValue) = 0;
			}


			break;
//		case PARA_IDX_can_validatenum:
//
//			if(0 != parameterDynamic.can_validatenum)
//			{
//				*(uint16_t* )(ptrValue) = 1;
//				parameterDynamic.can_validatenum = 0;
//			}
//			else
//			{
//				*(uint16_t*)( ptrValue) = 0;
//			}
//
//
//			break;
		case PARA_IDX_weldStatus:

			if( parameterDynamic.weldStatus == PARA_WELD_ING)
			{
				if(numbit == parameterDynamic.can_validatenum)
				{
					int32_t time_len = TickTimeDiff(weldTime);
					shell_AddRTC("time_len = %d\r\n",  time_len);

					if(time_len >=1500)
					{
						shell_AddRTC("Cleared!\r\n");
						parameterDynamic.weldStatus = PARA_WELD_IDLE;
						parameterDynamic.weldCurrent = 0;
						parameterDynamic.weldWorkPoint = 0;
					}
				}
				else
				{
					weldTime =  HAL_GetTick();
					numbit = parameterDynamic.can_validatenum;
				}
			}
			else
			{
				numbit = parameterDynamic.can_validatenum;
			}

			result = Get_T_UNIT(me, objectIndex, attributeIndex, ptrValue);
			break;
		case PARA_IDX_getWireSize:
			GetWireSize(modeinfo.wiresize,ptrValue);
			result = OK;
			break;
		case PARA_IDX_getWireType:

			GetWireType(modeinfo.wiretype,ptrValue);
			result = OK;
			break;

		case PARA_IDX_getProcessType:
			GetProcessType(modeinfo.processtype,ptrValue);
			result = OK;
			break;

		case PARA_IDX_getGasType:

			GetGasType(modeinfo.gastype,ptrValue);
			result = OK;
			break;

		case PARA_IDX_getUnitCode:

			//GetUnitCode(modeinfo.,ptrValue);
			result = OK;
			break;


		default:
			result = Get_T_UNIT(me, objectIndex, attributeIndex, ptrValue);
			break;
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
\brief	Put overload
\author
\date
\param  me = pointer to subsystem
\param  objectIndex = object index.
\param  attributeIndex = attribute index.
\param  ptrValue = pointer to object value.
\return T_UNIT error code.
\warning
\test
\n by:
\n environment: C-SPY, HART
\n
\n intention: 1. complete instruction-coverage 2. see if can work with HART
\n result module test:
\n result Lint Level 3: PASS
*/
//--------------------------------------------------------------------------------------------------
uint16_t Put_PARAMETER(const T_UNIT *me,
					  uint16_t objectIndex, int16_t attributeIndex, void * ptrValue)
{
	uint16_t result = (uint16_t)OK;
	VIP_ASSERT(me==&parameter);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = (uint16_t)ILLEGAL_OBJ_IDX;
	}
	else
	{
		switch(objectIndex)
		{
			case PARA_IDX_InitIIC_Data:
				if(*(uint8_t*)ptrValue == 0x50)
				{
					result = ResetNVData();
				}
				break;
			case PARA_IDX_sd_status:

				result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
				break;
			case PARA_IDX_usbConnectedStatus:
				result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
				if(parameterDynamic.usbConnectedStatus == USH_USR_FS_READY)
				{
				//	ff_mount_usb();
					uint32_t tickOut = HAL_GetTick();
					FileAction act;
					act.action = USB_FILE_TEST;
					SendFileMsg(&act);
					if(( parameterDynamic.weldStatus == PARA_WELD_ING) || (tickOut>=100000) || usbStartState )//in 100s
					{
						act.action = COPY_FILES_REQ;
						SendFileMsg(&act);
					}
					else
					{
						act.action = COPY_FILES_ALL;
						SendFileMsg(&act);
						usbStartState = 1;
					}

				}
				result = (uint16_t)OK;
				break;
			case PARA_IDX_usbInit_retry:
				parameterDynamic.usbInit_retry++;
				result = (uint16_t)OK;
				break;
			case PARA_IDX_setDifferenceTime:
			    result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
				if( result == OK )
				{
					result = Put_T_UNIT(&parameter,PARA_IDX_adjustRTCFactor,0x02,ptrValue);
					//result |= RTC_UpdateRegister((int32_t*) &parameterStaticCyclic.adjustRTCFactor[0] );
					result |= Trigger_EEPSave((uint8_t*)&parameterStaticRare, sizeof(parameterStaticRare),SYNC_CYCLE);
				}
				break;
			case PARA_IDX_resetFileID:
				result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
				SetStartCopyFileID(parameterDynamic.resetFileID);
				SetNewFileID(parameterDynamic.resetFileID);
				break;
			case PARA_IDX_loadRomDefault:
				result = ResetNVData();
				break;
			case PARA_IDX_saveNV:
				result = Trigger_EEPSave((uint8_t*)&parameterStaticRare, sizeof(parameterStaticRare), SYNC_CYCLE);
				break;
			case PARA_IDX_can_innum:
				parameterDynamic.can_innum++;
				result = (uint16_t)OK;
				break;
			case PARA_IDX_can_validatenum:
				parameterDynamic.can_validatenum++;
				parameterDynamic.weldStatus = PARA_WELD_ING;
				weldTime = HAL_GetTick();
				result = (uint16_t)OK;
				break;
		case PARA_IDX_shortCount:
				parameterDynamic.shortCount++;
				result = (uint16_t)OK;
				break;

		case PARA_IDX_loseArcNum:
				parameterDynamic.loseArcNum++;
				result = (uint16_t)OK;
				break;
		case IDX_SYSTEM_RST:
				result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
				extern uint16_t systemReset;
				if(systemReset == 1)
				{
					//save eep data;
					SigPush(eepTaskHandle, EEP_SIGNAL_SAVE);
					MsgPush ( SHELL_TX_ID, (uint32_t)0, 0);
					//trigger reset;
				}
				break;
		case PARA_IDX_simulateSave:
			result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
			if(result == OK)
			{
				if(parameterDynamic.simulateSave[0] == 0)
				{
					StopTim6();
				}
				else
				{
					StartTim6();
				}
			}
			break;
			default:
				result = Put_T_UNIT(&parameter,objectIndex,attributeIndex,ptrValue);
				break;
		}
	}

	return result;
}



void SetErrorFileCount(uint32_t num)
{
	parameter_Put(IDX_FILE_ID ,ERR_CNT, &num);
}

uint32_t GetStartCopyFileID(void)
{
	uint32_t copyfileid = fileID[COPY_ID];//prvReadBackupRegister(RTC_BKP_DR4);
	return copyfileid;
}
uint32_t GetEndCopyFileID(void)
{
	uint32_t copyfileid = fileID[NEW_ID];//prvReadBackupRegister(RTC_BKP_DR3);
	return copyfileid;
}
void SetStartCopyFileID(uint32_t id)
{
	parameter_Put(IDX_FILE_ID ,COPY_ID, &id);
}
uint32_t GetNewFileID(void)
{
	uint32_t createfileid = fileID[NEW_ID];//prvReadBackupRegister(RTC_BKP_DR3);
	return createfileid;
}
uint32_t UpdateNewFileID(void)
{
	uint32_t createfileid = fileID[NEW_ID] + 1;
	parameter_Put(IDX_FILE_ID ,NEW_ID, &createfileid);
	return createfileid;
}

void SetNewFileID(uint32_t id)
{
	parameter_Put(IDX_FILE_ID ,NEW_ID, &id);
}

