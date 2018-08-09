/*
* cpu_stm32.c
*
* Copyright (c) 2012-2016 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32F4_coocox.c 12565 2016-03-02 10:10:04Z ro $
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
#include "can.h"
#include "stm32f4xx_it.h"
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


/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/
extern void	codrvCanTransmitInterrupt(void);
extern void codrvCanReceiveInterrupt(void);
extern void codrvCanErrorInterrupt(void);

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
- wenn IRQ Table im RAM -> Einspr�nge konfigurieren
---------------------------------------------------------------------------*/

#ifdef USE_HAL_DRV
/***************************************************************************/
/**
* SysTick Interrupt - CANopen timer interrupt
*/
void SysTick_Handler (void) 
{
	codrvTimerISR();
}
#endif
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
* CAN Error Interrupt 
*/
void CAN1_SCE_IRQHandler(void)
{
	codrvCanErrorInterrupt();
}

#if 0

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
	if(timerInterval != 1000ul) {
		return(RET_INTERNAL_ERROR);
	}
#ifdef USE_HAL_DRV
	/* start hardware timer */

    /* SysTick event each 1 ms
     * SystemCoreClockUpdate(); set SystemCoreClock
     * */

	(void)SysTick_Config(SystemCoreClock / 1000ul);
#else

	extern void MX_TIM6_Init(void);
	MX_TIM6_Init();
	extern TIM_HandleTypeDef htim6;
	HAL_TIM_Base_Start_IT(&htim6);

#endif
	return(RET_OK);
}

#endif

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
//	CO_OS_SIGNAL_CAN_STATE();
	CO_OS_SIGNAL_TIMER();
}



/*
 1.  Enable the CAN controller interface clock using
  *                  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); for CAN1
  *              and RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE); for CAN2
  *  @note   In case you are using CAN2 only, you have to enable the CAN1 clock.
  *
  *          2. CAN pins configuration
  *               - Enable the clock for the CAN GPIOs using the following function:
  *                   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
  *               - Connect the involved CAN pins to AF9 using the following function
  *                   GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_CANx);
  *                - Configure these CAN pins in alternate function mode by calling
  *                  the function  GPIO_Init();
  */




/***************************************************************************/
/**
* \brief codrvHardwareCanInit - CAN related hardware initialization
*
*/
void codrvHardwareCanInit(void)
{
#ifdef USE_HAL_DRV
GPIO_InitTypeDef  GPIO_InitStructure;	

/*
TODO
- wenn Vectortabelle im RAM
  -> CAN Interrupte eintragen
*/


#if defined(STM32F4XX)

#define CANx                       CAN1
#define CAN_FILTER_NMR	           0
#define CAN_CLK                    RCC_APB1Periph_CAN1
#define CAN_RX_PIN                 GPIO_Pin_0
#define CAN_TX_PIN                 GPIO_Pin_1
#define CAN_GPIO_PORT              GPIOD
#define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define CAN_AF_PORT                GPIO_AF_CAN1
#define CAN_RX_SOURCE              GPIO_PinSource0
#define CAN_TX_SOURCE              GPIO_PinSource1

	RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

#endif


	/* CAN RX/TX Pin settings ================================ */
#if defined(STM32F4XX)
	/* CAN-RX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

	/* CAN-TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   /* Push Pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);

#endif

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  /* CAN1 */

   /* Release CAN from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);

#endif

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
#ifdef USE_HAL_DRV


const UNSIGNED32 canPriority = 1u;
const UNSIGNED32 canSubPriority = 1u;
UNSIGNED32 canEncodedPriority;


#ifdef BOOT
//extern UNSIGNED32 __Vectors;
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

    NVIC_SetPriority(CAN1_RX0_IRQn, canEncodedPriority);
    NVIC_SetPriority(CAN1_TX_IRQn, canEncodedPriority);
    NVIC_SetPriority(CAN1_SCE_IRQn, canEncodedPriority);

	/* enable CAN interrupts */
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    NVIC_EnableIRQ(CAN1_TX_IRQn);
    NVIC_EnableIRQ(CAN1_SCE_IRQn);

    /*-------------------------------------------------------*/
#endif
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
	NVIC_SetPendingIRQ(CAN1_TX_IRQn     /* 19 */);
#elif defined(CONFIG_BXCAN2)
//	NVIC_SetPendingIRQ(CAN2_TX_IRQn     /* -- */);
#endif 
}

/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initalization
*
*/
void codrvHardwareInit(void)
{
#ifdef USE_HAL_DRV
	SystemInit();
	SystemCoreClockUpdate(); /* in case SystemCoreClock is req. */


	NVIC_Configuration();

	codrvHardwareCanInit();
#endif
	MX_CAN_Open_Init(125);
}

