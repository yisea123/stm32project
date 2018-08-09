/*
* main.c - contains program main
*
* It implements a slave1 with Micrium OS III.
*
* Copyright (c) 2012 - 2017 emtas GmbH
*----------------------------------------------------------------------------
* $Id: main.c 18397 2017-05-31 13:46:38Z hil $
*
*
*----------------------------------------------------------------------------
*
*
*/

/***************************************************************************/
/**
* \file
* \brief main routine
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>						/* emtas CANopen stack defines */
#include <co_canopen.h>						/* emtas CANopen stack header */
#include <tx_api.h>							/* ThreadX API */
#include <stm32f4xx_hal.h>

/* constant definitions
---------------------------------------------------------------------------*/
#define DEMO_STACK_SIZE         1024
#define SPARE                   1024
#define DEMO_QUEUE_SIZE         100
#define DEMO_BLOCK_POOL_SIZE    100
#define NUMBER_TASK             9
#define DEMO_BYTE_POOL_SIZE     SPARE+ DEMO_STACK_SIZE + \
								(NUMBER_TASK*DEMO_STACK_SIZE) + \
								(2*DEMO_QUEUE_SIZE) + DEMO_BLOCK_POOL_SIZE
/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern void codrvWaitForEvent(UNSIGNED32 msecTimeout);	/* wait for event */
extern RET_T codrvOSConfig(void);

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static void applExit(int error);			/* application error handler */
static void initLED(void);					/* LED initialization */
/* indication function */
static RET_T nmtInd(						/* NMT state indication */
		BOOL_T	execute,
		CO_NMT_STATE_T newState);
static void hbState(						/* error control indication */
		UNSIGNED8 nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static RET_T sdoServerReadInd(				/* SDO server read indication */
		BOOL_T execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16 index,
		UNSIGNED8	subIndex);
static RET_T sdoServerCheckWriteInd(		/* SDO server check write event */
		BOOL_T execute,
		UNSIGNED8 node,
		UNSIGNED16 index,
		UNSIGNED8	subIndex,
		const UNSIGNED8 *pData);
static RET_T sdoServerWriteInd(				/* SDO server write indication */
		BOOL_T execute,
		UNSIGNED8 sdoNr,
		UNSIGNED16 index,
		UNSIGNED8	subIndex);
static void pdoInd(UNSIGNED16);				/* PDO receive indication */
static void pdoRecInd(UNSIGNED16);			/* PDO receive indication */
static void canInd(CO_CAN_STATE_T);			/* CAN state indication */
static void commInd(
		CO_COMM_STATE_EVENT_T commEvent);
static void ledGreenInd(BOOL_T);			/* CANopen LED indication */
static void ledRedInd(BOOL_T);				/* error LED indication */

/* CANopen functions */
void CANopenThread(ULONG thread_input);		/* start and run CANopen */

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static char				pool[DEMO_BYTE_POOL_SIZE];
static TX_THREAD		CANopen;
static TX_BYTE_POOL		byte_pool_0;
static int				error_count = 0;


/***************************************************************************/
/**
* \brief main - initialize the system and start the application and OS
*
* \returns int
*/
int main(
		void						/* nothing */
	)
{
	/* start ThreadX RTOS */
	tx_kernel_enter();

	return -1;
}


/***************************************************************************/
/**
* \brief tx_application_define - create application objects and threads
*
* \return void
*/
void tx_application_define(
		void *first_unused_memory
	)
{
CHAR    *pointer;
UINT    status;

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    status = tx_byte_pool_create(&byte_pool_0, "byte pool 0", pool, DEMO_BYTE_POOL_SIZE);
    if (status != TX_SUCCESS){
    	error_count++;
    }

    /* Allocate the stack for CANopen thread.  */
    status = tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    if (status) {
        error_count++;
    }

    /* Create the CANopen thread.  */
    status = tx_thread_create(&CANopen, "CANopen Thread", CANopenThread, 0,
            pointer, DEMO_STACK_SIZE,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    if (status){
        error_count++;
    }

    /* error handler */
    if (error_count > 0){
        applExit(0);
    }
}


/***************************************************************************/
/**
* \brief CANopenThread - initialize and start CANopen
*
* \return void
*/
void CANopenThread(ULONG thread_input)
{
UNSIGNED8 emcyData[5] = { 1, 2, 3, 4, 5 };

	/* Initialize all hardware needed by the CANopen stack:
	 * CAN controller clock,
	 * CAN controller RX/TX pins
	 */
	codrvHardwareInit();

	codrvOSConfig();

	/* Initialize used LEDs */
	initLED();

	/* initialize CAN */
	if (codrvCanInit(250) != RET_OK)  {
		applExit(1);
	}

	/* initialize CANopen stack */
	if (coCanOpenStackInit(NULL) != RET_OK)  {
		applExit(2);
	}

	/* OS specific setup */
	if (codrvOSConfig() != RET_OK)  {
		applExit(3);
	}

	/* register indication callback functions */

	/* register callback that is called
	 * if a NMT state change occurs. */
	if (coEventRegister_NMT(nmtInd) != RET_OK)  {
		applExit(4);
	}

	/* register callback that is called
	 * if an error control state changed. */
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		applExit(5);
	}

	/* register callback that is called
	 * before a SDO read request is executed. */
	if (coEventRegister_SDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		applExit(6);
	}

	/* register callback that is called
	 * before SDO write access is executed. */
	if (coEventRegister_SDO_SERVER_CHECK_WRITE(sdoServerCheckWriteInd)
			!= RET_OK)  {
		applExit(7);
	}

	/* register callback that is called
	 * after a SDO write access was finished. */
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		applExit(8);
	}

	/* register callback that is called
	 * if a PDO was received. */
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		applExit(9);
	}

	/* register callback that is called
	 * before a SDO read request is executed. */
	if (coEventRegister_PDO_REC_EVENT(pdoRecInd) != RET_OK)  {
		applExit(10);
	}

	/* register callback that is called
	 * if the green LED state has changed. */
	if (coEventRegister_LED_GREEN(ledGreenInd) != RET_OK)  {
		applExit(11);
	}

	/* register callback that is called
	 * if the red LED state has changed. */
	if (coEventRegister_LED_RED(ledRedInd) != RET_OK)  {
		applExit(12);
	}

	/* register callback that is called
	 * if the CAN state changed. */
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		applExit(13);
	}

	/* register callback that is called
	 * if the communication state has been changed. */
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		applExit(14);
	}

	/* enable CAN */
	if (codrvCanEnable() != RET_OK)  {
		applExit(15);
	}

	/* write an predefined EMCY message */
	if (coEmcyWriteReq(0x1234, &emcyData[0]) != RET_OK)  {
		applExit(16);
	}

	/* infinite loop */
	for(;;)
	{
		/* do CANopen job */
    	coCommTask();

		/* wait for CAN or timer event with timeout in in milliseconds
		 * or use 0 to wait forever */
    	codrvWaitForEvent(TX_WAIT_FOREVER);
	}
}


