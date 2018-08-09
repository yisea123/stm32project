/*
* cpu_stm32F4xx.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32F4xx.c 19990 2017-10-05 09:00:09Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief 
*	CPU driver for Cortex-M4 from STM32 (e.g. STM32F407)
*
* This module contains the cpu specific routines for initialization
* and timer handling. 
*
* Note: Adapt this initialization according to your requirements!
*
* \author emtas GmbH
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_datatype.h>
#include <co_timer.h>
#include <co_drv.h>

#include "codrv_bxcan.h"
#include "cpu_stm32.h"

/* hardware header
---------------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>

/* constant definitions
---------------------------------------------------------------------------*/

#if defined(CONFIG_BXCAN1) || defined(CONFIG_BXCAN2)
#else
	/* default CAN1 */
# define CONFIG_BXCAN1 1
/* #define CONFIG_BXCAN2 1 */
#endif

#ifdef CONFIG_BXCAN2
# warning "This initialization was created for CAN1. Adapt it to CAN2 (GPIO, IRQ)!"
#endif

#ifdef USE_STM32446E_EVAL
# define CAN_PB89 1
#elif defined(USE_STM32F4_DISCO)
/* STM32F407 Discovery */
# define CAN_PD01 1
#else
/* STM32F407 Discovery */
# define CAN_PD01 1
#endif

/* OS related default definition */
#ifdef CO_OS_SIGNAL_TIMER
#else
#  define CO_OS_SIGNAL_TIMER()
#endif

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
void codrvTimerISR( void);
void SystemClock_Config(void);
static void codrvUartInit(void);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/

/* interrupt entrys
---------------------------------------------------------------------------*/

/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initalization
*
*/
void codrvHardwareInit(void)
{
	SystemClock_Config();
	SystemCoreClockUpdate(); /* in case SystemCoreClock is req. */
	HAL_Init(); /* HAL_Init() also configure the Systick */

	NVIC_Configuration();

	codrvHardwareCanInit();

#ifdef STM32F446xx
	codrvUartInit();
#endif
}


/***************************************************************************/
/**
* \brief codrvHardwareCanInit - CAN related hardware initialization
*
*/
void codrvHardwareCanInit(void)
{
GPIO_InitTypeDef  GPIO_InitStruct;

	/* enable CAN clock - e.g. PCKL1 = 36MHz */
	__CAN_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();

	/* CAN RX/TX Pin settings ================================ */
#ifdef CAN_PD01
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef CAN_PB89
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif


   /* Release CAN from reset state */
	__CAN_FORCE_RESET();
	__CAN_RELEASE_RESET();

}

#ifdef STM32F446xx
#  ifdef NO_PRINTF
#  else
/* printf() initialization */
static UART_HandleTypeDef huart;

static void codrvUartInit(void)
{
GPIO_InitTypeDef  GPIO_InitStruct;

	__GPIOA_CLK_ENABLE();
	__USART1_CLK_ENABLE();

	//PA 9/10
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



	  huart.Instance = USART1;
	  huart.Init.BaudRate = 115200;
	  huart.Init.WordLength = UART_WORDLENGTH_8B;
	  huart.Init.StopBits = UART_STOPBITS_1;
	  huart.Init.Parity = UART_PARITY_NONE;
	  huart.Init.Mode = UART_MODE_TX_RX;
	  huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart.Init.OverSampling = UART_OVERSAMPLING_16;
	  HAL_UART_Init(&huart);


	  //HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	  //HAL_UART_Transmit(&huart, "Test\n", 5, HAL_MAX_DELAY);

	 printf("Test\n");

}

int _write (int fd, char *pBuffer, int size)
{
	HAL_UART_Transmit(&huart, (uint8_t*)pBuffer, (uint16_t)size, HAL_MAX_DELAY);
	return size;
}
#  endif
#endif

/***************************************************************************/
/**
* \brief
*
*/
void codrvCanEnableInterrupt(void)
{

	/* enable CAN interrupts */
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    //HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

}

/***************************************************************************/
/**
* \brief
*
*/
void codrvCanDisableInterrupt(void)
{
	/* disable CAN interrupts */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
//    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
}


