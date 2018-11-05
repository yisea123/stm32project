/*
* cpu_stm32f1_atollic.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32f1_atollic.c 19889 2017-09-22 09:02:43Z hil $
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
*	CPU driver for Cortex-M3 from STM32 (e.g. STM32F105)
*
* This module contains the cpu specific routines for initialization
* and timer handling.
*
* \author emtas GmbH
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>

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
#include <stm32f10x.h>

/* constant definitions
---------------------------------------------------------------------------*/
#if defined(CONFIG_BXCAN1) || defined(CONFIG_BXCAN2)
#else
	/* default CAN1 */
# define CONFIG_BXCAN1 1
/* #define CONFIG_BXCAN2 1 */
#endif

#ifdef CONFIG_BXCAN2
//# warning "This initialization was created for CAN1. Adapt it to CAN2 (GPIO, IRQ)!"
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

TODO:
- Wenn IRQ Tabelle im Flash -> vordefinierte Namen nutzen
- wenn IRQ Table im RAM -> Einsprünge konfigurieren
---------------------------------------------------------------------------*/

#ifdef STM32F10X_MD

#define CAN1_TX_IRQn USB_HP_CAN1_TX_IRQn
#define CAN1_RX0_IRQn USB_LP_CAN1_RX0_IRQn

#define CAN1_TX_IRQHandler  USB_HP_CAN1_TX_IRQHandler
#define CAN1_RX0_IRQHandler	USB_LP_CAN1_RX0_IRQHandler

#endif

/***************************************************************************/
/**
* SysTick Interrupt - CANopen timer interrupt
*/
void SysTick_Handler (void) 
{
	codrvTimerISR();
}

#ifdef CONFIG_BXCAN1
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

#endif

#ifdef CONFIG_BXCAN2
/***************************************************************************/
/**
* CAN-TX interrupt
*/
void CAN2_TX_IRQHandler(void)
{
	codrvCanTransmitInterrupt();
}

/***************************************************************************/
/**
* CAN-RX Fifo 0 
*/
void CAN2_RX0_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* CAN-RX Fifo 1 - not used! 
*/
void CAN2_RX1_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
}

/***************************************************************************/
/**
* CAN Error interrupt
*/
void CAN2_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
}

#endif

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

	/* fix setting - check configuration */
	if(timerInterval != 10000ul) {
		return(RET_INTERNAL_ERROR);
	}

	/* start hardware timer */

    /* SysTick event each 10 ms with input clock equal to 72MHz */
	/* SysTick - 
	 * HCLK == SysClk == 72MHz
	 * HCLK/8 = 9MHz
	 * 9MHz/90.000 = 100Hz
	 * 1/100Hz = 10ms
	 *
	 * SysTick->CTRL in SysTick_Config() is configured to 
	 * SysTick clock == HCLK
	 */
	(void)SysTick_Config(720000ul);

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


	/* enable CAN clock - e.g. PCKL1 = 36MHz */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  /* remap */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /* PB8/9 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  /* CAN1 (also for CAN2! */

#ifdef CONFIG_BXCAN2
	/* CAN1 clock must be active (Master of CAN2) */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
#endif


	/* CAN RX/TX Pin settings ================================ */

#ifdef CONFIG_BXCAN1
	/* CAN-RX pin */
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* CAN-TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* remap CAN to PB8/9 - Olimex */
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
#endif

#ifdef CONFIG_BXCAN2
	/* CAN-RX pin */
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* CAN-TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* remap CAN  */
    GPIO_PinRemapConfig(GPIO_Remap_CAN2, ENABLE);
#endif

   /* Release CAN from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, DISABLE);
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
UNSIGNED32 canEncodedPriority;


#ifdef BOOT
extern UNSIGNED32 __Vectors;
	/* move ROM IRQ Table */
	/*NVIC_SetVectorTable(NVIC_VectTab_FLASH, __Vectors);*/ /* normal, vectors is an offset only */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4200); /* normal, vectors is an offset only */
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


    NVIC_SetPriorityGrouping(2u);

	/* CAN interrupts 
     *-------------------------------------------------------*/
	canEncodedPriority =
        NVIC_EncodePriority(2u, canPriority, canSubPriority);

#ifdef CONFIG_BXCAN1
    NVIC_SetPriority(CAN1_RX0_IRQn, canEncodedPriority);
    NVIC_SetPriority(CAN1_TX_IRQn, canEncodedPriority);
    NVIC_SetPriority(CAN1_SCE_IRQn, canEncodedPriority);

	/* enable CAN interrupts */
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_EnableIRQ(CAN1_TX_IRQn);
    NVIC_EnableIRQ(CAN1_SCE_IRQn);
#endif

#ifdef CONFIG_BXCAN2
    NVIC_SetPriority(CAN2_RX0_IRQn, canEncodedPriority);
    NVIC_SetPriority(CAN2_TX_IRQn, canEncodedPriority);
    NVIC_SetPriority(CAN2_SCE_IRQn, canEncodedPriority);

	/* enable CAN interrupts -
	 * global interrupts must be disabled,
	 * because the CAN is not initialized, yet.
	 */
    NVIC_EnableIRQ(CAN2_RX0_IRQn);
    NVIC_EnableIRQ(CAN2_TX_IRQn);
    NVIC_EnableIRQ(CAN2_SCE_IRQn);
#endif

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
#ifdef CONFIG_BXCAN1
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_EnableIRQ(CAN1_TX_IRQn);
    NVIC_EnableIRQ(CAN1_SCE_IRQn);
#endif

#ifdef CONFIG_BXCAN2
    NVIC_EnableIRQ(CAN2_RX0_IRQn);
    NVIC_EnableIRQ(CAN2_TX_IRQn);
    NVIC_EnableIRQ(CAN2_SCE_IRQn);
#endif

}

/***************************************************************************/
/**
* \brief 
*
*/
void codrvCanDisableInterrupt(void)
{

#ifdef CONFIG_BXCAN1
	/* disable CAN interrupts */
    NVIC_DisableIRQ(CAN1_RX0_IRQn);
    NVIC_DisableIRQ(CAN1_TX_IRQn);
    NVIC_DisableIRQ(CAN1_SCE_IRQn);
#endif

#ifdef CONFIG_BXCAN2
	/* disable CAN interrupts */
    NVIC_DisableIRQ(CAN2_RX0_IRQn);
    NVIC_DisableIRQ(CAN2_TX_IRQn);
    NVIC_DisableIRQ(CAN2_SCE_IRQn);
#endif

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
	NVIC_SetPendingIRQ(CAN1_TX_IRQn     /* 19 */);
#elif defined(CONFIG_BXCAN2)
	NVIC_SetPendingIRQ(CAN2_TX_IRQn     /* -- */);
#endif 
}

/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initalization
*
*/
void codrvHardwareInit(void)
{

//	SystemInit();

	/* clock is initializing before main - SystemInit() */
	/*
	SetupClock();
	*/

	NVIC_Configuration();

#if defined(NO_PRINTF) || defined(CAN_PRINTF)
#else
	//SetupUART(); /* req. for printf on Olimex */
#endif

	codrvHardwareCanInit();
}

