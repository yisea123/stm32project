/*
* codrv_threadx.c - contains OS specific function definitions
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
#ifdef CODRV_USE_THREADX
#include <gen_define.h>
#include <co_drv.h>
#include <codrv_threadx.h>
#include <stm32f4xx.h>

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
TX_MUTEX mutexCANopen; 							/* CANopen task lock */
TX_MUTEX mutexObjDir;							/* object dictionary lock */

/* local defined variables
---------------------------------------------------------------------------*/
static TX_TIMER CANopenTimer;					/* CANopen timer object */


/***************************************************************************/
/**
* \brief codrvOSConfig - setup OS specific settings
*
* creates and starts the CANopen timer and
* creates objects to lock object directory and CANopen thread
*
* \returns RET_T
*/
RET_T codrvOSConfig(void)
{
RET_T retVal = RET_DRV_ERROR;

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
UINT retVal = TX_TIMER_ERROR;

	/* create CANopenTimer */
	retVal = tx_timer_create(
			&CANopenTimer,
			"CANopen Timer",
			timerCallback,
			0,
			timerInterval/1000,
			timerInterval/1000,
			TX_NO_ACTIVATE);
	if (retVal != RET_OK)  {
		return RET_DRV_ERROR;
	}

	/* start CANopenTimer */
	retVal = tx_timer_activate(&CANopenTimer);
	if (retVal != RET_OK)  {
		return RET_DRV_ERROR;
	}

	return RET_OK;
}


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
UINT status;

	/* Create the semaphore used by CANopen thread.  */
	status = tx_mutex_create(&mutexCANopen, "CANopen Mutex", TX_NO_INHERIT);
	if (status != TX_SUCCESS){
		return RET_DRV_ERROR;
	}

	/* Create the semaphore used by CANopen thread.  */
	status = tx_mutex_create(&mutexObjDir, "Object Directory Mutex", TX_NO_INHERIT);
	if (status != TX_SUCCESS){
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
	tx_mutex_get(&mutexCANopen, msecTimeout);
}
#endif /* CODRV_USE_THREADX */
