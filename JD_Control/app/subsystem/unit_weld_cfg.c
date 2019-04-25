/*
 * unit_weld_cfg.c
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#include "main.h"
#include "t_unit_head.h"
#include "unit_weld_cfg.h"
#include "unit_rtc_cfg.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_encoder.h"
#include "string.h"
//! unit global attributes

static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;
#define FILEID			0x19041819
static const uint32_t fileId1_Default = FILEID;
static const uint32_t fileId2_Default = ~FILEID;


static CaliCurrent	currCaliPoint[MAX_CALI_CURR]		__attribute__ ((section (".configbuf_static")));
CaliVolt 	voltCaliPoint[2]							__attribute__ ((section (".configbuf_static")));
CaliSpeed 	speedCaliPoint[2]							__attribute__ ((section (".configbuf_static")));
//	cnter per 1/360
uint32_t   	caliTime									__attribute__ ((section (".configbuf_static")));
int32_t 	motorPosHome								__attribute__ ((section (".configbuf_static")));
uint16_t   	currCaliPointNum							__attribute__ ((section (".configbuf_static")));
uint32_t   	rev_loc[15]									__attribute__ ((section (".configbuf_static")));
float		currConvertRation							__attribute__ ((section (".configbuf_static")));

static uint32_t fileId1							__attribute__ ((section (".configbuf_measdata")));
static SegWeld segWeld[MAX_SEG_SIZE]			__attribute__ ((section (".configbuf_measdata")));
MotorSpeed motorSpeedSet       					__attribute__ ((section (".configbuf_measdata")));
WeldProcessCfg weldProcess    					__attribute__ ((section (".configbuf_measdata")));
static uint32_t fileId2							__attribute__ ((section (".configbuf_measdata")));
float gearRation					 			__attribute__ ((section (".configbuf_measdata")));
uint16_t pulsePerLap				 			__attribute__ ((section (".configbuf_measdata")));
uint16_t segNum_weld				 			__attribute__ ((section (".configbuf_measdata")));
static uint32_t fileId2							__attribute__ ((section (".configbuf_measdata")));

static SegWeld segWeldTmp;
uint32_t    weldUsedCnt	= 1;
float   	ang2CntRation = 1;
uint16_t    gasRemainTime = 0;
uint16_t    motorHomeSet = 0;
uint16_t   	devLock  = 10;
uint16_t 	weldDir = MOTOR_DIR_CW;
int32_t 	lastMotorPos_PowerDown = 0;
uint16_t    daOutputRawDA[CHN_DA_MAX];
float  		daOutputSet[2];
uint32_t  digitOutput;
uint32_t  digitInput;
uint32_t  digitInputWeldBtn;
float	  currOutputPwmFloat[2];
float     actWelCurrUsed[2];
float     actWelSpeedUsed;
uint16_t  currOutputPwmTime[2];

float 	  actReqSpeed = 0.0f;
float     actReqCurr = 0.0f;

volatile  int32_t   motorPos_Read;
float     motorAng_Read[2];
float 	  motorSpeed_Read;
float     weldCurr_Read;
float 	  weldVolt_Read;
float 	  adcValue_Read[CHN_AD_MAX];
int32_t   motorPos_WeldStart;
int32_t   motorPos_WeldFinish;


uint16_t  weldState = 0;
uint16_t 	password;
extern uint16_t 	EEP_INIT_CRC;

/*
 * UI
 */
float speedAdjust = 1.0f;
float currMicroAdjust = 1.0f;
uint16_t uiWeldSeg = 0;

uint16_t uiBtn_Home = 0;
uint16_t uiBtn_Weld = 0;
uint16_t uiBtn_Cali = 0;
uint16_t uiBtn_JogP = 0;
uint16_t uiBtn_JogN = 0;
uint16_t voltCaliReq = 0;
uint16_t currCaliReq = 0;
uint16_t speedCaliReq = 0;
uint16_t caliAllReq = 0;
/*


*/

float currCaliSetDiff = 0.6f;
uint16_t currCaliPreGas = 10;
/*
 *
 *
 * Global variable
 */
