/*
 * unit_weld_cfg.c
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#include "main.h"
#include "t_unit_head.h"
#include "unit_weld_cfg.h"


//! unit global attributes

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;
static const uint8_t fileID_Default = 0x0F;


CaliPoints currCaliPoint[MAX_CALI_CURR]			__attribute__ ((section (".configbuf_static")));
CaliPoints voltCaliPoint[2]						__attribute__ ((section (".configbuf_static")));
CaliPoints speedCaliPoint[2]					__attribute__ ((section (".configbuf_static")));
uint32_t   speedRation							__attribute__ ((section (".configbuf_static")));
uint32_t   caliTime								__attribute__ ((section (".configbuf_static")));
int32_t motorPosHome							__attribute__ ((section (".configbuf_static")));
uint16_t   currCaliPointNum						__attribute__ ((section (".configbuf_static")));
uint32_t   rev_loc[15]							__attribute__ ((section (".configbuf_static")));


static uint8_t fileId1							__attribute__ ((section (".configbuf_measdata")));
SegWeld segWeld[MAX_SEG_SIZE]					__attribute__ ((section (".configbuf_measdata")));
MotorSpeed motorSpeedSet       					__attribute__ ((section (".configbuf_measdata")));
WeldProcessCfg weldProcess    					__attribute__ ((section (".configbuf_measdata")));
uint16_t segWeldNum 							__attribute__ ((section (".configbuf_measdata")));
static uint8_t fileId2							__attribute__ ((section (".configbuf_measdata")));

uint16_t	weldStatus = 0;

SegWeld tmp_SegWeld;
CaliPoints tmp_CaliPointCurr;
uint16_t clearCurrCali = 0;
static const CaliPoints currCaliPoint_Default[MAX_CALI_CURR] = {
                                                    {10.0f, 3500, 0},
													{30.0f, 10000, 0},
													{50.0f, 16500, 0},
													{70.0f, 23000, 0},
													{90.0f, 29500, 0},
													{110.0f, 36000, 0},
													{130.0f, 42500, 0},
													{150.0f, 49000, 0},
													{170.0f, 55500, 0},
													{190.0f, 62000, 0},
												};
static const CaliPoints voltCaliPoint_Default[2] =
{
		{5.0f,	0x00200000,	0},
		{40.0f,	0x00900000,	0},
};

static const CaliPoints speedCaliPoint_Default[2] =
{
		{0.1f,	1000,	0},
		{5.0f,	50000,	0},
};

static const uint16_t currCaliPointNum_Default = 10;

static const uint32_t speedRation_Default = 78*2*32;
/*
 * 	float weldSpeed;
	float currHigh;
	float currLow;
	uint16_t currHighMs;
	uint16_t currLowMs;
	uint16_t startAng;
	uint16_t endAng;
 */
static const SegWeld segWeld_Default[MAX_SEG_SIZE] = {
		{0.2f, 80.0f, 5.0f, 100, 50, 0, 45,},
		{0.2f, 80.0f, 5.0f, 100, 50, 45, 90,},
		{0.2f, 80.0f, 5.0f, 100, 50, 90, 135,},
		{0.2f, 80.0f, 5.0f, 100, 50, 135, 180,},
		{0.2f, 80.0f, 5.0f, 100, 50, 180, 225,},
		{0.2f, 80.0f, 5.0f, 100, 50, 225, 270,},
		{0.2f, 80.0f, 5.0f, 100, 50, 270, 315,},
		{0.2f, 80.0f, 5.0f, 100, 50, 315, 360,},
};
static const uint16_t segWeldNum_Default = 8;
static const int32_t motorPosHome_Default = 0;
/*
float homeSpeed;
float jogSpeed;
float accSpeedPerSeond;
*/
MotorSpeed motorSpeedSet_Default =
{
		4.0f,2.0f,0.01f,
};
/*
uint16_t preGasTime;
uint16_t postGasTime;
uint16_t preVolt;
uint16_t preDelay;
uint16_t upSlopeAcc;
uint16_t upSlopeAccTime;
uint16_t downSploeAcc;
uint16_t downSlopeAcc;
*/

static const WeldProcessCfg weldProcess_Default =
{
		300,300,600,30,1,1,1,1,
};

static const  T_DATACLASS _ClassList[]=
{
    //lint -e545  -e651
 	//lint -e545
    CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1,fileID_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2,fileID_Default),

	CONSTRUCTOR_DC_STATIC_CONSTDEF(segWeld,segWeld_Default),


