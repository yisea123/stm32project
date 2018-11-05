/*
* unit_flow_act.c
*
*  Created on: 2018Äê2ÔÂ11ÈÕ
*      Author: vip
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
#define VALID_PASSWORD_ID		6000

static const uint8_t fileID_Default = 0x0B;

#define MAP_DEFAULT 	\
{\
PRINT_MAP_SHELL,	/*TSK_ID_AD_DETECT =0, */ \
PRINT_MAP_SHELL,	/*TSK_ID_MEASURE,	*/\
PRINT_MAP_SHELL,	/*TSK_ID_FLOW_CTRL,	*/\
PRINT_MAP_SHELL,	/*TSK_ID_VALVE_CTRL,*/\
PRINT_MAP_SHELL,	/*TSK_ID_MIX_CTRL,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SUBSTEP_CTRL,*/\
PRINT_MAP_SHELL,	/*TSK_ID_MOTOR_CTRL,*/\
\
PRINT_MAP_SHELL,	/*TSK_ID_FLOW_POLL,*/\
PRINT_MAP_SHELL,	/*TSK_ID_TEMP_MONITOR,*/\
PRINT_MAP_SHELL,	/*TSK_ID_TEMP_CTRL,*/\
PRINT_MAP_SHELL,	/*TSK_ID_EEP,*/\
\
PRINT_MAP_SHELL,	/*TSK_ID_PRINT,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SHELL_RX,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SHELL_TX,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SCH,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SCH_CLEAN,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SCH_CALI,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SCH_MEAS,*/\
PRINT_MAP_SHELL,	/*TSK_ID_SCH_IO,*/\
PRINT_MAP_SHELL,  /*TSK_ID_DATA_LOG*/ \
PRINT_MAP_SHELL,  /*TSK_ID_LOCAL_BUS */\
PRINT_MAP_SHELL,  /*TSK_ID_POLL_SCH_POLL*/ \
PRINT_MAP_SHELL,  /*TSK_ID_MCU_STATUS*/ \
PRINT_MAP_SHELL,	/*TSK_ID_CAN_TSK,*/\
PRINT_MAP_SHELL,	/*TSK_ID_CAN_MASTER,*/\
PRINT_MAP_SHELL,	/*TSK_ID_CAN_IO,*/\
PRINT_MAP_SHELL,	/*TSK_ID_MONITOR,*/\
}



static const uint8_t		printChnMap_Default[MAX_TASK_ID] =
{
		PRINT_MAP_NONE,		//TSK_ID_AD_DETECT =0,
		PRINT_MAP_NONE,		//TSK_ID_MEASURE,
		PRINT_MAP_NONE,		//TSK_ID_FLOW_CTRL,
		PRINT_MAP_NONE,		//TSK_ID_VALVE_CTRL,//3
		PRINT_MAP_NONE,		//TSK_ID_MIX_CTRL,
		PRINT_MAP_NONE,		//TSK_ID_SUBSTEP_CTRL,
		PRINT_MAP_NONE,		//TSK_ID_MOTOR_CTRL,//6



		PRINT_MAP_NONE,		//TSK_ID_FLOW_POLL,
		PRINT_MAP_NONE,		//TSK_ID_IDLE,//8
		PRINT_MAP_NONE,		//TSK_ID_TEMP_CTRL,
		PRINT_MAP_SHELL,	//TSK_ID_EEP,//10


		PRINT_MAP_NONE,		//TSK_ID_PRINT,//11
		PRINT_MAP_NONE,		//TSK_ID_SHELL_RX,
		PRINT_MAP_NONE,		//TSK_ID_SHELL_TX,//13
		PRINT_MAP_NONE,		//TSK_ID_SCH,
		PRINT_MAP_NONE,		//TSK_ID_SCH_CLEAN,//15
		PRINT_MAP_NONE,		//TSK_ID_SCH_CALI,
		PRINT_MAP_NONE,		//TSK_ID_SCH_MEAS,//17
		PRINT_MAP_NONE,		//TSK_ID_SCH_IO,
		PRINT_MAP_NONE,		//TSK_ID_DATA_LOG,//19
		PRINT_MAP_NONE,		//TSK_ID_LOCAL_BUS,//20


		PRINT_MAP_NONE,		//TSK_ID_POLL_SCH_POLL,//21
		PRINT_MAP_NONE,		//TSK_ID_MCU_STATUS,//22
		PRINT_MAP_NONE,		//TSK_ID_CAN_TSK,//23
		PRINT_MAP_NONE,		//TSK_ID_CAN_MASTER,
		PRINT_MAP_NONE,		//TSK_ID_CAN_IO,//25
		PRINT_MAP_NONE,		//TSK_ID_MONITOR,//26
		PRINT_MAP_NONE,		//TSK_ID_AUTO_RANGE,//27
		PRINT_MAP_NONE,		//TSK_ID_MICROSTEP,
		PRINT_MAP_NONE,		//TSK_ID_MAINSTEP,
		PRINT_MAP_NONE,		//TSK_ID_SUBSTEP,
		PRINT_MAP_NONE,		//TSK_ID_PRESUB,
		PRINT_MAP_NONE,		//TSK_ID_PREMAIN,
		PRINT_MAP_NONE,		//TSK_ID_MOTOR,
		PRINT_MAP_NONE,		//TSK_ID_POLLSCH,
		PRINT_MAP_NONE,		//TSK_ID_SP,
};


