/*
* mbx_function.c - contains mailbox function
*
* have to be adapted to operating system specification
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------*/


/********************************************************************/
/**
* \file
* \brief mailbox function
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>		/* For O_* constants */
#include <sys/stat.h>	/* For mode constants */
#include <errno.h>
#include <cmsis_os.h>

#include <co_canopen.h>
#include "mbx_function.h"

/* header of project specific types
---------------------------------------------------------------------------*/

/* constant definitions
---------------------------------------------------------------------------*/

/* command definition for CANopen functions 
---------------------------------------------------------------------------*/

/* local defined data types 
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern void canopenTask(void *ptr);

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
BaseType_t Mbx_Create_QueueSet(void);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
SemaphoreHandle_t semphCANopen = NULL;	/* for locking CANopen thread*/
TaskHandle_t pCOthread;					/* CANopen task handle */

QueueSetHandle_t queueSet = NULL;
QueueHandle_t	mqCmd = NULL;			/* command message queue */
QueueHandle_t	mqResp = NULL;			/* response message queue */
QueueHandle_t	mqEvent = NULL;			/* event message queue */


/* local defined variables
---------------------------------------------------------------------------*/
static UNSIGNED32	cmdIdent = 0;

/***************************************************************************/
/**
* \brief Mbx_Init_CAN
*
* Create command queue
* setup semaphore for object dictionary locking
* Instantiate CANopen task with the given bit rate
*
*
*/
int Mbx_Init_CAN(
		UNSIGNED16	bitRate			/**< bit rate for CANopen task */
	)
{
static UNSIGNED16 coArgs;
BaseType_t	ret = 0;

	/* save bit rate as arguments for CANopen task */
	coArgs = bitRate;

	/* start CANopen task */
	ret = xTaskCreate(canopenTask, "CANopenTask", 1024/4, &coArgs,
			((tskIDLE_PRIORITY + 3) | portPRIVILEGE_BIT ),
			&pCOthread);
	if (ret != pdPASS)  {
		return(pdFALSE);
	}

	return(ret);
}


/***************************************************************************/
/**
* \brief Mbx_DeInit_CAN
*
* Cancel CANopen task
*
*/
int Mbx_DeInit_CAN(
		void
	)
{
	vTaskDelete(pCOthread);
	if (pCOthread != NULL)  {
		return(-1);
	}

	return(1);
}


/***************************************************************************/
/**
* \brief Mbx_Init_CmdMailBox - open command mailbox
*
* open command mailbox for read/write or write only access
*
* \return QueueHandle_t
*
*/
QueueHandle_t Mbx_Init_CmdMailBox(
	)
{
QueueHandle_t	mqCmd;

	mqCmd = xQueueCreate(10, sizeof(MBX_COMMAND_T));
	if (mqCmd == NULL)  {
		return(NULL);
	}

	return(mqCmd);
}


/***************************************************************************/
/**
* \brief Mbx_Init_ResponeMailBox - create response mailbox
*
* create response mailbox
*
* \return QueueHandle_t
*
*/
QueueHandle_t Mbx_Init_ResponseMailBox(
		QueueHandle_t	mqCmd			/**< command mailbox */
	)
{
MBX_COMMAND_T	queueName;
QueueHandle_t mqResp = NULL;
int		ret;

	mqResp = xQueueCreate(10,sizeof(MBX_RESPONSE_T));
	if (mqResp == NULL)  {
		return(NULL);
	}

	/* send it to CANopen process */
	queueName.respId = mqResp;
	queueName.cmd = MBX_CMD_RESP_MBOX;
	queueName.answer = 0;
	ret = Mbx_PostCommandMbx(mqCmd, &queueName);
	if (ret == 0)  {
		return(NULL);
	}

	return(mqResp);
}


/***************************************************************************/
/**
* \brief Mbx_Init_EventMailBox - create event mailbox
*
* create a event Mailbox
*
* \return QueueHandle_t
*/
QueueHandle_t Mbx_Init_EventMailBox(
		QueueHandle_t	mqCmd		/**< command mailbox */
	)
{
MBX_COMMAND_T	queueName;
QueueHandle_t	mqEvent = NULL;
UNSIGNED32		ret;

	mqEvent = xQueueCreate(10,sizeof(MBX_EVENT_T));
	if (mqEvent == NULL)  {
		return(NULL);
	}

	/* send it to CANopen process */
	queueName.respId = mqEvent;
	queueName.cmd = MBX_CMD_EVENT_MBOX;
	queueName.answer = 0;
	ret = Mbx_PostCommandMbx(mqCmd, &queueName);
	if (ret == 0)  {
		return(NULL);
	}

	return(mqEvent);
}


/***************************************************************************/
/**
* \brief Mbx_Init_CANopen_Events - initialize CANopen event
*
* register CANopen event at CANopen task
*
*
*/
BaseType_t Mbx_Init_CANopen_Event(
		QueueHandle_t		mqCmd,			/**< command mailbox */
		QueueHandle_t		mqResp,			/**< response mailbox */
		MBX_CANOPEN_EVENT_T coEvent			/**< CANopen event */
	)
{
MBX_COMMAND_T	event;
int	ret;

	/* send it to CANopen process */
	event.respId = mqResp;
	event.cmd = MBX_CMD_EVENT;
	event.answer = 0;
	event.data.event = coEvent;
	ret = Mbx_PostCommandMbx(mqCmd, &event);
	if (ret == 0)  {
		return(pdFALSE);
	}

	return(pdTRUE);
}


