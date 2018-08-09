/*
* main.c - contains program main
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 18869 2017-06-27 12:29:08Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief main routine
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/

/* header of project specific types
---------------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#include <gen_define.h>
#include <co_canopen.h>

#include "mbx_function.h"

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern void appl1(void *ptr);

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
void errorHandler(BaseType_t errorCode);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
BOOL_T canInitialized = CO_FALSE;

/***************************************************************************/
/**
* \brief main entry
*/
int main(void)
{
BaseType_t ret = pdPASS;

	/* start application task */
	ret = xTaskCreate(appl1, "Appplication1Task", 1024/4, (void*)0,
			(tskIDLE_PRIORITY + 2),
			NULL);
	if (ret != pdPASS)  {
		errorHandler(2);
	}

	/* initialize and start CANopen task */
	ret = Mbx_Init_CAN(250u);
	if (ret != pdPASS)  {
		errorHandler(3);
	}

	/* start scheduler */
	osKernelStart();

	/* we should never get here as control is now taken by the scheduler */
	errorHandler(4);
}


void errorHandler(BaseType_t errorCode) {
	(void)errorCode;
	while(1){

	}
}
