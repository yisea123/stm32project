/*
* main.c - contains program main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 14734 2016-08-03 12:55:38Z phi $
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
extern void coInitObjDictionary();

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static void emcyConsumer(UNSIGNED8	node, UNSIGNED16	emcyErrorCode,
		UNSIGNED8	errorRegister, UNSIGNED8 const	*addErrorBytes);


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
CO_TIME_T       cotime;

	/* HW initialization */
	codrvHardwareInit();

	/* init can driver */
	if (codrvCanInit(250) != RET_OK)  {
		exit(1);
	}

	/* init canopen */
    if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		exit(1);
	}

	/* setup timer */
	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		exit(2);
	}

	/* register event functions */
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		exit(3);
	}
	if (coEventRegister_EMCY_CONSUMER(emcyConsumer) != RET_OK)  {
		exit(4);
	}
	if (codrvCanEnable() != RET_OK)  {
		exit(5);
	}

	/* send out time message */
	cotime.msec = 0x1234;
	cotime.days = 0x7890;
	coTimeWriteReq(&cotime);

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
static void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	printf("hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

	/* start node, if heartbeat of the node has been started */
	if (state == CO_ERRCTRL_HB_STARTED)  {
		coNmtStateReq(nodeId, CO_NMT_STATE_OPERATIONAL, CO_TRUE);
	}

    return;
}

/*********************************************************************/
static void emcyConsumer(
		UNSIGNED8	node,
		UNSIGNED16	emcyErrorCode,
		UNSIGNED8	errorRegister,
		UNSIGNED8	const *addErrorBytes
	)
{
	printf("emcyConsumer: node %d, EmcyErrorCode %x, errorRegister: %x, addBytes %x %x %x %x %x\n",
		node, emcyErrorCode, errorRegister, addErrorBytes[0],
		addErrorBytes[1], addErrorBytes[2], addErrorBytes[3], addErrorBytes[4]);
		
}
