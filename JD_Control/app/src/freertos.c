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
#include "shell_io.h"
/* USER CODE BEGIN Includes */
#include "cmd.h"

#include "dev_eep.h"
#include "tsk_head.h"
#include "tsk_ethernet.h"
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



/* Function prototypes -------------------------------------------------------*/
/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
void configureTimerForRunTimeStats(void)
{

}

unsigned long getRunTimeCounterValue(void)
{
return 0;
}

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
extern void StartShellRXTask(void const * argument);
extern void StartShellTXTask(void const * argument);
extern void StartFileTask(void const * argument);
extern void StartGuiTask(void const * argument);
extern void StartGpioTask(void const * argument);
extern void ArcLinkProcess(void);

void StartArcLinkTask(void const * argument)
{
	ArcLinkProcess();
}

static const TaskConfiguration_t TaskConfiguration[] =
{
		//0
		{{"eep ctrl",		StartEEPTask,		osPriorityNormal, 	0,		320},		TSK_ID_EEP, NULL},
		{{"LB Task",		StartLBTask,		osPriorityNormal, 	0,		450},		TSK_ID_LOCAL_BUS, NULL},
		{{"Motor Ctrl",		StartMotorTsk,		osPriorityNormal, 	0,		420},		TSK_ID_MOTOR, NULL},
		{{"Weld Tsk",		StartWeldTask,		osPriorityNormal, 	0,		380},		TSK_ID_WELD, NULL},
		{{"Output Tsk",		StartOutputTsk,		osPriorityHigh, 	0,		300},		TSK_ID_OUTPUT, NULL},
		//5
		{{"Ethernet Tsk",	tcp_server_thread,		osPriorityHigh, 	0,		800},		TSK_ID_ETHERNET, NULL},
		{{"ADC Monitor",	StartADCMonitor,		osPriorityNormal, 	0,	400},		TSK_ID_ADC_MONITOR, NULL},
		{{"shell tx",		StartShellTXTask,		osPriorityLow, 	0,			256},			TSK_ID_SHELL_TX, NULL},
		{{"shell rx",		StartShellRXTask,		osPriorityLow, 	0,			256},			TSK_ID_SHELL_RX, NULL},
		//10
		{{"currcali",		StartCurrCaliTask,		osPriorityLow, 	0,			320},			TSK_ID_CURR_CALI, NULL},
		{{"schedule",		StartSchTask,			osPriorityLow, 	0,			384},			TSK_ID_CURR_CALI, NULL},
		{{"pwmoutput",		StartPWMTsk,			osPriorityLow, 	0,			320},			TSK_ID_PWM, NULL},

};

/* USER CODE END Variables */


/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void)
{
	uint16_t size = sizeof(TaskConfiguration)/sizeof(TaskConfiguration_t);
	assert(size <= MAX_TASK_ID);

	for(uint16_t idx = 0;idx<size;idx++)
	{
		taskThreadID[TaskConfiguration[idx].TaskId] = osThreadCreate(&TaskConfiguration[idx].threadDef, TaskConfiguration[idx].para);
	}
	//tcp_server_init();
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
