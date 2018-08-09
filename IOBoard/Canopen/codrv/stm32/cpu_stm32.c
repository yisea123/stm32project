/*
* cpu_stm32.c
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32.c 12592 2016-03-03 11:10:25Z ro $
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
*	CPU driver for Cortex-M3 from STM32 (e.g. STM32F103)
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

#if defined(STM32F4XX)
// #warning "compiles for STM32F4XX"

#include <stm32f4xx.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#else
 #include <stm32f10x.h>
#endif
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





#else
	/* enable CAN clock - e.g. PCKL1 = 36MHz */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  /* remap */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /* PB8/9 */
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

#else    /* STM32F107 ?? */

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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  /* CAN1 */

   /* Release CAN from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);
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
	/* switch ROM IRQ Table - better done in the startup file 
	 * at this point for better demonstration
	* Attention: There are Vector table alignment requirements!
	 */
	/* Keil */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (UNSIGNED32)&__Vectors); /* normal, vectors is an offset only */
	/* other */
	/* NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4100); */ /* normal, vectors is an offset only */
	/* NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4200); */ /* normal, vectors is an offset only */
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
	/* clock is initializing before main - SystemInit() */
	/*
	SetupClock();
	*/

	NVIC_Configuration();

#if defined(NO_PRINTF) || defined(CAN_PRINTF)
#else
# ifdef HW_OLIMEX
	SetupLED();
	SetupKBD();
# endif
	SetupUART(); /* req. for printf on Olimex */
#endif

	codrvHardwareCanInit();
}

