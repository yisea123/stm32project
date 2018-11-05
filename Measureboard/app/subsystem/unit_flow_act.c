/*
 * unit_flow_act.c
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_flowstep.h"

#include "t_dataclass.h"
#include "main.h"
#include "unit_flow_act.h"
#include "unit_flow_cfg.h"
#include "unit_meas_cfg.h"
#include "unit_rtc_cfg.h"
#include "tsk_flow_all.h"
#include "dev_log_sp.h"
#include "dev_eep.h"
#include "unit_sch_cfg.h"
#include "tsk_sch.h"

#define FILE_ID		0x18072316

#define ATR_POLL_START		4

#define USE_POLL


#define MAP_DEFAULT 	\
{\
MAP_SHELL,	/*TSK_ID_AD_DETECT =0, */ \
MAP_SHELL,	/*TSK_ID_MEASURE,	*/\
MAP_SHELL,	/*TSK_ID_FLOW_CTRL,	*/\
MAP_SHELL,	/*TSK_ID_VALVE_CTRL,*/\
MAP_SHELL,	/*TSK_ID_MIX_CTRL,*/\
MAP_SHELL,	/*TSK_ID_SUBSTEP_CTRL,*/\
MAP_SHELL,	/*TSK_ID_MOTOR_CTRL,*/\
\
MAP_SHELL,	/*TSK_ID_FLOW_POLL,*/\
MAP_SHELL,	/*TSK_ID_TEMP_MONITOR,*/\
MAP_SHELL,	/*TSK_ID_TEMP_CTRL,*/\
MAP_SHELL,	/*TSK_ID_EEP,*/\
\
MAP_SHELL,	/*TSK_ID_PRINT,*/\
MAP_SHELL,	/*TSK_ID_SHELL_RX,*/\
MAP_SHELL,	/*TSK_ID_SHELL_TX,*/\
MAP_SHELL,	/*TSK_ID_SCH,*/\
MAP_SHELL,	/*TSK_ID_SCH_CLEAN,*/\
MAP_SHELL,	/*TSK_ID_SCH_CALI,*/\
MAP_SHELL,	/*TSK_ID_SCH_MEAS,*/\
MAP_SHELL,	/*TSK_ID_SCH_IO,*/\
MAP_SHELL,  /*TSK_ID_DATA_LOG*/ \
MAP_SHELL,  /*TSK_ID_LOCAL_BUS */\
MAP_SHELL,  /*TSK_ID_POLL_SCH_POLL*/ \
MAP_SHELL,  /*TSK_ID_MCU_STATUS*/ \
MAP_SHELL,	/*TSK_ID_CAN_TSK,*/\
MAP_SHELL,	/*TSK_ID_CAN_MASTER,*/\
MAP_SHELL,	/*TSK_ID_CAN_IO,*/\
MAP_SHELL,	/*TSK_ID_MONITOR,*/\
}



enum
{
	ATR_SUBSTEP,
	ATR_DURING_TIME,
	ATR_PRE_DELAY,
	ATR_CYCLE,
	ATR_START,
	ATR_MAX
};


//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;
static uint32_t passwordID = 0;


static uint16_t flowStep = 0;
static uint16_t ldRomDefaults = 0;

//P3 door
//P4 float




uint32_t putValidNum = 0;

static const PollConfig	pollConfig_Default = {0,0,1,0,0};

static PollConfig	pollConfig = {0,0,1,0,0};

static uint32_t 	fileID1					__attribute__ ((section (".configbuf_act")));
MixConfig	mixConfig						__attribute__ ((section (".configbuf_act")));
uint8_t		printChnMap[MAX_TASK_ID]		__attribute__ ((section (".configbuf_act")));
uint8_t		printMsgMap[MAX_TASK_ID]		__attribute__ ((section (".configbuf_act")));
PollSch     pollSchCfg[POLL_SCH_SIZE]		__attribute__ ((section (".configbuf_act")));
uint8_t		pollSchTrig[POLL_SCH_SIZE]		__attribute__ ((section (".configbuf_act")));
MeasExtend	measExtend[0x08]				__attribute__ ((section (".configbuf_act")));
ValveCtrl   valveCtrl						__attribute__ ((section (".configbuf_act")));
static uint32_t 	fileID2					__attribute__ ((section (".configbuf_act")));
uint16_t 	printAllRawADC = 0;
uint16_t	ValveMask = 0;