static const uint8_t printMsgMap_Default[MAX_TASK_ID] =
{


		PRINT_MAP_NONE,		//TSK_ID_AD_DETECT =0,
		PRINT_MAP_NONE,		//TSK_ID_MEASURE,
		PRINT_MAP_NONE,		//TSK_ID_FLOW_CTRL,
		PRINT_MAP_NONE,		//TSK_ID_VALVE_CTRL,//3
		PRINT_MAP_NONE,		//TSK_ID_MIX_CTRL,
		PRINT_MAP_NONE,		//TSK_ID_SUBSTEP_CTRL,
		PRINT_MAP_NONE,		//TSK_ID_MOTOR_CTRL,//6



		PRINT_MAP_NONE,		//TSK_ID_FLOW_POLL,
		PRINT_MAP_NONE,		//TSK_ID_IDLE,//8
		PRINT_MAP_NONE,		//TSK_ID_TEMP_CTRL,
		PRINT_MAP_SHELL,		//TSK_ID_EEP,//10


		PRINT_MAP_NONE,		//TSK_ID_PRINT,//11
		PRINT_MAP_NONE,		//TSK_ID_SHELL_RX,
		PRINT_MAP_NONE,		//TSK_ID_SHELL_TX,//13
		PRINT_MAP_NONE,		//TSK_ID_SCH,
		PRINT_MAP_NONE,		//TSK_ID_SCH_CLEAN,//15
		PRINT_MAP_NONE,		//TSK_ID_SCH_CALI,
		PRINT_MAP_NONE,		//TSK_ID_SCH_MEAS,//17
		PRINT_MAP_NONE,		//TSK_ID_SCH_IO,
		PRINT_MAP_NONE,		//TSK_ID_DATA_LOG,//19
		PRINT_MAP_NONE,		//TSK_ID_LOCAL_BUS,//20


		PRINT_MAP_NONE,		//TSK_ID_POLL_SCH_POLL,//21
		PRINT_MAP_NONE,		//TSK_ID_MCU_STATUS,//22
		PRINT_MAP_NONE,		//TSK_ID_CAN_TSK,//23
		PRINT_MAP_NONE,		//TSK_ID_CAN_MASTER,
		PRINT_MAP_NONE,		//TSK_ID_CAN_IO,//25
		PRINT_MAP_NONE,		//TSK_ID_MONITOR,//26
		PRINT_MAP_NONE,		//TSK_ID_AUTO_RANGE,//27
		PRINT_MAP_SHELL,		//TSK_ID_MICROSTEP,
		PRINT_MAP_SHELL,		//TSK_ID_MAINSTEP,
		PRINT_MAP_NONE,		//TSK_ID_SUBSTEP,
		PRINT_MAP_NONE,		//TSK_ID_PRESUB,
		PRINT_MAP_SHELL,		// TSK_ID_PREMAIN,
		PRINT_MAP_NONE,		//TSK_ID_MOTOR,
		PRINT_MAP_SHELL,		//TSK_ID_POLLSCH,
		PRINT_MAP_NONE,		//TSK_ID_SP,
};


