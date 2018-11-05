/*
 * unit_io_cfg.c
 *
 *  Created on: 2017骞�5鏈�22鏃�
 *      Author: pli
 */



#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_dataclass.h"
#include "t_data_obj_time.h"
#include "t_data_obj_measdata.h"
#include "main.h"
#include "unit_io_cfg.h"
#include "unit_sch_cfg.h"
#include "tsk_sch.h"
#include "dev_log_sp.h"
#include "co_nmt.h"


//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;




#define FILE_ID		0x18042314




static uint32_t fileID1								__attribute__ ((section (".configbuf_IO")));
uint8_t		IO_BoardEnable[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
uint8_t		IO_BD_TypeSetting[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
uint8_t		IO_FuncSelChn1[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
uint8_t		IO_FuncSelChn2[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
float		IO_AOSrcChn1_Low[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
float		IO_AOSrcChn2_Low[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
float		IO_AOSrcChn1_High[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
float		IO_AOSrcChn2_High[MAX_IO_BOARD_NUM]		__attribute__ ((section (".configbuf_IO")));
uint16_t	RO_PWM_Cfg[2]							__attribute__ ((section (".configbuf_IO")));
uint16_t 	IO_Poll_Time							__attribute__ ((section (".configbuf_IO")));
TimeCfg		RO_StartTime							__attribute__ ((section (".configbuf_IO")));
float		AO_TransferValue						__attribute__ ((section (".configbuf_IO")));
uint32_t	syncTime_us								__attribute__ ((section (".configbuf_IO")));
uint16_t	heartBeat								__attribute__ ((section (".configbuf_IO")));
uint16_t	IOConsumer[2]							__attribute__ ((section (".configbuf_IO")));
static uint32_t fileID2								__attribute__ ((section (".configbuf_IO")));

uint32_t	timeCanIO_ST[ID_IOCAN_MAX];
uint32_t	timesErrCnt[ID_IOCAN_MAX];
float 		IORawValueFloat = 0;
uint32_t 		IORawValueU32 = 0;
typedef struct
{
	uint16_t adr;
	uint16_t obj;
	uint16_t atr;
	uint16_t type;

}IO_CON;


#define REV_IO	{0,0,0,0,\
	0,0,0,0,\
	0,0,0,0,\
	0,0,0,0,}


static IO_CON IOCmd;
static const uint8_t IO_FuncDefault[MAX_IO_BOARD_NUM] =
{
		1,1,1,1,
		1,1,1,1,
		1,1,1,1,
		1,1,1,1,
};
static const uint8_t IO_Default[MAX_IO_BOARD_NUM] = REV_IO;
static const float AO_Low[MAX_IO_BOARD_NUM] = REV_IO;
static const float AO_High[MAX_IO_BOARD_NUM] = {10,10,10,10,\
		10,10,10,10,\
		10,10,10,10,\
		10,10,10,10,};

static const uint16_t pwmCfg_Default[2] = {1000,500};
static const TimeCfg timeCfg_Default = {2017,1,1,12,0,0};
static uint16_t fakeInit = 0;
static const uint32_t syncTime_us_Default = 500000;
static const uint16_t IOConsumer_Default[2] = {2,2};
Calibration_AO AO_Calibration_Chn[2];

uint16_t    AO_Event_Trig = OK;

//read only to UI
uint8_t		IO_Connected[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t		IO_BoardType[MAX_IO_BOARD_NUM] = REV_IO;

uint16_t	IO_Status[MAX_IO_BOARD_NUM] = REV_IO;

uint16_t  	masterState = 0;
uint8_t 	slaveState[MAX_IO_BOARD_NUM] = {0,0,0};
static uint8_t 	_slaveState[MAX_IO_BOARD_NUM] = {0,0,0};
uint8_t			DI_Chn1_Value[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t			DI_Chn2_Value[MAX_IO_BOARD_NUM] = REV_IO;

uint8_t			DO_Chn1_Value[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t			DO_Chn2_Value[MAX_IO_BOARD_NUM] = REV_IO;
uint16_t		AO_Chn1_Value[MAX_IO_BOARD_NUM] = REV_IO;
uint16_t		AO_Chn2_Value[MAX_IO_BOARD_NUM] = REV_IO;
//set simulation

uint8_t		IO_SimuCfgChn1[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t		IO_SimuCfgChn2[MAX_IO_BOARD_NUM] = REV_IO;
uint16_t	IO_AO_Chn1_Simu[MAX_IO_BOARD_NUM] = REV_IO;
uint16_t	IO_AO_Chn2_Simu[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t		IO_DI_Chn1_Simu[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t		IO_DI_Chn2_Simu[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t		IO_DO_Chn1_Simu[MAX_IO_BOARD_NUM] = REV_IO;
uint8_t		IO_DO_Chn2_Simu[MAX_IO_BOARD_NUM] = REV_IO;
uint16_t 	IO_BD_ClearError = 0;


uint32_t	TickCntCanOpen[2] ={0,0};
uint32_t	TickDelayCanOpen[2] ={0,0};

uint16_t    calibrateAO[2]	= {0,0};
static const float AO_Range[2][2] = {{0.0f,20.0f},{4.0f,20.0f}};

static uint16_t heartBeat_Default = 500;
static Measurements lastValidMeasure = { 0.0f, 0,0};
static Measurements currMeasure = { 0.0f, 0,0};

static Measurements simulateMeasureValue = { 0.0f, 0, 0};
static uint16_t simulateMeasureFunc = 0;


static const uint32_t fileID_Default = FILE_ID;
static const uint16_t IO_Poll_Time_Default = 500;

static Measurements lastValidMeasure_BK;
static Measurements currMeasure_BK;

static const float AO_TransferValue_Default = 1.0f;


static const  T_DATACLASS _ClassList[]=
{
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_BoardEnable,IO_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_BD_TypeSetting,IO_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_FuncSelChn1,IO_FuncDefault),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_FuncSelChn2,IO_FuncDefault),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_AOSrcChn1_Low,AO_Low),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_AOSrcChn2_Low,AO_Low),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_AOSrcChn1_High,AO_High),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_AOSrcChn2_High,AO_High),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IO_Poll_Time,IO_Poll_Time_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(RO_PWM_Cfg,pwmCfg_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(RO_StartTime,timeCfg_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(AO_TransferValue,AO_TransferValue_Default),

	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(syncTime_us,syncTime_us_Default),

	CONSTRUCTOR_DC_STATIC_CONSTDEF(heartBeat,heartBeat_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(IOConsumer,IOConsumer_Default),


};

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
		//0
		CONSTRUCT_ARRAY_SIMPLE_U16(
								&AO_Calibration_Chn[0],
								sizeof(AO_Calibration_Chn[0])/sizeof(uint16_t),
								RAM),
		CONSTRUCT_ARRAY_SIMPLE_U16(
								&AO_Calibration_Chn[1],
								sizeof(AO_Calibration_Chn[1])/sizeof(uint16_t),
								RAM),
		CONSTRUCT_SIMPLE_U16(
							&IO_BD_ClearError,
							RAM),
		CONSTRUCT_SIMPLE_U16(
							&simulateMeasureFunc,
							RAM),
		CONSTRUCT_SIMPLE_FLOAT(
							&simulateMeasureValue.measure,
							RAM),
		//5
		CONSTRUCT_SIMPLE_U16(
							&simulateMeasureValue.status,
							RAM),
		CONSTRUCT_SIMPLE_U16(
							&masterState,
							RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
									&_slaveState[0],
									sizeof(_slaveState),
									RAM),
		CONSTRUCT_SIMPLE_U16(
							&fakeInit,
							RAM),
		CONSTRUCT_SIMPLE_U32(
				&syncTime_us,
									NON_VOLATILE),
		//10
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_Connected[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_BoardType[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),

		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
//15
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_Status[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&DI_Chn1_Value[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&DI_Chn2_Value[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&DO_Chn1_Value[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&DO_Chn2_Value[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
//20
		CONSTRUCT_ARRAY_SIMPLE_U16(
								&AO_Chn1_Value[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U16(
								&AO_Chn2_Value[0],
								MAX_IO_BOARD_NUM,
								READONLY_RAM),

		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_SimuCfgChn1[0],
								MAX_IO_BOARD_NUM,
								RAM),

		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_SimuCfgChn2[0],
								MAX_IO_BOARD_NUM,
								RAM),

		CONSTRUCT_ARRAY_SIMPLE_U16(
								&IO_AO_Chn1_Simu[0],
								MAX_IO_BOARD_NUM,
								RAM),
//25
		CONSTRUCT_ARRAY_SIMPLE_U16(
								&IO_AO_Chn2_Simu[0],
								MAX_IO_BOARD_NUM,
								RAM),


		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_DI_Chn1_Simu[0],
								MAX_IO_BOARD_NUM,
								RAM),

		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_DI_Chn2_Simu[0],
								MAX_IO_BOARD_NUM,
								RAM),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_DO_Chn1_Simu[0],
								MAX_IO_BOARD_NUM,
								RAM),

		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_DO_Chn2_Simu[0],
								MAX_IO_BOARD_NUM,
								RAM),
//30
		CONSTRUCT_ARRAY_SIMPLE_U8(
									&IO_BoardEnable[0],
									MAX_IO_BOARD_NUM,
									NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U8(
									&IO_BD_TypeSetting[0],
									MAX_IO_BOARD_NUM,
									READONLY_NOV),



		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_FuncSelChn1[0],
								MAX_IO_BOARD_NUM,
								NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U8(
								&IO_FuncSelChn2[0],
								MAX_IO_BOARD_NUM,
								NON_VOLATILE),


		CONSTRUCT_ARRAY_SIMPLE_FLOAT(
									&IO_AOSrcChn1_Low[0],
									MAX_IO_BOARD_NUM,
									NON_VOLATILE),
//35
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(
									&IO_AOSrcChn2_Low[0],
									MAX_IO_BOARD_NUM,
									NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(
									&IO_AOSrcChn1_High[0],
									MAX_IO_BOARD_NUM,
									NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(
									&IO_AOSrcChn2_High[0],
									MAX_IO_BOARD_NUM,
									NON_VOLATILE),
		CONSTRUCT_STRUCT_TIME(&RO_StartTime,NON_VOLATILE),

		CONSTRUCT_ARRAY_SIMPLE_U16(
									&RO_PWM_Cfg[0],
									2,
									NON_VOLATILE),

//40
		CONSTRUCT_SIMPLE_FLOAT(
									&AO_TransferValue,
									NON_VOLATILE),
		NULL_T_DATA_OBJ,


		CONSTRUCT_SIMPLE_FLOAT(
								&lastValidMeasure.measure,
								READONLY_RAM),
		CONSTRUCT_SIMPLE_FLOAT(
								&currMeasure.measure,
								READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(
								&lastValidMeasure.status,
								READONLY_RAM),
//45
		CONSTRUCT_SIMPLE_U16(
								&currMeasure.status,
								READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(
							&IO_Poll_Time,
							NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U8(
									&doFuncValue[0],
									sizeof(doFuncValue),
									RAM),
		CONSTRUCT_SIMPLE_U16(
								&AO_Event_Trig,
								RAM),

		CONSTRUCT_ARRAY_SIMPLE_U32(
										&TickDelayCanOpen,
										sizeof(TickDelayCanOpen)/sizeof(uint32_t),
										RAM),
//50
		CONSTRUCT_SIMPLE_U16(
										&heartBeat,
										NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U32(
										&TickCntCanOpen,
										sizeof(TickCntCanOpen)/sizeof(uint32_t),
										RAM),
		CONSTRUCT_ARRAY_SIMPLE_T32(
										&timeCanIO_ST,
										sizeof(timeCanIO_ST)/sizeof(uint32_t),
										RAM),
		CONSTRUCT_ARRAY_SIMPLE_U32(
										&timesErrCnt,
										sizeof(timesErrCnt)/sizeof(uint32_t),
				 						RAM),

		CONSTRUCT_ARRAY_SIMPLE_U16(
							&IOConsumer,
							sizeof(IOConsumer)/sizeof(uint16_t),
							NON_VOLATILE),
//55
		CONSTRUCT_ARRAY_SIMPLE_U16(
							&IOCmd,
							sizeof(IOCmd)/sizeof(uint16_t),
							RAM),
		CONSTRUCT_SIMPLE_U32(
								&IORawValueU32,
								RAM),
		CONSTRUCT_SIMPLE_FLOAT(
								&IORawValueFloat,
								RAM),
		CONSTRUCT_SIMPLE_U16(
								&simulateMeasureValue.rangeIdx,
								RAM),
};


//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT IOCfg =
{
	(uint8_t*)"IOCfg",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_IOCfg, // will be overloaded
	LoadRomDefaults_T_UNIT,
	Get_IOCfg,
	Put_IOCfg,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};

uint16_t DiagIOState(uint16_t nodeId, uint8_t id, uint8_t nmtState)
{
	(void)nodeId;
	uint16_t ret = OK;
	if(IO_Connected[id] && IO_BoardEnable[id])
	{
		if(IO_BoardType[id] == IO_BD_TypeSetting[id])
		{
			if( (canOpenState >= STATE_OPERATION) && (nmtState != CO_NMT_STATE_OPERATIONAL) )
			{
				ret = FATAL_ERROR;
			}
		}
	}
	return ret;
}

void AlignFunc_IO(void)
{
	for(uint16_t i = 0; i< MAX_IO_BOARD_NUM; i++)
	{
		if(IO_Connected[i] != 0)
		{
			if(IO_BoardType[i] == DI_BOARD)
			{
				if(IO_FuncSelChn1[i] >= IN_MODE_MAX) 	IO_FuncSelChn1[i] = 1;
				if(IO_FuncSelChn2[i] >= IN_MODE_MAX) 	IO_FuncSelChn2[i] = 1;
			}
			else if(IO_BoardType[i] == DO_BOARD)
			{
				if(IO_FuncSelChn1[i] >= DO_FUNC_MAX)	IO_FuncSelChn1[i] = 1;
				if(IO_FuncSelChn2[i] >= DO_FUNC_MAX)	IO_FuncSelChn2[i] = 1;
			}
			else if(IO_BoardType[i] == AO_BOARD)
			{
				if(IO_FuncSelChn1[i] >= AO_MAX)			IO_FuncSelChn1[i] = 1;
				if(IO_FuncSelChn2[i] >= AO_MAX)			IO_FuncSelChn2[i] = 1;
			}
			else
			{}
		}
	}
	Trigger_EEPSave(&IO_FuncSelChn1[0], sizeof(IO_FuncSelChn1),SYNC_CYCLE);
	Trigger_EEPSave(&IO_FuncSelChn2[0], sizeof(IO_FuncSelChn2),SYNC_CYCLE);
}

uint16_t Initialize_IOCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&IOCfg);
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
	}
	AlignFunc_IO();

	return result;
}

static void ClearAllErrorSetting(void)
{
	for(uint16_t idx=0;idx<MAX_IO_BOARD_NUM;idx++)
	{
		if(IO_BoardEnable[idx] != 0)
		{
			if(IO_Connected[idx] != 0)
			{
				if(IO_BoardType[idx] != IO_BD_TypeSetting[idx])
				{
					IO_BoardEnable[idx] = 0;
					IO_BD_TypeSetting[idx] = IO_BoardType[idx];
				//	IO_FuncSelChn1[idx] = 1;
				//	IO_FuncSelChn2[idx] = 1;
				}
				else
				{
					// this board is right
				}
			}
			else
			{
				//not connect
				IO_BoardEnable[idx] = 0;
				IO_BD_TypeSetting[idx] = NONE_IO;
			//	IO_FuncSelChn1[idx] = 1;
			//	IO_FuncSelChn2[idx] = 1;
			}
		}
	}
	Trigger_EEPSave((void*)&IO_BD_TypeSetting,sizeof(IO_BD_TypeSetting), SYNC_IM);
	Trigger_EEPSave((void*)&IO_BoardEnable,sizeof(IO_BoardEnable), SYNC_IM);
	AlignFunc_IO();

	ReInit_Canopen();

}





//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get IO
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
uint16_t Get_IOCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &IOCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);


	switch(objectIndex)
	{
		case OBJ_IDX_CALI_AO_CHN1:
		case OBJ_IDX_CALI_AO_CHN2:
		{
			if(attributeIndex == 4)
			{
				result = CaliAOStatus( (uint8_t)AO_Calibration_Chn[objectIndex].adr, (uint8_t)objectIndex, &AO_Calibration_Chn[objectIndex].caliStatus);
			}
		}
		break;
		case OBJ_IDX_IO_RAW_VALUE_U32:
			if( (IOCmd.type != ST_NIL) && (IOCmd.type < ST_FLOAT))
			{
				result= SdoRead((uint8_t)IOCmd.adr,0x2006u , 4, (uint8_t*)&IORawValueU32, 4);
			}
			else
			{
				result = NOT_ON_GRID_ERR;
			}
			break;

		case OBJ_IDX_IO_RAW_VALUE_FLOAT:
			if (IOCmd.type == ST_FLOAT)
			{
				result= SdoRead((uint8_t)IOCmd.adr,0x2006u , 5, (uint8_t*)&IORawValueFloat, 4);
			}
			else
			{
				result = NOT_ON_GRID_ERR;
			}
			break;
		case OBJ_IDX_SLAVE_STATE:
			memcpy((void*)&_slaveState[0], (void*)&slaveState[0], sizeof(_slaveState));
			break;
	}
	if(result == OK)
		result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	return result;
}

//__IO uint16_t setObj = 0;
//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put IO cfg
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
uint16_t Put_IOCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &IOCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);



	if(result == OK)
	{

		if((OBJ_IDX_SIMU_CFG_START <=objectIndex) &&
				(objectIndex <= OBJ_IDX_SIMU_CFG_END))
		{
			MsgPush(CAN_IO_ID, E_CAN_OPEN_SIMU_UPDATE, 0);
		//	setObj = objectIndex;
		//	osStatus st= MsgPush(CAN_IO_ID, E_CAN_OPEN_SIMU_UPDATE, 0);
		//	TraceDBG(0xFFFF, "call queue: %d, ret :0x%x\n",objectIndex,st);
		}



		switch(objectIndex)
		{
			case OBJ_IDX_CALI_AO_CHN1:
			case OBJ_IDX_CALI_AO_CHN2:
			{
				if(attributeIndex == 0)
				{
					if(AO_Calibration_Chn[objectIndex].adr > 16)
					{
						result = GREATERTHAN_RANGEMAX_ERR;
					}
					else
					{
						if(IO_BoardType[AO_Calibration_Chn[objectIndex].adr] != AO_BOARD)
						{
							result = FATAL_ERROR;
						}
					}
				}
				else if(attributeIndex == 1)//set calibration
				{
					uint16_t val = AO_Calibration_Chn[objectIndex].setValue;
					if((val == 4000) || (val == 20000) || (val == 0) )
					{
						result = CaliAOCurrent( (uint8_t)AO_Calibration_Chn[objectIndex].adr,(uint8_t)objectIndex, AO_Calibration_Chn[objectIndex].setValue);
					}
					else
					{
						result = NOT_ON_GRID_ERR;
					}
				}
				else if(attributeIndex == 2)//set calibration
				{
					result = CaliAOReadback( (uint8_t)AO_Calibration_Chn[objectIndex].adr,(uint8_t)objectIndex, AO_Calibration_Chn[objectIndex].readBack);
				}
				else if(attributeIndex == 3)//reset calibration
				{
					result = CaliAOReset( (uint8_t)AO_Calibration_Chn[objectIndex].adr,(uint8_t)objectIndex);
				}
				else
				{
					result = FATAL_ERROR;
				}
			}
			break;
			case OBJ_IDX_CLR_ERROR:
				if(IO_BD_ClearError != 0)
				{
					ClearAllErrorSetting();
					IO_BD_ClearError = 0;
				}
				break;
			case OBJ_IDX_SIMU_MEAS_FUNC:
				if(simulateMeasureFunc != 0)
				{
					UpdateAOSrc(simulateMeasureValue.measure, simulateMeasureValue.rangeIdx, simulateMeasureValue.status, 1);
				}
				else
				{
					currMeasure_BK = currMeasure;
					lastValidMeasure_BK = lastValidMeasure;
				}
				break;
			case OBJ_IDX_FAKE_INI:
				if(fakeInit != 0)
				{
					fakeInit = 0;
					FakeInit_Setting();
				}
				break;

			case OBJ_IDX_MASTER_STATE:
				if(masterState == CO_NMT_STATE_PREOP)
				{
					coNmtLocalStateReq(CO_NMT_STATE_PREOP);
				}
				else if(masterState == CO_NMT_STATE_OPERATIONAL)
				{
					coNmtLocalStateReq(CO_NMT_STATE_OPERATIONAL);
				}
				break;
			case OBJ_IDX_SLAVE_STATE:
				if(_slaveState[attributeIndex] == CO_NMT_STATE_PREOP)
				{
					UpdateIOState((uint8_t)attributeIndex, CO_NMT_STATE_PREOP);
				}
				else if(_slaveState[attributeIndex] == CO_NMT_STATE_OPERATIONAL)
				{
					UpdateIOState((uint8_t)attributeIndex, CO_NMT_STATE_OPERATIONAL);
				}
				break;
			case OBJ_IDX_SLAVE_ENABLE:
				if(IO_BoardType[attributeIndex] != 0)
				{
					if(IO_Connected[attributeIndex] != 0)
					{
						IO_BD_TypeSetting[attributeIndex] = IO_BoardType[attributeIndex];
						Trigger_EEPSave((void*)IO_BD_TypeSetting,sizeof(IO_BD_TypeSetting), SYNC_IM);
					}
				}
				MsgPush(CAN_IO_ID, (uint32_t )E_CAN_OPEN_SLAVE_UPDATE, 0);
			//	MsgPush(CAN_IO_ID, (uint32_t )E_CAN_OPEN_SLAVE_REFRESH, 100);
				break;
			case OBJ_IDX_SLAVE_TYPECFG:
			case OBJ_IDX_MASTER_SYNCTIME:
				MsgPush(CAN_IO_ID, (uint32_t )E_CAN_OPEN_SLAVE_EN, 0);


				break;
			case OBJ_IDX_HEARTBEAT:
			case OBJ_IDX_IO_CONSUMER:
				MsgPush(CAN_IO_ID, (uint32_t )E_CAN_OPEN_HEART_BEAT, 0);

				break;
			case OBJ_IDX_IO_CON:
				if(1 == attributeIndex)
				{
					//obj
					result= SdoWrite((uint8_t)IOCmd.adr,0x2006u , 1, (uint8_t*)&IOCmd.obj,2);
					if(result == OK)
						result= SdoRead((uint8_t)IOCmd.adr,0x2006u , 3, (uint8_t*)&IOCmd.type,2);
				}
				else if(2 == attributeIndex)
				{
					//atr
					result= SdoWrite((uint8_t)IOCmd.adr,0x2006u , 2, (uint8_t*)&IOCmd.obj,2);
					if(result == OK)
						result= SdoRead((uint8_t)IOCmd.adr,0x2006u , 3, (uint8_t*)&IOCmd.type,2);
				}
				break;
			case OBJ_IDX_IO_RAW_VALUE_U32:
				if( (IOCmd.type != ST_NIL) && (IOCmd.type < ST_FLOAT))
				{
					result= SdoWrite((uint8_t)IOCmd.adr,0x2006u , 4, (uint8_t*)&IORawValueU32, 4);
				}
				else
				{
					result = NOT_ON_GRID_ERR;
				}
				break;

			case OBJ_IDX_IO_RAW_VALUE_FLOAT:
				if (IOCmd.type == ST_FLOAT)
				{
					result= SdoWrite((uint8_t)IOCmd.adr,0x2006u , 5, (uint8_t*)&IORawValueFloat, 4);
				}
				else
				{
					result = NOT_ON_GRID_ERR;
				}
				break;

			default:
				break;
		}
		SendTskMsg(SCH_IO_ID,TSK_RENEW_STATUS,0,NULL, NULL);
	}

	return result;
}



static uint8_t GetDOValue(uint16_t boardIdx, uint16_t chn, uint8_t* _doValue)
{
	uint8_t ret = 0;
	uint16_t funcSel = IO_FuncSelChn1[boardIdx];
	if(chn != 0)
	{
		funcSel = IO_FuncSelChn2[boardIdx];
	}
	switch(funcSel)
	{
		case DO_ALARM_LOW:
		case DO_ALARM_HIGH:
		case DO_EVENT:
		case DO_PRETREAT:
		case DO_SWITCH:
			ret = _doValue[funcSel];
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}




void UpdateDOValue(uint8_t* _doValue)
{
	for(uint16_t idx=0;idx<MAX_IO_BOARD_NUM;idx++)
	{
		if(IO_BoardType[idx] == DO_BOARD)
		{
			doValue[idx*2] = GetDOValue(idx, 0, _doValue);
			doValue[idx*2+1] = GetDOValue(idx, 1, _doValue);
		}
		else
		{
			doValue[idx*2] = 0;
			doValue[idx*2+1] = 0;
		}
	}
}



static uint16_t CheckValidInput(uint8_t* ptrOldState, uint8_t chn, uint16_t idx)
{
	uint16_t ret = 0;
	uint8_t newState = DI_CLR;


	//
	if(chn == 0)
	{
		if(IO_SimuCfgChn1[idx] != 0)
			newState = IO_DI_Chn1_Simu[idx] ;
		else
			newState = diValue[idx*2];
	}
	else
	{
		if(IO_SimuCfgChn2[idx] != 0)
			newState = IO_DI_Chn2_Simu[idx] ;
		else
			newState = diValue[idx*2+1] ;
	}
	//falling edge trigger
	if( (*ptrOldState == DI_SET) && (newState == DI_CLR))
	{
		ret = 1;
	}

	*ptrOldState = newState;
	return ret;
}




void UpdateDIValue(void)
{
	uint16_t schRequest = 0;
	uint16_t idx = 0;
	static uint8_t	DI_State_Chn1[MAX_IO_BOARD_NUM] = {0,0,};
	static uint8_t	DI_State_Chn2[MAX_IO_BOARD_NUM] = {0,0,};

	static uint16_t schMode = 0xFFFF;

	uint8_t  triggerState[IN_MODE_MAX] = {0,0,0};


	for(idx = 0; idx < MAX_IO_BOARD_NUM; idx++)
	{
		//valid input
		if((IO_BD_TypeSetting[idx] == DI_BOARD) && (IO_BoardEnable[idx] != DISABLE_MODE))
		{
		//	DI_Chn1_Value[idx] = diValue[idx*2];
		//	DI_Chn2_Value[idx] = diValue[idx*2+1];


			uint16_t result = CheckValidInput(&DI_State_Chn1[idx], 0,idx);
			if( (IO_FuncSelChn1[idx] < IN_MODE_DISABLE_SCH) && (IO_FuncSelChn1[idx] > IN_MODE_NONE) )
			{
				if(result != 0)
					triggerState[ IO_FuncSelChn1[idx] ] = 1;
			}
			else if (IO_FuncSelChn1[idx] == IN_MODE_DISABLE_SCH)
			{
				schMode = DI_State_Chn1[idx];
			}
			else
			{}

			result = CheckValidInput(&DI_State_Chn2[idx], 1,idx);
			if( (IO_FuncSelChn2[idx] < IN_MODE_DISABLE_SCH) && (IO_FuncSelChn2[idx] > IN_MODE_NONE) )
			{
				if(result != 0)
					triggerState[ IO_FuncSelChn2[idx] ] = 1;
			}
			else if (IO_FuncSelChn2[idx] == IN_MODE_DISABLE_SCH)
			{
				schMode = DI_State_Chn2[idx];
			}
			else
			{}
		}
		else
		{
			DI_State_Chn1[idx] = DI_CLR;
			DI_State_Chn2[idx] = DI_CLR;
		}

	}
	TrigAction locTrigger;

	for(idx =0; idx < IN_MODE_MAX; idx++)
	{
		if(triggerState[idx] != IN_MODE_NONE)
		{
			schRequest = 1;
			locTrigger.byte.triggerType = IO_ACTION;
			switch(idx)
			{
			case IN_MODE_CLEAN:
				locTrigger.byte.action = Trigger_Clean;
				locTrigger.byte.value0 = 0xFF;
				SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
				break;
			case IN_MODE_CALIBRATION:
				locTrigger.byte.action = Trigger_Calibration;
				locTrigger.byte.value0 = 0xFF;
				SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
				break;
			case IN_MODE_MEASURE:
				locTrigger.byte.action = Trigger_Measure;
				locTrigger.byte.value0 = 0xFF;
				SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
				break;
			case IN_MODE_DRAIN:
				locTrigger.byte.action = Trigger_Drain;
				locTrigger.byte.value0 = 0xFF;
				SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
				break;
			case IN_MODE_FLUSH:
				locTrigger.byte.action = Trigger_Flush;
				locTrigger.byte.value0 = 0xFF;
				SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
				break;
			case IN_MODE_PRIME:
				locTrigger.byte.action = Trigger_Prime;
				locTrigger.byte.value0 = 0xFF;
				SCH_Put(OBJ_SYSTEM_PUSH_NEW,WHOLE_OBJECT,(void*)&locTrigger);
				break;
			default:
				break;
			}

		}
	}
	if(schMode != 0xFFFF)
	{
		//disable schedule:
		if(schMode != Sch_Mode)
		{
			schRequest = 1;
			uint16_t mode = schMode;
			SCH_Put(OBJ_SCH_MODE, WHOLE_OBJECT,(void*)&mode);
		}
	}

	if(schRequest)
	{
		SCH_Put(OBJ_SYSTEM_STARTTRIG,WHOLE_OBJECT,(void*)&schRequest);
	}
}




static uint16_t GetAO_Value(uint16_t idx, uint8_t chn)
{
	//todo to get value from measure subsystem
	float measureValue = 0;
	uint16_t _aoValue = 0;

	if(chn == 0)
	{
//		if(IO_SimuCfgChn1[idx] == SIMU_IO)
//		{
//			_aoValue = IO_AO_Chn1_Simu[idx] ;
//		}
//		else
		{

			float currMeasureValue = currMeasure.measure;
			if((AO_MEASURE0_HOLD == IO_FuncSelChn1[idx]) || (AO_MEASURE1_HOLD == IO_FuncSelChn1[idx]))
			{
				currMeasureValue = lastValidMeasure.measure;
			}
			uint16_t idx_Loc = 0xFFFF;
			switch(IO_FuncSelChn1[idx])
			{
			case AO_NONE:
				idx_Loc = 0xFFFF;
				_aoValue = AO_DEFAULT_OUT;
				break;
			case AO_MEASURE0_ACTIVE:
			case AO_MEASURE0_HOLD:
				idx_Loc = 0;
				measureValue = currMeasureValue;
				break;
			case AO_MEASURE0_TRANSFER:
				if(AO_Event_Trig != OK)
				//if(currMeasure.status != OK)
				{
					idx_Loc = 0xFFFF;
					_aoValue = (uint16_t)(AO_TransferValue * 1000);
				}
				else
				{
					idx_Loc = 0;
					measureValue = currMeasureValue;
				}
				break;
			case AO_MEASURE1_ACTIVE:
			case AO_MEASURE1_HOLD:
				idx_Loc = 1;
				measureValue = currMeasureValue;
				break;
			case AO_MEASURE1_TRANSFER:
				//if(currMeasure.status != OK)
				if(AO_Event_Trig != OK)
				{
					idx_Loc = 0xFFFF;
					_aoValue = (uint16_t)(AO_TransferValue * 1000);
				}
				else
				{
					idx_Loc = 1;
					measureValue = currMeasureValue;
				}
				break;
			}
			if(idx_Loc <= 1)
			{
				float val = (1000.0f*(measureValue - IO_AOSrcChn1_Low[idx])/ \
						(IO_AOSrcChn1_High[idx] - IO_AOSrcChn1_Low[idx])*\
						(AO_Range[idx_Loc][1] - AO_Range[idx_Loc][0]) + 1000.0f*AO_Range[idx_Loc][0]) ;

				if(val > 20000)
					_aoValue = 20000;
				else if(val < 0)
					_aoValue = 0;
				else
					_aoValue = (uint16_t)val;
			//	TraceUser("AO chn 1 output: %d - %.4f\n",_aoValue, val);
			}
			else
			{
			//	TraceUser("AO chn 1 output Error: %d \n",_aoValue);
			}
		}
	}
	else
	{
//		if(IO_SimuCfgChn2[idx] == SIMU_IO)
//		{
//			_aoValue = IO_AO_Chn2_Simu[idx] ;
//		}
//		else
		{

			float currMeasureValue = currMeasure.measure;
			if((AO_MEASURE0_HOLD == IO_FuncSelChn2[idx]) || (AO_MEASURE1_HOLD == IO_FuncSelChn2[idx]))
			{
				currMeasureValue = lastValidMeasure.measure;
			}
			uint16_t idx_Loc = 0xFFFF;
			switch(IO_FuncSelChn2[idx])
			{
			case AO_NONE:
				idx_Loc = 0xFFFF;
				_aoValue = AO_DEFAULT_OUT;
				break;
			case AO_MEASURE0_ACTIVE:
			case AO_MEASURE0_HOLD:
				idx_Loc = 0;
				measureValue = currMeasureValue;
				break;
			case AO_MEASURE0_TRANSFER:
				//if(currMeasure.status != OK)
				if(AO_Event_Trig != OK)
				{
					idx_Loc = 0xFFFF;
					_aoValue = (uint16_t)(AO_TransferValue * 1000);
				}
				else
				{
					idx_Loc = 0;
					measureValue = currMeasureValue;
				}
				break;
			case AO_MEASURE1_ACTIVE:
			case AO_MEASURE1_HOLD:
				idx_Loc = 1;
				measureValue = currMeasureValue;
				break;
			case AO_MEASURE1_TRANSFER:
				//if(currMeasure.status != OK)
				if(AO_Event_Trig != OK)
				{
					idx_Loc = 0xFFFF;
					_aoValue = (uint16_t)(AO_TransferValue * 1000);
				}
				else
				{
					idx_Loc = 1;
					measureValue = currMeasureValue;
				}
				break;
			}

			if(idx_Loc <= 1)
			{
				float val = (1000.0f*(measureValue - IO_AOSrcChn2_Low[idx])/ \
						(IO_AOSrcChn2_High[idx] - IO_AOSrcChn2_Low[idx])* \
						(AO_Range[idx_Loc][1] - AO_Range[idx_Loc][0]) + 1000.0f*AO_Range[idx_Loc][0]) ;

				if(val > 20000)
					_aoValue = 20000;
				else if(val < 0)
					_aoValue = 0;
				else
					_aoValue = (uint16_t)val;
			//	TraceUser("AO chn 2 output: %d - %.4f\n",_aoValue, val);
			}
			else
			{
			//	TraceUser("AO chn 2 output Error: %d \n",_aoValue);
			}
		}
	}


	return _aoValue;

}



void UpdateAOValue(void)
{
//
	for(uint16_t idx =0; idx<MAX_IO_BOARD_NUM;idx++)
	{
		if((IO_BD_TypeSetting[idx] == AO_BOARD) && (IO_BoardEnable[idx] != DISABLE_MODE))
		{
			aoValue[idx*2] = GetAO_Value(idx, 0);
			aoValue[idx*2+1] = GetAO_Value(idx, 1);
		}
		else
		{
			aoValue[idx*2] = 0;
			aoValue[idx*2+1] = 0;
		}
	}
}



void UpdateAOSrc(float val, uint16_t rangeIdx, uint16_t status, uint16_t src)
{
	currMeasure.rangeIdx = rangeIdx;
	currMeasure.measure = val;
	currMeasure.status = status;
	rangeIdx = rangeIdx & FLAG_RANGE_MSK;

//	CheckMeasRange(rangeIdx,val);
	uint16_t st = (CALI_STATUS_MSK|FLAG_SHORT_SAT|FLAG_RETRY_MAX) & status;

	if(st == OK)
	{
		lastValidMeasure.measure = val;
		lastValidMeasure.status = status;
		lastValidMeasure.rangeIdx = rangeIdx;
	}
	if(src == 0)
	{
		currMeasure_BK = currMeasure;
		lastValidMeasure_BK = lastValidMeasure;
	}
}

