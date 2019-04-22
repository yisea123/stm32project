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
#include "bsp.h"
#include "unit_sys_info.h"
#include "main.h"
#include "usart.h"
#include "can.h"
#include "shell_io.h"
#include "LB_Layer1_Uart.h"
#define FILE_ID		0x19041610u
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;

#define MAJOR_VERSION			0
#define SUB_VERSION				1
#define MINOR_VERSION			1
#define CSV_VERSION				0x87


#define SW_GIT_VERSION			19041610



typedef struct
{
	uint32_t size;
	uint32_t revert;
}CheckSumAdr;

typedef struct
{
	uint32_t initCheck;
	uint32_t chekSum;
}CheckSumST;


__attribute__ ((section(".chesumpart"),used))
const CheckSumAdr romCheckSumAdr =
{
	0x80000,
	(uint32_t)(~0x80000),
};

__attribute__ ((section(".chesumpart"),used))
const CheckSumST romCheckSum =
{
	0x809FFFE,
	0x2345,
};



__attribute__ ((section(".mainVersion"),used))
const SysInfoST _sysInfo =
{
		0x001,
		__DATE__,
		__TIME__,
		"JD_Ctrl",
		"Measure&Control",
		{MAJOR_VERSION, SUB_VERSION, MINOR_VERSION},
		{1, 0, 0},
		MAJOR_VERSION*256*256*256+SUB_VERSION*256*256+MINOR_VERSION*256+CSV_VERSION,
};




static uint32_t 	SW_Version = SW_GIT_VERSION;


const uint16_t   	burstLength = BURST_LEN_CFG;
uint16_t 			deviceStatus = 0x0;



static uint32_t 	fileId1						__attribute__ ((section (".configsys_info")));
static uint8_t 		SerialNumber[INFO_LEN] 		__attribute__ ((section (".configsys_info")));
static uint32_t		usedSWVersion				__attribute__ ((section (".configsys_info")));
static uint8_t		deviceName[INFO_LEN] 		__attribute__ ((section (".configsys_info")));
uint8_t		deviceName_Short[SHORT_LEN] 		__attribute__ ((section (".configsys_info")));
static uint32_t		registerId					__attribute__ ((section (".configsys_info")));
static uint32_t		registerRev					__attribute__ ((section (".configsys_info")));
static uint32_t 	fileId2						__attribute__ ((section (".configsys_info")));

uint16_t			instrumentType = STANDARD_VERSION;
static uint32_t 	bootVersion = 0;
#define MAX_BUFF_LEN	245
uint16_t systemMsgClr = 0;
uint16_t systemReset = 0;
uint16_t ldRomDefaults = 0;
uint8_t  burstBuff0[MAX_BUFF_LEN];
uint8_t  burstBuff1[MAX_BUFF_LEN];
uint16_t	autoReset = 1;
uint16_t FVT_Test = 0;


typedef struct
{
	  uint16_t Port;
	  uint16_t Pins;
	  uint16_t Value;
}CPU_GPIO;

static const uint32_t 	fileId1_Default = FILE_ID;
static const uint32_t 	fileId2_Default = ~FILE_ID;
static const uint8_t 	SerialNumber_Default[INFO_LEN] = "0000000000000000";
static const uint8_t 	deviceName_Default[INFO_LEN] = "0000000000000000";
static const uint32_t 	swDefaultVersion	= 0;
static const uint8_t 	deviceName_Short_Default[SHORT_LEN] = "COD_1";
static CPU_GPIO	cpuGpio;
static GPIO_TypeDef ioReg;
static GPIO_InitTypeDef ioCfg;
#define PORT_MAX_NUM 	11
#define PIN_MAX_NUM		16

static GPIO_TypeDef* GpioReg[9] =
{
		GPIOA,
		GPIOB,
		GPIOC,
		GPIOD,
		GPIOE,
		GPIOF,
		GPIOG,
		GPIOH,
		GPIOI,
};


const BurstCfg burstCfg[BURST_LEN_CFG] =
{
		{&burstBuff0[0], sizeof(burstBuff0)},
		{&burstBuff1[0], sizeof(burstBuff1)},
};

