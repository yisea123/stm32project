/*
 * unit_statistics_data.c
 *
 *  Created on: 2016��9��6��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"

#include "t_dataclass.h"
#include "main.h"
#include "tsk_substep.h"
#include "unit_flow_act.h"
#include "unit_flow_cfg.h"

#include "unit_statistics_data.h"
#include "unit_rtc_cfg.h"
#include "dev_log_sp.h"

#include "dev_eep.h"
#define FILE_ID			0x18042514
//-------------------------------------------------------------------------------------------------
//! unit global attributes
static uint16_t _State;     // Subsystem state
static OS_RSEMA _Semaphore;
static uint32_t fileID1 									__attribute__ ((section (".configbuf_StaData")));

volatile uint32_t staDataValve[VALVE_PINS_NUM]					__attribute__ ((section (".configbuf_StaData")));
volatile uint64_t staDataMotorSteps[IDX_MOTOR_MAX] 				__attribute__ ((section (".configbuf_StaData")));
volatile uint32_t lifeSpan_RuningSeconds[LIFESPAN_W_MAX_ID]		__attribute__ ((section (".configbuf_StaData")));
volatile uint32_t lifeSpanInstallTime_ST[LIFESPAN_W_MAX_ID]		__attribute__ ((section (".configbuf_StaData")));
volatile uint32_t lifeSpan_StaticStart[LIFESPAN_W_TUBING_MAX]	__attribute__ ((section (".configbuf_StaData")));
volatile float		fluidVolume_Remain[VOL_MAX_ID]				__attribute__ ((section (".configbuf_StaData")));
volatile PumpValve 	pumpValve[VOL_MAX_ID]						__attribute__ ((section (".configbuf_StaData")));
volatile float		usedVolumePPS[VOL_MAX_ID]					__attribute__ ((section (".configbuf_StaData")));
volatile PowerOnOff powerOnOff_ST								__attribute__ ((section (".configbuf_StaData")));
volatile STA_LifeTime lifeTime_STA 								__attribute__ ((section (".configbuf_StaData")));

static uint32_t fileID2 									__attribute__ ((section (".configbuf_StaData")));

volatile uint32_t lifeSpan_InstalledSeconds[LIFESPAN_W_MAX_ID];

volatile uint32_t lifeSpanTubing_StaticSeconds[LIFESPAN_W_TUBING_MAX] = {0,0,0};
volatile uint32_t communicationStatus[COM_MAX_COUNT] = {0,0,0,0,};
volatile uint32_t communicationHappenST[COM_MAX_COUNT] = {0,0,0,0};
volatile uint32_t	freeRtosTskTick[MAX_TASK_ID] = {0,0,0,0,0,0,};
float		fluidVolume_RemainPercentage[VOL_MAX_ID];

static uint16_t resetTublingLifetime = 0;
static uint16_t resetMotorLifetime = 0;
static uint16_t resetFactory = 0;

typedef struct
{
	float powerOffFactor;
	float powerOnFactor;
	float runFactor;
}LifeSpanFactor;

static const LifeSpanFactor lifeSpanFactor[LIFESPAN_W_MAX_ID] =
{
		{0.0f,1.0f,0.0f},
		{0.0f,1.0f,0.0f},
		{0.0f,1.0f,0.0f},
		{0.0f,1.0f,0.0f},

		{0.0f,1.0f,0.0f},
		{0.0f,0.2f,0.8f},
		{0.0f,0.2f,0.8f},
		{0.0f,0.2f,0.8f},

};

static const uint32_t lifeSpan_StaticStart_Default[LIFESPAN_W_TUBING_MAX] =
{
		0,0,0,0
};
static const uint32_t fileID_Default = FILE_ID;

static const PowerOnOff powerOnOff_ST_Default =
{
	0,0
};

static const STA_LifeTime lifeTime_STA_Default =
{
	0,0,
};

static const uint32_t lifeSpanInstallTime_ST_Default[LIFESPAN_W_MAX_ID] =
{
		0,0,0,0,0,0,0,0
};

static const uint64_t staDataMotorSteps_Default[IDX_MOTOR_MAX] =
{
	0,
};
static const uint32_t staDataValve_Default[VALVE_PINS_NUM] =
{
	0,
};
static const uint32_t lifeSpan_RuningSeconds_Default[LIFESPAN_W_MAX_ID] =
{
	0,
};

static const PumpValve 	pumpValve_Default[VOL_MAX_ID] =
{
		{IDX_PUMP2,0xFFFF},//rg_a
		{IDX_PUMP2,0xFFFF},//rg_b
		{IDX_PUMP2,0xFFFF},//rg_c
		{IDX_PUMP1,(VALVE0 | VALVE2 | VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE0)},//st0
		{IDX_PUMP1,(VALVE0 | VALVE0 | VALVE3 | VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE0)},//std1
		{IDX_PUMP1,(VALVE0 | VALVE0 | VALVE0 | VALVE4 | VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE0)},//std2
		{IDX_PUMP1,(VALVE0 | VALVE0 | VALVE0 | VALVE0 | VALVE5 | VALVE0 | VALVE0 | VALVE0 | VALVE0)},//clean
};

static const float 	usedVolumePPS_Default[VOL_MAX_ID] =
{
		0.0003850f,	0.0003848f,	0.0003887f,
		0.0004117f,  0.0004117f,  0.0004117f,
		0.0004f,
};



static const  T_DATACLASS _ClassList[]=
{
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(staDataValve,staDataValve_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(staDataMotorSteps,staDataMotorSteps_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(lifeSpan_RuningSeconds,lifeSpan_RuningSeconds_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID1,fileID_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fileID2,fileID_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(pumpValve,pumpValve_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(usedVolumePPS,usedVolumePPS_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(lifeSpanInstallTime_ST,lifeSpanInstallTime_ST_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(powerOnOff_ST,powerOnOff_ST_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(lifeTime_STA,lifeTime_STA_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(lifeSpan_StaticStart,lifeSpan_StaticStart_Default),
	//lint -e545 -e651
	CONSTRUCTOR_DC_STATIC_CONSTDEF(fluidVolume_Remain,fluidVolume_Set_Default),


};




//-------------------------------------------------------------------------------------------------
//! objectList
static const T_DATA_OBJ _ObjList[] =
{
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&fluidVolume_RemainPercentage[0],sizeof(fluidVolume_RemainPercentage)/sizeof(float), READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&resetTublingLifetime,RAM),
		CONSTRUCT_SIMPLE_U16(&resetFactory,RAM),
		CONSTRUCT_ARRAY_SIMPLE_U32(&lifeSpanTubing_StaticSeconds[0],sizeof(lifeSpanTubing_StaticSeconds)/sizeof(uint32_t), READONLY_RAM),
		CONSTRUCT_SIMPLE_U16(&resetMotorLifetime,RAM),

//5
		CONSTRUCT_ARRAY_SIMPLE_U32(&lifeSpan_InstalledSeconds[0],sizeof(lifeSpan_InstalledSeconds)/sizeof(uint32_t), READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U32(&lifeSpan_StaticStart[0],sizeof(lifeSpan_StaticStart)/sizeof(uint32_t), NON_VOLATILE),
		//this object saved in
		NULL_T_DATA_OBJ,//through saved in NV, but not accessed by UI,
		CONSTRUCT_ARRAY_SIMPLE_T32(&powerOnOff_ST,sizeof(powerOnOff_ST)/sizeof(uint32_t), READONLY_NOV),//through saved in NV, but not accessed by UI
		CONSTRUCT_ARRAY_SIMPLE_U32(&lifeTime_STA,sizeof(lifeTime_STA)/sizeof(uint32_t), NON_VOLATILE),//through saved in NV, but not accessed by UI
//10
		//used volume each pps
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&usedVolumePPS[0],sizeof(usedVolumePPS)/sizeof(uint32_t), NON_VOLATILE),
		//
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_RG_A],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_RG_B],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_RG_C],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_STD_0],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),

//15
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_STD_1],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_STD_2],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U16(&pumpValve[VOL_CLEAN],sizeof(PumpValve)/sizeof(uint16_t), NON_VOLATILE),

		CONSTRUCT_ARRAY_SIMPLE_T32(&lifeSpanInstallTime_ST[0],sizeof(lifeSpanInstallTime_ST)/sizeof(uint32_t), NON_VOLATILE),
		//unit: second
		CONSTRUCT_ARRAY_SIMPLE_U32(&lifeSpan_RuningSeconds[0],sizeof(lifeSpan_RuningSeconds)/sizeof(uint32_t),READONLY_NOV),
		//unit:mL
//20
		CONSTRUCT_ARRAY_SIMPLE_FLOAT(&fluidVolume_Remain[0],sizeof(fluidVolume_Remain)/sizeof(float), NON_VOLATILE),

		CONSTRUCT_ARRAY_SIMPLE_U32(&staDataValve[0],VALVE_PINS_NUM,NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U64(&staDataMotorSteps[0],IDX_MOTOR_MAX,NON_VOLATILE),
		CONSTRUCT_ARRAY_SIMPLE_U32(&communicationStatus[0],sizeof(communicationStatus)/sizeof(uint32_t),READONLY_RAM),
		CONSTRUCT_ARRAY_SIMPLE_U32(&freeRtosTskTick[0],sizeof(freeRtosTskTick)/sizeof(uint32_t),READONLY_RAM),

//25
		CONSTRUCT_ARRAY_SIMPLE_T32(&communicationHappenST[0],sizeof(communicationHappenST)/sizeof(uint32_t),READONLY_RAM),

};


//-------------------------------------------------------------------------------------------------
//! define the T_UNIT-Class object
const T_UNIT STA_Data =
{
	(uint8_t*)"STA_Data",
	&_State,
	&_Semaphore,

	_ObjList,
	//lint -e{778}
	sizeof(_ObjList)/sizeof(_ObjList[0])-1,

	_ClassList,
	sizeof(_ClassList)/sizeof(_ClassList[0]),

	Initialize_STA_Data, // will be overloaded
	LoadRomDefaults_STA_Data,
	Get_STA_Data,
	Put_STA_Data,
	Check_T_UNIT,
	GetAttributeDescription_T_UNIT,
	GetObjectDescription_T_UNIT,
	GetObjectName_T_UNIT
};

static void STA_Init(void)
{

	uint32_t stTime = GetCurrentST();
	powerOnOff_ST.powerOnST = stTime;
	powerOnOff_ST.lastPowerDownST = stTime;

	uint32_t currTime = GetCurrentSeconds();
	uint32_t lastTime = GetSecondsFromST(powerOnOff_ST.lastPowerDownST);
	lifeTime_STA.lastPowerDownSeconds += (currTime - lastTime);

	for(uint16_t idx =0; idx < LIFESPAN_W_MAX_ID;idx++)
	{
		if(lifeSpanInstallTime_ST[idx] <=1000)//reserved
		{
			lifeSpanInstallTime_ST[idx] = stTime;

		}
	}
	Trigger_EEPSave((void*)&powerOnOff_ST, sizeof(powerOnOff_ST),SYNC_CYCLE);
	Trigger_EEPSave((void*)&lifeTime_STA, sizeof(lifeTime_STA),SYNC_CYCLE);
	Trigger_EEPSave((void*)&lifeSpanInstallTime_ST[0],sizeof(lifeSpanInstallTime_ST),SYNC_CYCLE);
}


static uint32_t GetStaticsPowerOnoffTime(uint16_t i)
{
	if( i >= LIFESPAN_W_TUBING_MAX)
		return 0;
	return (uint32_t)((double)lifeTime_STA.lastPowerDownSeconds * lifeSpanFactor[i].powerOffFactor) + \
					(uint32_t)((double)lifeTime_STA.powerOnSeconds * lifeSpanFactor[i].powerOnFactor);
}


void UpdateSTA_LifeTime(void)
{
	static uint32_t oldTime = 0;
//	OS_Use_Unit(STA_Data.ptrDataSemaphore);
	if(oldTime == 0)
		oldTime = GetCurrentSeconds();
	uint32_t newTime = GetCurrentSeconds();

	powerOnOff_ST.lastPowerDownST = GetCurrentST();
	lifeTime_STA.powerOnSeconds += (newTime - oldTime);
	oldTime = newTime;
	Trigger_EEPSave((void*)&lifeTime_STA,sizeof(lifeTime_STA),SYNC_CYCLE);
	Trigger_EEPSave((void*)&powerOnOff_ST,sizeof(powerOnOff_ST),SYNC_CYCLE);
//	OS_Unuse_Unit(STA_Data.ptrDataSemaphore);

}

static void ChangeTubing_LifeTime(uint16_t tubId)
{
	uint32_t stTime = GetCurrentST();

	if(tubId < LIFESPAN_W_TUBING_MAX)
	{
		lifeSpan_RuningSeconds[tubId] = 0;
		lifeSpanInstallTime_ST[tubId] = stTime;
		lifeSpan_StaticStart[tubId] = GetStaticsPowerOnoffTime(tubId);
	}
	else
	{
		for(uint16_t i =0;i<LIFESPAN_W_TUBING_MAX;i++)
		{
			lifeSpan_RuningSeconds[i] = 0;
			lifeSpanInstallTime_ST[i] = stTime;
			//
			lifeSpan_StaticStart[i] = GetStaticsPowerOnoffTime(i);
		}
	}
	Trigger_EEPSave((void*)&lifeSpan_RuningSeconds,sizeof(lifeSpan_RuningSeconds),SYNC_CYCLE);
	Trigger_EEPSave((void*)&lifeSpanInstallTime_ST,sizeof(lifeSpanInstallTime_ST),SYNC_CYCLE);
	Trigger_EEPSave((void*)&lifeSpan_StaticStart,sizeof(lifeSpan_StaticStart),SYNC_CYCLE);


}


static void ChangeMotor_LifeTime(uint16_t motorId)
{
	uint32_t stTime = GetCurrentST();
	if(motorId < IDX_MOTOR_MAX)
	{
		staDataMotorSteps[motorId] = 0;
		lifeSpan_RuningSeconds[LIFESPAN_W_PUMP1 + motorId] = 0;
		lifeSpanInstallTime_ST[LIFESPAN_W_PUMP1 + motorId] = stTime;
	}
	else
	{
		for(uint16_t i=0;i<IDX_MOTOR_MAX;i++)
		{
			staDataMotorSteps[i] = 0;
			lifeSpan_RuningSeconds[LIFESPAN_W_PUMP1 + i] = 0;
			lifeSpanInstallTime_ST[LIFESPAN_W_PUMP1 + i] = stTime;
		}
	}

	Trigger_EEPSave((void*)&lifeSpanInstallTime_ST,sizeof(lifeSpanInstallTime_ST),SYNC_CYCLE);
	Trigger_EEPSave((void*)&lifeSpan_RuningSeconds,sizeof(lifeSpan_RuningSeconds),SYNC_CYCLE);
	Trigger_EEPSave((void*)&staDataMotorSteps,sizeof(staDataMotorSteps),SYNC_IM);
}


static void UpdateTubingLife_StaticsTime(void)
{
	for(uint16_t i = 0; i< LIFESPAN_W_TUBING_MAX;i++)
	{
		 uint32_t calcVal = GetStaticsPowerOnoffTime(i);

		 if(calcVal >= lifeSpan_StaticStart[i])
			 lifeSpanTubing_StaticSeconds[i] = calcVal - lifeSpan_StaticStart[i];
		 else
		 {
			 lifeSpan_StaticStart[i] = 0;
			 lifeSpanTubing_StaticSeconds[i] = calcVal;
		 }
		 lifeSpanTubing_StaticSeconds[i] += (uint32_t)(lifeSpanFactor[i].runFactor * (double)(lifeSpan_RuningSeconds[i] ));
	}
	Trigger_EEPSave((void*)&lifeSpan_StaticStart,sizeof(lifeSpan_StaticStart),SYNC_CYCLE);

}


uint16_t LoadRomDefaults_STA_Data(const T_UNIT *me, int16_t dataClassIndex)
{
	uint16_t ret = LoadRomDefaults_T_UNIT(me, dataClassIndex);
	if(ret == OK)
		STA_Init();

	return ret;
}
uint16_t Initialize_STA_Data(const struct _T_UNIT *me, uint8_t typeOfStartUp)
{
	uint16_t result = OK;
	//lint -e{746}
	assert(me==&STA_Data);
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
			STA_Init();
		}

		/*if( (typeOfStartUp & INIT_CALCULATION) != 0)
		{
		}*/
	}
	return result;
}



