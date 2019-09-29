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
#include "dev_ad.h"
#include "dev_can.h"
#include "At_Gui.h"
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
		{{"eep ctrl",		StartEEPTask,		osPriorityNormal, 	0,		256},		TSK_ID_EEP, NULL},
		{{"gui task",		StartGuiTask,		osPriorityNormal, 	0,		1024},		TSK_ID_GUI, NULL},
		{{"file task",		StartFileTask,		osPriorityHigh, 	0,		1024},		TSK_ID_FILE, NULL},

		{{"can rx1",		StartCanRXTask,		osPriorityHigh, 	0,		384},		TSK_ID_CAN_RX1, NULL},
		{{"can error",		StartCanErrTask,	osPriorityBelowNormal, 	0,		384},		TSK_ID_CAN_ERR, NULL},
		{{"can tx1",		StartCanTXTask,		osPriorityBelowNormal, 	0,		384},		TSK_ID_CAN_TX1, NULL},

		{{"AD monitor",		MonitorADTask,		osPriorityNormal, 		0,		384},		TSK_ID_AD_MONITOR, NULL},
		{{"Gpio task",		StartGpioTask,		osPriorityNormal, 		0,		384},		TSK_ID_GPIO, NULL},

		{{"shell tx",		StartShellTXTask,		osPriorityLow, 	0,			384},			TSK_ID_SHELL_TX, NULL},
		{{"shell rx",		StartShellRXTask,		osPriorityLow, 	0,			384},			TSK_ID_SHELL_RX, NULL},
//		{{"arcLink",		StartArcLinkTask,		osPriorityLow, 	0,			384},			TSK_ID_TST, NULL},

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

}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