//	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliFactor,caliFactor_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(motorSpeedSet,motorSpeedSet_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(weldProcess,weldProcess_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(segWeldNum,segWeldNum_Default),
};


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{

    //0
		CONSTRUCT_STRUCT_CALIPOINT(&voltCaliPoint[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&voltCaliPoint[1], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&speedCaliPoint[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&speedCaliPoint[1], NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&speedRation,	 RAM),

    //5
		CONSTRUCT_SIMPLE_U32(&motorPosHome,	 NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&weldStatus,	 RAM),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
	//10
		CONSTRUCT_STRUCT_CALIPOINT(&tmp_CaliPointCurr,RAM),
		CONSTRUCT_SIMPLE_U16(&currCaliPointNum, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&clearCurrCali,	 RAM),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
	//15
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[1], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[2], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[3], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[4], NON_VOLATILE),
	//20
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[5], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[6], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[7], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[8], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[9], NON_VOLATILE),
	//25
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[10], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[11], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[12], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[13], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[14], NON_VOLATILE),
	//30
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[15], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[16], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[17], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[18], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[19], NON_VOLATILE),
	//35
		CONSTRUCT_SIMPLE_U16(&segWeldNum,	 NON_VOLATILE),

		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&motorSpeedSet, sizeof(motorSpeedSet)/sizeof(float),    NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&weldProcess, sizeof(weldProcess)/sizeof(uint16_t),    NON_VOLATILE),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
	//40
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[1], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[2], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[3], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[4], NON_VOLATILE),


		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[5], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[6], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[7], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[8], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[9], NON_VOLATILE),

		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[10], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[11], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[12], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[13], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[14], NON_VOLATILE),


		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[15], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[16], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[17], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[18], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&currCaliPoint[19], NON_VOLATILE),

};




//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT weldCfg =
{
    (uint8_t*)"weldCfg",
    &_State,
    &_Semaphore,

    _ObjList,
    //lint -e{778}
    sizeof(_ObjList)/sizeof(_ObjList[0])-1,

    _ClassList,
    sizeof(_ClassList)/sizeof(_ClassList[0]),

    Initialize_WeldCfg, // will be overloaded
    LoadRomDefaults_T_UNIT,
    Get_T_UNIT,
    Put_WeldCfg,
    Check_T_UNIT,
    GetAttributeDescription_T_UNIT,
    GetObjectDescription_T_UNIT,
    GetObjectName_T_UNIT
};


uint16_t Initialize_WeldCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
    uint16_t result = OK;
    //lint -e{746}
    assert(me==&weldCfg);
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
    TimeCfg timeCfg;
    ConvertBack_U32Time( caliTime, &timeCfg);

    if(timeCfg.year < 2019)
    {
    	caliTime = GetCurrentST();

    	memcpy((void*)&motorPosHome, (void*)&motorPosHome_Default, sizeof(motorPosHome));
    	memcpy((void*)&segWeld, (void*)&segWeld_Default, sizeof(segWeld));
    	memcpy((void*)&speedRation, (void*)&speedRation_Default, sizeof(speedRation));
    	memcpy((void*)&currCaliPointNum, (void*)&currCaliPointNum_Default, sizeof(currCaliPointNum));
    	memcpy((void*)&speedCaliPoint[0], (void*)&speedCaliPoint_Default[0], sizeof(speedCaliPoint));
    	memcpy((void*)&voltCaliPoint[0], (void*)&voltCaliPoint_Default[0], sizeof(voltCaliPoint));
    	memcpy((void*)&currCaliPoint[0], (void*)&currCaliPoint_Default[0], sizeof(currCaliPoint));
    	Trigger_EEPSave((void*)motorPosHome, sizeof(motorPosHome),SYNC_CYCLE);
    	Trigger_EEPSave((void*)&segWeld[0], sizeof(segWeld),SYNC_CYCLE);
    	Trigger_EEPSave((void*)speedRation, sizeof(speedRation),SYNC_CYCLE);
    	Trigger_EEPSave((void*)currCaliPointNum, sizeof(currCaliPointNum),SYNC_CYCLE);
    	Trigger_EEPSave((void*)&speedCaliPoint[0], sizeof(speedCaliPoint),SYNC_CYCLE);
       	Trigger_EEPSave((void*)&voltCaliPoint[0], sizeof(voltCaliPoint),SYNC_CYCLE);
       	Trigger_EEPSave((void*)&currCaliPoint[0], sizeof(currCaliPoint),SYNC_CYCLE);
       	Trigger_EEPSave((void*)&caliTime, sizeof(caliTime),SYNC_IM);

    }
    return result;
}


uint16_t Put_WeldCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
                     void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &weldCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{
		switch(objectIndex)
		{
		default:
			break;
		}
	}

	return result;
}

