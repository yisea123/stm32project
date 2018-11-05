/*
* cpu_stm32_hal_gpio.c
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_stm32_hal_gpio.c 19895 2017-09-25 07:25:36Z hil $
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
* This module contains the CPU specific routines,
* for the GPIO initialization.
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

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/
void codrvGPIO_Config(void);

/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/

/***************************************************************************/
/**
*
*/
void codrvGPIO_Config(void)
{
#ifdef CODRV_STM32F0
GPIO_InitTypeDef  GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();


    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif /* CODRV_STM32F0 */

#ifdef CODRV_STM32F1
GPIO_InitTypeDef  GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	__HAL_RCC_AFIO_CLK_ENABLE();

	/* Peripheral clock enable */
	__HAL_RCC_CAN1_CLK_ENABLE();

	/**CAN GPIO Configuration
	PB8     ------> CAN_RX
	PB9     ------> CAN_TX
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_AFIO_REMAP_CAN1_2();

	/* Release CAN from reset state */
	__HAL_RCC_CAN1_RELEASE_RESET();
#endif /* CODRV_STM32F1 */

#ifdef CODRV_STM32F2

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
#endif /* CODRV_STM32F2 */

#ifdef CODRV_STM32F3
GPIO_InitTypeDef  GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Peripheral clock enable */
	__HAL_RCC_CAN1_CLK_ENABLE();


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
#endif /* CODRV_STM32F3 */

#ifdef CODRV_STM32F4
GPIO_InitTypeDef  GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/* Peripheral clock enable */
	__HAL_RCC_CAN1_CLK_ENABLE();

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif /* CODRV_STM32F4 */

#ifdef CODRV_STM32L4
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
#endif /* CODRV_STM32L4 */
}
#endif /* defined(CODRV_STM32XX) */
