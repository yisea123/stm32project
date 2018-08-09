/*
* appl_task.c - contains application task 1
*
* Copyright (c) 2012-2017 emtas GmbH
*-------------------------------------------------------------------*/


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

#include <fcntl.h>		/* For O_* constants */
#include <sys/stat.h>	/* For mode constants */
#include <errno.h>
#include <cmsis_os.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>
#include <co_canopen.h>

#include <mbx_function.h>

/* constant definitions
---------------------------------------------------------------------------*/

/* command definition for CANopen functions 
---------------------------------------------------------------------------*/

/* local defined data types 
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/
void appl1(void *ptr);

/* list of local defined functions
---------------------------------------------------------------------------*/
static void checkEvent(MBX_EVENT_T	*response);
static int requestCommand(MBX_CMD_T	cmd, MBX_COMMAND_T	*pCmdData);

/* external variables
---------------------------------------------------------------------------*/
extern QueueHandle_t mqResp;
extern QueueHandle_t mqCmd;
extern QueueHandle_t mqEvent;

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/


/***************************************************************************/
/**
* \brief application main
*
*/
void appl1(
		void *ptr
	)
{
int ret;
MBX_RESPONSE_T	response;
MBX_EVENT_T		event;

	(void)ptr;

	/* register CANopen events */
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_NMT);
	if (ret != pdTRUE)  {/* error */};
#ifdef xxx
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_HB);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_PDO);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_SDO_SRV_READ);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_SDO_SRV_WRITE);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_CAN);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_COMM);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_EMCY);
	if (ret != pdTRUE)  {/* error */};
#endif
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_SDO_CL_READ);
	if (ret != pdTRUE)  {/* error */};
	ret = Mbx_Init_CANopen_Event(mqCmd, mqEvent, MBX_CANOPEN_EVENT_SDO_CL_WRITE);
	if (ret != pdTRUE)  {/* error */};

	/* send commands to CANopen task */

	/*-----------------------------------------------------------------*
	 * emergency
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	emcy;
UNSIGNED8 emcyData[5] = { 1, 2, 3, 4, 5 };

	emcy.data.emcyReq.errCode = 0x1234;
	memcpy(&emcy.data.emcyReq.addErrCode[0], &emcyData, 5);
	ret = requestCommand(MBX_CMD_EMCY_REQ, &emcy);

	/*-----------------------------------------------------------------*
	 * NMT request
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	nmt;
	nmt.data.nmtReq.newState = CO_NMT_STATE_OPERATIONAL;
	nmt.data.nmtReq.node = 0;
	nmt.data.nmtReq.master = CO_TRUE;
	ret = requestCommand(MBX_CMD_NMT_REQ, &nmt);

	/*-----------------------------------------------------------------*
	 * PDO
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	pdo;
	pdo.data.pdoReq.pdoNr = 1;
	pdo.data.pdoReq.flags = 0;
	ret = requestCommand(MBX_CMD_PDO_REQ, &pdo);

#ifdef xxx
	/*-----------------------------------------------------------------*
	 * SDO read
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	sdo;
UNSIGNED32	u32;
	coOdSetCobid(0x1280, 1, 0x620);
	coOdSetCobid(0x1280, 2, 0x5a0);
	sdo.data.sdoReq.sdoNr = 1;
	sdo.data.sdoReq.index = 0x1000;
	sdo.data.sdoReq.subIndex = 0;
	sdo.data.sdoReq.pData = (UNSIGNED8 *)&u32;
	sdo.data.sdoReq.dataLen = 4;
	sdo.data.sdoReq.numeric = 1;
	sdo.data.sdoReq.timeout = 1000;
	ret = requestCommand(MBX_CMD_SDO_RD_REQ, &sdo);

	/*-----------------------------------------------------------------*
	 * SDO write
	 *-----------------------------------------------------------------*/
UNSIGNED16	u16 = 1000;
	coOdSetCobid(0x1281, 1, 0x621);
	coOdSetCobid(0x1281, 2, 0x5a1);
	sdo.data.sdoReq.sdoNr = 2;
	sdo.data.sdoReq.index = 0x1017;
	sdo.data.sdoReq.subIndex = 0;
	sdo.data.sdoReq.pData = (UNSIGNED8 *)&u16;
	sdo.data.sdoReq.dataLen = 2;
	sdo.data.sdoReq.numeric = 1;
	sdo.data.sdoReq.timeout = 1000;
	ret = requestCommand(MBX_CMD_SDO_WR_REQ, &sdo);
