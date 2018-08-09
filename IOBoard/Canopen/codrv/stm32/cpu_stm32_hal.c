/*
* cpu_stm32_hal.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32_hal.c 19895 2017-09-25 07:25:36Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief CPU driver for STM32
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

#if defined(CODRV_STM32F0) || \
	defined(CODRV_STM32F1) || \
	defined(CODRV_STM32F2) || \
	defined(CODRV_STM32F3) || \
	defined(CODRV_STM32F4) || \
	defined(CODRV_STM32L4)
#include <co_datatype.h>
#include <co_timer.h>
#include <co_drv.h>

/* hardware header
---------------------------------------------------------------------------*/
#ifdef CODRV_STM32F0
#include <stm32f0xx_hal.h>
#endif
#ifdef CODRV_STM32F1
#include <stm32f1xx_hal.h>
#endif
#ifdef CODRV_STM32F2
#include <stm32f2xx_hal.h>
#endif
#ifdef CODRV_STM32F3
#include <stm32f3xx_hal.h>
#endif
#ifdef CODRV_STM32F4
#include <stm32f4xx_hal.h>
#endif
#ifdef CODRV_STM32L4
#include <stm32l4xx_hal.h>
#endif

#include <codrv_bxcan.h>
#include <cpu_stm32.h>

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/
extern void SystemClock_Config(void);
extern void codrvGPIO_Config(void);

/* list of global defined functions
---------------------------------------------------------------------------*/
void codrvTimerISR( void);

/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
CAN_HandleTypeDef hcan;


/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initialization
*
*/
void codrvHardwareInit(void)
{
	/* initialize NVIC controller */
	NVIC_Configuration();

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

#ifdef CO_USER_CLOCK_SETTINGS
	CO_USER_CLOCK_SETTINGS();
#else /* CO_USER_CLOCK_SETTINGS */
	/* Configure the system clock */
	SystemClock_Config();
#endif /* CO_USER_CLOCK_SETTINGS */

	/* CAN related hardware initialization */
	codrvHardwareCanInit();
}


/***************************************************************************/
/**
* \brief codrvHardwareCanInit - CAN related hardware initialization
*
*/
void codrvHardwareCanInit(void)
{
	codrvGPIO_Config();
}


/***************************************************************************/
/**
* \brief codrvCanEnableInterrupt - enable CAN interrupts
*
*/
void codrvCanEnableInterrupt(void)
{
#ifdef CONFIG_BXCAN2
	/* enable CAN2 interrupts */
    HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    /* HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn); */
    HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
#else /* CONFIG_BXCAN2 */
#ifdef CODRV_STM32F0
    HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
#else /* CODRV_STM32F0 */
#ifdef CODRV_STM32F3
	/* enable CAN interrupts */
    HAL_NVIC_EnableIRQ(CAN_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN_RX0_IRQn);
    /* HAL_NVIC_EnableIRQ(CAN_RX1_IRQn);*/
    HAL_NVIC_EnableIRQ(CAN_SCE_IRQn);
#else /* CODRV_STM32F3 (STM32F1, STM32F2, STM32F4, STM32L4) */
	/* enable CAN1 interrupts */
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    /* HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);*/
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
#endif /* CODRV_STM32F3 */
#endif /* CODRV_STM32F0 */
#endif /* CONFIG_BXCAN2 */
}


/***************************************************************************/
/**
* \brief codrvCanDisableInterrupt - disable CAN interrupts
*
*/
void codrvCanDisableInterrupt(void)
{
#ifdef CONFIG_BXCAN2
	/* disable CAN2 interrupts */
	HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
	HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
	/*HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);*/
	HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
#else /* CONFIG_BXCAN2 */
#ifdef CODRV_STM32F0
	HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
#else /* CODRV_STM32F0 */
#ifdef CODRV_STM32F3
	/* disable CAN1 interrupts */
	HAL_NVIC_DisableIRQ(CAN_TX_IRQn);
	HAL_NVIC_DisableIRQ(CAN_RX0_IRQn);
	/* HAL_NVIC_DisableIRQ(CAN_RX1_IRQn);*/
    HAL_NVIC_EnableIRQ(CAN_SCE_IRQn);
#else /* CODRV_STM32F3 (STM32F1, STM32F2, STM32F4, STM32L4) */
	/* disable CAN1 interrupts */
	HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
	/* HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);*/
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
#endif /* CODRV_STM32F3 */
#endif /* CODRV_STM32F0 */
#endif /* CONFIG_BXCAN2 */
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
#ifdef CONFIG_BXCAN2
	/* set pending bit of the CAN2 transmit interrupt */
	HAL_NVIC_SetPendingIRQ(CAN2_TX_IRQn);
#else /* CONFIG_BXCAN2 */
#ifdef CODRV_STM32F0
	/* set pending bit of the CAN interrupt */
	HAL_NVIC_SetPendingIRQ(CEC_CAN_IRQn);
#else /* CODRV_STM32F0 */
#ifdef CODRV_STM32F3
	HAL_NVIC_SetPendingIRQ(CAN_TX_IRQn);
#else /* CODRV_STM32F3 (STM32F1, STM32F2, STM32F4, STM32L4) */
	/* set pending bit of the CAN1 transmit interrupt */
	HAL_NVIC_SetPendingIRQ(CAN1_TX_IRQn);
#endif /* CODRV_STM32F3 */
#endif /* CODRV_STM32F0 */
#endif /* CONFIG_BXCAN2 */
}