void UpdateTubingLifeTime(void)
{
	uint32_t tick = GetCurrentSeconds();

	for(uint16_t i=0;i<LIFESPAN_W_MAX_ID;i++)
	{
		uint32_t stTime = GetSecondsFromST(lifeSpanInstallTime_ST[i]);
		if(stTime <= tick)
			lifeSpan_InstalledSeconds[i] = tick - stTime;
		else
		{
			lifeSpan_InstalledSeconds[i] = 0;
			lifeSpanInstallTime_ST[i] = GetCurrentST();
			TraceDBG(TSK_ID_MCU_STATUS, "UpdateTubingLifeTime found the start time %d> current time: %d\n",stTime, tick );
		}
	}

	Trigger_EEPSave( (void*)&lifeSpanInstallTime_ST, sizeof(lifeSpanInstallTime_ST), SYNC_CYCLE);
	UpdateTubingLife_StaticsTime();
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the get STA_Data
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
uint16_t Get_STA_Data(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &STA_Data);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);
	if(result == OK)
	{
		switch(objectIndex)
		{
		case OBJ_IDX_FLOW_PERCENTAGE:
			for(uint16_t idx=0;idx<VOL_MAX_ID; idx++)
			{
				fluidVolume_RemainPercentage[idx] = (float)fluidVolume_Remain[idx] / fluidVolume_Set[idx] * 100.0f;
			}
			break;
		case OBJ_IDX_TUBING_LIFE:
		case OBJ_IDX_INSTALL_TIME:
			UpdateTubingLifeTime();
			break;
		}
	}

	result = Get_T_UNIT(me,objectIndex,attributeIndex,ptrValue);

	return result;
}




