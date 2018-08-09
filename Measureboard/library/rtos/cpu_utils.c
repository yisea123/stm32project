/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// cpu_utils.cpp - CPU utilities for runtime statistics.
//
// Created on: 05/07/16
//
// Author: Hach
//
// Copyright(c) 2016-2017 Hach DDC
//             All Rights Reserved
//
// Free to use.
//
// Notes:
//   Partly from STMicroelectronics, to use this module, the following steps
// should be followed :
//
//	 1- in the _OS_Config.h file (ex. FreeRTOSConfig.h) enable the following macros :
//	     - #define configUSE_IDLE_HOOK        1
//		 - #define configUSE_TICK_HOOK        1
//
//	 2- in the _OS_Config.h define the following macros :
//		 - #define traceTASK_SWITCHED_IN()  extern void StartIdleMonitor(void);
//											 StartIdleMonitor()
//		 - #define traceTASK_SWITCHED_OUT() extern void EndIdleMonitor(void);
//											 EndIdleMonitor()
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stm32f4xx.h>
#include <cmsis_os.h>
#include "trace.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CALCULATION_PERIOD    1000

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

xTaskHandle xIdleHandle = NULL;
__IO uint32_t osCPU_Usage = 0;
uint32_t osCPU_IdleStartTime = 0;
uint32_t osCPU_IdleSpentTime = 0;
uint32_t osCPU_TotalIdleTime = 0;
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Application Idle Hook
  * @param  None 
  * @retval None
  */
static void vApplicationIdleHook_cpuload(void)
{
    if( xIdleHandle == NULL )
    {
        /* Store the handle to the idle task. */
        xIdleHandle = osThreadGetId();  //xTaskGetCurrentTaskHandle();
    }
}
void vApplicationIdleHook(void)
{
	volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	 FreeRTOSConfig.h.

	 This function is called on each cycle of the idle task.  In this case it
	 does nothing useful, other than report the amount of FreeRTOS heap that
	 remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if (xFreeStackSpace > 100)
	{
		/* By now, the kernel has allocated everything it is going to, so
		 if there is a lot of heap remaining unallocated then
		 the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		 reduced accordingly. */
	}
	vApplicationIdleHook_cpuload();
}

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	 configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 function is called if a stack overflow is detected.  pxCurrentTCB can be
	 inspected in the debugger if the task name passed into this function is
	 corrupt. */

	dbg_printf("\n\nTask [%s,%x] Stack Overflow..", pcTaskName, pxTask);

	for (;;)
		;
}

/**
  * @brief  Application Idle Hook
  * @param  None 
  * @retval None
  */
void vApplicationTickHook_cpuload(void)
{
	static int tick = 0;
  
	if(tick ++ > CALCULATION_PERIOD)
	{
		tick = 0;

		if(osCPU_TotalIdleTime > 1000)
		{
			osCPU_TotalIdleTime = 1000;
		}
		osCPU_Usage = (100 - (osCPU_TotalIdleTime * 100) / CALCULATION_PERIOD);
		osCPU_TotalIdleTime = 0;
	}
}

void vApplicationTickHook(void)
{
	vApplicationTickHook_cpuload();

}

/**
  * @brief  Start Idle monitor
  * @param  None 
  * @retval None
  */
void StartIdleMonitor (void)
{
	if( osThreadGetId() == xIdleHandle )
	{
		osCPU_IdleStartTime = osKernelSysTick();    //xTaskGetTickCount();
	}

#if CFG_ENABLE_PRINTTASKRUNTIME == 1	
	//sPrintTaskRunTimeObj.lastSwichInTime = xTaskGetTickCount();
	sPrintTaskRunTimeObj.lastSwichInTime = vGetTimerCounterForRunTimeStats();
	
	sPrintTaskRunTimeObj.lastSwichInTask = xTaskGetCurrentTaskHandle();
#endif	
}

/**
  * @brief  Stop Idle monitor
  * @param  None 
  * @retval None
  */
void EndIdleMonitor (void)
{
	if( osThreadGetId() == xIdleHandle )
	{
		/* Store the handle to the idle task. */
		osCPU_IdleSpentTime = osKernelSysTick() - osCPU_IdleStartTime;
		osCPU_TotalIdleTime += osCPU_IdleSpentTime;
	}

}

/**
  * @brief  Stop Idle monitor
  * @param  None 
  * @retval None
  */
unsigned int osGetCPUUsage(void)
{
	return osCPU_Usage;
}


// End of file
