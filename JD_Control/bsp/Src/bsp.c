/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "rtc.h"
#include "shell_io.h"
#include "dev_ad5689.h"
#include "dev_ad7190.h"
/* USER CODE BEGIN Includes */
#define UART_BUFF_SIZE 		128
#define UART_IDX_MSK		(0x00007FU)
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void Error_Handler(void);

void* MallocMem(uint16_t size)
{
	if(size)
	{
		//lint -e586 -e526 -e628
		return (void* )malloc(size);
	}

	return NULL;
}
void FreeMem(void* adr)
{
	if(adr)
	{
		//lint -e586 -e526 -e628
		free(adr);
	}

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

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void bsp_init(void)
{
//	HAL_Init();
//	SystemClock_Config();
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
    MX_DMA_Init();
	MX_SPI2_Init();
//	MX_SPI1_Init();
	MX_CAN1_Init();
	MX_TIM2_Init();
	//MX_TIM3_Init();
	MX_UART4_Init();
	MX_USART3_UART_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_TIM8_Init();
	MX_UART5_Init();
	/* USER CODE BEGIN 2 */
	Init_RTC();

  /* USER CODE END 2 */
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /**Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_EnableCSS();                          
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, ISR_PRIORITY_TICK, ISR_SUB_PRIORITY_TICK);
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
	dbg_printf("Error handler");

  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}
void delay_us(uint32_t count)
{
	// todo : need minus sp operations
	count = count * (SystemCoreClock / 1000000); // / 16;	// divider of 16 is measured by osc
	for (; count > 0; count--)
		;
}


osStatus MessagePush (uint32_t line,char* file, osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
	if(queue_id == NULL)
		return osErrorResource;
//	assert_param(queue_id);
	osStatus ret = osMessagePut(queue_id,info, millisec);

	if(ret == osOK)
	{
	//	if(queue_id == SCH_IO_ID)
	//		TracePrint(0xFFFF, "Queue push : %d, line: %d\n",queue_id, line);
		//it is ok
	}
	else
	{
		TraceDBG(TSK_ID_PRINT, "Queue push Error: %d,%x,file:%s line: %d\n",queue_id,queue_id,file, line);
		//xQueueReset( queue_id);
		osMessagePut(queue_id,info, millisec);
	}

	return ret;


}

void Adc_Setup(void)
{

	uint16_t data = 0x1000;
	if (AD7190_Init() == 0)
	{
		TraceUser("No  AD7190 !\n");
		while (1)
		{
			HAL_Delay(10);
			if (AD7190_Init())
				break;
		}
	}
	TraceUser("detect AD7190 !\n");
	AD7190_conf();

	uint32_t weight_Zero_Data = AD7190_ReadAvg(6);
	TraceUser("zero:%d\n",weight_Zero_Data);
	ad7190_unipolar_multichannel_conf();

}


osStatus SignalPush(osThreadId thread_id, int32_t signal)
{
	if(thread_id == NULL)
		return osErrorResource;

	return (osStatus)osSignalSet(thread_id,signal);
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
    ex: TraceUser("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
//	while(1);
	TraceDBG(0xFFFF, "Assert Failed: %s, line: %d",file, line);
}



static uint8_t shellData1[UART_BUFF_SIZE];

void ShellRXHandle(void)
{
	uint8_t data = (uint8_t)(shellUart->Instance->DR & (uint8_t)0x00FFU);
	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint8_t uartShellIdx = 0;

	if ('\r' == data ||'\n' == data)
	{
		if(uartShellIdx != 0)
		{
			uartBuff[uartShellIdx++] = '\n';
			memset((void*)shellData1,0,sizeof(shellData1));
			memcpy((void*)shellData1,(void*)uartBuff,uartShellIdx);
			MsgPush(SHELL_RX_ID, (uint32_t)&shellData1[0],0);
		}
		else if('\r' == data)
		{
			MsgPush(SHELL_RX_ID, 0,0);
		}
		uartShellIdx = 0;
	}
	else
	{
		uartBuff[uartShellIdx++] = data;
		if(uartShellIdx >= UART_BUFF_SIZE)
			uartShellIdx = 0;
	}
}




uint16_t NewUartData(uint16_t type,uint8_t* ptrData)
{


	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint32_t uartShellIdx = 0;
	static uint32_t lastId = 0;
	uint16_t ret = OK;
	if(type == 0)
	{
		uartBuff[uartShellIdx&UART_IDX_MSK] = *ptrData;
		uartShellIdx++;
	}
	else // not whole data
	{
		if(lastId < uartShellIdx)
		{
			*ptrData = uartBuff[lastId&UART_IDX_MSK];
			lastId++;
		}
		else
		{
			ret = FATAL_ERROR;
		}
	}
	return ret;
}


#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