/***************************************************************************/
/**
* \brief initUserLED - initialize user LEDs
*
* \return void
*/
static void initLED()
{
GPIO_InitTypeDef GPIO_InitStruct;

	/* initialize CANopen LED */
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* initialize Error LED */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


/***************************************************************************/
/**
* \brief nmtInd - registered NMT state indication
*
* This function is called if a NMT state change occurs.
*
* \returns RET_T
*
* \retval RET_OK	- state change allowed 		(only valid for OPERATIONAL)
* \retval RET_*		- state change not allowed 	(only valid for OPERATIONAL)
*/
static RET_T nmtInd(
		BOOL_T			execute,	/* execute or test only */
		CO_NMT_STATE_T	newState	/* new NMT state */
	)
{
	/* print indication details */
	printf("nmtInd: New NMT state: %d - execute: %d\n",
			newState, execute);

	/* reject state change */
	//return(RET_INVALID_PARAMETER);

	/* allow state change */
	return(RET_OK);
}


/***************************************************************************/
/**
* \brief pdoRecEvent - registered PDO receive indication
*
* This function is called if a PDO was received.
*
* \return void
*/
static void pdoInd(
		UNSIGNED16	pdoNr			/* PDO number */
	)
{
	/* print indication details */
	printf("pdoInd: PDO: %d received\n", pdoNr);
}


/***************************************************************************/
/**
* \brief pdoRecEvent - registered asynchronous PDO receive indication
*
* This function is called if a asynchronous PDO was received.
*
* \return void
*/
static void pdoRecInd(
		UNSIGNED16	pdoNr			/* PDO number */
	)
{
	/* print indication details */
	printf("pdoRecEvent: PDO: %d timed out\n", pdoNr);
}


/***************************************************************************/
/**
* \brief hbState - registered error control indication
*
* This function is called if an error control state changed.
*
* \return void
*/
static void hbState(
		UNSIGNED8		nodeID,		/* node ID */
		CO_ERRCTRL_T 	state,		/* error control state */
		CO_NMT_STATE_T	nmtState	/* actual NMT state */
	)
{
	/* print indication details */
	printf("hbInd: HB indication: "
			"%d nodeID: %d nmtState: %d\n",
			state, nodeID, nmtState);

    return;
}


/***************************************************************************/
/**
* \brief sdoServerReadInd - registered SDO server read indication
*
* This function is called before a SDO read request is executed,
* so the application can update the data before the response is sent.
*
* \return RET_T
*
* \retval RET_OK	- confirm SDO read access
* \retval RET_*		- reject SDO read access
*/
static RET_T sdoServerReadInd(
		BOOL_T		execute,		/* execute or test only */
		UNSIGNED8	sdoNr,			/* SDO number */
		UNSIGNED16	index,			/* object index */
		UNSIGNED8	subIndex		/* object sub index */
	)
{
	/* print indication details */
	printf("SDO server read indication: "
			"execute: %d, sdoNr: %d, index: %x:%d\n",
			execute, sdoNr, index, subIndex);

	/* reject SDO read access */
	//return(RET_INVALID_PARAMETER);

	/* approve SDO read access */
	return(RET_OK);
}


/***************************************************************************/
/**
* \brief sdoServerCheckWriteInd - registered SDO server check-write indication
*
* This function is called before SDO write access is executed,
* so the application can reject a SDO write access.
*
* \return RET_T
*
* \retval RET_OK	- confirm SDO write access
* \retval RET_*		- reject SDO write access
*/
static RET_T sdoServerCheckWriteInd(
		BOOL_T		execute,		/* execute or test only */
		UNSIGNED8	sdoNr,			/* SDO number */
		UNSIGNED16	index,			/* object index */
		UNSIGNED8	subIndex,		/* object sub index */
		const UNSIGNED8	*pData		/* pointer to receive buffer */
	)
{
	/* print indication details */
	printf("SDO server check write indication: "
			"execute: %d, sdoNr: %d, index: %x:%d\n",
			execute, sdoNr, index, subIndex);

	/* reject SDO write access */
	//return(RET_INVALID_PARAMETER);

	/* approve SDO write access */
	return(RET_OK);
}


/***************************************************************************/
/**
* \brief sdoServerWriteInd - registered SDO server write indication
*
* This function is called, after a SDO write access was finished,
* so the application can confirm a SDO write access.
*
* \return RET_T
*
* \retval RET_OK	- confirm SDO write access
* \retval RET_*		- reject SDO write access
*/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,		/* execute or test only */
		UNSIGNED8	sdoNr,			/* SDO number */
		UNSIGNED16	index,			/* object index */
		UNSIGNED8	subIndex		/* object sub index */
	)
{
	/* print indication details */
	printf("SDO server write indication: "
			"execute: %d, sdoNr %d, index %x:%d\n",
			execute, sdoNr, index, subIndex);

	/* reject SDO write access */
	//return(RET_INVALID_PARAMETER);

	/* confirm SDO write access */
	return(RET_OK);
}