static CaliCurrent tmp_CaliPointCurr;
static uint16_t clearCurrCali = 0;
const SegWeld* ptrCurrWeldSeg = &segWeld[0];
static uint32_t segWeldCnt[MAX_SEG_SIZE] = {0,0,0};
static const float currConvertRation_Default = 20.0f;
static uint16_t segWeldClr = 20;
static const uint16_t segNum_weld_Default = 0;
static const CaliCurrent currCaliPoint_Default[MAX_CALI_CURR] = {
                                                    {1.0f, 	20.0f, 0},
													{2.0f, 	40.0f, 0},
													{3.0f, 	60.0f, 0},
													{4.0f, 	80.0f, 0},
													{5.0f, 	100.0f, 0},
													{6.0f, 	120.0f, 0},
													{7.0f, 	140.0f, 0},
													{8.0f, 	160.0f, 0},
													{9.0f, 	180.0f, 0},
													{10.0f, 200.0f, 0},
												};
static const float gearRation_Default = 344.0f;
static const uint16_t pulsePerLap_Default = 32;
static CaliCurrent	currCaliPointUsed[MAX_CALI_CURR+2];
static const CaliVolt voltCaliPoint_Default[2] =
{
		{0.36f,	1.8f,	0},
		{3.6f,	18.0f,	0},
};

static const CaliSpeed speedCaliPoint_Default[2] =
{
		{1.0f,	1.2f,	0},
		{8.0f,	9.6f,	0},
};



static const uint16_t currCaliPointNum_Default = 10;

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
		{0.2f, 80.0f, 5.0f, 100, 50, 0, 45, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 45, 90, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 90, 135, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 135, 180, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 180, 225, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 225, 270, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 270, 315, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 315, 360, 1},
		{0.2f, 80.0f, 5.0f, 100, 50, 315, 360, 0},
};