uint16_t	pollFunctionEnabled	= 0;

FlowStepRun flowStepRun[FLOW_STEP_MAX];
uint16_t mainActionDetail = 0;
uint8_t	actionStepName[16];
uint8_t	mainStepName[16];
uint8_t	subStepName[16];
uint8_t actionpName[16];
uint16_t fakePara = 0;

uint8_t		freeRtosTskState[MAX_TASK_ID] = {0,0,0,0,0,0,};

uint8_t dbgMsgMap[MAX_TASK_ID] = MAP_DEFAULT;
static uint16_t			schCmd0[2] = {TSK_INIT, 0 };
static uint16_t			schCmd1[2] = {TSK_INIT, 0 };
static uint16_t			schCmd2[2] = {TSK_INIT, 0 };
static uint16_t			schCmd3[2] = {TSK_INIT, 0 };
static uint16_t			schCmd4[2] = {TSK_INIT, 0 };
uint16_t				systemReset = 0;
uint16_t				systemMsgClr = 0;
__IO uint16_t		printfEnable[MAP_PRINT_MAX] = {1,1,1,1};
MeasExtendAction measExtendAction =
{
		0,0,0,0,0,
};


static const ValveCtrl   valveCtrl_Default =
{
		100,
		50,
		200,
		20,
};

static const uint32_t fileID_Default = FILE_ID;
static const MixConfig mixConfig_Default =
{
		1600,400,600,3000,29,50,1,1,200,
};


static const MeasExtend	measExtend_Default[0x08] =
{
		{0x03,10,3,0,},//0
		{0x04,5,2,0,},
		{0x02,6,3,0,},//2
		{0x01,7,2,0,},
		{0x03,8,3,0,},//4
		{0x02,9,2,0,},
		{0x0,0,5,0,},//6
		{0,0,3,0,},
};


const char* TskName[MAX_TASK_ID] =
{
	TO_STR(AD_DETECT),
	TO_STR(MEASURE),
	TO_STR(FLOW_),
	TO_STR(VALVE_),
	TO_STR(MIX_TSK),
	TO_STR(SUBSTEP),
	TO_STR(MOTOR_CTRL),//6

	TO_STR(FLOW_POLL),
	TO_STR(TEMP_MON),
	TO_STR(TEMP_CTRL),
	TO_STR(EEP_TSK),

	TO_STR(PRINT_),
	TO_STR(SHELL_RX),
	TO_STR(SHELL_TX),
	TO_STR(SCH_ALL),
	TO_STR(SCH_CLEAN),
	TO_STR(SCH_CALI),
	TO_STR(SCH_MEAS),
	TO_STR(SCH_IO),
	TO_STR(DATA_LOG),
	TO_STR(LOCAL_BUS),

	TO_STR(SCH_POLL),
	TO_STR(MCU_STATUS),
	TO_STR(CAN_TSK),
	TO_STR(CAN_MASTER),
	TO_STR(CAN_IO),
	TO_STR(TSK_ID_MONITOR),
	TO_STR(TSK_ID_AUTO_RANGE),
};

/*
 * TSK_ID_AD_DETECT =0,
 	TSK_ID_MEASURE,
 	TSK_ID_FLOW_CTRL,
 	TSK_ID_VALVE_CTRL,//3
 	TSK_ID_MIX_CTRL,
 	TSK_ID_SUBSTEP_CTRL,
 	TSK_ID_MOTOR_CTRL,//6



 	TSK_ID_FLOW_POLL,
 	TSK_ID_TEMP_MONITOR,//8
 	TSK_ID_TEMP_CTRL,
 	TSK_ID_EEP,//10


 	TSK_ID_PRINT,//11
 	TSK_ID_SHELL_RX,
 	TSK_ID_SHELL_TX,//13
 	TSK_ID_SCH,
 	TSK_ID_SCH_CLEAN,//15

 	TSK_ID_SCH_CALI,
 	TSK_ID_SCH_MEAS,//17
 	TSK_ID_SCH_IO,
 	TSK_ID_DATA_LOG,//19
 	TSK_ID_LOCAL_BUS,//20


 	TSK_ID_POLL_SCH_POLL,//21
	TSK_ID_MCU_STATUS,//22
 	TSK_ID_CAN_TSK,//23
 	TSK_ID_CAN_MASTER,
 	TSK_ID_CAN_IO,//25
 	TSK_ID_MONITOR,//26
 */

