/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// cpu_utils.h - CPU utilities for runtime statistics.
//
// Created on: 05/05/14
//
// Author: Hozen Shi
//
// Copyright(c) 2014-2015 Hach DDC
//             All Rights Reserved
//
// Free to use.
//
// Notes:
//   Partly from STMicroelectronics.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CPU_UTILS_H__
#define _CPU_UTILS_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
unsigned int osGetCPUUsage (void);

void os_heap_dump(char *buf);

void vApplicationTickHook_cpuload(void);

void vApplicationTickHook(void);
void vApplicationIdleHook(void);
void StartIdleMonitor (void);
void EndIdleMonitor(void);

void vConfigureTimerForRunTimeStats( void );

unsigned int vGetTimerCounterForRunTimeStats( void );

//void vResetTimerCounterForRunTimeStats( void );

#ifdef __cplusplus
}
#endif

#endif /* _CPU_UTILS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