static const int32_t motorPosHome_Default = 0;
/*
float homeSpeed;
float jogSpeed;
float accSpeedPerSeond;
*/
MotorSpeed motorSpeedSet_Default =
{
	8.0f,4.0f,0.5f,
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
    CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId1,fileId1_Default),
	//lint -e545
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileId2,fileId2_Default),

	CONSTRUCTOR_DC_STATIC_CONSTDEF(segWeld,segWeld_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(currConvertRation,currConvertRation_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(gearRation,gearRation_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pulsePerLap,pulsePerLap_Default),


//	CONSTRUCTOR_DC_STATIC_CONSTDEF(caliFactor,caliFactor_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(motorSpeedSet,motorSpeedSet_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(weldProcess,weldProcess_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(currCaliPoint,currCaliPoint_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(voltCaliPoint,voltCaliPoint_Default),
	CONSTRUCTOR_DC_STATIC_CONSTDEF(segNum_weld,segNum_weld_Default),


};


//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{

	// 0 test interfaces:
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&daOutputSet[0],sizeof(daOutputSet)/sizeof(float),	 		RAM),
		CONSTRUCT_SIMPLE_U32(&digitOutput,	 RAM),
		CONSTRUCT_SIMPLE_U32(&digitInputWeldBtn,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U32(&digitInput,	 READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U16(&daOutputRawDA[0],sizeof(daOutputRawDA)/sizeof(uint16_t),	 READONLY_RAM),
		//5
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&currOutputPwmFloat[0],sizeof(currOutputPwmFloat)/sizeof(float),	 READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U16(&currOutputPwmTime[0],sizeof(currOutputPwmTime)/sizeof(uint16_t),	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&password,	 RAM),
		CONSTRUCT_SIMPLE_U16(&EEP_INIT_CRC,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&devLock,	 RAM),

    //10
		CONSTRUCT_STRUCT_CALIPOINT(&voltCaliPoint[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&voltCaliPoint[1], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&speedCaliPoint[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_CALIPOINT(&speedCaliPoint[1], NON_VOLATILE),
		CONSTRUCT_SIMPLE_FLOAT(&ang2CntRation,	 READONLY_RAM),

    //15
		CONSTRUCT_SIMPLE_U32(&motorPosHome,	 NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&weldState,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&weldUsedCnt,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&motorHomeSet,	 RAM),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&motorAng_Read[0],sizeof(motorAng_Read)/sizeof(float),	 READONLY_RAM),
	//20
		CONSTRUCT_STRUCT_CALIPOINT(&tmp_CaliPointCurr,RAM),
		CONSTRUCT_SIMPLE_U16(&currCaliPointNum, NON_VOLATILE),
		CONSTRUCT_SIMPLE_U16(&clearCurrCali,	 RAM),
		CONSTRUCT_SIMPLE_U16(&pulsePerLap,	 NON_VOLATILE),
		CONSTRUCT_SIMPLE_FLOAT(&gearRation,	 NON_VOLATILE),
	//25
		CONSTRUCT_SIMPLE_FLOAT(&speedAdjust,	 RAM),
		CONSTRUCT_SIMPLE_FLOAT(&currMicroAdjust,	 RAM),
		CONSTRUCT_SIMPLE_U16(&uiBtn_Weld,	 RAM),
		CONSTRUCT_SIMPLE_U16(&uiBtn_Cali,	 RAM),
		CONSTRUCT_SIMPLE_U16(&uiBtn_JogP,	 RAM),

	//30
		CONSTRUCT_SIMPLE_U16(&uiBtn_JogN,	 RAM),
		CONSTRUCT_SIMPLE_U16(&voltCaliReq,	 RAM),
		CONSTRUCT_SIMPLE_U16(&currCaliReq,	 RAM),
		CONSTRUCT_SIMPLE_U16(&speedCaliReq,	 RAM),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&actWelCurrUsed,sizeof(actWelCurrUsed)/sizeof(float),	 READONLY_RAM),
	//35
		CONSTRUCT_SIMPLE_U16(&caliAllReq,	 RAM),
		CONSTRUCT_SIMPLE_U16(&lastMotorPos_PowerDown,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_FLOAT(&currCaliSetDiff,	 RAM),
		CONSTRUCT_SIMPLE_U16(&currCaliPreGas,	 RAM),
		CONSTRUCT_SIMPLE_FLOAT(&actWelSpeedUsed,	 READONLY_RAM),
		//40
		CONSTRUCT_SIMPLE_U16(&uiBtn_Home,	 RAM),
		CONSTRUCT_SIMPLE_U16(&uiWeldSeg,	 RAM),
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,
		NULL_T_DATA_OBJ,

		//45
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&adcValue_Read[0],sizeof(adcValue_Read)/sizeof(float),	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U32(&motorPos_Read,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_FLOAT(&motorSpeed_Read, READONLY_RAM),
		CONSTRUCT_SIMPLE_FLOAT(&weldCurr_Read,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_FLOAT(&weldVolt_Read,	 READONLY_RAM),

		//50
		CONSTRUCT_SIMPLE_I32(&motorPos_WeldStart,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_I32(&motorPos_WeldFinish,	 READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&gasRemainTime,	 READONLY_RAM),
		NULL_T_DATA_OBJ,
		CONSTRUCT_SIMPLE_U16(&segNum_weld,  NON_VOLATILE),
		//55
		CONSTRUCT_SIMPLE_U16(&segWeldClr,	 RAM),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&motorSpeedSet, sizeof(motorSpeedSet)/sizeof(float),    NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&weldProcess, sizeof(weldProcess)/sizeof(uint16_t),    NON_VOLATILE),
		CONSTRUCT_SIMPLE_FLOAT(&currConvertRation,    NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeldTmp,	 RAM),
	//25
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[0], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[1], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[2], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[3], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[4], NON_VOLATILE),
	//30
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[5], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[6], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[7], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[8], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[9], NON_VOLATILE),
	//35
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[10], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[11], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[12], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[13], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[14], NON_VOLATILE),
	//40
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[15], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[16], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[17], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[18], NON_VOLATILE),
		CONSTRUCT_STRUCT_SEGWELD(&segWeld[19], NON_VOLATILE),

	//50
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
	Get_WeldCfg,
    Put_WeldCfg,
    Check_T_UNIT,
    GetAttributeDescription_T_UNIT,
    GetObjectDescription_T_UNIT,
    GetObjectName_T_UNIT
};

uint16_t GetInputState(uint16_t chn)
{
	assert(chn < CHN_IN_MAX);
	return (uint16_t)(digitInput & (uint32_t)(1<<chn));
}

static void SortCurrentCali(void)
{
	CaliCurrent tmp;
	currCaliPointUsed[0].actCurrent = 0;
	currCaliPointUsed[0].outValue = 0;
	currCaliPointUsed[0].caliFlag = 0;
	currCaliPointUsed[MAX_CALI_CURR+1].actCurrent = MAX_CURRENT_OUTPUT;
	currCaliPointUsed[MAX_CALI_CURR+1].outValue = 12.0f;
	currCaliPointUsed[MAX_CALI_CURR+1].caliFlag = 0;
	memcpy((void*)&currCaliPointUsed[1],(void*)&currCaliPoint[0],sizeof(currCaliPoint));

	for(uint16_t i=1;i<MAX_CALI_CURR+1;i++)
	{
		for(uint16_t j=i;j<MAX_CALI_CURR+1;j++)
		{
			if(currCaliPointUsed[j].actCurrent <= currCaliPointUsed[i].actCurrent)
			{
				tmp = currCaliPointUsed[j];
				currCaliPointUsed[j] = currCaliPointUsed[i];
				currCaliPointUsed[i] = tmp;
			}
		}
	}
}

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
            if((fileId1 != fileId1_Default) || (fileId2_Default != fileId2))
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

    	memcpy((void*)&currCaliPointNum, (void*)&currCaliPointNum_Default, sizeof(currCaliPointNum));
    	memcpy((void*)&speedCaliPoint[0], (void*)&speedCaliPoint_Default[0], sizeof(speedCaliPoint));
    	memcpy((void*)&voltCaliPoint[0], (void*)&voltCaliPoint_Default[0], sizeof(voltCaliPoint));
    	memcpy((void*)&currCaliPoint[0], (void*)&currCaliPoint_Default[0], sizeof(currCaliPoint));

    	Trigger_EEPSave((void*)motorPosHome, sizeof(motorPosHome),SYNC_CYCLE);
    	Trigger_EEPSave((void*)&segWeld[0], sizeof(segWeld),SYNC_CYCLE);
    	Trigger_EEPSave((void*)&currCaliPointNum, sizeof(currCaliPointNum),SYNC_CYCLE);
    	Trigger_EEPSave((void*)&speedCaliPoint[0], sizeof(speedCaliPoint),SYNC_CYCLE);
       	Trigger_EEPSave((void*)&voltCaliPoint[0], sizeof(voltCaliPoint),SYNC_CYCLE);
       	Trigger_EEPSave((void*)&currCaliPoint[0], sizeof(currCaliPoint),SYNC_CYCLE);
       	Trigger_EEPSave((void*)&caliTime, sizeof(caliTime),SYNC_IM);


    }
    SortCurrentCali();
    ang2CntRation = gearRation * pulsePerLap /360.0f;

    return result;
}