static const uint8_t dbgMsgMap_Default[MAX_TASK_ID] = MAP_DEFAULT;


static const BubbleCfg bubbleCfg_Default[MAX_BUBBLE] =
{
    {4,4,4,4,FAILED,50},
    {4,4,4,4,FAILED,50},
    {4,4,4,4,FAILED,300},
};

static const uint16_t pumpExecDelay_Default = 300;
static uint8_t fileId1								__attribute__ ((section (".configbuf_act")));
uint8_t printChnMap[MAX_TASK_ID]					__attribute__ ((section (".configbuf_act")));
uint8_t printMsgMap[MAX_TASK_ID]					__attribute__ ((section (".configbuf_act")));
uint8_t dbgMsgMap[MAX_TASK_ID]						__attribute__ ((section (".configbuf_act")));
BubbleCfg bubbleCfg[ MAX_BUBBLE ]					__attribute__ ((section (".configbuf_act")));
uint16_t pumpExecDelay								__attribute__ ((section (".configbuf_act")));
uint8_t pollSchTrig[POLL_SCH_SIZE]					__attribute__ ((section (".configbuf_act")));
static uint8_t fileId2								__attribute__ ((section (".configbuf_act")));
PumpCfg pumpCfg2									__attribute__ ((section (".configbuf_act")));
PollSch	pollSchCfg[POLL_SCH_SIZE]					__attribute__ ((section (".configbuf_act")));
uint16_t motorHighSpeed								__attribute__ ((section (".configbuf_act")));
uint8_t freeRtosTskState[MAX_TASK_ID];
uint16_t 	printAllRawADC = 0;
/*
 * 	uint8_t dir;
	uint8_t mode;
	uint16_t pps;
	uint32_t steps;
 */
//23*64 steps //1/4
/*
 * 	uint8_t dir;
	uint8_t speed;
	uint16_t steps;
    uint16_t preDefineTime;
    uint16_t preDefineSteps;
    uint16_t errorAction;
 */
static const PumpCfg pumpCfg2_Default =
{
		PULL|FREE_POS	,	9	,	86	,	0	,	0	,	ACT_NONE
};
static const PollSch     pollSchCfg_Default[POLL_SCH_SIZE]	=
{
		{10,1,230},
		{10,24,233},
		{10,5,236},
		{10,5,235},
		{10,60,233},
		{10,5,236},
		{10,5,235},
		{10,300,233},

		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
		{0,1,0},
};
static const uint16_t motorHighSpeed_Default = MOTOR_PPS_HS;
//ram
uint16_t				systemReset = 0;
uint16_t				systemMsgClr = 0;

static PollConfig		pollConfig = {1,ALL_STEPS_RUN,0};
uint16_t 				mainActionDetail = 0;
uint16_t 				fakePara = 0;
FlowStepRun 			flowStepRun[ID_MAX_STEP];
uint8_t					actionStepName[16];
uint8_t					mainStepName[16];
uint8_t					subStepName[16];
uint8_t 				actionpName[16];
uint16_t 				FlowTskStatus = 0;
uint16_t                printCtrlCfg[MSG_TYPE_MAX] = {1,1,1,1};
static uint32_t                microSecondsEachStep[e_main_step_end+1];
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;


static const uint8_t pollSchTrig_Default[POLL_SCH_SIZE] = {
    233,233,233,233,233,
    233,233,233,233,233,
    39,39,39,39,39,//3
};