#endif

	/*-----------------------------------------------------------------*
	 * get object
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	get;
UNSIGNED16	u16 = 1000;
	get.data.object.index = 0x1017;
	get.data.object.subIndex = 0;
	get.data.object.dType = CO_DTYPE_U16_VAR;
	get.data.object.pData = (UNSIGNED8 *)&u16;
	ret = requestCommand(MBX_CMD_GET_OBJ, &get);

	/*-----------------------------------------------------------------*
	 * put object
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	put;
UNSIGNED16	u16x = 1000;
	u16x = 1500;
	put.data.object.index = 0x1017;
	put.data.object.subIndex = 0;
	put.data.object.dType = CO_DTYPE_U16_VAR;
	put.data.object.pData = (UNSIGNED8 *)&u16x;
	ret = requestCommand(MBX_CMD_PUT_OBJ, &put);

	/*-----------------------------------------------------------------*
	 * set COB ID
	 *-----------------------------------------------------------------*/
MBX_COMMAND_T	sci;
UNSIGNED32	cobid = 0x123;

	sci.data.object.index = 0x1280;
	sci.data.object.subIndex = 1;
	sci.data.object.dType = CO_DTYPE_U32_VAR;
	sci.data.object.pData = (UNSIGNED8 *)&cobid;
	ret = requestCommand(MBX_CMD_SET_COBID, &sci);

	/* main loop */
	while (1)  {

		/* wait for new messages */
		if (Mbx_WaitForResponseMbx(mqResp, &response) == pdTRUE)  {
			switch (response.cmd)  {
				/* answer from command interface */
				case MBX_CMD_EMCY_REQ:
				case MBX_CMD_PDO_REQ:
				case MBX_CMD_NMT_REQ:
				case MBX_CMD_SDO_RD_REQ:
				case MBX_CMD_SDO_WR_REQ:
				case MBX_CMD_PUT_OBJ:
				case MBX_CMD_SET_COBID:
					/* no error, don't say anything */
					if (response.retVal != 0)  {
						/* error */
					}
					break;

				case MBX_CMD_GET_OBJ:
					break;

				default:
					break;
			}
		}

		/* wait for new events */
		if (Mbx_WaitForEventMbx(mqEvent, &event) == pdTRUE)  {
			checkEvent(&event);
		}
	}
}


/***************************************************************************/
/**
* \brief checkEvent - check received event from CANopen
*
*/
static void checkEvent(
		MBX_EVENT_T	*response
	)
{
	/* message depends on event type */
	switch (response->type)  {
		case MBX_CANOPEN_EVENT_NMT:
			break;
		case MBX_CANOPEN_EVENT_HB:
			break;
		case MBX_CANOPEN_EVENT_PDO:
			break;
		case MBX_CANOPEN_EVENT_SDO_SRV_READ:
			break;
		case MBX_CANOPEN_EVENT_SDO_SRV_WRITE:
			break;
		case MBX_CANOPEN_EVENT_CAN:
			break;
		case MBX_CANOPEN_EVENT_COMM:
		case MBX_CANOPEN_EVENT_SDO_CL_READ:
			break;
		case MBX_CANOPEN_EVENT_SDO_CL_WRITE:
			break;
		case MBX_CANOPEN_EVENT_EMCY:
			break;
		default:
			break;
	}
}


/***************************************************************************/
/**
* \brief requestCommand
*
*/
static int requestCommand(
		MBX_CMD_T		cmd,			/* command for mailbox */
		MBX_COMMAND_T	*pCmdData		/* command data structure filled with
											data depending on command */
	)
{
int	ret;

	/* fill in response queue and standard data */
	pCmdData->respId = mqResp;
	pCmdData->cmd = cmd;
	pCmdData->answer = 1;
	ret = Mbx_PostCommandMbx(mqCmd, pCmdData);

	return(ret);
}
