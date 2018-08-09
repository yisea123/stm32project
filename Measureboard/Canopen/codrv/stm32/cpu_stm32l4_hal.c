/*
* cpu_stm32l4_hal.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32l4_hal.c 18866 2017-06-27 10:15:25Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief CPU driver for Cortex-M4 from STM32 (e.g. STM32L476)
*
* This module contains the CPU specific routines for initialization
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

#include <stm32l4xx.h>

#include "codrv_bxcan.h"
#include "cpu_stm32.h"

/* hardware header
---------------------------------------------------------------------------*/

/* constant definitions
---------------------------------------------------------------------------*/

/* OS related default definition */
#ifdef CO_OS_SIGNAL_TIMER
#else /* CO_OS_SIGNAL_TIMER */
#  define CO_OS_SIGNAL_TIMER()
#endif /* CO_OS_SIGNAL_TIMER */

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


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
CAN_HandleTypeDef hcan;

#ifdef CODRV_USE_FREERTOS
extern SemaphoreHandle_t semphCANopen;
#endif /* USE_FREERTOS */

/* interrupt entries
---------------------------------------------------------------------------*/


/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initialization
*
*/
void codrvHardwareInit(void)
{
#ifdef CODRV_USE_FREERTOS
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* CAN related hardware initialization */
	codrvHardwareCanInit();

	NVIC_Configuration();

#else /* USE_FREERTOS */
	/* initialize NVIC controller */
	NVIC_Configuration();

	/* Configure the system clock */
	SystemClock_Config();

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* CAN related hardware initialization */
	codrvHardwareCanInit();
#endif /* USE_FREERTOS */
}


/***************************************************************************/
/**
* \brief codrvHardwareCanInit - CAN related hardware initialization
*
*/
void codrvHardwareCanInit(void)
{
GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/***************************************************************************/
/**
* \brief codrvCanEnableInterrupt - enable CAN interrupts
*
*/
void codrvCanEnableInterrupt(void)
{
	/* enable CAN1 interrupts */
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
}


/***************************************************************************/
/**
* \brief codrvCanDisableInterrupt - disable CAN interrupts
*
*/
void codrvCanDisableInterrupt(void)
{
	/* disable CAN1 interrupts */
	HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
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
	/* set pending bit of the CAN1 transmit interrupt */
	HAL_NVIC_SetPendingIRQ(CAN1_TX_IRQn);
}


/***************************************************************************/
/**
* \brief CAN1_TX_IRQHandler - CAN-TX interrupt
*
*/
void CAN1_TX_IRQHandler(void)
{
	codrvCanTransmitInterrupt();
}

/***************************************************************************/
/**
* \brief CAN1_SCE_IRQHandler - CAN-Error interrupt
*
*/
void CAN1_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
}

/***************************************************************************/
/**
* \brief CAN1_RX0_IRQHandler - CAN-RX interrupt
*
*/
void CAN1_RX0_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* \brief HAL_InitTick - configure the time base
*
* This function configures the source of the time base.
* The time source is configured  to have 1ms time base with a dedicated
* Tick interrupt priority.
*
* This function is called  automatically at the beginning of program after
* reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
*
* In the default implementation, SysTick timer is the source of time base.
* It is used to generate interrupts at regular time intervals.
* Care must be taken if HAL_Delay() is called from a peripheral ISR process,
* The the SysTick interrupt must have higher priority (numerically lower)
* than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
* The function is declared as __weak  to be overwritten  in case of other
* implementation  in user file.
*
*/
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
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
*/
void codrvTimerISR(
		void	/**< */
    )
{
	/* inform stack about new timer event */
	coTimerTick();

#ifdef CODRV_USE_FREERTOS
	CO_OS_SIGNAL_TIMER();
#endif /* USE_FREERTOS */
}

#ifdef USE_FREERTOS
#else /* USE_FREERTOS */
/***************************************************************************/
/**
* SysTick Interrupt - CANopen timer interrupt
*
* Instead of the move of this Handler from the Interrupt file
* the HAL_SYSTICK_Callback() can be used to call
* codrvTimerISR().
*
*/
void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

	codrvTimerISR();
}
#endif /* USE_FREERTOS */

/***************************************************************************/
/**
* NVIC_Configuration - initialize NVIC controller
*
* Possible, that also the interrupt table is moving in the RAM
* (required for Boot Loader)
*
*/
void NVIC_Configuration(void)
{
#ifdef CODRV_USE_FREERTOS
/* canPriority between
 * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
 * &
 * configLIBRARY_LOWEST_INTERRUPT_PRIORITY
 * (numerical lower is higher)*/
const UNSIGNED32 canPriority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1;
const UNSIGNED32 canSubPriority = 0u;
	/* CAN interrupts
     *-------------------------------------------------------*/
	HAL_NVIC_SetPriority(CAN1_TX_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, canPriority, canSubPriority);
#else /* USE_FREERTOS */
const UNSIGNED32 canPriority = 1u;
const UNSIGNED32 canSubPriority = 1u;
	/* CAN interrupts
     *-------------------------------------------------------*/
	HAL_NVIC_SetPriority(CAN1_TX_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, canPriority, canSubPriority);
#endif /* USE_FREERTOS */
#ifdef BOOT
	/*
	 * configure the Vector table
	 * At this point very late. Better place is system_stm32f4xx.c.
	 * Correct VECT_TAB_OFFSET.
	 *
	 * NOTE: There are some reserved bits. Means, not all addresses are allowed!
	 */
#  if 0
	/* wrong with HAL */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4200); /* normal, vectors is an offset only */
#  endif /* 0 */

#  if 0
extern UNSIGNED32 __Vectors;
	/* Keil RealARM or additional symbol in the Linker file */
	SCB->VTOR = (UNSIGNED32)&__Vectors;
#  endif /* 0 */

#  if 1
extern UNSIGNED32 g_pfnVectors;
	/* ST linker files for gcc */
	SCB->VTOR = (UNSIGNED32)&g_pfnVectors;
#  endif /* 1 */
#endif /* BOOT */
}


/***************************************************************************/
/**
* SystemClock_Config - System Clock Configuration
*
*/
void SystemClock_Config(void)
{
RCC_OscInitTypeDef RCC_OscInitStruct;
RCC_ClkInitTypeDef RCC_ClkInitStruct;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
						  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

	__HAL_RCC_PWR_CLK_ENABLE();

	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

#ifdef CODRV_USE_FREERTOS
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
#endif
}


#ifdef CODRV_USE_FREERTOS
void codrvWaitForEvent(UNSIGNED32 msecTimeout)
{
	/* wait until semaphore is unlocked */
	(void)xSemaphoreTake(semphCANopen, msecTimeout);
}
#endif /* USE_FREERTOS */