static uint32_t passwordID = 0;
static uint16_t flowStep = 0;
static uint16_t ldRomDefaults = 0;
FlowStepInfo gFlowStep;
uint16_t ctrlTemp;
static const  T_DATACLASS _ClassList[]=
{
 	//lint -e545
    CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(printChnMap,printChnMap_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(printMsgMap,printMsgMap_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(dbgMsgMap,dbgMsgMap_Default),
 	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(bubbleCfg,bubbleCfg_Default),
 	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pumpExecDelay,pumpExecDelay_Default),
 	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pumpCfg2,pumpCfg2_Default),
   
 	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pollSchTrig,pollSchTrig_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pollSchCfg,pollSchCfg_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(motorHighSpeed,motorHighSpeed_Default),

    
    
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
                               &bubbleCfg[LED0_BUBBLE],
                               sizeof(bubbleCfg[LED0_BUBBLE])/sizeof(uint16_t),
                               NON_VOLATILE),
    
    
    //5
	CONSTRUCT_ARRAY_SIMPLE_U16(
                               &bubbleCfg[LED1_BUBBLE],
                               sizeof(bubbleCfg[LED1_BUBBLE])/sizeof(uint16_t),
                               NON_VOLATILE),
    
	NULL_T_DATA_OBJ,
	CONSTRUCT_ARRAY_SIMPLE_U16(
                               &printCtrlCfg,
                               sizeof(printCtrlCfg)/sizeof(uint16_t),
                               RAM),
	CONSTRUCT_SIMPLE_U16(
                         &pumpExecDelay,	NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(
                               &bubbleCfg[LED0_BUBBLE_EX],
                               sizeof(bubbleCfg[LED1_BUBBLE])/sizeof(uint16_t),
                               NON_VOLATILE),
    
	//10
	CONSTRUCT_ARRAY_SIMPLE_U8(
                               &pollSchTrig,
                               sizeof(pollSchTrig)/sizeof(uint8_t),
                               NON_VOLATILE),
    
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
    
	NULL_T_DATA_OBJ,
    
	CONSTRUCT_SIMPLE_U16(
                         &systemMsgClr,	RAM),
    
	CONSTRUCT_STRUCT_STEPRUN(
                              &flowStepRun[0],
                              READONLY_RAM),
    
    
    
    //35
	CONSTRUCT_STRUCT_STEPRUN(
                              &flowStepRun[1],
                              READONLY_RAM),
	CONSTRUCT_STRUCT_STEPRUN(
                              &flowStepRun[2],
                              READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(
                         &mainActionDetail,
                         READONLY_RAM),
	CONSTRUCT_STRUCT_PUMPCFG(
							   &pumpCfg2, NON_VOLATILE),
	NULL_T_DATA_OBJ,
    //40
	CONSTRUCT_SIMPLE_U16(
                         &fakePara,
                         RAM),
    CONSTRUCT_STRUCT_STEPINFO(&gFlowStep, READONLY_RAM),
	CONSTRUCT_SIMPLE_U16(
					&printAllRawADC,
					RAM),
	NULL_T_DATA_OBJ,
	NULL_T_DATA_OBJ,
	//45
	CONSTRUCT_SIMPLE_U16(
						 &fakePara,
						 RAM),
	CONSTRUCT_SIMPLE_U16(
						 &motorHighSpeed,
						 NON_VOLATILE),


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
	Get_FlowAct,
	Put_FlowAct,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};


static uint32_t GetDuringMs_ConbineSteps(uint16_t subStep)
{
	uint32_t tick = 0;
    uint16_t step = 0;
	if(IN_RANGE(subStep, e_pre_sub_start, e_pre_sub_end))
	{
		uint16_t idMax = PRESUB_STEP_MAX;
		for(uint16_t i = 0; i<idMax; i++)
		{
            step = preSubSteps[subStep-e_pre_sub_start][i];
			if(step == 0)
				break;
			else
				tick += microSecondsEachStep[step];
		}
	}
    else if(IN_RANGE(subStep, e_sub_step_start, e_sub_step_end))
	{
		uint16_t idMax = SUB_STEP_MAX;
		for(uint16_t i = 0; i<idMax; i++)
		{
            step = subSteps[subStep-e_sub_step_start][i];
			if(step == 0)
				break;
			else
				tick += microSecondsEachStep[step];
		}
	}
    else if(IN_RANGE(subStep, e_pre_main_start, e_pre_main_end))
	{
		uint16_t idMax = PRE_MAIN_STEP_MAX;
		for(uint16_t i = 0; i<idMax; i++)
		{
            step = preMainSteps[subStep-e_pre_main_start][i];
			if(step == 0)
				break;
			else
				tick += microSecondsEachStep[step];
		}
	}
	else if(IN_RANGE(subStep, e_main_step_start, e_main_step_end))
	{
		uint16_t idMax = MAIN_STEP_MAX;
		for(uint16_t i = 0; i<idMax; i++)
		{
            step = mainSteps[subStep-e_main_step_start][i];
			if(step == 0)
				break;
			else
				tick += microSecondsEachStep[step];
		}
	}
	else
	{}
	return tick;
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
            if((fileId1 != fileID_Default) || (fileID_Default != fileId2))
            {
                (void)me->LoadRomDefaults(me,ALL_DATACLASSES);
                TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);
            }
		}
		UpdateTimeCostVal();
        memset(dbgMsgMap, PRINT_MAP_SHELL, sizeof(dbgMsgMap));
	}
	return result;
}


