/*
* main.c - contains program main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 9471 2015-06-12 14:42:40Z ro $
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
static RET_T	startSdoReadTransfer(UNSIGNED8 sdo, UNSIGNED8 node,
		UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED32 *val);
static RET_T	startSdoWriteTransfer(UNSIGNED8 sdo, UNSIGNED8	node,
		UNSIGNED16 index, UNSIGNED8	subIndex, UNSIGNED16 val);
static void sdoClientReadInd(UNSIGNED8, UNSIGNED16, UNSIGNED8, UNSIGNED32,
		UNSIGNED32);
static void sdoClientWriteInd(UNSIGNED8	sdoNr, UNSIGNED16 index,
		UNSIGNED8	subIndex, UNSIGNED32 errorVal);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
UNSIGNED32 devType;


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
	void		/**< kein Parameter */
  )
{
#ifdef CO_SDO_QUEUE
UNSIGNED8	u8;
UNSIGNED8	u16;
UNSIGNED8	u32;
#endif

	/* HW initialization */
	codrvHardwareInit();

	if (codrvCanInit(250) != RET_OK)  {
		exit(1);
	}
	if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		exit(1);
	}

	codrvTimerSetup(CO_TIMER_INTERVAL);

	codrvCanEnable();

	/* setup indication for SDO client */
	coEventRegister_SDO_CLIENT_READ(sdoClientReadInd);
	coEventRegister_SDO_CLIENT_WRITE(sdoClientWriteInd);

	/* start 2 SDO transfers
	 * use 2 SDO channels, so it can start in parallel 
	 * if the transfer is finished, the registered indication will be called 
	 */
	/* start sdo read transfer to node 32, index 0x1000:0, to variable devType*/
	startSdoReadTransfer(1, 32, 0x1000, 0, &devType);

	/* start sdo write transfer to node 33, index 0x1017:0, value 1000 */
	//startSdoWriteTransfer(2, 33, 0x1017, 0, 1000);
	startSdoWriteTransfer(2, 2, 0x5ffe, 2, 1000);

#ifdef CO_SDO_QUEUE
	coSdoQueueAddTransfer(0, 1, 0x1017, 0, &u16, 2, NULL, NULL);
	coSdoQueueAddTransfer(0, 1, 0x1400, 0, &u8, 1, NULL, NULL);
	coSdoQueueAddTransfer(0, 1, 0x1400, 1, &u32, 4, NULL, NULL);
	coSdoQueueAddTransfer(0, 1, 0x1800, 0, &u8, 1, NULL, NULL);
	coSdoQueueAddTransfer(0, 1, 0x1800, 1, &u32, 4, NULL, NULL);
#endif

	while (1)  {
    	coCommTask();
	}
}


/*********************************************************************/
RET_T	startSdoReadTransfer(
		UNSIGNED8	sdo,			/* sdo number */
		UNSIGNED8	node,			/* remote node */
		UNSIGNED16	index,			/* remote index */
		UNSIGNED8	subIndex,		/* remote subindex */
		UNSIGNED32	*pVal			/* pointer to variable */
	)
{
RET_T	retVal;

	/* setup cob-ids */
	retVal = coOdSetCobid(0x1280 + sdo - 1, 1, 0x600 + node);
	if (retVal != RET_OK)  {
		return(retVal);
	}
	retVal = coOdSetCobid(0x1280 + sdo - 1, 2, 0x580 + node);
	if (retVal != RET_OK)  {
		return(retVal);
	}

	/* start transfer */
	retVal = coSdoRead(sdo, index, subIndex, (UNSIGNED8 *)pVal, 4, 1, 5000);

	return(retVal);
}


/*********************************************************************/
RET_T	startSdoWriteTransfer(
		UNSIGNED8	sdo,			/* sdo number */
		UNSIGNED8	node,			/* remote node */
		UNSIGNED16	index,			/* remote index */
		UNSIGNED8	subIndex,		/* remote subindex */
		UNSIGNED16	val				/* value to transfer */
	)
{
RET_T	retVal;

	/* setup cob-ids */
	retVal = coOdSetCobid(0x1280 + sdo - 1, 1, 0x600 + node);
	if (retVal != RET_OK)  {
		return(retVal);
	}
	retVal = coOdSetCobid(0x1280 + sdo - 1, 2, 0x580 + node);
	if (retVal != RET_OK)  {
		return(retVal);
	}

	/* start transfer */
	//retVal = coSdoWrite(sdo, index, subIndex, (UNSIGNED8 *)&val, 2, 1, 5000);
	retVal = coSdoWrite(sdo, index, subIndex, (UNSIGNED8 *)&val, 14, 1, 5000);

	return(retVal);
}


/*********************************************************************/
static void sdoClientReadInd(
	UNSIGNED8	sdoNr,
	UNSIGNED16	index,
	UNSIGNED8	subIndex,
	UNSIGNED32	size,
	UNSIGNED32	errorVal
	)
{
	printf("SDO client %d READ transfer %x:%d size %ld returns 0x%lx\n",
		sdoNr, index, subIndex, size, errorVal);
}


/*********************************************************************/
static void sdoClientWriteInd(
	UNSIGNED8	sdoNr,
	UNSIGNED16	index,
	UNSIGNED8	subIndex,
	UNSIGNED32	errorVal
	)
{
	printf("SDO client %d WRITE transfer %x:%d returns 0x%lx\n",
		sdoNr, index, subIndex, errorVal);
}
