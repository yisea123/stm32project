/*
* main.c - enthält programm main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 2031 2012-12-17 15:35:47Z  $
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

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T	startSdoReadTransfer(UNSIGNED8 sdo, UNSIGNED8 node,
		UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED8 *val, UNSIGNED32 size);
static RET_T	startSdoWriteTransfer(UNSIGNED8 sdo, UNSIGNED8	node,
		UNSIGNED16 index, UNSIGNED8	subIndex, UNSIGNED8 *val, UNSIGNED32 size);
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
char	strg[1000];


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
UNSIGNED16	i;

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

	/* start SDO transfers
	 * if the transfer is finished, the registered indication will be called 
	 */
	/* start sdo read transfer to node 32, index 0x1000:0, to variable devType*/
//	startSdoReadTransfer(1, 127, 0x1008, 0, (UNSIGNED8 *)strg, sizeof(strg));
//	startSdoReadTransfer(1, 127, 0x3002, 0, (UNSIGNED8 *)strg, sizeof(strg));
for (i = 0; i < sizeof(strg); i++)  {
	strg[i] = i & 0xff;
}
	startSdoWriteTransfer(1, 127, 0x3002, 0, (UNSIGNED8 *)strg, sizeof(strg));

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
		UNSIGNED8	*pVal,			/* pointer to variable */
		UNSIGNED32	size			/* max size */
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
	retVal = coSdoRead(sdo, index, subIndex, pVal, size, 1, 5000);

	return(retVal);
}


/*********************************************************************/
RET_T	startSdoWriteTransfer(
		UNSIGNED8	sdo,			/* sdo number */
		UNSIGNED8	node,			/* remote node */
		UNSIGNED16	index,			/* remote index */
		UNSIGNED8	subIndex,		/* remote subindex */
		UNSIGNED8	*val,			/* value to transfer */
		UNSIGNED32	size			/* transfer size */
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
	retVal = coSdoWrite(sdo, index, subIndex, val, size, 1, 5000);

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

	startSdoWriteTransfer(1, 127, 0x3001, 0, (UNSIGNED8 *)strg, sizeof(strg));
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
