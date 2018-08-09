/*
* fwupdate.c - contains update functionality
*
* Copyright (c) 2014 emtas GmbH
*-------------------------------------------------------------------
* $Id: fwupdate.c 2269 2013-01-31 16:28:07Z  $
*
*
*-------------------------------------------------------------------
* Changelog:
*
*
*/

/********************************************************************/
/**
* \file
* \brief firmware update
* Jump to the bootloader for updating.
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>


#ifdef BOOT
#define COBL_USB 1

#include <co_datatype.h>
#include <fwupdate.h>

#include <stm32f4xx.h>
#ifdef COBL_USB
#include "usbd_core.h"
#endif


/* constant definitions
---------------------------------------------------------------------------*/



/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
//UNSIGNED8 cobl_command[16] __attribute__((section(".noinit"))); /* gcc */
UNSIGNED8 cobl_command[16] __attribute__((section("command"))); /* gcc */


/* local defined variables
---------------------------------------------------------------------------*/


/***************************************************************************/
/**
* \brief jump to the bootloader
*
* \param
*	nothing
* \results
*	nothing
*/
void jump2Bootloader(void)
{
#ifdef COBL_USB
extern volatile int openPort;
extern USBD_HandleTypeDef hUsbDeviceHS;
#endif

#ifdef COBL_USB
	while(openPort != 0) {
		;
	}
	(void)USBD_DeInit(&hUsbDeviceHS);
#endif

	/* disable all irqs */
	__disable_irq();

	memcpy( &cobl_command[0], COMMAND_BOOT, COMMAND_SIZE);
	/* request reset */	
	NVIC_SystemReset();

	while(1);
}

#endif /* BOOT */
/*********************************************************************/