static const uint8_t		printChnMap_Default[MAX_TASK_ID] =
{
		MAP_NONE,	//TSK_ID_AD_DETECT =0,
		MAP_NONE,	//TSK_ID_MEASURE,
		MAP_NONE,	//TSK_ID_FLOW_CTRL,
		MAP_NONE,	//TSK_ID_VALVE_CTRL,
		MAP_NONE,	//TSK_ID_MIX_CTRL,
		MAP_NONE,	//TSK_ID_SUBSTEP_CTRL,
		MAP_NONE,	//TSK_ID_MOTOR_CTRL,



		MAP_NONE,	//TSK_ID_FLOW_POLL,
		MAP_NONE,	//TSK_ID_TEMP_MONITOR,
		MAP_NONE,	//TSK_ID_TEMP_CTRL,
		MAP_NONE,	//TSK_ID_EEP,

		MAP_TRACE,	//TSK_ID_PRINT,
		MAP_TRACE,	//TSK_ID_SHELL_RX,
		MAP_TRACE,	//TSK_ID_SHELL_TX,
		MAP_NONE,	//TSK_ID_SCH,
		MAP_NONE,	//TSK_ID_SCH_CLEAN,

		MAP_NONE,	//TSK_ID_SCH_CALI,
		MAP_NONE,	//TSK_ID_SCH_MEAS,
		MAP_NONE,	//TSK_ID_SCH_IO,
		MAP_NONE,  //TSK_ID_DATA_LOG
		MAP_NONE,  //TSK_ID_LOCAL_BUS

		MAP_NONE,  //TSK_ID_POLL_SCH_POLL
		MAP_NONE,  //TSK_ID_MCU_STATUS
		MAP_TRACE,	//TSK_ID_CAN_TSK,
		MAP_TRACE,	//TSK_ID_CAN_MASTER,
		MAP_TRACE,	//TSK_ID_CAN_IO,
		MAP_TRACE,	//TSK_ID_MONITOR,
		MAP_SHELL,  //TSK_ID_AUTO_RANGE
};


static const uint8_t printMsgMap_Default[MAX_TASK_ID] =
{
		MAP_NONE,	//TSK_ID_AD_DETECT =0,
		MAP_NONE,	//TSK_ID_MEASURE,
		MAP_NONE,	//TSK_ID_FLOW_CTRL,
		MAP_NONE,	//TSK_ID_VALVE_CTRL,
		MAP_SHELL,	//TSK_ID_MIX_CTRL,
		MAP_SHELL,	//TSK_ID_SUBSTEP_CTRL,
		MAP_SHELL,	//TSK_ID_MOTOR_CTRL,



		MAP_NONE,	//TSK_ID_FLOW_POLL,
		MAP_NONE,	//TSK_ID_TEMP_MONITOR,
		MAP_NONE,	//TSK_ID_TEMP_CTRL,
		MAP_SHELL,	//TSK_ID_EEP,

		MAP_NONE,	//TSK_ID_PRINT,
		MAP_NONE,	//TSK_ID_SHELL_RX,
		MAP_NONE,	//TSK_ID_SHELL_TX,
		MAP_SHELL,	//TSK_ID_SCH,
		MAP_SHELL,	//TSK_ID_SCH_CLEAN,
		MAP_SHELL,	//TSK_ID_SCH_CALI,
		MAP_SHELL,	//TSK_ID_SCH_MEAS,
		MAP_NONE,	//TSK_ID_SCH_IO,
		MAP_TRACE,  //TSK_ID_DATA_LOG
		MAP_SHELL,  //TSK_ID_LOCAL_BUS
		MAP_SHELL,  //TSK_ID_POLL_SCH_POLL
		MAP_TRACE,  //TSK_ID_MCU_STATUS

		MAP_TRACE,	//TSK_ID_CAN_TSK,
		MAP_TRACE,	//TSK_ID_CAN_MASTER,
		MAP_TRACE,	//TSK_ID_CAN_IO,
		MAP_TRACE,	//TSK_ID_MONITOR,
		MAP_SHELL,  //TSK_ID_AUTO_RANGE
};

