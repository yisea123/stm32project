/*
* main.c - contains program main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 2027 2012-12-17 15:21:57Z  $
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
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void lssInd(CO_LSS_SERVICE_T service, UNSIGNED16 bitrate,
		UNSIGNED8 *pErrCode, UNSIGNED8 *pErrSpec);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED16	test;

/* local defined variables
---------------------------------------------------------------------------*/
UNSIGNED8	persistantNodeId = 255;

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
	/* HW initialization */
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
	if (coEventRegister_LSS(lssInd) != RET_OK)  {
		exit(4);
	}
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		exit(12);
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
RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
	printf("nmtInd: New Nmt state %d - execute %d\n", newState, execute);

	return(RET_OK);
}


/*********************************************************************/
static void canInd(
	CO_CAN_STATE_T	canState
	)
{
	switch (canState)  {
		case CO_CAN_STATE_BUS_OFF:
			printf("CAN: Bus Off\n");
			break;
		case CO_CAN_STATE_BUS_ON:
			printf("CAN: Bus On\n");
			break;
		case CO_CAN_STATE_PASSIVE:
			printf("CAN: Passiv\n");
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


/*********************************************************************/
static void lssInd(
		CO_LSS_SERVICE_T service,
		UNSIGNED16 bitrate,
		UNSIGNED8 *pErrCode,
		UNSIGNED8 *pErrSpec
	)
{
	switch (service)  {
		case CO_LSS_SERVICE_STORE:
			/* return ok */
			persistantNodeId = (UNSIGNED8)(bitrate & 0xff);
			*pErrCode = 0;
			*pErrSpec = 0;
			break;
		case CO_LSS_SERVICE_NEW_BITRATE:
			/* onlyl DS301 bitrates allowed */
			if (*pErrCode != 0)  {
				*pErrCode = 1;
				*pErrSpec = 255;
				return;
			}
			printf("New bitrate: %d received\n", bitrate);
			*pErrCode = 0;
			*pErrSpec = 0;
			break;
		case CO_LSS_SERVICE_NEW_NODE_ID:
			printf("New Node Id is %d\n", bitrate);
			break;

		case CO_LSS_SERVICE_BITRATE_OFF:
			printf("Set own Bitrate off\n");
			break;
		case CO_LSS_SERVICE_BITRATE_SET:
			printf("Set new Bitrate\n");
			break;
		case CO_LSS_SERVICE_BITRATE_ACTIVE:
			printf("Activate new Bitrate\n");
			break;
	}
}


unsigned char getNodeId()
{
	return(persistantNodeId);
}
