/*
* main.c - contains program main
*
* Copyright (c) 2013-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 9269 2015-04-29 13:59:44Z boe $
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
* CANopen example application implementing the CiA 401 profile
* for generic IO devices.
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* in case of Linux, delay the main() loop */
#include <unistd.h> /* usleep() */


/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_canopen.h>
#include <co_p401.h>
#include <usr_401.h>

#include "terminal.h" 	/* for coloring LED output */

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
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static RET_T sdoServerReadInd(BOOL_T execute, UNSIGNED8	sdoNr, UNSIGNED16 index,
		UNSIGNED8	subIndex);
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void pdoInd(UNSIGNED16);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void ledGreenInd(BOOL_T);
static void ledRedInd(BOOL_T);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/


/***************************************************************************/
/**
* \brief main entry
*
* \param
*	nothing
* \results
*	nothing
*/
int main(
	void
  )
{
	/* hardware initialization */
	codrvHardwareInit();

	if (codrvCanInit(250) != RET_OK)  {
		exit(1);
	}
    if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		exit(1);
	}

	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		exit(2);
	}

	/* register event functions */
	if (coEventRegister_NMT(nmtInd) != RET_OK)  {
		exit(3);
	}
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		exit(4);
	}
	if (coEventRegister_SDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		exit(5);
	}
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		exit(7);
	}
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		exit(8);
	}
	if (coEventRegister_LED_GREEN(ledGreenInd) != RET_OK)  {
		exit(10);
	}
	if (coEventRegister_LED_RED(ledRedInd) != RET_OK)  {
		exit(11);
	}
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		exit(12);
	}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		exit(13);
	}

	/* --- 401 specific ------------------------------------------------*/
	/* profile 401 HW acess registration */
	if (co401Init() != RET_OK)  {
		printf("error init 401 profile handler\n");
		exit(14);
	}

	if (coEventRegister_401(
				byte_in_piInd,		/* digital in  HW API function   */
				byte_out_piInd,		/* digital out HW API function   */
				analog_in_piInd,	/* the analog in HW API function */
				analog_out_piInd	/* analog out HW API function    */
				) != RET_OK)  {
		printf("registration of 401 HW acess functions failed\n");
		exit(14);
	}
	/* -----------------------------------------------------------------*/


	/* all is ready now,
	 * enable CAN driver to receive and transmit CAN frames
	 */
	if (codrvCanEnable() != RET_OK)  {
		exit(14);
	}

	while (1) {
	    usleep(200);
    	coCommTask();
    	co401Task();
	}
}


/*********************************************************************
The following are definitions of communication indication functions.
The application can use these functions to process communication events
application specific. This example, implementing the CiA 401 profile,
is only using the indications to display it using simple printf() 
messages.
**********************************************************************/
/*********************************************************************/
RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
	printf("USR: nmtInd: New Nmt state %d - execute %d\n", newState, execute);

	return(RET_OK);
}


/*********************************************************************/
void pdoInd(
		UNSIGNED16	pdoNr
	)
{
	// printf("USR: pdoInd: pdo %d received\n", pdoNr);
}


/*********************************************************************/
void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	printf("USR: hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

    return;
}



/*********************************************************************/
static RET_T sdoServerReadInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	// printf("USR: sdo server read ind: exec: %d, sdoNr %d, index %x:%d\n",
	// 	execute, sdoNr, index, subIndex);

	return(RET_OK);
}


/*********************************************************************/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	// printf("USR: sdo server write ind: exec: %d, sdoNr %d, index %x:%d\n",
	//	execute, sdoNr, index, subIndex);

	return(RET_OK);
}


/*********************************************************************/
static void canInd(
	CO_CAN_STATE_T	canState
	)
{
	switch (canState)  {
		case CO_CAN_STATE_BUS_OFF:
			printf("USR: CAN: Bus Off\n");
			break;
		case CO_CAN_STATE_BUS_ON:
			printf("USR: CAN: Bus On\n");
			break;
		case CO_CAN_STATE_PASSIVE:
			printf("USR: CAN: Passiv\n");
			break;
		case CO_CAN_STATE_UNCHANGED:
			printf("USR: CAN: unchanged\n");
			break;
		default:
			break;
	}
}


/*********************************************************************/
static void commInd(
		CO_COMM_STATE_EVENT_T	commEvent
	)
{
	switch (commEvent)  {
		case CO_COMM_STATE_EVENT_BUS_OFF:
			printf("COMM-Event Bus Off\n");
			break;
		case CO_COMM_STATE_EVENT_BUS_OFF_RECOVERY:
			printf("COMM-Event Bus Off\n");
			break;
		case CO_COMM_STATE_EVENT_BUS_ON:
			printf("COMM-Event Bus On\n");
			break;
		case CO_COMM_STATE_EVENT_PASSIVE:
			printf("COMM-Event Bus Passive\n");
			break;
		case CO_COMM_STATE_EVENT_ACTIVE:
			printf("COMM-Event Bus Active\n");
			break;
		case CO_COMM_STATE_EVENT_CAN_OVERRUN:
			printf("COMM-Event CAN Overrun\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_FULL:
			printf("COMM-Event Rec Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_OVERFLOW:
			printf("COMM-Event Rec Queue Overflow\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_FULL:
			printf("COMM-Event Tr Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_OVERFLOW:
			printf("COMM-Event Tr Queue Empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_EMPTY:
			printf("COMM-Event Tr Queue Empty\n");
			break;
		default:
			break;
	}
}


/* define colors */
#define RED     ATTBOLD FGRED
#define GREEN   ATTBOLD FGGREEN

/* define LED index */
#define LGREEN	0
#define LRED	1

static int leds[2] = { 0 , 0 };

/*********************************************************************/
static void showLEDs(void)
{
    printf("> " GREEN "%c  " RED "%c"  ATTRESET "\r",
		leds[LGREEN] ? 'O' : ' ',
		leds[LRED]   ? 'O' : ' ');
	fflush(stdout);
}

/*********************************************************************/
static void ledGreenInd(
		BOOL_T	on
	)
{
	// printf("GREEN: %d\n", on);
	leds[LGREEN] = on;
	showLEDs();
}


/*********************************************************************/
static void ledRedInd(
		BOOL_T	on
	)
{
	// printf("RED: %d\n", on);
	leds[LRED] = on;
	showLEDs();
}

/* vim: set ts=4 sw=4 spelllang=en : */
