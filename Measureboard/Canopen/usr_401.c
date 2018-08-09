/*
* usr_401.c -  process io functions needed for co_p401.c 
*
* Copyright (c) 2013 emtas GmbH
*-------------------------------------------------------------------
* $Id: usr_401.c 10124 2015-08-04 13:12:58Z ro $
*
*
*-------------------------------------------------------------------
*
*
*/


/********************************************************************/
/**
* \brief Implements hardware access to fulfill co_p401.c requirements
*
* These functions are called by the CANopen library module co_p401
* which implements the CiA 401 profile behaviour.
* The user has to register the functions to the CANopen stack
* before the stack is activated.
*
* \file usr_401.c
*
*/


/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdlib.h>		/* system() */

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>


#ifdef CO_PROFILE_401
#include <co_datatype.h>
#include <co_timer.h>

#include <usr_401.h>

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

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
* \brief - set an 8 bit digital output - TraceMsg(TSK_ID_CAN_MASTER,)
*
* This simple version uses only TraceMsg(TSK_ID_CAN_MASTER,) to print the information
* to the stdout of the process.
*
* This function has to be registered using coEventRegister_401()
* \code
* 	coEventRegister_401(NULL, byte_out_printfInd, NULL, NULL)
* \endcode
*
* \return void
*
*/
void byte_out_printfInd(
		UNSIGNED8 port,			/**< selected 8bit port */
		UNSIGNED16 outVal		/**< output value */
	)
{
	TracePrint(TSK_ID_CAN_MASTER,"digout port:%2d, value:0x%02x, %3d\n",
			port, outVal, outVal);
}


/***************************************************************************/
/**
* \brief - set an 8 bit digital output - process image
*
* This version uses  a virtual file system
* where digout ports are files named like the port number
* under the directory digout.
* The port value is written there as decimal value.
*
* This function has to be registered using coEventRegister_401()
* \code
* 	coEventRegister_401(NULL, byte_out_piInd, NULL, NULL)
* \endcode
*
* \return void
*
*/

void byte_out_piInd(
		UNSIGNED8 port,			/**< selected 8bit port */
		UNSIGNED8 outVal		/**< output value */
	)
{
	TracePrint(TSK_ID_CAN_MASTER, "DO: P%d > %d\n", port, outVal );
	//system(command);
}

/***************************************************************************/
/**
* \brief - read an 8 bit digital input - process image
*
* This version uses  a virtual file system
* where digin ports are files named like the port number
* under the directory digin.
* The port value is read there as decimal value.
*
* This function has to be registered using coEventRegister_401()
* \code
* 	coEventRegister_401(byte_in_piInd, NULL, NULL, NULL)
* \endcode
*
* \return void
*
*/

UNSIGNED8 byte_in_piInd(
		UNSIGNED8 port,			/**< selected 8bit port */
		UNSIGNED8 filter		/**< port filter value */
	)
{
	static UNSIGNED8 value = 1;

	(void)(port);
	(void)filter;			/* not used in this version */
	value++;
//	TraceMsg(TSK_ID_CAN_MASTER, "Get digit in %d \n", port);
	/* TraceMsg(TSK_ID_CAN_MASTER,"Input from port %d, val=0x%02x\n", port, value); */
	return value;
}



/***************************************************************************/
/**
* \brief - set an 16bit analog output - TraceMsg(TSK_ID_CAN_MASTER,)
*
* This simple version uses only TraceMsg(TSK_ID_CAN_MASTER,) to print the information
* to the stdout of the process.
*
* This function has to be registered using coEventRegister_401()
* \code
* coEventRegister_401(NULL, NULL, NULL, analog_out_printfInd)
* \endcode
*
* \return void
*
*/
void analog_out_printfInd(
		UNSIGNED8 port,			/**< selected 16bit analog channel */
		INTEGER16 outVal		/**< output value */
	)
{
	TracePrint(TSK_ID_CAN_MASTER,"analog out port:%2d, value:%5d\n",
			port, outVal);
}

/***************************************************************************/
/**
* \brief - set an 16bit analog output - process image
*
* This version uses a virtual file system
* where anout ports are files named like the port number
* under the directory anout.
* The port value is written there as decimal value.
*
* \note 
* All analog channels are 16 bit integer.
* If hardware has a lower resolution, use the most left aligned bits.
*
* This function has to be registered using coEventRegister_401()
* \code
* 	coEventRegister_401(NULL, NULL, NULL, analog_out_piInd)
* \endcode
*
*  \return void
*
*/
void analog_out_piInd(
		UNSIGNED8 port,			/**< selected 16bit analog channel */
		INTEGER16 outVal		/**< output value */
	)
{
	char command[100];

	TracePrint(TSK_ID_CAN_MASTER,"analog out port:%2d, value:%5d\n",
			port, outVal);
	snprintf(command, 100, "/bin/echo %d > anout/%d\n", outVal, port);
	system(command);
}

/***************************************************************************/
/**
* \brief - read an 16 bit analog input - process image
*
* This version uses  a virtual file system
* where anin ports are files named like the port number
* under the directory anin.
* The port value is read there as decimal value.
*
* \note 
* All analog channels are 16 bit integer.
* If hardware has a lower resolution, use the most left aligned bits.
*
* This function has to be registered using coEventRegister_401()
* \code
* 	coEventRegister_401(NULL, NULL, analog_in_piInd, NULL)
* \endcode
*
* \return void
*
*/

INTEGER16 analog_in_piInd(
		UNSIGNED8 port			/**< selected 8bit port */
	)
{
	INTEGER16 value = 100;
	TracePrint(TSK_ID_CAN_MASTER,"Input from port %2d, val=%7d\n", port, value);
	return value;
}




#endif /* CO_PROFILE_401 */
/* vim: set ts=4 sw=4 spelllang=en : */