void UpdateWeldFInishPos(void)
{
	for(uint16_t i= 0;i<MAX_SEG_SIZE;i++)
	{
		if(segWeld[i].endAng > 0.0f)
			segWeldCnt[i] = (uint32_t)(segWeld[i].endAng * ang2CntRation);
		else
			segWeldCnt[i] = 0;
	}
}



uint32_t GetWeldFinishPos(uint16_t id)
{
	uint32_t pos = 0;
	if(id < MAX_SEG_SIZE)
	{
		if(segWeld[id].endAng > 0.0f)
			pos = (uint32_t)(segWeld[id].endAng * ang2CntRation);

	}
	else
	{
		for(uint16_t i= 0;i<MAX_SEG_SIZE;i++)
		{
			if(segWeld[i].endAng > 0.0f)
				pos = (uint32_t)(segWeld[i].endAng * ang2CntRation);
			else
				break;
		}
	}
	return pos;
}

uint16_t Get_WeldCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
                     void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &weldCfg);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);

	switch(objectIndex)
	{
	case OBJ_IDX_MOTOR_POSREAD:
		motorAng_Read[0] = (motorPos_Read - motorPosHome) / ang2CntRation;
		if(weldState != ST_WELD_IDLE)
			motorAng_Read[1] = (motorPos_Read - motorPos_WeldStart) / ang2CntRation;
		else
			motorAng_Read[1] = motorAng_Read[0];
		break;
	}
	return Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
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
		case OBJ_IDX_SEG_CLR:
			if(segWeldClr != 0)
			{
				segNum_weld = 0;
				segWeldTmp.state = 0;
				memset((void*)&segWeld[0],0, sizeof(segWeld));
				Trigger_EEPSave((void*)&segWeld[0], sizeof(segWeld),SYNC_CYCLE);
				Trigger_EEPSave((void*)&segNum_weld,sizeof(segNum_weld),SYNC_IM);
			}
			break;
		case OBJ_IDX_SEG_NEWSEG:
			if(attributeIndex == 7)
			{
				if(segWeldTmp.state != 0)
				{
					if(segNum_weld < MAX_SEG_SIZE)
					{
						memcpy(&segWeld[segNum_weld],&segWeldTmp, sizeof(segWeldTmp));
						segNum_weld = segNum_weld + 1;

						Trigger_EEPSave((void*)&segWeld[0], sizeof(segWeld),SYNC_CYCLE);
						Trigger_EEPSave((void*)&segNum_weld,sizeof(segNum_weld),SYNC_CYCLE);
						segWeldTmp.state = 0;
					}
					else
					{
						result = RANGE_TOO_HIGH_ERR;
					}
				}
			}

			break;
		case OBJ_IDX_SPEED_GEAR:
		case OBJ_IDX_SPEED_PULSE_LAP:

			ang2CntRation = gearRation * pulsePerLap /360.0f;
			break;
		case OBJ_IDX_MOTOR_HOME:
			if(weldState == ST_WELD_IDLE)
			{
				if(uiBtn_Home != 0)
				{
					uiBtn_Home = 0;
					SendTskMsg(MOTOR_CTRL,TSK_INIT, ST_MOTOR_HOME, NULL, NULL);
				}
			}
			break;
		case OBJ_IDX_OUTPUTDA:
			SetCurrOutVolt(daOutputSet[CHN_DA_CURR_OUT]);
			SetSpeedOutVolt(daOutputSet[CHN_DA_SPEED_OUT]);
		break;
		case OBJ_IDX_OUTPUTDO:
			SigPush(outputTaskHandle, (DA_OUT_REFRESH_SPEED|DA_OUT_REFRESH_CURR|DO_OUT_REFRESH));
			break;
		case OBJ_IDX_DEV_LOCK:
			if(password == 6000)
			{
				if(devLock == (3+ (~EEP_INIT_CRC)))
				{
					ResetNVData();
					devLock = 0;
				}
			}
			break;
		case OBJ_IDX_VOLT0_CALI:
			voltCaliPoint[0].adValue = adcValue_Read[CHN_VOLT_READ];
			voltCaliPoint[0].caliFlag = 0x33;
			Trigger_EEPSave((void*)&voltCaliPoint[0],sizeof(voltCaliPoint[0]), SYNC_CYCLE);
			break;
		case OBJ_IDX_VOLT1_CALI:
			voltCaliPoint[1].adValue = adcValue_Read[CHN_VOLT_READ];
			voltCaliPoint[1].caliFlag = 0x33;
			Trigger_EEPSave((void*)&voltCaliPoint[1],sizeof(voltCaliPoint[1]), SYNC_IM);
			break;
		case OBJ_IDX_MOTOR_HOMESET:
			motorPosHome = motorPos_Read;
			Trigger_EEPSave((void*)&motorPosHome,sizeof(motorPosHome),SYNC_IM);
			break;
		case OBJ_IDX_SPEED_RATION:
			UpdateWeldFInishPos();
			break;
		case OBJ_IDX_CURR_CALI_NEW:
			if(attributeIndex == 2)
			{
				if(tmp_CaliPointCurr.caliFlag == 0x33)
				{
					if(currCaliPointNum >= MAX_CALI_CURR)
					{
						result = RULE_VIOLATION_ERR;
					}
					else
					{
						currCaliPoint[currCaliPointNum] = tmp_CaliPointCurr;

						currCaliPointNum++;
						Trigger_EEPSave((void*)&currCaliPointNum, sizeof(currCaliPointNum),SYNC_CYCLE);
						Trigger_EEPSave((void*)&currCaliPoint[0], sizeof(currCaliPoint),SYNC_CYCLE);
						SortCurrentCali();
					}
				}
			}
			break;
		case OBJ_IDX_CURR_CALI_CLR:
			if(clearCurrCali != 0)
			{
				clearCurrCali = 0;
				currCaliPointNum = 0;
				memcpy((void*)&currCaliPoint[0],(void*)&currCaliPoint_Default,sizeof(currCaliPoint));
				Trigger_EEPSave((void*)&currCaliPointNum, sizeof(currCaliPointNum),SYNC_CYCLE);
				Trigger_EEPSave((void*)&currCaliPoint[0], sizeof(currCaliPoint),SYNC_CYCLE);
				SortCurrentCali();
			}
			break;

		default:
			break;
		}
	}

	return result;
}




