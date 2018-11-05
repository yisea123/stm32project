/*
* unit_meas_data.c
*
*  Created on: 2018��2��9��
*      Author: paul
*/

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_flowstep.h"
#include "t_data_obj_pid.h"
#include "t_data_obj_measdata.h"
#include "t_dataclass.h"
#include "main.h"
#include "unit_rtc_cfg.h"
#include "unit_flow_act.h"
#include "unit_meas_cfg.h"

#include "unit_data_log.h"
#include "dev_log_sp.h"
#include "dev_spiflash.h"
#include "dev_logdata.h"
#include "dev_flash_fat.h"
#include "unit_meas_data.h"
#include "unit_sch_cfg.h"
#include "unit_sys_diagnosis.h"
#include "tsk_substep.h"
//! unit global attributes

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;
static const uint8_t fileID_Default = 0x0F;


CalibrationFactor caliFactor[4]					__attribute__ ((section (".configbuf_static")));
uint32_t caliTime								__attribute__ ((section (".configbuf_static")));
uint32_t rev_loc[15]							__attribute__ ((section (".configbuf_static")));


static uint8_t fileId1							__attribute__ ((section (".configbuf_measdata")));
HeatCfg	heatCfg									__attribute__ ((section (".configbuf_measdata")));
uint32_t  ledDetectLimit[ID_LED_POS_MAX]		__attribute__ ((section (".configbuf_measdata")));
PID_VAL   sPID[4]								__attribute__ ((section (".configbuf_measdata")));
HeatCtrlCfg	heatCtrlCfg							__attribute__ ((section (".configbuf_measdata")));
uint8_t refGain									__attribute__ ((section (".configbuf_measdata")));
uint8_t measGain								__attribute__ ((section (".configbuf_measdata")));
static uint8_t fileId2							__attribute__ ((section (".configbuf_measdata")));

volatile uint16_t measDataSaturation	= NONE_SATURATION;
uint16_t advalFinal[ID_ADC_MAX];
uint16_t adValChn[ID_ADC_MAX];
uint16_t tempIdle[3];
uint16_t tempMeas[3];
volatile uint32_t PumpPosStatus[ID_PUMP_MAX] = {0,0};
volatile uint16_t leakageReadback;
volatile uint16_t safetyLockReadback;
volatile uint16_t heatStatusReadback;
volatile uint16_t ceramicReadback;
volatile uint16_t heatStatusReadback;
volatile uint8_t led0DetectLiquid = 0;
volatile uint8_t led1DetectLiquid = 0;
volatile uint8_t chkLeakageCfg = 0;
volatile uint8_t chkSafetyLockCfg = 0;
volatile uint16_t cpuLoad;
uint16_t measuredValue = 0;
uint16_t blankValue = 0;
PID_VAL pidUsed ;
float  realTimeAbs = 0;
float  realTimeConcentration;
float  tmpConcentration;
static const PID_VAL pid_Default[4] =
{
	{3.5f, 0.2f, 0.0f,20,1500,},
	{3.2f, 0.2f, 0.0f,20,1100,},
	{2.7f, 0.1f, 0.0f,20,800,},
	{2.6f, 0.1f, 0.0f,20,800,},
};
static const uint16_t   ledDetectLimit_Default[ID_LED_POS_MAX] = {150,150,150};
static const uint8_t    measGain_Default = 0;
static const uint8_t    refGain_Default = 0;
static const uint16_t   adValChn_Default[ID_ADC_MAX] = {0,0,0,0};
static const HeatCfg	heatCfg_Default =
{
	800,// idleTemp; + hourse temperature
	5500,// preHeatTemp;
    9000,//tempStep2;
	10000,// measureTemp;
	17500,// digTemp;
	9000,// drainTemp;
    10000,//cleanTemp
    500,//tempDecrease
};
static HeatCtrlCfg	heatCtrl_Default =
{
	4000, //	uint16_t cool_delay;
	350, //	uint16_t cool_buffer;
	3000, //	uint16_t heat_delay;
	300, //	uint16_t heat_buffer;
};