/***************************************************************************/
/**
* \brief Mbx_PostCommandMbx - transmit command
*
* transmit command to command mailbox
*
* \return command identifier
*	identify command for answer 1..n
*	0 - if error
*
*/
UNSIGNED32 Mbx_PostCommandMbx(
		QueueHandle_t	mqCmd,	/**< command queue id */
		MBX_COMMAND_T	*pCmd	/**< command to transmit */
	)
{
BaseType_t	ret;

	/* start with 1 */
	if (cmdIdent == 0)  {
		cmdIdent++;
	}

	pCmd->ident = cmdIdent;
	ret = xQueueSend(mqCmd, pCmd, 0);
	if (ret != pdTRUE)  {
		return(0);
	}

	cmdIdent++;

	return(pCmd->ident);
}


/***************************************************************************/
/**
* \brief Mbx_PostresponseMbx - transmit response
*
* transmit response to mailbox
*
* \return mq_send
*
*/
BaseType_t Mbx_PostResponseMbx(
		QueueHandle_t	mqResp,	/**< response queue id */
		MBX_RESPONSE_T	*pResp	/**< response to transmit */
	)
{
BaseType_t	ret;

	ret = xQueueSend(mqResp, pResp, 0);
	if (ret != pdTRUE)  {
		return(pdFALSE);
	}

	return(ret);
}


/***************************************************************************/
/**
* \brief Mbx_WaitForResponseMbx - wait for new message at response mailbox
*
* \return
*	size of mq_receive
*/
BaseType_t Mbx_WaitForResponseMbx(
		QueueHandle_t	mqResp,		/**< response message queue */
		MBX_RESPONSE_T	*pResp		/**< pointer for receive data */
	)
{
BaseType_t ret;

	ret = xQueueReceive(mqResp, pResp, portMAX_DELAY);
	if (ret == pdTRUE && pResp != NULL) {
		return(ret);
	}

	return(pdFALSE);
}


/***************************************************************************/
/**
* \brief Mbx_WaitForEventMbx - wait for new event at mailbox
*
* \return
*	size of mq_receive
*/
BaseType_t Mbx_WaitForEventMbx(
		QueueHandle_t	mqEvent,	/**< event message queue */
		MBX_EVENT_T		*pEvent		/**< pointer for receive data */
	)
{
BaseType_t ret;

	ret = xQueueReceive(mqEvent, pEvent, portMAX_DELAY);
	if (ret == pdTRUE && pEvent != NULL) {
		return(ret);
	}

	return(pdFALSE);
}


/***************************************************************************/
/**
* \brief Mbx_WaitForCommandMbx - wait for new message at command mailbox
*
* \return
*	size of mq_receive
*/
BaseType_t Mbx_WaitForCommandMbx(
		QueueHandle_t	mqCmd,		/**< command message queue */
		MBX_COMMAND_T	*pCmd		/**< pointer to receive data */
	)
{
BaseType_t ret;

	ret = xQueueReceive(mqCmd, pCmd, portMAX_DELAY);
	if (ret == pdTRUE && pCmd != NULL) {
		return(ret);
	}

	return(pdFALSE);
}


/***************************************************************************/
/**
* \brief Mbx_Lock_objDict - lock object dictionary
*
*/
void Mbx_Lock_objDict(
		void
	)
{
	(void)xSemaphoreGive(coOdMutex);
}


/***************************************************************************/
/**
* \brief Mbx_UnLock_objDict - unlock object dictionary
*
*/
void Mbx_Unlock_objDict(
		void
	)
{
	(void)xSemaphoreTake(coOdMutex, portMAX_DELAY);
}

/***************************************************************************/
/**
* \brief Mbx_UnLock_objDict - unlock object dictionary
*
*/
BaseType_t Mbx_Create_QueueSet(
		void
	)
{
BaseType_t retVal;

	/* create queueSet which wakes up the CANopen task*/
	queueSet = xQueueCreateSet(10);
	configASSERT(queueSet);
	if (queueSet == NULL) {
		return(pdFAIL);
	}

	/* create CANopen Lock ID */
	semphCANopen = xSemaphoreCreateBinary();
	configASSERT(semphCANopen);
	if (semphCANopen == NULL) {
		return(pdFAIL);
	}

	/* add CANopen lock semaphore to queueSet */
	retVal = xQueueAddToSet(semphCANopen, queueSet);
	if (retVal != pdPASS)  {
		return(retVal);
	}

	/* connect to command mailbox */
	mqCmd = Mbx_Init_CmdMailBox();
	configASSERT(mqCmd);
	if (mqCmd == NULL)  {
		return(pdFAIL);
	}

	/* add command queue to queueSet */
	retVal = xQueueAddToSet(mqCmd, queueSet);
	if (retVal != pdPASS)  {
		return(retVal);
	}

	/* create response mailbox */
	mqResp = Mbx_Init_ResponseMailBox(mqCmd);
	configASSERT(mqResp);
	if (mqResp == NULL)  {
		return(pdFAIL);
	}

	/* add response queue to queueSet */
	retVal = xQueueAddToSet(mqResp, queueSet);
	if (retVal != pdPASS)  {
		return(retVal);
	}

	/* create event mailbox */
	mqEvent = Mbx_Init_EventMailBox(mqCmd);
	configASSERT(mqEvent);
	if (mqEvent == NULL)  {
		return(pdFAIL);
	}

	/* add event queue to queueSet */
	retVal = xQueueAddToSet(mqEvent, queueSet);
	if (retVal != pdPASS)  {
		return(retVal);
	}

	return(retVal);
}
