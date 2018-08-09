/*
* cpu_stm32f0_keil.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32f0_keil.c 18660 2017-06-16 10:18:04Z ro $
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
*	CPU driver for Cortex-M0 from STM32 (e.g. STM32F072)
*
* This module contains the cpu specific routines for initialization
* and timer handling.
*
* This modul use the HAL functions from ST.
* The HAL use the Systick as timebase. Our Stack, too. In case of problems
* one part must be switch to a periodical timer.
* 
* \author emtas GmbH
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <string.h>

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
#include <stm32f0xx.h>
#include <stm32f0xx_hal.h>

/* constant definitions
---------------------------------------------------------------------------*/

/* OS related default definition */
#ifdef CO_OS_SIGNAL_TIMER
#else
#  define CO_OS_SIGNAL_TIMER()
#endif

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/
extern void	codrvCanTransmitInterrupt(void);
extern void codrvCanReceiveInterrupt(void);

/* list of global defined functions
---------------------------------------------------------------------------*/

void codrvInitDebugHardware(void);

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


/***************************************************************************/
/**
* SysTick Interrupt - CANopen timer interrupt
*
* The Systick is also used for HAL internal functionality!
*/
void SysTick_Handler (void) 
{
	// for HAL functionality
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
	
	codrvTimerISR();
}

/***************************************************************************/
/**
* CAN interrupt
*/
void CEC_CAN_IRQHandler(void)
{
	codrvCanReceiveInterrupt();
	codrvCanTransmitInterrupt();
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

  /* SysTick event each 10 ms with input clock equal to 48MHz */
	(void)SysTick_Config(480000ul);

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
GPIO_InitTypeDef  GPIO_InitStruct;	


	/* enable CAN clock - e.g. PCKL1 = 36MHz */
	__CAN_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	/* CAN RX/TX Pin settings ================================ */
	
	 /* CAN1 TX GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  GPIO_AF4_CAN;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* CAN1 RX GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; /* ??? bisher IPU */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  GPIO_AF4_CAN;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
   /* Release CAN from reset state */
	__CAN_FORCE_RESET();
	__CAN_RELEASE_RESET();

}

/***************************************************************************/
/**
* NVIC_Configuration - init NVIC controller
*
* Possible, that also the interrupt table is moving in the RAM
* (req. for Bootloader)
*
*/
#ifdef BOOT
UNSIGNED32 ramVectors[48] __attribute__((at(0x20000000)));  /* Keil */
#endif
void NVIC_Configuration(void)
{

#ifdef BOOT
extern UNSIGNED32 __Vectors;
	/* Cortex-M0 cannot switch ROM IRQ Tables
	 * The ROM table of the application is copied to 0x20000000.
	 *  After them the RAM is remaped to 0x00000000.
	 * The table is now at 0x00000000. The ram is visible at 0x20000000, too.
	 */
	memcpy(&ramVectors, &__Vectors, sizeof(ramVectors));
	
	//  Enable the SYSCFG peripheral clock
	__SYSCFG_CLK_ENABLE();
	__SYSCFG_RELEASE_RESET();
	
    //   Remap SRAM at 0x00000000
	__HAL_REMAPMEMORY_SRAM();
	
	__DMB();
    
#endif
	
	/* NVIC configuration for CAN1 Reception complete interrupt */
	HAL_NVIC_SetPriority(CEC_CAN_IRQn, 1u, 0u);
	HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);

}

/***************************************************************************/
/**
* \brief 
*
*/
void codrvCanEnableInterrupt(void)
{
	/* enable CAN interrupts */
	HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
}

/***************************************************************************/
/**
* \brief 
*
*/
void codrvCanDisableInterrupt(void)
{

	/* disable CAN interrupts */
	HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
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
	HAL_NVIC_SetPendingIRQ(CEC_CAN_IRQn);

}

/***************************************************************************/
/**
* \brief codrvInitHardware - general customer hardware initalization
*
*/
void codrvHardwareInit(void)
{
	
	HAL_Init();
	SystemClock_Config();

	NVIC_Configuration();

#if defined(NO_PRINTF) || defined(CAN_PRINTF)
#else
	SetupUART(); /* req. for printf */
#endif

	codrvHardwareCanInit();
	
//	codrvInitDebugHardware();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI48)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 48000000
  *            PREDIV                         = 2
  *            PLLMUL                         = 2
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  * (c) ST Micro
  */
#ifdef CUSTOMER_SYSCLOCK_CONFIG
#else
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Select HSI48 Oscillator as PLL source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
  {
    while(1);
  }
}
#endif



/***************************************************************************/
/**
* \brief codrvInitDebugHardware - LED initalization
*
* LEDS: PC6..PC9
*/
void codrvInitDebugHardware(void)
{
GPIO_InitTypeDef  GPIO_InitStruct;  
	
	/* Enable the GPIO LEDs Clock */
	__GPIOC_CLK_ENABLE();

	/* Configure the GPIO_LED pin */	  
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Alternate =  GPIO_AF4_CAN;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
#define BSRR_VAL 0x03C0

	  /* Reset PC6, PC7, PC8 and PC9 */
    GPIOC->BRR = BSRR_VAL; //OFF
    /* Set PC6, PC7, PC8 and PC9 */
    GPIOC->BSRR = BSRR_VAL; //ON

}
