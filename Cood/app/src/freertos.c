/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 *
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "unit_temp_cfg.h"
/* USER CODE BEGIN Includes */
#include "cmd.h"
#include <string.h>
#include "tsk_flow_all.h"
#include "tsk_measure.h"
#include "dev_eep.h"
#include "tsk_temp_ctrl.h"
#include "dev_temp.h"
#include "tsk_sch.h"
#include "tsk_datalog.h"
#include "lb_task.h"
#include "dev_spi.h"
/* USER CODE END Includes */


/* Variables -----------------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Private variables ---------------------------------------------------------*/


osThreadId taskThreadID[MAX_TASK_ID] = {NULL,};

typedef struct{
	osThreadDef_t threadDef;
	uint32_t TaskId;
	void*    para;
}TaskConfiguration_t;


static void StartIdleTask(void const * argument)
{

#define AVG_TEMP_TIME		70

	(void)argument; // pc lint

	const uint16_t taskID = TSK_ID_IDLE;
	WDI_TRIGGER;
	HAL_GPIO_WritePin(LED_WORK1_GPIO_Port, LED_WORK1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_WORK2_GPIO_Port, LED_WORK2_Pin, GPIO_PIN_SET);

	while(TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		WDI_TRIGGER;
		osDelay(500);

		HAL_GPIO_TogglePin(LED_WORK1_GPIO_Port, LED_WORK1_Pin);
		HAL_GPIO_TogglePin(LED_WORK2_GPIO_Port, LED_WORK2_Pin);


	}
}



static const TaskConfiguration_t TaskConfiguration[] =
{
#if 1
		{{"StartMotorCmdTask",		StartMotorCmdTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
		{{"StartPollSchTask",		StartPollSchTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
		{{"StartPreMainTask",		StartPreMainTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
		{{"StartPreSubTask",		StartPreSubTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
		{{"StartSpCtrlTask",		StartSpCtrlTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
		{{"StartMainStepTask",		StartMainStepTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
		{{"StartMicroStepTask",		StartMicroStepTask,			osPriorityNormal, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},

#endif
//	{{"sch task",			StartSchTask,			osPriorityNormal, 	0,		512}, 	TSK_ID_SCH, NULL},
	{{"ad_detector",		StartAdTask,			osPriorityRealtime, 	0,		384}, 	TSK_ID_AD_DETECT, NULL},
	{{"ad measure", 		StartMeasCalcTask,		osPriorityHigh,			0,		512}, 	TSK_ID_MEASURE, NULL},
//	{{"task Monitor", 		StartMonitorTask,		osPriorityNormal,		0,		256}, 	TSK_ID_MONITOR, NULL},
//	{{"auto range", 		StartAutoRangeTask,		osPriorityNormal,		0,		256}, 	TSK_ID_AUTO_RANGE, NULL},

	{{"eep ctrl",			StartEEPTask,			osPriorityNormal, 	0,		256},	TSK_ID_EEP, NULL},
//	{{"mcu status task",	StartMCUStatusTask,		osPriorityNormal, 	0,			384}, 	TSK_ID_MCU_STATUS, NULL},
	{{"datalog task",		StartDataLogTask,		osPriorityBelowNormal, 	0,		384}, 	TSK_ID_DATA_LOG, NULL},

//	{{"poll sch task",		StartPollSchTask,		osPriorityNormal, 	0,		256}, 	TSK_ID_POLL_SCH_POLL, NULL},
	{{"flow poll",			StartPollTask,			osPriorityNormal, 	0,		256},	TSK_ID_FLOW_POLL, NULL},
//	{{"sch clean",			StartCleanTask,			osPriorityNormal, 	0,		256}, 	TSK_ID_SCH_CLEAN, NULL},
//	{{"sch calibration",	StartCalibrationTask,	osPriorityNormal, 	0,		384}, 	TSK_ID_SCH_CALI, NULL},
//	{{"sch meas",			StartSchMeasTask,		osPriorityNormal, 	0,		384}, 	TSK_ID_SCH_MEAS, NULL},

	{{"substep ctrl",	StartSubStepTask,	osPriorityNormal, 	0,		512},			TSK_ID_SUBSTEP_CTRL, NULL},
//	{{"flow ctrl",		StartFlowTask,		osPriorityNormal, 	0,		256},			TSK_ID_FLOW_CTRL, NULL},
	{{"valve ctrl",		StartValveTask,		osPriorityNormal, 	0,		256},			TSK_ID_VALVE_CTRL, NULL},
//	{{"mix ctrl",		StartMixTask,		osPriorityNormal, 	0,		384},			TSK_ID_MIX_CTRL, NULL},
	{{"motor ctrl",		StartMotorTask,		osPriorityNormal, 	0,		384},			TSK_ID_MOTOR_CTRL, NULL},
//	{{"sch IO",			StartIOTask,		osPriorityNormal, 	0,		384}, 			TSK_ID_SCH_IO, NULL},
#if 1
	//for local bus communication
//	{{"can tsk",		StartCanTask,		osPriorityNormal, 	0,		512},		TSK_ID_CAN_TSK, NULL},
//	{{"localbus task",	StartLBTask,		osPriorityNormal, 	0,		1024}, 		TSK_ID_LOCAL_BUS, NULL},


	{{"temp ctrl",		StartTempCtrlTask,	osPriorityNormal, 		0,				512},			TSK_ID_TEMP_CTRL, NULL},
	{{"idle",			StartIdleTask,		osPriorityLow, 	0,			256},			TSK_ID_IDLE, NULL},

#endif

	{{"print",			StartPrintTask,			osPriorityBelowNormal, 	0,			384},			TSK_ID_PRINT, NULL},
	{{"shell tx",		StartShellTXTask,		osPriorityBelowNormal, 	0,			384},			TSK_ID_SHELL_TX, NULL},
	{{"shell rx",		StartShellRXTask,		osPriorityBelowNormal, 	0,			512},			TSK_ID_SHELL_RX, NULL},

#if 1
//	{{"can master",		StartCanopenTask,	osPriorityAboveNormal, 	0,		512},			TSK_ID_CAN_MASTER, NULL},
//	{{"io401 tsk",		StartCanIOTask,		osPriorityNormal, 	0,		512},			TSK_ID_CAN_IO, NULL},

#endif
};


/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void)
{
	uint16_t tskSize = sizeof(TaskConfiguration)/sizeof(TaskConfiguration_t);
	assert(tskSize <= MAX_TASK_ID);
	for(uint16_t idx = 0;idx<tskSize;idx++)
	{
		taskThreadID[TaskConfiguration[idx].TaskId] = osThreadCreate(&TaskConfiguration[idx].threadDef, TaskConfiguration[idx].para);
		assert(taskThreadID[TaskConfiguration[idx].TaskId] != NULL);
	}

}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
