/*
* main.c - enthält programm main
*
* Copyright (c) 2012-2014 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 8312 2015-01-25 09:42:42Z boe $
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
extern void coInitObjDictionary();

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void timeInd(CO_TIME_T *pTime);
static void sdoDomainInd(UNSIGNED16	index, UNSIGNED8 subIndex,
		UNSIGNED32 domainBufSize, UNSIGNED32 transferedSize);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
char myString[] = "noConst String1";
CO_DOMAIN_PTR  myDomain = NULL;
#define	DOM_SIZE	1000
UNSIGNED8	domain[DOM_SIZE];

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
UNSIGNED16	i;

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
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		exit(4);
	}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		exit(5);
	}
	if (coEventRegister_TIME(timeInd) != RET_OK)  {
		exit(6);
	}
	if (coEventRegister_SDO_SERVER_DOMAIN_WRITE(sdoDomainInd) != RET_OK)  {
		exit(7);
	}

	if (codrvCanEnable() != RET_OK)  {
		exit(8);
	}

	coOdDomainAddrSet(0x3002, 0, &domain[0], DOM_SIZE);

	for (i = 0; i < DOM_SIZE; i++)  {
		domain[i] = i & 0xff;
	}

printf("3001:0 is %x\n", coPdoObjIsMapped(1, 0x3001, 0));
printf("3002:0 is %x\n", coPdoObjIsMapped(1, 0x3002, 0));
printf("3003:0 is %x\n", coPdoObjIsMapped(1, 0x3003, 0));
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
static void timeInd(
		CO_TIME_T *pTime
	)
{
	printf("timeInd: msec: %lx, days: %x\n", pTime->msec, pTime->days);

}

/*********************************************************************/
static void sdoDomainInd(
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		UNSIGNED32	domainBufSize,
		UNSIGNED32	transferedSize
	)
{
UNSIGNED16	i;

	printf("sdoDomainInd: index %x:%d, actual domainBufSize %ld, transfered size is %ld\n",
		index, subIndex, domainBufSize, transferedSize);

	for (i = 0; i < domainBufSize; i++)  {
		printf("%02x ", domain[i]);
	}
	printf("\n");
}
