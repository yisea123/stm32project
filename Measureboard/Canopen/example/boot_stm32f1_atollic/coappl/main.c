/*
* main.c - contains programm main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 6656 2014-08-08 08:55:20Z ro $
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
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_canopen.h>
#include <cpu_stm32.h>

#include <stm32f10x.h>

#ifdef BOOT
# include <fwupdate.h>
#endif


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
static RET_T sdoServerCheckWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex, const UNSIGNED8 *pData);
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void pdoInd(UNSIGNED16);
static void pdoRecEvent(UNSIGNED16);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void ledGreenInd(BOOL_T);
static void ledRedInd(BOOL_T);

static void myexit(int error);

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
UNSIGNED8	emcyData[5] = { 1, 2, 3, 4, 5 };

	/* HW initialization */
	codrvHardwareInit();

	if (codrvCanInit(250) != RET_OK)  {
		myexit(1);
	}
	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		myexit(2);
	}

    if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		myexit(1);
	}

	/* register event functions */
	if (coEventRegister_NMT(nmtInd) != RET_OK)  {
		myexit(3);
	}
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		myexit(4);
	}
	if (coEventRegister_SDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		myexit(5);
	}
	if (coEventRegister_SDO_SERVER_CHECK_WRITE(sdoServerCheckWriteInd) != RET_OK)  {
		myexit(6);
	}
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		myexit(7);
	}
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		myexit(8);
	}
	if (coEventRegister_PDO_REC_EVENT(pdoRecEvent) != RET_OK)  {
		myexit(9);
	}
	if (coEventRegister_LED_GREEN(ledGreenInd) != RET_OK)  {
		myexit(10);
	}
	if (coEventRegister_LED_RED(ledRedInd) != RET_OK)  {
		myexit(11);
	}
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		myexit(12);
	}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		myexit(13);
	}

	__enable_irq();

	if (codrvCanEnable() != RET_OK)  {
		myexit(14);
	}

	/* write emcy */
	if (coEmcyWriteReq(0x1234, &emcyData[0]) != RET_OK)  {
		myexit(15);
	}


	while (1)  {
    	coCommTask();
	}
}


/*********************************************************************/
static RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
	printf("nmtInd: New Nmt state %d - execute %d\n", newState, execute);

	return(RET_OK);
}


/*********************************************************************/
static void pdoInd(
		UNSIGNED16	pdoNr
	)
{
	printf("pdoInd: pdo %d received\n", pdoNr);
}


/*********************************************************************/
static void pdoRecEvent(
		UNSIGNED16	pdoNr
	)
{
	printf("pdoRecEvent: pdo %d time out\n", pdoNr);
}


/*********************************************************************/
static void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	printf("hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

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
	printf("sdo server read ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}


/*********************************************************************/
static RET_T sdoServerCheckWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		const UNSIGNED8	*pData
	)
{
	printf("sdo server check write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

   // return(RET_INVALID_PARAMETER);
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
#ifdef BOOT
UNSIGNED8 u8tmp;
RET_T retVal;
#endif

	printf("sdo server write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

#ifdef BOOT
	/*
	 * call bootloader after 'stop program' command
	 * write '0' to 0x1F51:1
	 * additional you can check passwords and so on
	 */
	if((index == 0x1F51) && (subIndex == 1)) {
		retVal = coOdGetObj_u8(0x1F51, 1, &u8tmp);
		if (retVal != RET_OK) {
			return retVal;
		}

		if (u8tmp != 0 /* Stop program */) {
			return (RET_OK);
		}

	 	jump2Bootloader();
	}
#endif


   // return(RET_INVALID_PARAMETER);
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
			printf("CAN: Passive\n");
			break;
		case CO_CAN_STATE_UNCHANGED:
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
		case CO_COMM_STATE_EVENT_CAN_OVERRUN:
			printf("COMM-Event CAN Overrun\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_FULL:
			printf("COMM-Event Rec Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_OVERFLOW:
			printf("COMM-Event Rec Queue Overflow\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_EMPTY:
			printf("COMM-Event Rec Queue Empty\n");
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
			printf("COMM-Event - unknown event %d\n", (int)commEvent);
			break;
	}

}

/*********************************************************************/
static void ledGreenInd(
		BOOL_T	on
	)
{
	printf("GREEN: %d\n", on);
}


/*********************************************************************/
static void ledRedInd(
		BOOL_T	on
	)
{
	printf("RED: %d\n", on);
}

/*********************************************************************/
static void myexit(
	int error
	)
{
	printf("myexit %d\n", error);
	while(1) {}
}
