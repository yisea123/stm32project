/*
* canopen_FreeRTOS.c - contains OS specific function definitions
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 16557 2017-01-16 09:07:23Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief OS specific function definitions
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/

/* header of project specific types
---------------------------------------------------------------------------*/

#include <gen_define.h>
#include <co_drv.h>
#include <codrv_FreeRTOS.h>
#include <stm32f4xx.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <task.h>

#ifdef CODRV_USE_FREERTOS
/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern void codrvTimerISR(void);			/* Timer interrupt routine */

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static void timerCallback(UNSIGNED32 value);
static RET_T codrvCreateLockObjects(void);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
SemaphoreHandle_t semphCANopen;			/* CANopen task lock */
SemaphoreHandle_t semphObjDir;			/* object dictionary lock */

/* local defined variables
---------------------------------------------------------------------------*/
static TimerHandle_t timerHandle = NULL;		/* CANopen timer object */


/***************************************************************************/
/**
* \brief codrvOSConfig - OS specific setup
*
* creates and starts the CANopen timer and
* creates objects to lock object directory and CANopen thread
*
* \return RET_T
*/
RET_T codrvOSConfig(void)
{
RET_T retVal = RET_DRV_ERROR;
const UNSIGNED32 canPriority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1;
const UNSIGNED32 canSubPriority = 0u;

	/* setup canPriority between
	 * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
	 * &
	 * configLIBRARY_LOWEST_INTERRUPT_PRIORITY
	 * (numerical lower is higher)*/

	/* CAN interrupts
     *-------------------------------------------------------*/
	HAL_NVIC_SetPriority(CAN1_TX_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, canPriority, canSubPriority);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, canPriority, canSubPriority);

	/* create and setup CANopen timer */
	retVal = codrvTimerSetup(CO_TIMER_INTERVAL);
	if (retVal != RET_OK) {
		return retVal;
	}

	/* create objects to lock object directory and CANopen thread */
	retVal = codrvCreateLockObjects();
	if (retVal != RET_OK) {
		return retVal;
	}

	return RET_OK;
}
#if 1

/***************************************************************************/
/**
* \brief codrvTimerSetup - initialize timer
*
* Start a cyclic hardware timer to provide timing interval.
* alternatively it can be derived from an other system timer
* with the interval given from the DeviceDesigner.
*
* \returns RET_T
*/
RET_T codrvTimerSetup(
		UNSIGNED32 timerInterval
	)
{
BaseType_t ret = pdPASS;

	/* define CANopenTimer */
	timerHandle = xTimerCreate("CANopenTimer", pdMS_TO_TICKS(timerInterval/1000), pdTRUE,
			(void*)0, (TimerCallbackFunction_t)timerCallback);
	if (timerHandle == NULL) {
		return RET_DRV_ERROR;
	}

	/* start CANopenTimer */
	ret = xTimerStart(timerHandle, 0ul);
	if (ret == pdFAIL) {
		return RET_DRV_ERROR;
	}

	return RET_OK;
}

#endif
/***************************************************************************/
/**
* \brief timerCallback - call timer interrupt service routine
*
* \returns void
*/
static void timerCallback(UNSIGNED32 value)
{
	(void)value;

	/* call timer interrupt service routine */
	codrvTimerISR();
}


/***************************************************************************/
/**
* \brief codrvCreateLockObjects - create lock objects
*
* Create objects to lock CANopen thread and CANopen object dictionary
*
* \returns RET_T
*/
static RET_T codrvCreateLockObjects(
		void
		)
{
	/* create and setup CANopen timer */
	semphObjDir = xSemaphoreCreateMutex();
	if(semphObjDir == NULL) {
		return RET_DRV_ERROR;
	}

	/* create objects to lock object directory and CANopen thread */
	semphCANopen = xSemaphoreCreateBinary();
	if (semphCANopen == NULL) {
		return RET_DRV_ERROR;
	}


	return RET_OK;
}


/***************************************************************************/
/**
* \brief codrvWaitForEvent - wait for timer or CAN event
*
* \returns void
*/
void codrvWaitForEvent(UNSIGNED32 msecTimeout)
{
	/* wait until semaphore is unlocked */
	(void)xSemaphoreTake(semphCANopen, msecTimeout);
}
#endif /* CODRV_USE_THREADX */