/***************************************************************************/
/**
* \brief codrvSetCanTxInterrupt - set pending bit of the Transmit interrupt
*
* This function set the interrupt pending bit. In case of the NVIC
* enable interrupt and the CAN specific enable TX Interrupt mask
* the CAN interrupt handler is calling.
*
*/
void codrvCanSetTxInterrupt(void)
{
	/* set pending bit of the transmit interrupt */
#ifdef CONFIG_BXCAN1
	HAL_NVIC_SetPendingIRQ(CAN1_TX_IRQn     /* 19 */);
#elif defined(CONFIG_BXCAN2)
//	NVIC_SetPendingIRQ(CAN2_TX_IRQn     /* -- */);
#endif
}


/***************************************************************************/
/**
* CAN-TX interrupt
*/
void CAN1_TX_IRQHandler(void)
{
	codrvCanTransmitInterrupt();
}

/***************************************************************************/
/**
* CAN-RX Fifo 0 
*/
void CAN1_RX0_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* CAN-RX Fifo 1 - not used! 
*/
void CAN1_RX1_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* CAN Error interrupt
*/
void CAN1_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
}


/*-------------------------------------------------------------------------*/

/**
  * @brief This function configures the source of the time base.
  *        The time source is configured  to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function is called  automatically at the beginning of program after
  *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
  * @note In the default implementation, SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals.
  *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
  *       The the SysTick interrupt must have higher priority (numerically lower)
  *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  *       The function is declared as __weak  to be overwritten  in case of other
  *       implementation  in user file.
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/*Configure the SysTick IRQ priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);

	/* Return function status */
	return HAL_OK;
}

/***************************************************************************/
/**
* \brief codrvTimerSetup - init Timer
*
* Start a cyclic hardware timer to provide timing interval.
* Alternatively it can be derived from an other system timer
* with the interval given from the DeviceDesigner.
*
*/
RET_T codrvTimerSetup(
		UNSIGNED32 timerInterval
	)
{
	HAL_SYSTICK_Config((HAL_RCC_GetHCLKFreq()/(1000ul*1000ul))*timerInterval);

	return(RET_OK);
}

/***************************************************************************/
/**
* \brief codrvTimerISR - Timer interrupt service routine
*
* is normally called from timer interrupt or from an other system timer
*
* \param
*	none
* \results
*	none
*/
void codrvTimerISR(
		void
    )
{
	/* inform stack about new timer event */
	coTimerTick();

	/* signal in case of use of an OS */
	CO_OS_SIGNAL_TIMER();
}

/***************************************************************************/
/**
* SysTick Interrupt - CANopen timer interrupt
*
* Instead of the move of this Handler from the Interrzpt file
* the HAL_SYSTICK_Callback() can be used to call 
* codrvTimerISR().
*/
void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

	codrvTimerISR();
}



/***************************************************************************/
/**
* NVIC_Configuration - init NVIC controller
*
* Possible, that also the interrupt table is moving in the RAM
* (req. for Bootloader)
*
*/
void NVIC_Configuration(void)
{
const UNSIGNED32 canPriority = 1u;
const UNSIGNED32 canSubPriority = 1u;


#ifdef BOOT
	/*
	 * configure the Vector table
	 * At this point very late. Better place is system_stm32xxxx.c.
	 * Correct VECT_TAB_OFFSET.
	 *
	 * NOTE: There are some reserved bits. Means, not all addresses are allowed!
	 */

#  if defined(__Vectors)
extern UNSIGNED32 __Vectors;
	/* Keil RealARM or additional symbol in the Linker file */
	SCB->VTOR = (UNSIGNED32)&__Vectors;
#  endif

#  if defined(g_pfnVectors)
extern UNSIGNED32 g_pfnVectors;
	/* ST linker files for gcc */
	SCB->VTOR = (UNSIGNED32)&g_pfnVectors;
#  endif
#endif /* BOOT */

	/* Priority group in HAL_MspInit() */
	
	/* CAN interrupts 
     *-------------------------------------------------------*/
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, canPriority, canSubPriority);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, canPriority, canSubPriority);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, canPriority, canSubPriority);

	codrvCanEnableInterrupt();

    /*-------------------------------------------------------*/

}


#ifdef CUSTOMER_SYSCLOCK_CONFIG
#else

/***************************************************************************/
/** System Clock Configuration
*     -> STM32F407 Discovery Board - 168MHz
*
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

#endif