#ifdef CONFIG_BXCAN2
/***************************************************************************/
/**
* \brief CAN2_RX0_IRQHandler - CAN-RX interrupt
*
*/
void CAN2_RX0_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}


/***************************************************************************/
/**
* \brief CAN2_RX1_IRQHandler - unused
*
*/
void CAN2_RX1_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}


/***************************************************************************/
/**
* \brief CAN2_TX_IRQHandler - CAN-TX interrupt
*
*/
void CAN2_TX_IRQHandler(void)
{
	codrvCanTransmitInterrupt();
}


/***************************************************************************/
/**
* \brief CAN2_SCE_IRQHandler - CAN-Error interrupt
*
*/
void CAN2_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
}


#else /* CONFIG_BXCAN2 */
#ifdef CODRV_STM32F0
/***************************************************************************/
/**
* \brief CEC_CAN_IRQHandler - CAN interrupt
*
*/
void CEC_CAN_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
	codrvCanTransmitInterrupt();
	codrvCanErrorInterrupt();
}
#else /* CODRV_STM32F0 */
#ifdef CODRV_STM32F3
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
* \brief CAN_RX1_IRQHandler - CAN-RX interrupt
*
*/
void CAN_RX1_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
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
* \brief CAN_SCE_IRQHandler - CAN-Error interrupt
*
*/
void CAN_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
}
#else /* CODRV_STM32F3 (STM32F1, STM32F2, STM32F4, STM32L4) */
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
* \brief CAN1_RX1_IRQHandler - unused
*
*/
void CAN1_RX1_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
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
#endif /* CODRV_STM32F3 */
#endif /* CODRV_STM32F0 */
#endif /* CONFIG_BXCAN2 */

#ifdef CODRV_USE_OS
#else /* CODRV_USE_OS */
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
	HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0U);

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
#endif /* CODRV_USE_OS */


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
* NVIC_Configuration - initialize NVIC controller
*
* Possible, that also the interrupt table is moving in the RAM
* (required for Boot Loader)
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
#ifdef CODRV_USE_OS
#else /* CODRV_USE_OS */
	/* CAN interrupts
	 *-------------------------------------------------------*/
#ifdef CONFIG_BXCAN2
	HAL_NVIC_SetPriority(CAN2_TX_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN2_RX0_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN2_RX1_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN2_SCE_IRQn, canPriority, canSubPriority);
#else /* CONFIG_BXCAN2 */
#ifdef CODRV_STM32F0
	HAL_NVIC_SetPriority(CEC_CAN_IRQn, canPriority, canSubPriority);
#else /* CODRV_STM32F0 */
#ifdef CODRV_STM32F3
	HAL_NVIC_SetPriority(CAN_TX_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN_RX0_IRQn, canPriority, canSubPriority);
	/* HAL_NVIC_SetPriority(CAN_RX1_IRQn, canPriority, canSubPriority); */
	HAL_NVIC_SetPriority(CAN_SCE_IRQn, canPriority, canSubPriority);
#else /* CODRV_STM32F3 (STM32F1, STM32F2, STM32F4, STM32L4) */
	HAL_NVIC_SetPriority(CAN1_TX_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, canPriority, canSubPriority);
#endif /* CODRV_STM32F3 */
#endif /* CODRV_STM32F0 */
#endif /* CONFIG_BXCAN2 */
#endif /* CODRV_USE_OS */

	codrvCanEnableInterrupt();
}
#endif /* defined(CODRV_STM32XX) */
