/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"
#include "dev_eep.h"
#include <string.h>
#include "trace.h"
#include "shell_io.h"
#include "sys_info.h"

/* USER CODE BEGIN Includes */
#ifdef NO_PRINTF
#  define printf(...)
#endif
/* USER CODE END Includes */

typedef struct
{
	osMessageQId* ptrQid;
	uint16_t size;
} QueIDInit;
osMessageQId SHELL_RX_ID;

static const QueIDInit QID[]=
{
	{&SHELL_RX_ID, 			8},
};
typedef struct {
	uint32_t startAdr;
	uint32_t endAdr;
	uint32_t rwStatus;
	uint16_t (*GetMem)(uint32_t adr, uint8_t* data, uint16_t len);
	uint16_t (*PutMem)(uint32_t adr, uint8_t* data, uint16_t len);
}AdrInfo;


static uint16_t RamRdAccess(uint32_t adr, uint8_t* data, uint16_t len)
{
	memcpy((void*)data,(void*)adr,len);
	return OK;
}


static uint16_t RamWrAccess(uint32_t adr, uint8_t* data, uint16_t len)
{
	memcpy((void*)adr,(void*)data,len);
	return OK;
}


static const AdrInfo ST32Adr[] =
{
	//lint -e655
	{0x20000000, 0x20000000+0x30000, (uint32_t)(READ_ACCESS|WRITE_ACCESS), RamRdAccess, RamWrAccess},//ram
	//lint -e655
	{0x10000000, 0x10000000+0x10000, (uint32_t)(READ_ACCESS|WRITE_ACCESS), RamRdAccess, RamWrAccess},//ccram
};


//{0,			 0x800000},//flash

uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus)
{
	uint16_t ret = FATAL_ERROR;

	for(uint16_t idx = 0; idx < sizeof(ST32Adr)/sizeof(AdrInfo);idx++)
	{
		if(adr >=ST32Adr[idx].startAdr && (adr+len) < ST32Adr[idx].endAdr)
		{
			if(ST32Adr[idx].rwStatus & rwStatus )
			{
				ret = OK;
			}
			break;
		}
	}
	return ret;
}
/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

osStatus MessagePush(uint32_t line, osMessageQId queue_id, uint32_t info,
		uint32_t millisec)
{

	if (queue_id == NULL)
		return osErrorResource;
//	assert_param(queue_id);
	return osMessagePut(queue_id, info, millisec);
}
static void CreateAllQid(void)
{
	uint16_t qNum = sizeof(QID)/sizeof(QueIDInit);
	for(uint16_t idx = 0; idx<qNum; idx++)
	{
		osMessageQDef(TSK_Queue, QID[idx].size, uint32_t);
		*(QID[idx].ptrQid) = osMessageCreate(osMessageQ(TSK_Queue), NULL);
	}
}


osStatus MessagePurge(uint32_t line, osMessageQId queue_id, uint32_t info,
		uint32_t millisec)
{
	if (queue_id == NULL)
		return osErrorResource;
	osEvent event;
	event = osMessageGet(queue_id, 0);
	while (event.status == osEventMessage)
	{
		event = osMessageGet(queue_id, 0);
	}
//	assert_param(queue_id);
	return osMessagePut(queue_id, info, millisec);
}

osStatus SignalPush(osThreadId thread_id, int32_t signal)
{
	if (thread_id == NULL)
		return osErrorResource;

	return osSignalSet(thread_id, signal);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */



void vApplicationMallocFailedHook(void)
{

//	dbg_printf("\n\nheap exhausted..");

	for (;;)
		;
}


void OS_Use(OS_RSEMA x)
{
	if(x) xSemaphoreTakeRecursive((QueueHandle_t)x, portMAX_DELAY);
	else {}
}
void OS_Unuse(OS_RSEMA x)
{
	if(x) xSemaphoreGiveRecursive((QueueHandle_t)x);
	else {}
}

OS_RSEMA OS_CreateSemaphore(void)
{
	return (OS_RSEMA)xSemaphoreCreateRecursiveMutex();
}


/* USER CODE END 0 */
SemaphoreHandle_t shellLock = NULL;

uint32_t version = 0;
void UpdateVersion()
{
    version = (uint32_t)((((BUILD_YEAR-2000)*100000000)+(BUILD_MONTH*1000000)+(BUILD_DAY*10000)+(BUILD_HOUR*100)+ BUILD_MIN));
}
int main(int argc, char* argv[])
{

	/* USER CODE BEGIN 1 */
	(void) argc;
	(void) argv;
	/* USER CODE END 1 */
//	while(1);
	/* MCU Configuration----------------------------------------------------------*/
	CreateAllQid();
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	/* Configure the system clock */
	HAL_Init();
	SystemClock_Config();
	shellLock = OS_CreateSemaphore();
	UpdateVersion();
	trace_printf("System clock: %u Hz\n", SystemCoreClock);
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
//	MX_CAN_Init();
	MX_TIM6_Init();
	MX_USART1_UART_Init();


	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}

/** System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
	Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 3, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
	{
	}
	/* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