static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(SerialNumber,SerialNumber_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1,fileId1_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2,fileId2_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(usedSWVersion,swDefaultVersion),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(deviceName,deviceName_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(deviceName_Short, deviceName_Short_Default),



};

enum
{
	IDX_INFO = 0,
	IDX_SW_VERSION=1,
	IDX_FVT_TEST = 16,
	IDX_CPU_GPIO = 17,
	IDX_CPU_GPIO_CFG = 18,
	IDX_CPU_GPIO_REG = 19,
	OBJ_CLR_DBG_MSG = 25,
	OBJ_DEVICE_RESET = 26,
	IDX_LOAD_ROM_DF = 27,
};


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
		//0
	CONSTRUCT_ARRAY_SIMPLE_U8(_sysInfo.SW_Info, 3, ROM),
	CONSTRUCT_SIMPLE_U32(&SW_Version, READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(_sysInfo.HW_Info, 3, ROM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(_sysInfo.Type_Info, sizeof(_sysInfo.Type_Info), ROM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(_sysInfo.compileDate,sizeof(_sysInfo.compileDate), ROM),

	//5
	CONSTRUCT_ARRAY_SIMPLE_CHAR(SerialNumber,sizeof(SerialNumber), NON_VOLATILE),
	CONSTRUCT_SIMPLE_U32(&_sysInfo.DeviceID, ROM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(deviceName, sizeof(deviceName), NON_VOLATILE),
	CONSTRUCT_SIMPLE_U32(&_sysInfo.VersionCode, ROM),
	CONSTRUCT_SIMPLE_U32(&deviceStatus, RAM),

	//10
	CONSTRUCT_ARRAY_SIMPLE_U8(burstBuff0, sizeof(burstBuff0), READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U8(burstBuff1, sizeof(burstBuff1), READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(&autoReset, RAM),
	CONSTRUCT_SIMPLE_U16(&instrumentType, READONLY_RAM),
	CONSTRUCT_SIMPLE_U32(&bootVersion, READONLY_RAM),

	//15
	CONSTRUCT_ARRAY_SIMPLE_CHAR(_sysInfo.compileTime,sizeof(_sysInfo.compileTime), ROM),
	CONSTRUCT_SIMPLE_U16(&FVT_Test, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&cpuGpio,sizeof(cpuGpio)/sizeof(uint16_t), RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(&ioCfg,sizeof(ioCfg)/sizeof(uint32_t), RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(&ioReg,sizeof(ioReg)/sizeof(uint32_t), RAM),

	//20
	CONSTRUCT_SIMPLE_U32(&usedSWVersion, NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_CHAR(_sysInfo.DeviceType, sizeof(_sysInfo.DeviceType), ROM),
	CONSTRUCT_ARRAY_SIMPLE_U8(deviceName, sizeof(deviceName), NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(SerialNumber,sizeof(SerialNumber), NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(deviceName_Short,sizeof(deviceName_Short), NON_VOLATILE),

	//25
	CONSTRUCT_SIMPLE_U16(&systemMsgClr,	RAM),
	CONSTRUCT_SIMPLE_U16(&systemReset,	RAM),
	CONSTRUCT_SIMPLE_U16(&ldRomDefaults,	RAM),

};





//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT sysInfo =
{
	(uint8_t*)"sysInfo",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_SysInfo, // will be overloaded
	LoadRomDefaults_T_UNIT,
	Get_SysInfo,
	Put_SysInfo,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
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
			break;
		case IDX_CPU_GPIO:
			if(attributeIndex == 2)
			{
				if( (cpuGpio.Port < PORT_MAX_NUM) && (cpuGpio.Pins < PIN_MAX_NUM) )
				{
					cpuGpio.Value = (uint16_t)HAL_GPIO_ReadPin( GpioReg [cpuGpio.Port], (uint16_t)(1<<cpuGpio.Pins));
				}
				else
				{
					result = RULE_VIOLATION_ERR;
				}
			}
			break;
		case IDX_CPU_GPIO_REG:
			if(cpuGpio.Port < PORT_MAX_NUM)
			{
				memcpy((void*)&ioReg, (void*)& GpioReg [cpuGpio.Port], sizeof(ioReg) );
			}
			else
				result = RULE_VIOLATION_ERR;
			break;
		default:
			break;

	}
	if(result == OK)
		result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	return result;
}


static uint16_t Can1_TxRaw(const uint8_t* data, uint16_t len)
{
	assert(data);
	for(uint16_t idx = 0; idx < len; )
	{
		CAN_TxHeaderTypeDef head1;
		head1.DLC = (uint16_t)(len-idx)%8;
		head1.ExtId = 0x1FFFFFFFU;
		head1.IDE = CAN_ID_EXT;

		if(head1.DLC != 0)
		{
			HAL_CAN_AddTxMessage(&hcan1, &head1, (void*)data, 0);
		}
		idx= (uint16_t)(idx + 8);
	}
	return OK;
}
static const uint8_t* sendData[] =
{
		(uint8_t*)"Uart 1 MeasureBoart Test\n",
		(uint8_t*)"Uart 2 MeasureBoart Test\n",
		(uint8_t*)"Uart 3 MeasureBoart Test\n",
		(uint8_t*)"CAN 1 MeasureBoart Test\n",
		(uint8_t*)"CAN 2 MeasureBoart Test\n",
};

uint16_t CheckRegStatus()
{
	return OK;
}

uint16_t Put_SysInfo(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &sysInfo);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{

		uint16_t len = 0;
		 switch(objectIndex)
		 {
			 case IDX_FVT_TEST:
			 {
				 switch(FVT_Test)
				 {
				 case FVT_SEND_UART1:
					 len = (uint16_t)strlen((char*)sendData[0]);
					 Print_SendRaw(sendData[0],len);
					 break;

				 case FVT_SEND_UART2:
					 len = (uint16_t)strlen((char*)sendData[1]);
					 shell_Add_Rawdata((const char*)sendData[1],len);
					 break;

				 case FVT_SEND_UART3:
					 len = (uint16_t)strlen((char*)sendData[2]);
					// LB_Layer1_Uart_Tx(sendData[2],len);
					 break;

				 case FVT_SEND_CAN1:
					 len = (uint16_t)strlen((char*)sendData[3]);
					// Can1_TxRaw(&sendData[3][0],len);
					 break;

				 case FVT_SEND_CAN2:
					 len = (uint16_t)strlen((char*)sendData[4]);
					 //LB_Layer2_Can_TxRaw(sendData[4],len);
					 break;
				 default:
					 break;
				 }
			 }
			 break;

			 case IDX_CPU_GPIO:
				if(attributeIndex == 2)
				{
					if( (cpuGpio.Port < PORT_MAX_NUM) && (cpuGpio.Pins < PIN_MAX_NUM) )
					{
						if(cpuGpio.Value)
							HAL_GPIO_WritePin( GpioReg [cpuGpio.Port],(uint16_t)(1<<cpuGpio.Pins), GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin( GpioReg [cpuGpio.Port],(uint16_t)(1<<cpuGpio.Pins), GPIO_PIN_RESET);
					}
					else
					{
						result = RULE_VIOLATION_ERR;
					}
				}
				break;
			 case IDX_CPU_GPIO_CFG:
				 if(attributeIndex == 2)
				{
					if(cpuGpio.Port < PORT_MAX_NUM)
					{
						uint16_t ret1 = IS_GPIO_PIN(ioCfg.Pin);
						uint16_t ret2 = IS_GPIO_MODE(ioCfg.Mode);
						uint16_t ret3 = IS_GPIO_PULL(ioCfg.Pull);
						if(ret1 && ret2 && ret3)
							HAL_GPIO_Init( GpioReg [cpuGpio.Port], &ioCfg);
						else
							result = RULE_VIOLATION_ERR;
					}
					else
					{
						result = RULE_VIOLATION_ERR;
					}
				}
				 break;
			case IDX_CPU_GPIO_REG:
				if(cpuGpio.Port < PORT_MAX_NUM)
				{
					memcpy((void*)&ioReg, (void*)& GpioReg [cpuGpio.Port], sizeof(ioReg) );
				}
				else
				{
					result = RULE_VIOLATION_ERR;
				}
				break;
			case OBJ_CLR_DBG_MSG:
				if(systemMsgClr != 0)
				{
					validPrintMsg = 0;
					validPrintLen = 0;
				}
				break;
			case IDX_LOAD_ROM_DF:
				LoadDefaultCfg(ldRomDefaults);

				break;

			case OBJ_DEVICE_RESET:
				if(systemReset != 0)
				{
					//save eep data;
					Trigger_DeviceReset();

					//trigger reset by eep task

				}
				break;
		 }
	}
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
			if((fileId1 != fileId1_Default) || (fileId2 != fileId2_Default))
			{
				(void)me->LoadRomDefaults(me,ALL_DATACLASSES);
				TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);
				result = WARNING;
			}
		}
		SW_Version = (uint32_t)((((BUILD_YEAR-2000)*100000000)+(BUILD_MONTH*1000000)+(BUILD_DAY*10000)+(BUILD_HOUR*100)+ BUILD_MIN));
		if(usedSWVersion != SW_Version)
		{
			//set event
			usedSWVersion = SW_Version;
			Trigger_EEPSave((void*)&usedSWVersion, sizeof(usedSWVersion), SYNC_IM);
		}

		const SysInfoST* ptrBootInfo = (void*)(0x8020000-256);
		bootVersion = ptrBootInfo->VersionCode;
		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/

		{
	//		TraceDBG(TSK_ID_MCU_STATUS,"Instrument type is detected wrong,use STANDARD_VERSION as default!\n");
		}
	}
	return result;
}