void UpdateTimeCostVal(void)
{
	for(uint16_t i = 1; i <e_micro_step_end;i++)
	{
		microSecondsEachStep[i] = GetDuringMs_MicroStep(i) ;
	}

	for(uint16_t i = e_pre_sub_start; i <e_main_step_end;i++)
	{
		microSecondsEachStep[i] = GetDuringMs_ConbineSteps(i);
	}
}

void FakeParaInit(uint16_t set)
{
    //todo
}
void WakeUpSchedule(void)
{
    //todo
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
            
			if((e_valve_start <=flowStep) && (flowStep <= e_main_step_end))
			{
				if(FlowTskStatus != TASK_IDLE)
				{
					result = WARNING;
				}
				else
				{
                    //todo
                    //SendTskMsg(FLOW_TSK_ID, TSK_INIT, flowStep, NULL);
                    SendTskMsg(MB_MAINSTEP, TSK_INIT, (uint32_t)flowStep, NULL,NULL);
				}
			}
			else
			{
                //todo
			 //	SendTskMsg(FLOW_TSK_ID, TSK_INIT, flowStep, NULL);
                SendTskMsg(MB_MAINSTEP, TSK_INIT, (uint32_t)flowStep, NULL,NULL);
			}
			break;
		case IDX_POLL_ACTION:
			if(ATR_POLL_START == attributeIndex)
			{
				if(pollConfig.start)
				{
                    SendTskMsg(POLL_SCH_ID, TSK_INIT, (uint32_t)&pollConfig, NULL,NULL);
				}
				else
				{
                    SendTskMsg(POLL_SCH_ID, TSK_FORCE_BREAK, (uint32_t)0, NULL,NULL);
                    osDelay(500);
                    SendTskMsg(MB_MAINSTEP, TSK_INIT, 128, NULL,NULL);
                //    SendTskMsg(MB_MAINSTEP, TSK_FORCE_BREAK, (uint32_t)0, NULL,NULL);
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
			UpdateTimeCostVal();
			WakeUpSchedule();

			break;
            
		case IDX_SYSTEM_RST:
			if(systemReset != 0)
			{
				if(systemReset == VALID_PASSWORD_ID)
				{
					validPrintMsg = VALID_RST_MSG;
				}
				//save eep data;
				Trigger_DeviceReset();

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
		uint32_t tickSecond = GetCurrentSeconds();
		for(int idx = 0;idx<ID_MAX_STEP;idx++)
		{
			if(flowStepRun[idx].duringTime)
			{
				if(tickSecond - GetSecondsFromST(flowStepRun[idx].startTime) >= (flowStepRun[idx].duringTime + 1))
				{
					flowStepRun[idx].remainTime = 0;
				}
				else
				{
					flowStepRun[idx].remainTime = (flowStepRun[idx].duringTime - tickSecond + GetSecondsFromST(flowStepRun[idx].startTime));
				}
			}
		}
	}
	else if(OBJ_MAINSTEP_NAME == objectIndex)
	{
		memcpy(mainStepName, GetStepName(gFlowStep.mainStep), sizeof(mainStepName));
	}
	else if(OBJ_SUBSTEP_NAME == objectIndex)
	{
		memcpy(subStepName, GetStepName(gFlowStep.subStep), sizeof(subStepName));
	}
	else if(OBJ_ACTION_NAME == objectIndex)
	{
		memcpy(actionpName, GetStepName(gFlowStep.microStep), sizeof(actionpName));
	}
    
	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);

	return result;
}

uint16_t CalcDuringSecondsStep_WithDelay(uint16_t subStep)
{
	return microSecondsEachStep[subStep]/1000 + TSK_SW_TIME_NEW;
}

uint32_t GetStepsMS_WithDelay(uint16_t subStep)
{
	uint32_t val = microSecondsEachStep[subStep];
	val += TSK_SW_TIME_NEW;
	return val;
}