float GetWeldSegSpeed(uint16_t id)
{
	float speed = 0;
	if(id < MAX_SEG_SIZE)
	{
		if(segWeld[id].state != 0)
			speed = segWeld[id].weldSpeed;
	}
	return speed;
}

uint32_t GetTickDuring(uint32_t start)
{
	uint32_t curr = HAL_GetTick();
	if(curr > start)
	{
		curr = curr - start;
	}
	else
	{
		curr += ((0xFFFFFFFF-start)+1);

	}
	return curr;
}
const SegWeld* GetWeldSeg(int32_t cnt)
{
	static const SegWeld segWeld_C = {0,0,0,0,0,0,0,0};
	int32_t pos = cnt;
	if(cnt < 0)
		pos = -cnt;
	const SegWeld* ptrSeg = &segWeld_C;
	uint16_t uiWeldSegLoc = 0;
	for(uint16_t i= 0;i<MAX_SEG_SIZE;i++)
	{
		if(segWeld[i].endAng > 0.0f)
		{
			if(segWeldCnt[i] > (uint32_t)pos)
			{
				ptrSeg = &segWeld[i];
				uiWeldSegLoc = i;
			}
		}
		else
			break;
	}
	uiWeldSeg = uiWeldSegLoc;
	return ptrSeg;
}

