/*
* main.c - contains program main
*
* Copyright (c) 2012-15 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 8720 2015-03-06 10:47:13Z oe $
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
* This example shows how objects can be added to the object dictionary
* of the CANopen slave at run time.
* The file dynod.c with generic dynod functions have to be added to the project.
* In main.c it is shown how to objects 0x3000 and 0x3001 wit sub-indicies
* are added to the fixed object dictionary in gen_objdict.c
* created with the CANopen DeviceDesigner.
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
#include "dynod.h"

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
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void pdoInd(UNSIGNED16);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);


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
#define OBJ_IDX_CNT 2
#define VAR_U8	4
#define VAR_U16	1
RET_T	retVal;
UNSIGNED8	testVars_u8[VAR_U8] = { 4, 0x11, 0x22, 0x33};
UNSIGNED16	testVars_u16[VAR_U16] = { 0x1234} ;

	/* HW initialization */
	codrvHardwareInit();

	/* create 2 index */
	retVal = coDynOdInit(OBJ_IDX_CNT, VAR_U8, VAR_U16, 0, 0, 0, 0);
	if (retVal != RET_OK)  {
		printf("coDynOdInit returns: %d\n", retVal);
	}

	/* add index at 0x3000, 4 subs, array */
	retVal = coDynOdAddIndex(0x3000, 4, CO_ODTYPE_ARRAY);
	if (retVal != RET_OK)  {
		printf("coDynOdAddIndex returns: %d\n", retVal);
	}
	/* add subindex 0x3000:1..4 as U8 value */
	retVal = coDynOdAddSubIndex(0x3000, 1, CO_DTYPE_U8_PTR,
		CO_ATTR_NUM | CO_ATTR_READ, &testVars_u8[1]);
	if (retVal != RET_OK)  {
		printf("coDynOdAddSubIndex returns: %d\n", retVal);
	}
	retVal = coDynOdAddSubIndex(0x3000, 5, CO_DTYPE_U8_PTR,
		CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE, &testVars_u8[3]);
	if (retVal != RET_OK)  {
		printf("coDynOdAddSubIndex returns: %d\n", retVal);
	}
	retVal = coDynOdAddSubIndex(0x3000, 2, CO_DTYPE_U8_PTR,
		CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP, &testVars_u8[2]);
	if (retVal != RET_OK)  {
		printf("coDynOdAddSubIndex returns: %d\n", retVal);
	}
	retVal = coDynOdAddSubIndex(0x3000, 0, CO_DTYPE_U8_PTR,
		CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP, &testVars_u8[0]);
	if (retVal != RET_OK)  {
		printf("coDynOdAddSubIndex returns: %d\n", retVal);
	}

	/* add index at 0x3001, 1 subs, var */
	retVal = coDynOdAddIndex(0x3001, 1, CO_ODTYPE_VAR);
	if (retVal != RET_OK)  {
		printf("coDynOdAddIndex returns: %d\n", retVal);
	}
	/* add subindex 0x3001:0 as U16 value */
	retVal = coDynOdAddSubIndex(0x3001, 0, CO_DTYPE_U16_PTR,
		CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE, &testVars_u16[0]);
	if (retVal != RET_OK)  {
		printf("coDynOdAddSubIndex returns: %d\n", retVal);
	}

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
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		exit(7);
	}
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		exit(8);
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
void pdoInd(
		UNSIGNED16	pdoNr
	)
{
	printf("pdoInd: pdo %d received\n", pdoNr);
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
