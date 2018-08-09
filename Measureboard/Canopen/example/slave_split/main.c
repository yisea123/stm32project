/*
* main.c - contains program main
*
* Copyright (c) 2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 8310 2015-01-25 09:41:11Z boe $
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_canopen.h>

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
static RET_T sdoServerReadInd(BOOL_T execute, UNSIGNED8	sdoNr, UNSIGNED16 index,
		UNSIGNED8	subIndex);
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void commInd(CO_COMM_STATE_EVENT_T);
static void sdoReadCont(void *ptr);
static void sdoWriteCont(void *ptr);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED16	test;

/* local defined variables
---------------------------------------------------------------------------*/
static CO_TIMER_T	sdoReadTimer;
static CO_TIMER_T	sdoWriteTimer;


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

	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		exit(2);
	}

	/* CANopen Initialization */

    if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		exit(1);
	}

	/* register event functions */
	if (coEventRegister_SDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		exit(5);
	}
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		exit(7);
	}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		exit(13);
	}

	if (codrvCanEnable() != RET_OK)  {
		exit(14);
	}

	while (1)  {
    	coCommTask();

	}
}


/*********************************************************************/
static RET_T sdoServerReadInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	printf("sdo server read ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

	/* continue sdo indication later .... */
	coTimerStart(&sdoReadTimer, 100000u, sdoReadCont, NULL, CO_TIMER_ATTR_ROUNDUP);

	return(RET_SDO_SPLIT_INDICATION);
}


/*********************************************************************/
static void sdoReadCont(
		void *ptr
	)
{
	coSdoServerReadIndCont(1, RET_OK);
}


/*********************************************************************/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	printf("sdo server write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

	/* continue sdo indication later .... */
	coTimerStart(&sdoWriteTimer, 200000u, sdoWriteCont, NULL, CO_TIMER_ATTR_ROUNDUP);
	return(RET_SDO_SPLIT_INDICATION);
}


/*********************************************************************/
static void sdoWriteCont(
		void *ptr
	)
{
	coSdoServerWriteIndCont(1, RET_OK);
}


/*********************************************************************/
static void commInd(
		CO_COMM_STATE_EVENT_T	commEvent
	)
{
	switch (commEvent)  {
		case CO_COMM_STATE_EVENT_CAN_OVERRUN:
			printf("COMM-Event: CAN Overrun\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_FULL:
			printf("COMM-Event: Rec Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_OVERFLOW:
			printf("COMM-Event: Rec Queue Overflow\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_EMPTY:
			printf("COMM-Event: Rec Queue Empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_FULL:
			printf("COMM-Event: Tr Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_OVERFLOW:
			printf("COMM-Event: Tr Queue Empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_EMPTY:
			printf("COMM-Event: Tr Queue Empty\n");
			break;
		default:
			printf("COMM-Event: %d\n", commEvent);
			break;
	}

}

