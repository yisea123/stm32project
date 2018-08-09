/*
* cpu_stm32f2_keil.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32f2_keil.c 12592 2016-03-03 11:10:25Z ro $
*
*
*-------------------------------------------------------------------
*
*/

/********************************************************************/
/**
* \file
* \brief 
*	CPU driver for Cortex-M3 from STM32 (e.g. STM32F207)
*
* This module contains the cpu specific routines for initialization
* and timer handling.
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
#include <stm32f2xx_hal.h>

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
* SysTick Interrupt - CANopen timer interrupt
*/
void SysTick_Handler (void) 
{
	codrvTimerISR();
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


/***************************************************************************/
/**
* \brief codrvTimerSetup - init Timer
*
* Start a cyclic hardware timer to provide timing interval.
* Alternativly it can be derived from an other system timer
* with the interval given from the DeviceDesigner.
*
* \param
*	none
* \results
*	RET_T
*/
RET_T codrvTimerSetup(
		UNSIGNED32 timerInterval
	)
{

	/* update SystemCoreClock */
	SystemCoreClockUpdate();
	(void)SysTick_Config((SystemCoreClock / 1000000ul) * timerInterval); 

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
* \brief codrvHardwareCanInit - CAN related hardware initialization
*
*/
void codrvHardwareCanInit(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;	
	
 
	  /* clock for GPIO A */
   	__GPIOA_CLK_ENABLE();
	 
	 /* enable CAN clock */
	__CAN1_CLK_ENABLE();

	/* CAN RX/TX Pin settings ================================ */

	/* CAN-RX pin A.11*/
	GPIO_InitStructure.Pin = GPIO_PIN_11;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* CAN-TX pin  A.12*/
	GPIO_InitStructure.Pin = GPIO_PIN_12;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
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
extern UNSIGNED32 __Vectors;
	/* switch ROM IRQ Table - better done in the startup file 
	 * at this point for better demonstration
	* Attention: There are Vector table alignment requirements!
	 */
	/* Keil */
	HAL_NVIC_SetVectorTable(NVIC_VectTab_FLASH, (UNSIGNED32)&__Vectors); /* normal, vectors is an offset only */
	/* other */
	/* NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4200); */ /* normal, vectors is an offset only */

#endif

#ifdef MUST_ADAPT
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

	/* req.
	 * - place in RAM - 0x100 aligned
	 * - copy/initialize interrupt table
	 */
#endif

    /* Configure two bit for preemption priority */
    HAL_NVIC_SetPriorityGrouping(2u);

	/* CAN interrupts 
     *-------------------------------------------------------*/

    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, canPriority, canSubPriority);
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, canPriority, canSubPriority);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn,canPriority, canSubPriority);

	/* enable CAN interrupts */
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
	

    /*-------------------------------------------------------*/

}

/***************************************************************************/
/**
* \brief 
*
*/
void codrvCanEnableInterrupt(void)
{

	/* enable CAN interrupts */
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_EnableIRQ(CAN1_TX_IRQn);
    NVIC_EnableIRQ(CAN1_SCE_IRQn);

}

/***************************************************************************/
/**
* \brief 
*
*/
void codrvCanDisableInterrupt(void)
{

	/* disable CAN interrupts */
    NVIC_DisableIRQ(CAN1_RX0_IRQn);
    NVIC_DisableIRQ(CAN1_TX_IRQn);
    NVIC_DisableIRQ(CAN1_SCE_IRQn);

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
//	HAL_NVIC_SetPendingIRQ(CAN2_TX_IRQn     /* -- */);
#endif 
}

/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initalization
*
*/
void codrvHardwareInit(void)
{
	/* clock is initializing before main - SystemInit() */
	//SetupClock();
	
	NVIC_Configuration();

	codrvHardwareCanInit();
}