//--------------------------------------------------------------------------------------------------
/*!
 \brief	Perform the put STA_Data
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
uint16_t Put_STA_Data(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue)
{
	uint16_t result = OK;
	VIP_ASSERT(me == &STA_Data);
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED); // exception if not initialized
	assert(me->ptrObjectList);


	result = Put_T_UNIT(me,objectIndex,attributeIndex,ptrValue);
	if(result == OK)
	{

		switch(objectIndex)
		{
		case OBJ_IDX_CHANGE_TUBING:
			ChangeTubing_LifeTime(resetTublingLifetime);
			break;
		case OBJ_IDX_RESET_FACT:
			if(resetFactory)
			{
				uint32_t stTime = GetCurrentST();
				me->LoadRomDefaults(me,ALL_DATACLASSES);
				powerOnOff_ST.lastPowerDownST = stTime;
				powerOnOff_ST.powerOnST = stTime;
				for(uint16_t i =0;i < LIFESPAN_W_MAX_ID;i++)
					lifeSpanInstallTime_ST[i] = powerOnOff_ST.powerOnST;

				for(uint16_t i =0;i < LIFESPAN_W_TUBING_MAX;i++)
					lifeSpan_StaticStart[i] = GetStaticsPowerOnoffTime(i);

				Trigger_EEPSave((void*)&lifeSpanInstallTime_ST,sizeof(lifeSpanInstallTime_ST),SYNC_CYCLE);
				Trigger_EEPSave((void*)&powerOnOff_ST,sizeof(powerOnOff_ST),SYNC_CYCLE);
				Trigger_EEPSave((void*)&lifeSpan_StaticStart,sizeof(lifeSpan_StaticStart),SYNC_IM);
			}
			break;
		case OBJ_IDX_CHANGE_MOTOR:
			ChangeMotor_LifeTime(resetMotorLifetime);
			break;
		}
	}
	return result;
}


uint16_t Sta_AddMixTime(uint32_t mixTime,uint32_t pps)
{
	uint16_t result = OK;
//	OS_Use_Unit(STA_Data.ptrDataSemaphore);
//	staDataMotorTime[IDX_MIXING] += mixTime;
	lifeSpan_RuningSeconds[LIFESPAN_W_MIX] += mixTime;
	//lint -e647
	staDataMotorSteps[IDX_MIXING] = staDataMotorSteps[IDX_MIXING] + (uint64_t)(mixTime*pps);
//	OS_Unuse_Unit(STA_Data.ptrDataSemaphore);
	Trigger_EEPSave((void*)&lifeSpan_RuningSeconds[0], sizeof(lifeSpan_RuningSeconds), SYNC_CYCLE);
	Trigger_EEPSave((void*)&staDataMotorSteps[0], sizeof(staDataMotorSteps), SYNC_CYCLE);
	return result;
}


uint16_t Sta_AddValveCount(uint16_t pins)
{
	uint16_t result = OK;
//	OS_Use_Unit(STA_Data.ptrDataSemaphore);
	for(uint16_t idx = 0; idx <VALVE_PINS_NUM; idx++)
	{
		if(pins & (1<<(15-idx)))
		{
			staDataValve[idx] = (uint32_t)(staDataValve[idx]+1);
		}
	}
//	OS_Unuse_Unit(STA_Data.ptrDataSemaphore);
	Trigger_EEPSave((void*)&staDataValve[0], sizeof(staDataValve), SYNC_CYCLE);
	return result;
}