#ifndef DEBUG_TEST
static const PollSch     pollSchCfg_Default[POLL_SCH_SIZE]	=
{
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},

		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
};


static const uint8_t		pollSchTrig_Default[POLL_SCH_SIZE+2] =
{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,
};
#else
static const PollSch     pollSchCfg_Default[POLL_SCH_SIZE]	=
{
		{300,1,130},
		{300,2,131},
		{300,3,132},
		{300,1,133},
		{300,2,134},
		{300,3,135},
		{0,1,0},
		{0,1,0},

		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
};


static const uint8_t		pollSchTrig_Default[POLL_SCH_SIZE+2] =
{
		1,2,3,4,5,6,7,8,
		9,10,0,0,0,0,0,0,
		0,0,
};

#endif
static const  T_DATACLASS _ClassList[]=
{
	//lint -e545 -e651
	CONSTRUCTOR_DC_DYNAMIC(pollConfig,pollConfig_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(mixConfig,mixConfig_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(printChnMap,printChnMap_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(printMsgMap,printMsgMap_Default),
	//lint -e545
	CONSTRUCTOR_DC_DYNAMIC(dbgMsgMap,printMsgMap_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pollSchTrig,pollSchTrig_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pollSchCfg,pollSchCfg_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measExtend,measExtend_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(valveCtrl,valveCtrl_Default),


};




//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
		//0
	CONSTRUCT_SIMPLE_U16(&flowStep, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(
			&pollConfig,
			sizeof(pollConfig)/sizeof(uint32_t),
			RAM),
	CONSTRUCT_SIMPLE_U16(&ldRomDefaults, RAM),

	CONSTRUCT_SIMPLE_U32(&passwordID, RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
				&mixConfig,
				sizeof(mixConfig)/sizeof(uint16_t),
				NON_VOLATILE),


		//5
	CONSTRUCT_SIMPLE_U16(
				&printAllRawADC,
				RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
					schCmd0,2,
					RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
	schCmd1,2,
	RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
	schCmd2,2,
	RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
	schCmd3,2,
	RAM),


	//10
	CONSTRUCT_ARRAY_SIMPLE_U16(
	schCmd4,2,	RAM),

	CONSTRUCT_SIMPLE_U16(
	&systemReset,	RAM),

	CONSTRUCT_ARRAY_SIMPLE_U8(
		&printChnMap[0],sizeof(printChnMap),
		NON_VOLATILE),

	CONSTRUCT_ARRAY_SIMPLE_U8(
			&printMsgMap[0],sizeof(printMsgMap),
			NON_VOLATILE),

	CONSTRUCT_ARRAY_SIMPLE_U8(
			&dbgMsgMap[0],sizeof(dbgMsgMap),
			RAM),



	//15
	CONSTRUCT_ARRAY_SIMPLE_U8(
				&freeRtosTskState[0],sizeof(freeRtosTskState) ,	RAM),

	CONSTRUCT_ARRAY_SIMPLE_U16(
				&pollSchCfg[0],3,
				NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[1],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[2],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[3],3,
			NON_VOLATILE),



			//20
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[4],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[5],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[6],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[7],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[8],3,
			NON_VOLATILE),



			//25
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[9],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[10],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[11],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[12],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[13],3,
			NON_VOLATILE),



			//30
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[14],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
			&pollSchCfg[15],3,
			NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U8(
			&pollSchTrig[0],sizeof(pollSchTrig)/sizeof(uint8_t),
			NON_VOLATILE),

	CONSTRUCT_SIMPLE_U16(
			&systemMsgClr,	RAM),

	CONSTRUCT_STRUCT_STEPINFO(
					&flowStepRun[0],
					READONLY_RAM),



		//35
	CONSTRUCT_STRUCT_STEPINFO(
							&flowStepRun[1],
					READONLY_RAM),
	CONSTRUCT_STRUCT_STEPINFO(
							&flowStepRun[2],
					READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(
			&ValveMask,
			RAM),
	CONSTRUCT_SIMPLE_U16(
							&pollFunctionEnabled,
							RAM),

	CONSTRUCT_ARRAY_SIMPLE_CHAR( &actionpName[0],sizeof(actionpName),READONLY_RAM),

	//40
	CONSTRUCT_ARRAY_SIMPLE_CHAR( &mainStepName[0],sizeof(mainStepName),READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_CHAR( &subStepName[0],sizeof(subStepName),READONLY_RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
							&printfEnable,sizeof(printfEnable)/sizeof(uint16_t),
							RAM),

	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtendAction,sizeof(measExtendAction)/sizeof(uint16_t),
					RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[0],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),


	//45
	CONSTRUCT_ARRAY_SIMPLE_U16(
						&measExtend[1],sizeof(MeasExtend)/sizeof(uint16_t),
						NON_VOLATILE),

	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[2],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[3],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[4],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[5],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),


	//50
	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[6],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
					&measExtend[7],sizeof(MeasExtend)/sizeof(uint16_t),
					NON_VOLATILE),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,

//55
	CONSTRUCT_ARRAY_SIMPLE_U16(
						&valveCtrl,sizeof(valveCtrl)/sizeof(uint16_t),
						NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(
						&mainActionDetail,
						READONLY_RAM),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
//60
	CONSTRUCT_SIMPLE_U16(
						&fakePara,
						RAM),
};



//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT flowAct =
{
	(uint8_t*)"flowAct",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_FlowAct, // will be overloaded
	LoadRomDefaults_T_UNIT,
	ResetToDefault_T_UNIT,
	SaveAsDefault_T_UNIT,
	Get_FlowAct,
	Put_FlowAct,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};

void SetFlowStep(uint16_t id, uint16_t step, uint32_t _duringTime)
{
	assert(id < FLOW_STEP_MAX);
	flowStepRun[id].step = step ;
	flowStepRun[id].duringTime = (int32_t)_duringTime;
	flowStepRun[id].startTime = GetCurrentST();
	flowStepRun[id].startTick = HAL_GetTick();
}

int32_t GetFlowStepRemainTime(uint16_t id)
{
	uint32_t tick = HAL_GetTick();
	int32_t remain = 0;
	assert(id < FLOW_STEP_MAX);

	if( tick >= flowStepRun[id].startTick)
	{
		remain = (int32_t)(tick - flowStepRun[id].startTick)/1000;
	}
	else
	{
		remain = (int32_t)(0xFFFFFFFF - flowStepRun[id].startTick + tick)/1000;
	}
	remain = flowStepRun[id].duringTime - remain;
	if(remain < 0)
		remain = 0;

	return remain;
}

uint16_t Initialize_FlowAct(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&flowAct);
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





static void UpdateSchMeasCfg(uint16_t mode)
{
	extern uint16_t ValveMask;
	ValveMask = 0;
	if(mode)
	{
		if(measExtendAction.start)
		{
			if(measExtendAction.currentIdx < sizeof(measExtend)/sizeof(MeasExtend))
			{
				ValveMask = measExtend[measExtendAction.startIdx].valveIO;
				//delay time configuration;
				extern Measure_Sch			measSch;
				if(measExtend[measExtendAction.currentIdx].measureInterval == 0)
					measSch.measInterval = 1;
			}
			else
			{
//				measSch.measureMode = MANUAL_MODE;
			}
		}

		else
		{
//			measSch.measureMode = MANUAL_MODE;
		}
	}
	else
	{
//		measSch.measureMode = MANUAL_MODE;
	}

	//no need to save, as it is only run-time change
//	Trigger_EEPSave((void*)&measSch, sizeof(measSch), SYNC_IM);
}



uint16_t MeasExecuteOnce(void)
{
	//this function shall be called before measure is scheduled
	uint16_t ret = OK;
	uint16_t cfg = 0;
	//measure poll schedule
	if(measExtendAction.start)
	{
		ret = FATAL_ERROR;
		if(measExtendAction.currentIdx < sizeof(measExtend)/sizeof(MeasExtend))
		{
			if(measExtend[measExtendAction.currentIdx].maxCycles != 0)
			{
				ret = OK;
				measExtend[measExtendAction.currentIdx].counter++;

				if(measExtend[measExtendAction.currentIdx].counter >= measExtend[measExtendAction.currentIdx].maxCycles)
				{
					//clear old
					measExtend[measExtendAction.currentIdx].counter = 0;
					measExtendAction.currentIdx++;
					if(measExtend[measExtendAction.currentIdx].maxCycles)
					{
						//clear new
						measExtend[measExtendAction.currentIdx].counter = 0;
						cfg = 1;
						//UpdateSchMeasCfg(1);
					}
					else
					{
						//UpdateSchMeasCfg(0);
					}
				}
				else
				{
					//ok
					cfg = 1;
					//UpdateSchMeasCfg(1);
				}
			}
			else
			{
				//UpdateSchMeasCfg(0);
			}
		}
		else
		{
			//UpdateSchMeasCfg(0);
		}
		TraceMsg(TSK_ID_SCH, "Meas poll is schedlued; current ID: %d, Current Counter: %d\n", measExtendAction.currentIdx, measExtend[measExtendAction.currentIdx].counter);
		UpdateSchMeasCfg(cfg);
	}
	return ret;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put flow act
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
uint16_t Put_FlowAct(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	static uint16_t	pollFunctionEnabled_Old = 0;
	VIP_ASSERT(me == &flowAct);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
		switch(objectIndex)
		{
		case OBJ_MEAS_FAKE_PARA:
			if(passwordID == 6000)
			{
				FakeParaInit(fakePara);
			}
			else
			{
				result = RULE_VIOLATION;
			}

			break;
		case IDX_FLOW_STEP:

			if((A_0 <=flowStep) && (flowStep < (A_MAX)))
			{
				if(FlowTskStatus != TASK_IDLE)
				{
					result = WARNING;
				}
				else
				{
					SendTskMsg(FLOW_TSK_ID, TSK_INIT, flowStep, NULL);
				}
			}
			else
			{
				SendTskMsg(FLOW_TSK_ID, TSK_INIT, flowStep, NULL);
			}
			break;
		case IDX_POLL_ACTION:
			if(ATR_POLL_START == attributeIndex)
			{
				if(pollFunctionEnabled)
				{
					if(pollConfig.start)
					{
						SendTskMsg(POLL_SCH_ID, TSK_INIT, (uint32_t)&pollConfig, NULL);
					}
					else
					{
						SendTskMsg(POLL_SCH_ID, TSK_FORCE_BREAK, (uint32_t)&pollConfig, NULL);
					}
				}
				else
				{
					result = RULE_VIOLATION_ERR;
				}
			}
		break;
		case IDX_LOAD_ROM_DF:
			if(ldRomDefaults == 0xFFFF)
			{
				LoadDefaultCfg(0xFFFF);
			}
			else if(ldRomDefaults < IDX_SUB_MAX)
			{
				LoadDefaultCfg(ldRomDefaults);
			}
			else
			{
				result = ILLEGAL_ATTRIB_IDX;
			}
			WakeUpSchedule();
			break;
		case IDX_SET_PASSWORD:
			if(VALID_PASSWORD_ID == passwordID)
				putValidNum = 1000;
		break;
		case IDX_SET_SCH:
			if(attributeIndex == 1)
			{
				SendTskMsg(SCH_ID, (TSK_STATE)schCmd0[0], (uint32_t)schCmd0[1], NULL);

			}
			break;
		case IDX_SET_SCH_MEAS:
			if(attributeIndex == 1)
			{
				SendTskMsg(SCH_MEAS_ID, (TSK_STATE)schCmd1[0], (uint32_t)schCmd1[1], NULL);

			}
			break;
		case IDX_SET_SCH_CALI:
			if(attributeIndex == 1)
			{
				SendTskMsg(SCH_CALI_ID, (TSK_STATE)schCmd2[0], (uint32_t)schCmd2[1], NULL);
			}
			break;
		case IDX_SET_SCH_CLEAN:
			if(attributeIndex == 1)
			{
				SendTskMsg(SCH_CLEAN_ID, (TSK_STATE)schCmd3[0], (uint32_t)schCmd3[1], NULL);
			}
			break;
		case IDX_SET_SCH_IO:
			if(attributeIndex == 1)
			{
				SendTskMsg(SCH_IO_ID, (TSK_STATE)schCmd4[0], (uint32_t)schCmd4[1], NULL);
			}
			break;
		case IDX_SYSTEM_RST:
			if(systemReset != 0)
			{
				if(systemReset == VALID_PASSWORD_ID)
				{
					validPrintMsg = VALID_RST_MSG;
				}
				//save eep data;
				SigPush(eepTaskHandle, EEP_SIGNAL_RESET_SAVE);

				//trigger reset by eep task

			}
			break;
		case OBJ_CLR_DBG_MSG:
			if(systemMsgClr != 0)
			{
				validPrintMsg = 0;
				validPrintLen = 0;
			}
			break;
		//case OBJ_MEAS_EXECUTE_ONCE:
		//	MeasExecuteOnce();
		//	break;
		case OBJ_POLL_OR_SCH:
			if(pollFunctionEnabled_Old != pollFunctionEnabled)
			{
				pollFunctionEnabled_Old = pollFunctionEnabled;
				if(pollFunctionEnabled)
				{
					pollConfig.start = 0;
					if(measExtendAction.start != 0)
					{
						measExtendAction.start = 0;
						SendTskMsg(SCH_ID, TSK_FORCE_BREAK, SCH_TSK_IDLE, NULL);
					}
				}
				else
				{
					measExtendAction.start = 0;
					if(pollConfig.start)
					{
						pollConfig.start = 0;
						SendTskMsg(POLL_SCH_ID, TSK_FORCE_BREAK, (uint32_t)&pollConfig, NULL);
					}
				}
			}
			break;
		case OBJ_MEAS_EXTENDACTION:
		{
			switch(attributeIndex)
			{
				case MEACTION_RESET:
				{
					for(uint32_t i=0; i < sizeof(measExtend)/sizeof(MeasExtend);i++)
					{
						measExtend[i].counter = 0;
					}
					Trigger_EEPSave((void*)&measExtend, sizeof(measExtend), SYNC_IM);
				}
					break;
				case MEACTION_ST_IDX:
					if(measExtendAction.startIdx > sizeof(measExtend)/sizeof(MeasExtend))
					{
						measExtendAction.startIdx = 0;
						result = GREATERTHAN_RANGEMAX_ERR;
					}
					break;

				case MEACTION_START:
				{

					ValveMask = 0;
					if(!pollFunctionEnabled)
					{
						if(measExtendAction.start != 0)
						{
							measExtendAction.currentIdx = measExtendAction.startIdx;
							UpdateSchMeasCfg(1);
							WakeUpSchedule();
						}
						else
						{
							SendTskMsg(SCH_ID, TSK_FORCE_BREAK, (uint32_t)SCH_TSK_IDLE, NULL);
							//close all existing functions:

							UpdateSchMeasCfg(0);
						}
					}
					else
					{
						result = RULE_VIOLATION_ERR;
					}
				}
				break;

				default:
					break;

			}

		}
			break;
		default:
			break;
		}
	}

	return result;
}



//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get flowact
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
uint16_t Get_FlowAct(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &flowAct);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	if(objectIndex == OBJ_GET_FLOW2_STATUS || objectIndex == OBJ_GET_FLOW1_STATUS || objectIndex == OBJ_GET_FLOW3_STATUS)
	{
		for(uint16_t idx = 0;idx<FLOW_STEP_MAX;idx++)
		{
			flowStepRun[idx].remainTime = GetFlowStepRemainTime(idx);
		}
	}
	else if(OBJ_MAINSTEP_NAME == objectIndex)
	{
		memcpy(mainStepName, GetStepName((uint16_t)flowStepRun[1].step), sizeof(mainStepName));
	}
	else if(OBJ_SUBSTEP_NAME == objectIndex)
	{
		memcpy(subStepName, GetStepName((uint16_t)flowStepRun[2].step), sizeof(subStepName));
	}
	else if(OBJ_ACTION_NAME == objectIndex)
	{
		memcpy(actionpName, GetActionName((uint16_t)flowStepRun[0].step), sizeof(actionpName));
	}

	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);

	return result;
}
