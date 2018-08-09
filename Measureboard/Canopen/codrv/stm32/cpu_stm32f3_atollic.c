/*
* cpu_stm32f3_atollic.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32f3_atollic.c 18368 2017-05-30 11:02:31Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief CPU driver for Cortex-M4 from STM32 (e.g. STM32F303)
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
#include <stm32f3xx_hal.h>

#include "codrv_bxcan.h"
#include "cpu_stm32.h"

/* hardware header
---------------------------------------------------------------------------*/

/* constant definitions
---------------------------------------------------------------------------*/
//#define PINS_PB8_PB9 1
//#define PINS_PD0_PD1 1
#if !defined(PINS_PB8_PB9) && !defined(PINS_PD0_PD1)
#error please define the pins you want to use with CAN! uncomment above or set in gen_define.h
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
void Error_Handler(void);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/

/* interrupt entries
---------------------------------------------------------------------------*/


/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initialization
*
*/
void codrvHardwareInit(void)
{
	NVIC_Configuration();

	/* Reset of all peripherals, Initializes the Flash interface and 
	 * the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	codrvHardwareCanInit();

}


/***************************************************************************/
/**
* \brief codrvHardwareCanInit - CAN related hardware initialization
*
*/
void codrvHardwareCanInit(void)
{
GPIO_InitTypeDef  GPIO_InitStruct;

	/* enable CAN clock */
	__CAN_CLK_ENABLE();

#ifdef PINS_PB8_PB9
    /* enable clock for Port B */
	__GPIOB_CLK_ENABLE();

	/* setup CAN pins
	 * CAN_RX - PB8
	 * CAN_TX - PB9
	 * */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN;

    /* initialize CAN pins */
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
#ifdef PINS_PD0_PD1
    /* enable clock for Port D */
	__GPIOD_CLK_ENABLE();

	/* setup CAN pins
	 * CAN_RX - PD0
	 * CAN_TX - PD1
	 * */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_CAN;

    /* initialize CAN pins */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
}


/***************************************************************************/
/**
* \brief codrvCanEnableInterrupt - enable CAN interrupts
*
*/
void codrvCanEnableInterrupt(void)
{
	/* enable CAN interrupts */
    HAL_NVIC_EnableIRQ(CAN_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN_RX0_IRQn);
    /* HAL_NVIC_EnableIRQ(CAN_RX1_IRQn);*/
    HAL_NVIC_EnableIRQ(CAN_SCE_IRQn);
}


/***************************************************************************/
/**
* \brief codrvCanDisableInterrupt - disable CAN interrupts
*
*/
void codrvCanDisableInterrupt(void)
{
	/* disable CAN interrupts */
    HAL_NVIC_DisableIRQ(CAN_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN_RX0_IRQn);
    /* HAL_NVIC_DisableIRQ(CAN_RX1_IRQn); */
    HAL_NVIC_DisableIRQ(CAN_SCE_IRQn);
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
	HAL_NVIC_SetPendingIRQ(CAN_TX_IRQn     /* 19 */);
}


/***************************************************************************/
/**
* \brief CAN_TX_IRQHandler - CAN-TX interrupt
*
*/
void CAN_TX_IRQHandler(void)
{
	codrvCanTransmitInterrupt();
}


/***************************************************************************/
/**
* \brief CAN_RX0_IRQHandler - CAN-RX interrupt
*
*/
void CAN_RX0_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* \brief CAN_RX1_IRQHandler - unused!
*
*/
void CAN_RX1_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* \brief CAN_SCE_IRQHandler - error interrupt
*
*/
void CAN_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
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

	/* signal in case of use of an OS */
	CO_OS_SIGNAL_TIMER();
}


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


/***************************************************************************/
/**
* NVIC_Configuration - initialize NVIC controller
*
* Customer Todo: Adapt the correct interrupt priorities for
*                your system.
*
*/
void NVIC_Configuration(void)
{
const UNSIGNED32 canPriority = 1u;
const UNSIGNED32 canSubPriority = 1u;

	/* Priority group in HAL_MspInit() */

	/* CAN interrupts
     *-------------------------------------------------------*/
    HAL_NVIC_SetPriority(CAN_TX_IRQn, canPriority, canSubPriority);
    HAL_NVIC_SetPriority(CAN_RX0_IRQn, canPriority, canSubPriority);
/*    HAL_NVIC_SetPriority(CAN_RX1_IRQn, canPriority, canSubPriority);*/
    HAL_NVIC_SetPriority(CAN_SCE_IRQn, canPriority, canSubPriority);

	codrvCanEnableInterrupt();
}


/***************************************************************************/
/**
* SystemClock_Config - System Clock Configuration
*
* HSE clock is the 8 MHz MCO clock from the Discovery board Debug interface.
*
*/
void SystemClock_Config(void)
{
RCC_OscInitTypeDef RCC_OscInitStruct;
RCC_ClkInitTypeDef RCC_ClkInitStruct;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


