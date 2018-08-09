/*
* main.c - contains program main
*
* Copyright (c) 2012-2015 emtas GmbH
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
//#define SET_BITRATE	1
//#define USE_FASTSCAN	1

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
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

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void lssInd(CO_LSS_MASTER_SERVICE_T service, UNSIGNED16 errorCode,
		UNSIGNED8 errorSpec, UNSIGNED32 *pIdentity);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED16	test;

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
	if (coEventRegister_LSS_MASTER(lssInd) != RET_OK)  {
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

coLssSwitchGlobal(CO_LSS_STATE_CONFIGURATION);
coLssInquireIdentity(1, 500);
//coLssSwitchSelective(0x319, 27, 445, 3, 5000);
	/* check for unconfigured nodes */
//	coLssIdentifyNonConfiguredSlaves(1000, 4);

	/* or start standard scan */
//	lssSearchNodes(0x319, 27, 0, 0xffffffff, 0, 0xffffffff, 50);

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
		CO_LSS_MASTER_SERVICE_T service,	/* service */
		UNSIGNED16	errorCode,				/* found/error */
		UNSIGNED8	errorSpec,				/* found/error */
		UNSIGNED32	*pIdentity				/* pointer to identity data */
	)
{
static UNSIGNED8	nodeId = 2;

	printf("LSS master indication service %d: errorCode %d - ", service, errorCode);

	/* time out ? */
	if (errorCode != 0)  {
		if (errorCode == 0xffff)  {
			printf("time out\n");
			lssStateHandler(0);
		} else {
			printf("error spec %d, abort handling\n", errorSpec);
		}
		/* ignore error for store service */
		if (service == CO_LSS_MASTER_SERVICE_STORE)  {
			printf("ignore store error - goto waiting \n");
			coLssSwitchGlobal(CO_LSS_STATE_WAITING);

#ifdef SET_BITRATE
			coLssSwitchGlobal(CO_LSS_STATE_CONFIGURATION);
			coLssActivateBitrate(2000);
#endif /* SET_BITRATE */
		}
	} else {
		switch (service)  {
			case CO_LSS_MASTER_SERVICE_NON_CONFIG_SLAVE:
				/* start fastscan while unconfigured nodes are available */
				printf("Start FastScan\n"),
//				coLssFastScanKnownDevice(0x319, 27, 445, 3, 20);
				coLssFastScan(20);
				break;

			case CO_LSS_MASTER_SERVICE_SWITCH_SELECTIVE:
				printf("switch selective was ok... \n");

				coLssSetNodeId(nodeId, 20);
				printf("set node id to %d\n", nodeId);
				nodeId++;
				break;

			case CO_LSS_MASTER_SERVICE_FASTSCAN:
				printf("node detected: 0x%lx 0x%lx 0x%lx 0x%lx\n",
					pIdentity[0], pIdentity[1],
					pIdentity[2], pIdentity[3]);
				coLssSetNodeId(nodeId, 20);
				printf("set node id to %d\n", nodeId);
				nodeId++;
				break;

			case CO_LSS_MASTER_SERVICE_SET_NODEID:
				printf("set node ok... \n");
				coLssInquireNodeId(20);
				printf("call LSS inquiry node id\n");
				break;

			case CO_LSS_MASTER_SERVICE_INQUIRE_NODEID:
				printf("Actual node id is %d\n", errorSpec);
#ifdef SET_BITRATE
				coLssSetBitrate(250, 1000);
#else /* SET_BITRATE */
				coLssStoreConfig(200);
				printf("call LSS store\n");
#endif /* SET_BITRATE */
				break;

			case CO_LSS_MASTER_SERVICE_STORE:
				printf("set LSS state waiting \n");
				coLssSwitchGlobal(CO_LSS_STATE_WAITING);

#ifdef SET_BITRATE
				coLssSwitchGlobal(CO_LSS_STATE_CONFIGURATION);
				coLssActivateBitrate(2000);
#endif /* SET_BITRATE */
				break;

			case CO_LSS_MASTER_SERVICE_SET_BITRATE:
				printf("Bitrate set answer is %d\n", errorSpec);
				coLssStoreConfig(200);
				printf("call LSS store\n");
				break;

			case CO_LSS_MASTER_SERVICE_BITRATE_OFF:
				printf("Set own Bitrate off\n");
				break;

			case CO_LSS_MASTER_SERVICE_BITRATE_SET:
				printf("Set new Bitrate\n");
				break;

			case CO_LSS_MASTER_SERVICE_BITRATE_ACTIVE:
				printf("Activate new Bitrate\n");
				break;

			case CO_LSS_MASTER_SERVICE_IDENTITY:
				printf("Identity answer received\n");
				lssStateHandler(1);
				break;

#ifdef CO_LSS_INQUIRY_SERVICES
			case CO_LSS_MASTER_SERVICE_INQUIRE_VENDOR:
				printf("Inquire vendor returns: 0x%04lx\n", coLssMasterGetInquireData());
				coLssInquireIdentity(2, 500);
				break;
			case CO_LSS_MASTER_SERVICE_INQUIRE_PRODUCT:
				printf("Inquire product returns: 0x%04lx\n", coLssMasterGetInquireData());
				coLssInquireIdentity(3, 500);
				break;
			case CO_LSS_MASTER_SERVICE_INQUIRE_REVISION:
				printf("Inquire revision returns: 0x%04lx\n", coLssMasterGetInquireData());
				coLssInquireIdentity(4, 500);
				break;
			case CO_LSS_MASTER_SERVICE_INQUIRE_SERIAL:
				printf("Inquire serial number returns: 0x%04lx\n", coLssMasterGetInquireData());
				break;
#endif /* CO_LSS_INQUIRY_SERVICES */

			default:
				printf("Unused service\n");
				break;

		}
	}
}