static CalibrationFactor caliFactor_Default[4] = {
                                                    {-0.013014f,0.753309f, 0.0f, 1500.0f},
                                                    {-0.013064f,0.752728f, 0.0f, 1500.0f},
													{-0.067848f,0.847026f, 0.0f, 1500.0f},
													{-0.067574f,0.846690f, 0.0f, 1500.0f},};


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
    //0
    CONSTRUCT_ARRAY_SIMPLE_U16(&adValChn[0],		sizeof(adValChn)/sizeof(uint16_t), RAM),
    CONSTRUCT_ARRAY_SIMPLE_U16(&PumpPosStatus[0],	sizeof(PumpPosStatus)/sizeof(uint16_t), RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&heatCfg,	sizeof(heatCfg)/sizeof(uint16_t), NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_U16(&advalFinal,	sizeof(advalFinal)/sizeof(uint16_t), RAM),
	CONSTRUCT_SIMPLE_U16(&led0DetectLiquid,	 RAM),
    //5
	CONSTRUCT_SIMPLE_U16(&led1DetectLiquid,	 RAM),
	CONSTRUCT_SIMPLE_U16(&cpuLoad,	 RAM),
	CONSTRUCT_ARRAY_SIMPLE_U32(&ledDetectLimit,	sizeof(ledDetectLimit)/sizeof(uint16_t), NON_VOLATILE),
	CONSTRUCT_SIMPLE_U16(&measuredValue,	 RAM),
	CONSTRUCT_SIMPLE_U16(&blankValue,	 RAM),
    //10
    CONSTRUCT_STRUCT_PID(&sPID[0],NON_VOLATILE),
    CONSTRUCT_STRUCT_PID(&pidUsed,RAM),
	CONSTRUCT_ARRAY_SIMPLE_U16(&heatCtrlCfg,	sizeof(heatCtrlCfg)/sizeof(uint16_t), NON_VOLATILE),
    CONSTRUCT_STRUCT_PID(&sPID[1],NON_VOLATILE),
    CONSTRUCT_STRUCT_PID(&sPID[2],NON_VOLATILE),
    
    //15
    CONSTRUCT_STRUCT_PID(&sPID[3],			 NON_VOLATILE),
	CONSTRUCT_SIMPLE_FLOAT(&realTimeAbs,	 RAM),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&caliFactor[0], sizeof(caliFactor[0])/sizeof(float),    NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&caliFactor[1], sizeof(caliFactor[1])/sizeof(float),    NON_VOLATILE),
	CONSTRUCT_SIMPLE_FLOAT(&realTimeConcentration, RAM),
    
    //20
	CONSTRUCT_SIMPLE_U8(&refGain,   NON_VOLATILE),
	CONSTRUCT_SIMPLE_U8(&measGain,  NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&caliFactor[2], sizeof(caliFactor[2])/sizeof(float),    NON_VOLATILE),
	CONSTRUCT_ARRAY_SIMPLE_FLOAT(&caliFactor[3], sizeof(caliFactor[3])/sizeof(float),    NON_VOLATILE),
    
};

static const  T_DATACLASS _ClassList[]=
{
    //lint -e545  -e651
    CONSTRUCTOR_DC_DYNAMIC(adValChn,adValChn_Default),
 	//lint -e545
    CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2,fileID_Default),
    //lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(heatCfg,heatCfg_Default),
    //lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(ledDetectLimit,ledDetectLimit_Default),
    
    //lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(sPID,pid_Default),
    //lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(heatCtrlCfg,heatCtrl_Default),
//	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliFactor,caliFactor_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(measGain,measGain_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(refGain,refGain_Default),


};


//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT measData =
{
    (uint8_t*)"measData",
    &_State,
    &_Semaphore,
    
    _ObjList,
    //lint -e{778}
    sizeof(_ObjList)/sizeof(_ObjList[0])-1,
    
    _ClassList,
    sizeof(_ClassList)/sizeof(_ClassList[0]),
    
    Initialize_MeasData, // will be overloaded
    LoadRomDefaults_T_UNIT,
    Get_T_UNIT,
    Put_MeasData,
    Check_T_UNIT,
    GetAttributeDescription_T_UNIT,
    GetObjectDescription_T_UNIT,
    GetObjectName_T_UNIT
};

float CalcConcentraction(float valN, const CalibrationFactor* ptrFactor)
{
    return ((valN - ptrFactor->std0)/(ptrFactor->std1 - ptrFactor->std0) * ptrFactor->kFactor + ptrFactor->offset);
}

uint16_t UpdateGain(uint8_t refVal, uint8_t measVal)
{
    //todo
	return OK;
}

uint16_t Initialize_MeasData(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
    uint16_t result = OK;
    //lint -e{746}
    assert(me==&measData);
    result = Initialize_T_UNIT(me,typeOfStartUp);
    if(result==OK)
    {
        if((typeOfStartUp & INIT_DATA) != 0)
        {
        	//todo?
            if((fileId1 != fileID_Default) || (fileID_Default != fileId2))
            {
                (void)me->LoadRomDefaults(me,ALL_DATACLASSES);
                TraceMsg(TSK_ID_EEP,"%s LoadRomDefaults is called\n",me->t_unit_name);
            }
        }
        
    }
    UpdateGain(refGain, measGain);
    if((realTimeConcentration > 90000) || (realTimeConcentration < -90000))
    {
        realTimeConcentration = 0;
    }
    TimeCfg timeCfg;
    ConvertBack_U32Time( caliTime, &timeCfg);

    if(timeCfg.year < 2018)
    {
    	memcpy((void*)caliFactor, (void*)caliFactor_Default, sizeof(caliFactor));
    	Trigger_EEPSave((void*)caliFactor, sizeof(caliFactor),SYNC_CYCLE);
    }
    return result;
}


uint16_t Put_MeasData(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
                     void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &measData);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
		switch(objectIndex)
		{
        case OBJ_IDX_PIDCFG:
            PIDInit(0, HEAT_NO_ADJUST);
            Position_PID_Init(0);
            break;
        case OBJ_IDX_REF:
        case OBJ_IDX_MEAS:
            
            UpdateGain(refGain, measGain);
            
            break;
            
		default:
			break;
		}
	}
    
	return result;
}

