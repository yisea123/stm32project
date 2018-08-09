/*
 * tsk_mcu_status.c
 *
 *  Created on: 2017骞�3鏈�3鏃�
 *      Author: pli
 */
#include "main.h"
#include "dev_eep.h"
#include "unit_sys_diagnosis.h"
#include <string.h>
#include "unit_temp_cfg.h"
#include "unit_statistics_data.h"
#include "dev_temp.h"
#include "dev_hdc1080.h"
#include "adc.h"
#include "rtc.h"
static char* taskStateDsp[] =
{
	TO_STR(MCU_IDLE),
	TO_STR(MCU_WARNING_HIGH_VOLT),
	TO_STR(MCU_WARNING_LOW_VOLT),
	TO_STR(MCU_FINISH),
};


extern int16_t rtcVolt;

/*
 * 	{{"temp ctrl",		StartTempCtrlTask,	osPriorityIdle, 	0,			512},			TSK_ID_TEMP_CTRL, NULL},
	{{"idle",			StartIdleTask,		osPriorityIdle, 	0,			256},			TSK_ID_IDLE, NULL},

#endif

	{{"print",			StartPrintTask,			osPriorityLow, 	0,			386},			TSK_ID_PRINT, NULL},
	{{"shell tx",		StartShellTXTask,		osPriorityLow, 	0,			386},			TSK_ID_SHELL_TX, NULL},
	{{"shell rx",		StartShellRXTask,		osPriorityLow, 	0,			512},			TSK_ID_SHELL_RX, NULL},

#if 1
	{{"can master",		StartCanopenTask,	osPriorityBelowNormal, 	0,		512},			TSK_ID_CAN_MASTER, NULL},
	{{"io401 tsk",		StartCanIOTask,		osPriorityBelowNormal, 	0,		512},			TSK_ID_CAN_IO, NULL},
 *
 */

static const uint16_t MonitorTskID[] =
{
		TSK_ID_TEMP_CTRL,
		TSK_ID_IDLE,
		TSK_ID_PRINT,
		TSK_ID_CAN_MASTER,
//		TSK_ID_CAN_IO,

};
void StartMonitorTask(void const * argument)
{
	(void)argument; // pc lint

	const uint16_t taskID = TSK_ID_MONITOR;

	uint16_t 	sizeMonitor = sizeof(MonitorTskID)/sizeof(uint16_t);
	uint32_t	freeRtosTskTick_Old[MAX_TASK_ID] = {0,0,0,0,0,0,};
	uint8_t  	count[MAX_TASK_ID] = {0,0,0,0,0,0,};
	while (TASK_LOOP_ST)
	{
		osDelay(3000);
		freeRtosTskTick[taskID]++;

		for(uint16_t idx = 0; idx < sizeMonitor; idx++)
		{
			uint16_t id = MonitorTskID[idx];
			if(freeRtosTskTick_Old[ id ] == freeRtosTskTick[ id ])
			{
				count[id]++;
				if(count[id] > 2)
				{
					uint8_t timeBuf[40];
					sprintf_rtc(timeBuf, 40);
										//lint -e586
					dbg_printf("Time %s==>Task failed:id:%d ->%s -> %d;\n",timeBuf, id, TskName[id], count[id] );
					for(uint16_t id1 = 0; id1 < sizeMonitor;id1++)
					{
						uint16_t id2 = MonitorTskID[id1];
						dbg_printf("other Task: id:%d ->%s -> %d;\n", id2, TskName[id2], count[id2] );
					}
					assert(0);
					//error;
				}
			}
			else
			{
				count[id] = 0;
				freeRtosTskTick_Old[id] = freeRtosTskTick[id];
			}

		}
	}
}
/* StartMCUStatusTask function */

volatile uint16_t lowVoltTage = 0;
volatile uint16_t highVoltTage = 0;
void StartMCUStatusTask(void const * argument)
{
	(void)argument; // pc lint
	static uint16_t _lowVoltTage = 0;
	static uint16_t _highVoltTage = 0;
	uint32_t tickOut = 1000;
	osEvent event;
	MCU_STate tskState = MCU_IDLE;

	TSK_MSG localMsg;
	const uint8_t taskID = TSK_ID_MCU_STATUS;

	localMsg.callBack = NULL;
	localMsg.tskState = TSK_SUBSTEP;
	TracePrint(taskID,"started  \n");
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	Init_hdc1080();
	for(uint16_t i =0; i < 10; i++)
	{
		GetAllTemperature();
		osDelay(10);
	}

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(MCU_STATUS_ID, tickOut);
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tskState = MCU_IDLE;


			uint32_t val = GetSTADC_Value(ADC_12V_SUPPLY);
			AdcVolts[ADC_12V_SUPPLY] = (uint16_t)(val * 4.833984375);//???? //todo

			//
			val = GetSTADC_Value(ADC_CURR_MONITOR);
			AdcVolts[ADC_CURR_MONITOR] = (uint16_t)(val * 2.44140625);//???? //todo

			rtcVolt = GetRTC_Volt() ;
			UpdateTH();


			for(uint16_t idx = 0;idx<DIA_EX_FUNC;idx++)
			{
				if(Dia_FuncList[idx] != NULL)
					Dia_FuncList[idx]();
			}
			RefreshDiagnosisOutput();
			UpdateSTA_LifeTime();
			//call adc3
			//ref 3.3;
			//multi with 6
			if(_lowVoltTage != lowVoltTage)
			{
				if(lowVoltTage)
					TraceDBG(TSK_ID_MCU_STATUS,"12V low voltage is detected.\n");
				else
					TraceDBG(TSK_ID_MCU_STATUS,"12V low voltage is clear.\n");
				_lowVoltTage = lowVoltTage;
			}
			if(_highVoltTage != highVoltTage)
			{
				if(highVoltTage)
					TraceDBG(TSK_ID_MCU_STATUS,"12V high voltage is detected.\n");
				else
					TraceDBG(TSK_ID_MCU_STATUS,"12V high voltage is clear.\n");
				_highVoltTage = highVoltTage;
			}

			Dia_UpdateDiagnosis(VOLT_OUT_RANGE,(uint16_t)(lowVoltTage+highVoltTage));


		}
		else
		{
			TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
				tskState = MCU_FINISH;
				tickOut = osWaitForever;
				//force state change to be break;
			}
			else if ( mainTskState == TSK_INIT)
			{
				tickOut = 1000;
				tskState = (MCU_STate)(TSK_MSG_CONVERT(event.value.p)->val.value);
				MsgPush ( MCU_STATUS_ID, (uint32_t)&localMsg, 0);
			}
			else if( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
					case MCU_IDLE:
					break;
					case MCU_VOLT_REFRESH:
						tskState = MCU_FINISH;
						tickOut = 0;
						break;


					case MCU_FINISH:
						tskState = MCU_IDLE;
						if(localMsg.callBack)
						{
							localMsg.callBack(taskID);
						}
						break;
				}
				//when finish->call back;
			}
			else
			{
				//todo
			}
		}
	}
}