/***************************************************************************/
/**
* \brief canInd - registered CAN state indication
*
* This function is called, if the CAN state changed.
*
* \return void
*/
static void canInd(
	CO_CAN_STATE_T	canState		/* new CAN state */
	)
{
	/* print new CAN state */
	switch (canState)  {
		case CO_CAN_STATE_BUS_OFF:
			printf("canInd: Bus Off\n");
			break;
		case CO_CAN_STATE_BUS_ON:
			printf("canInd: Bus On\n");
			break;
		case CO_CAN_STATE_PASSIVE:
			printf("canInd: Passive\n");
			break;
		default:
			printf("canInd: %d\n", canState);
			break;
	}
}


/***************************************************************************/
/**
* \brief commInd - registered communication state event indication
*
* This function is called, if the communication state has been changed.
*
* \return void
*/
static void commInd(
		CO_COMM_STATE_EVENT_T	commEvent	/* new communication state */
	)
{
	/* print new communication state */
	switch (commEvent)  {
		case CO_COMM_STATE_EVENT_CAN_OVERRUN:
			printf("commInd: CAN overrun\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_FULL:
			printf("commInd: receive queue full\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_OVERFLOW:
			printf("commInd: receive queue overflow\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_EMPTY:
			printf("commInd: receive queue empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_FULL:
			printf("commInd: transmit queue full\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_OVERFLOW:
			printf("commInd: transmit queue overflow\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_EMPTY:
			printf("commInd: transmit queue empty\n");
			break;
		default:
			printf("commInd: %d\n", commEvent);
			break;
	}
}

/***************************************************************************/
/**
* \brief ledGreenInd - registered CANopen LED indication
*
* This function is called, if the green LED state has changed.
*
* \return void
*/
static void ledGreenInd(
		BOOL_T	on					/* set led on/off */
	)
{
	/* hardware control of the CANopen Green LED */
	/* Low level on Pin switches LED on. */
	if (on == CO_TRUE) {
		/* turn green LED on */
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	} else {
		/* turn green LED off */
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	}
}


/***************************************************************************/
/**
* \brief ledRedInd - registered error LED indication
*
* This function is called, if the red LED state has changed.
*
* \return void
*/
static void ledRedInd(
		BOOL_T	on					/* set led on/off */
	)
{
	/* hardware control of the CANopen Green LED */
	/* Low level on Pin switches LED on. */
	if (on == CO_TRUE) {
		/* turn green LED on */
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
	} else {
		/* turn green LED off */
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}


/***************************************************************************/
/**
* \brief applExit - application error handler
*
* \return void
*/
static void applExit(
	int error						/* error code */
	)
{
	/* infinite loop */
	while(1) {}
}