float GetSpeedCtrlOutput(float speed)
{
	//todo
	float outVal = (speed - speedCaliPoint[0].actSpeed)/(speedCaliPoint[1].actSpeed - speedCaliPoint[0].actSpeed)*\
			(float)(speedCaliPoint[1].outValue - speedCaliPoint[0].outValue) + speedCaliPoint[0].outValue ;

	if(outVal < 0)
		outVal = 0;

	return outVal;
}

float GetCurrCtrlOutput(float curr)
{
	//todo
	float outVal = 0;
	for(uint16_t i=0; i<MAX_CALI_CURR-1;i++)
	{
		if((currCaliPointUsed[i].actCurrent <=curr) && (currCaliPointUsed[i+1].actCurrent >=curr) )
		{
			outVal = (curr - currCaliPointUsed[i].actCurrent)/(currCaliPointUsed[i+1].actCurrent - currCaliPointUsed[i].actCurrent) * \
					(currCaliPointUsed[i+1].outValue - currCaliPointUsed[i].outValue) + currCaliPointUsed[i].outValue;
		}
	}

	return outVal;
}

float GetVoltRead(float curr)
{
	float outVal = (curr - voltCaliPoint[0].adValue)/(voltCaliPoint[1].adValue - voltCaliPoint[0].adValue)*\
			(voltCaliPoint[1].actRead - voltCaliPoint[0].actRead) + voltCaliPoint[0].actRead;
	if(outVal <= 0.0f)
		outVal = 0.0f;
	return outVal;

}


float GetCurrRead(float curr)
{
	float outVal = curr*currConvertRation;
	if(outVal <= 0.0f)
		outVal = 0.0f;
	return outVal;

}
