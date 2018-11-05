/*
* main.c - contains program main
*
* Copyright (c) 2012-2016 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 18868 2017-06-27 10:18:01Z hil $
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

/* header of project specific types
---------------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#include <gen_define.h>
#include <co_canopen.h>
#include <cpu_stm32.h>

/* constant definitions
---------------------------------------------------------------------------*/

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

static void CANopenTask(void * argument);	/* start and run CANopen */
static void buttonTask(void * argument);	/* button handler */

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static BOOL_T sendPDO = CO_FALSE;

/***************************************************************************/
/**
* \brief main entry
*
* - This example shows the usage of FreeRTOS with emtas CANopen stack
*
* - 2 Tasks are configured:
* 	A user button task task with 1 priority higher than tskIDLE_PRIORITY,
* 	which initializes the user button and indicates if the button was pressed.
*	And a CANopen task with 2 priority higher than tskIDLE_PRIORITY,
*	which initializes CANopen and cyclic calls the stack functionality.
*
* - 1 Software timer is configured and started,
*	with a callback every 1 milliseconds,
*	which provides the timer tick to the CANopen stack
*
* - In Operational state the user LED can be activated or deactivated by
* 	sending the configured receive PDO (see mapTableRPDO0 in gen_objdict.c)
* 	and setting object 0x2001:0 to values:
*
* 	1 - to deactivate the LED
* 	0 - to deactivate the LED
*
* - The user button is used to iterate object 0x3000:1 value,
*	and sets a flag which is used by the CANopen Task to transmit a PDO.
*	To transmit the PDO the device has to be in Operational state!
*/
int main(void)
{
BaseType_t ret = pdPASS;

	/* create canopenTask */
	ret = xTaskCreate(CANopenTask, "CANopenTask", 1024/4, (void*)0,
			((tskIDLE_PRIORITY + 2) | portPRIVILEGE_BIT ),
			NULL);
	if (ret != pdPASS) {
		applExit(22);
	}

	/* create canopenTask */
	ret = xTaskCreate(buttonTask, "buttonTask", 512/4, (void*)0,
			((tskIDLE_PRIORITY + 1) | portPRIVILEGE_BIT ),
			NULL);

	/* initialize user LED */
	initLED();

	/* start scheduler */
	osKernelStart();

	/* we should never get here as control is now taken by the scheduler */
	/* infinite loop */
	while (1)
	{
	}
}


/***************************************************************************/
/**
* \brief CANopenTask - CANopen initialization + CANopen loop
*/
static void CANopenTask(void * argument)
{
UNSIGNED8 emcyData[5] = { 1, 2, 3, 4, 5 };

	/* Initialize all hardware needed by the CANopen stack:
	 * CAN controller clock,
	 * CAN controller RX/TX pins
	 */
	codrvHardwareInit();

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
		codrvWaitForEvent(portMAX_DELAY);

		/* send PDO if flag is set */
		if (sendPDO == CO_TRUE) {
			(void)coPdoReqNr(1u, 0u);
			sendPDO = CO_FALSE;
		}
	}
}


/***************************************************************************/
/**
* \brief buttonTask - initialize user button + button handler loop
*
* count up object 0x3000:1 value,
* and set a flag for manual PDO transmission
*
*/
static void buttonTask(void * argument)
{
GPIO_PinState ret = GPIO_PIN_RESET;
GPIO_InitTypeDef GPIO_InitStruct;
BOOL_T button_pressed = CO_FALSE;
INTEGER16 sub1 = 0;

	(void)argument;

	/* initialize user button */
	__GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	while (1) {
		/* get user button state */
		ret = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

		if ((ret == GPIO_PIN_RESET) && (button_pressed == CO_FALSE)) {

			/* get object 0x3000:1 value */
			(void)coOdGetObj_i16(0x3000, 1, &sub1);
			/* count object 0x3000:1 value up */
			sub1++;
			/* store object 0x3000:1 value */
			(void)coOdPutObj_i16(0x3000, 1, sub1);

			/* send PDO is not allowed in the application task,
			 * therefore it will, be sent in the CANopen task!
			 * set flag for PDO transmission
			 */
			sendPDO = CO_TRUE;
			button_pressed = CO_TRUE;

		} else if ((ret == GPIO_PIN_SET) && (button_pressed == CO_TRUE)) {
			button_pressed = CO_FALSE;
		}

		/* wait 1 milliseconds */
		vTaskDelay(pdMS_TO_TICKS(1));
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


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif
