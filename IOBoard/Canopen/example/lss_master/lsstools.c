/*
* lsstools.c - contains lss routines
*
* Copyright (c) 2014 emtas GmbH
*-------------------------------------------------------------------
* $Id: $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief lss routine
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>

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
void lssSearchNodes(
		UNSIGNED32	vendorId,		/* vendor id */
		UNSIGNED32	productId,		/* product */
		UNSIGNED32	revisionIdLow,	/* revision number low */
		UNSIGNED32	revisionIdHigh,	/* revision number high */
		UNSIGNED32	serialIdLow,	/* serial number low */
		UNSIGNED32	serialIdHigh, 	/* serial number high */
		UNSIGNED32	timeOutVal
    );
void lssStateHandler(
		UNSIGNED8	answer
	);

/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static UNSIGNED8	scanState = 0;
static UNSIGNED32	vendor;
static UNSIGNED32	product;
static UNSIGNED32	serialLow;
static UNSIGNED32	serialMiddle;
static UNSIGNED32	serialHigh;
static UNSIGNED32	revisionLow;
static UNSIGNED32	revisionMiddle;
static UNSIGNED32	revisionHigh;
static UNSIGNED32	timeOut;


/***************************************************************************/
/**
* \brief lssSearchNode - search for unconfigered lss slave
*
* Search the network for nodes with the given parameter
*
* \return
*	nothing
*/
void lssSearchNodes(
		UNSIGNED32	vendorId,		/* vendor id */
		UNSIGNED32	productId,		/* product */
		UNSIGNED32	revisionIdLow,	/* revision number low */
		UNSIGNED32	revisionIdHigh,	/* revision number high */
		UNSIGNED32	serialIdLow,	/* serial number low */
		UNSIGNED32	serialIdHigh, 	/* serial number high */
		UNSIGNED32	timeOutVal
    )
{
	/* save values */
	timeOut = timeOutVal;
	vendor = vendorId;
	product = productId;
	serialLow = serialIdLow;
	serialMiddle = serialIdHigh;
	serialHigh = serialIdHigh;

	revisionLow = revisionIdLow;
	revisionMiddle = revisionIdHigh;
	revisionHigh = revisionIdHigh;

	/* start first scan */
	scanState = 1;
	lssStateHandler(1);
}


/***************************************************************************/
/**
* \brief lssStateHandler
*
* Manage LSS search states
*
* \return
*	nothing
*/
void lssStateHandler(
		UNSIGNED8	answer		/* answer from indication */
	)
{
	/* search for revision */
	if (scanState == 1)  {
		if (answer == 1)  {
			if (revisionLow == revisionMiddle)  {
				/* search serial number */
				scanState = 2;
			}
				
			revisionHigh = revisionMiddle;
			revisionLow &= 0xffffffff;
			revisionMiddle = (revisionHigh + revisionLow) / 2;
		} else {
			revisionLow = revisionMiddle + 1; 
			revisionMiddle = revisionHigh;
		}
	}

	/* search for serial number */
	if (scanState == 2)  {
		if (answer == 1)  {
			if (serialLow == serialMiddle)  {
				/* found, finish scan */
				scanState = 3;
			}

			serialHigh = serialMiddle;
			serialMiddle = (serialHigh + serialLow) / 2;
		} else {
			serialLow = serialMiddle + 1; 
			serialMiddle = serialHigh;
		}
	}

	/* start next scan */
	if ((scanState == 1)  || (scanState == 2))  {
		if (coLssIdentifyRemoteSlaves(vendor, product,
				revisionLow, revisionMiddle,
				serialLow, serialMiddle, timeOut) != RET_OK)  {
			printf ("error start new scan\n");
			return;
		}
	}

	/* scan finished */
	if (scanState == 3)  {
		printf ("Node found vendor %ld, product %ld, revision = %ld, serial = %ld\n",
			vendor, product, revisionMiddle, serialMiddle);
		/* configure node-id */
		coLssSwitchSelective(vendor, product, revisionMiddle, serialMiddle, 20);
		scanState = 0;
	}
}

