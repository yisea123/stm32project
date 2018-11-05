/*
* co_task.c - contains CANopen main task
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

#include <gen_define.h>
#include "mbx_function.h"

/* header of project specific types
---------------------------------------------------------------------------*/
#include <co_canopen.h>

/* constant definitions
---------------------------------------------------------------------------*/
#define MAX_MBX_CNT	10	/* max. number of mailbox for application tasks */

typedef struct {
	QueueHandle_t		respId;		/* response id from application task */
	QueueHandle_t		mqResponse;	/* response handle (internal handle) */
	unsigned long int eventList;	/* bit coded event list */
} RESPONSE_MBOX_ID;


/* command definition for CANopen functions 
---------------------------------------------------------------------------*/

/* local defined data types 
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern RET_T codrvOSConfig(void);
extern void codrvWaitForEvent(UNSIGNED32 msecTimeout);
extern BaseType_t Mbx_Create_QueueSet(void);

/* list of global defined functions
---------------------------------------------------------------------------*/
void codrv_NewCanMsg(CO_CAN_MSG_T *recMsg);

/* list of local defined functions
---------------------------------------------------------------------------*/
static int addRespMbox(QueueHandle_t respId);
static int addEventMbox(QueueHandle_t respId);
static void writeResp(QueueHandle_t respId, MBX_CMD_T cmd, UNSIGNED32 ident,
		RET_T ret, MBX_OBJ_T *pObj);
static void registerEvent(QueueHandle_t respId, int event);
static void transmitEvent(MBX_CANOPEN_EVENT_T coEvent, MBX_EVENT_T	*event);

static int initCanopen(UNSIGNED16 bitRate);
#ifdef CO_EVENT_SDO_SERVER_READ_CNT
static RET_T sdoServerReadInd(BOOL_T execute, UNSIGNED8	sdoNr, UNSIGNED16 index,
		UNSIGNED8	subIndex);
#endif /* CO_EVENT_SDO_SERVER_READ_CNT */
#ifdef CO_EVENT_SDO_SERVER_WRITE_CNT
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
#endif /* CO_EVENT_SDO_SERVER_WRITE_CNT */
#ifdef CO_EVENT_PDO_CNT
static void pdoInd(UNSIGNED16);
#endif /* CO_EVENT_PDO_CNT */
#ifdef CO_EVENT_CAN_CNT
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
#endif /* CO_EVENT_CAN_CNT */
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
#ifdef CO_EVENT_DYNAMIC_SDO_CLIENT_READ
static void sdoClientReadInd(UNSIGNED8 sdoNr, UNSIGNED16 index,
		UNSIGNED8 subIndex, UNSIGNED32 size, UNSIGNED32 errorVal);
#endif /* CO_EVENT_DYNAMIC_SDO_CLIENT_READ */
#ifdef CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE
static void sdoClientWriteInd(UNSIGNED8 sdoNr, UNSIGNED16 index,
		UNSIGNED8 subIndex, UNSIGNED32 errorVal);
#endif /* CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE */
#ifdef CO_EVENT_DYNAMIC_EMCY_CONSUMER
static void emcyConsumer(UNSIGNED8 node, UNSIGNED16 emcyErrorCode,
		UNSIGNED8 errorRegister, UNSIGNED8 const *addErrorBytes);
#endif /* CO_EVENT_DYNAMIC_EMCY_CONSUMER */
#ifdef CO_EVENT_DYNAMIC_LSS
static void lssMasterEvent(CO_LSS_MASTER_SERVICE_T type, UNSIGNED16 errorCode,
	    UNSIGNED8 errorSpec,	UNSIGNED32 *pIdentity);
#endif /* CO_EVENT_DYNAMIC_LSS */
static RET_T putObj(MBX_OBJ_T *pObj);
static RET_T getObj(MBX_OBJ_T *pObj);
void timerCallback(void * argument);

/* external variables
---------------------------------------------------------------------------*/
extern QueueHandle_t mqEvent;
extern QueueHandle_t mqResp;
extern QueueHandle_t	mqCmd;
extern SemaphoreHandle_t CANopenLockID;
extern QueueSetHandle_t queueSet;

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static RESPONSE_MBOX_ID	respMboxId[MAX_MBX_CNT];
static RESPONSE_MBOX_ID	eventMboxId[MAX_MBX_CNT];
static int	mbxCnt = 0;			/* actual number of mailbox pairs */
static int	eventMbxCnt = 0;	/* actual number of event mailbox pairs */

TimerHandle_t timerHandle = NULL;
TaskHandle_t canopenTaskNotifyHandle = NULL;
BaseType_t pxHigherPriorityTaskWoken = 0;

QueueSetMemberHandle_t queueSetMember = NULL;

/***************************************************************************/
/**
* \brief CANopen task entry
*
*/
void canopenTask(
		void *ptr
	)
{
UNSIGNED16	bitRate;
RET_T ret;
MBX_COMMAND_T	cmd;
UNSIGNED32	u32;

	bitRate = *((UNSIGNED16 *)ptr);

	/* initialize CANopen functionality */
	ret = initCanopen(bitRate);
	if (ret != 0)  {
		return;
	}

	/* create QueueSet that holds the Mailbox queues
	 * and a lock mutex for the CANopen Thread */
	ret = Mbx_Create_QueueSet();
	if (ret != pdTRUE) {
		return;
	}

	/* enable CAN */
	if (codrvCanEnable() != RET_OK)  {
		return;
	}

	while (1)  {

		do {
		/* call CANopen task as long as messages are available */
		coCommTask();
		} while (coQueueReceiveMessageAvailable() == CO_TRUE);

		/* wait for event (CAN, timer, message queue) */
		queueSetMember = xQueueSelectFromSet(queueSet, portMAX_DELAY);

		if (queueSetMember != NULL) {

			/* get new command from command mailbox */
			if ((queueSetMember == mqCmd)
					&& (Mbx_WaitForCommandMbx(mqCmd, &cmd) == pdPASS))  {
				ret = RET_INVALID_PARAMETER;
				switch (cmd.cmd)  {
					case MBX_CMD_RESP_MBOX:		/* create response mailbox */
						ret = addRespMbox(cmd.respId);
						break;

					case MBX_CMD_EVENT_MBOX:	/* create event mailbox */
						ret = addEventMbox(cmd.respId);
						break;

					case MBX_CMD_CAN:			/* CAN message */
						codrv_NewCanMsg(&cmd.data.canMsg);
						break;

					case MBX_CMD_EVENT:			/* register event */
						registerEvent(cmd.respId, cmd.data.event);
						break;

	#ifdef CO_PDO_TRANSMIT_CNT
					case MBX_CMD_PDO_REQ:		/* request PDO  */
						ret = coPdoReqNr(cmd.data.pdoReq.pdoNr,
							cmd.data.pdoReq.flags);
						break;
	#endif /* CO_PDO_TRANSMIT_CNT */

	#ifdef CO_EMCY_PRODUCER
					case MBX_CMD_EMCY_REQ:		/* emergency message */
						ret = coEmcyWriteReq(cmd.data.emcyReq.errCode,
							&cmd.data.emcyReq.addErrCode[0]);
						break;
	#endif /* CO_EMCY_PRODUCER */

	#ifdef CO_NMT_MASTER
					case MBX_CMD_NMT_REQ:		/* request NMT command */
						ret = coNmtStateReq(cmd.data.nmtReq.node,
							cmd.data.nmtReq.newState,
							cmd.data.nmtReq.master);
						break;
	#endif /* CO_NMT_MASTER */

	#ifdef CO_SDO_CLIENT_CNT
					case MBX_CMD_SDO_RD_REQ:		/* request SDO read */
						ret = coSdoRead(cmd.data.sdoReq.sdoNr,
							cmd.data.sdoReq.index,
							cmd.data.sdoReq.subIndex,
							cmd.data.sdoReq.pData,
							cmd.data.sdoReq.dataLen,
							cmd.data.sdoReq.numeric,
							cmd.data.sdoReq.timeout);
						break;

					case MBX_CMD_SDO_WR_REQ:		/* request SDO write */
						ret = coSdoWrite(cmd.data.sdoReq.sdoNr,
							cmd.data.sdoReq.index,
							cmd.data.sdoReq.subIndex,
							cmd.data.sdoReq.pData,
							cmd.data.sdoReq.dataLen,
							cmd.data.sdoReq.numeric,
							cmd.data.sdoReq.timeout);
						break;
	#endif /* CO_SDO_CLIENT_CNT */

					case MBX_CMD_PUT_OBJ:
						cmd.answer = 1;
						ret = putObj(&cmd.data.object);
						break;

					case MBX_CMD_GET_OBJ:
						cmd.answer = 1;
						ret = getObj(&cmd.data.object);
						break;

					case MBX_CMD_SET_COBID:
						memcpy(&u32, cmd.data.object.pData, 4);
						ret = coOdSetCobid(cmd.data.object.index,
							cmd.data.object.subIndex,
							u32);
						break;

	#ifdef CO_LSS_MASTER_SUPPORTED
					case MBX_CMD_LSS_MASTER_REQ:
						switch (cmd.data.lssReq.type) {
						case CO_LSS_MASTER_SERVICE_NON_CONFIG_SLAVE:
							ret = coLssIdentifyNonConfiguredSlaves(cmd.data.lssReq.data.nonConfigSlaves.timeOutVal,
								cmd.data.lssReq.data.nonConfigSlaves.interval);
							break;

						case CO_LSS_MASTER_SERVICE_SET_NODEID:
							ret = coLssSetNodeId(cmd.data.lssReq.data.setNodeId.nodeId,
								cmd.data.lssReq.data.setNodeId.timeOutVal);
							break;

						case CO_LSS_MASTER_SERVICE_BITRATE_SET:
							ret = coLssSetBitrate(cmd.data.lssReq.data.setBitRate.bitRate,
								cmd.data.lssReq.data.setBitRate.timeOutVal);
							break;

						case CO_LSS_MASTER_SERVICE_BITRATE_ACTIVE:
							ret = coLssActivateBitrate(cmd.data.lssReq.data.activeBitrate.switchDelay);
							break;

						case CO_LSS_MASTER_SERVICE_SWITCH_GLOBAL:
							ret = coLssSwitchGlobal(cmd.data.lssReq.data.switchGlobal.mode);
							break;

						case CO_LSS_MASTER_SERVICE_SWITCH_SELECTIVE:
							ret  = coLssSwitchSelective(cmd.data.lssReq.data.switchSelective.vendorId,
								cmd.data.lssReq.data.switchSelective.productCode,
								cmd.data.lssReq.data.switchSelective.versionNr,
								cmd.data.lssReq.data.switchSelective.serNr,
								cmd.data.lssReq.data.switchSelective.timeOutVal);
							break;

						case CO_LSS_MASTER_SERVICE_STORE:
							ret = coLssStoreConfig(cmd.data.lssReq.data.storeConfig.timeOutVal);
							break;

						default:
							break;
						}
						break;
	#endif /* CO_LSS_MASTER_SUPPORTED */

					default:
						break;
				}

				/* answer requested ? */
				if (cmd.answer != 0)  {
					/* generate answer */
					writeResp(cmd.respId, cmd.cmd, cmd.ident, ret, &cmd.data.object);
				}
			} else if (queueSetMember == semphCANopen) {
				(void)xSemaphoreTake(semphCANopen, portMAX_DELAY);
			}
		}
	}
}


/***************************************************************************/
/**
* \internal
*
* \brief addRespMbox - assign response mailbox
*
* open mailbox with given mboxName for writing 
* and save it at internal array
*
*/
static int addRespMbox(
		QueueHandle_t	respId		/* response mailbox handle */
	)
{
	/* free entry available ?*/
	if (mbxCnt >= MAX_MBX_CNT)  {
		return(-1);
	}

	/* save mailbox id and handle pair */
	respMboxId[mbxCnt].respId = respId;
	respMboxId[mbxCnt].mqResponse = mqResp;

	/* increment pair array count */
	mbxCnt++;

	return(0);
}


/***************************************************************************/
/**
* \internal
*
* \brief addEventMbox - assign event mailbox
*
* open mailbox with given mboxName for writing 
* and save it at internal array
*
*/
static int addEventMbox(
		QueueHandle_t	eventId		/* event mailbox handle */
	)
{
	/* free entry available ?*/
	if (eventMbxCnt >= MAX_MBX_CNT)  {
		return(-1);
	}

	/* save mailbox id and handle pair */
	eventMboxId[eventMbxCnt].respId = eventId;
	eventMboxId[eventMbxCnt].mqResponse = mqEvent;

	/* increment pair array count */
	eventMbxCnt++;

	return(0);
}


/***************************************************************************/
/**
* \internal
*
* \brief getRespMboxIdx - get response mailbox handle for given responseId
*
* \return value 0xffff
*	no responseId found
*
*/
static int getRespMboxIdx(
		QueueHandle_t respId		/* response id */
	)
{
int i;

	/* search mbxId  for all registered mailbox */
	for (i = 0; i < mbxCnt; i++)  {
		if (respId == respMboxId[i].respId)  {
			/* found */
			return(i);
		}
	}

	/* not found, return error */
	return(0xffff);
}


/***************************************************************************/
/**
* \internal
*
* \brief getRespMboxIdx - get response mailbox handle for given responseId
*
* \return value 0xffff
*	no responseId found
*
*/
static int getEventMboxIdx(
		QueueHandle_t eventId		/* event id */
	)
{
int i;

	/* search mbxId  for all registered mailbox */
	for (i = 0; i < eventMbxCnt; i++)  {
		if (eventId == eventMboxId[i].respId)  {
			/* found */
			return(i);
		}
	}

	/* not found, return error */
	return(0xffff);
}


/***************************************************************************/
/**
* \internal
*
* \brief writeResp - write response for received command
*
*
*/
static void writeResp(
		QueueHandle_t	respId,		/* response id, given by command */
		MBX_CMD_T	cmd,			/* executed command */
		UNSIGNED32	ident,			/* command identifier */
		RET_T	ret,				/* return value */
		MBX_OBJ_T *pObj				/* pointer to object structure */
	)
{
MBX_RESPONSE_T	response;
int i;

	/* get response handle index */
	i = getRespMboxIdx(respId);
	if (i == 0xffff)  {
		return;
	}

	/* setup standard values */
	response.cmd = cmd;
	response.ident = ident;
	response.retVal = ret;
	memcpy(&response.object, pObj, sizeof(MBX_OBJ_T));

	/* transmit message */
	ret = Mbx_PostResponseMbx(respMboxId[i].mqResponse, (void *)&response);
	if (ret != pdTRUE)  {
		;
	} else {
		;
	}
}


/***************************************************************************/
/**
* \internal putObj - put object
*
* \brief
*
*/
static RET_T putObj(
		MBX_OBJ_T *pObj
	)
{
RET_T	retVal;
UNSIGNED16	u16;
UNSIGNED32	u32;
INTEGER8	i8;
INTEGER16	i16;
INTEGER32	i32;
REAL32		r32;

	switch (pObj->dType)  {
		case CO_DTYPE_U8_VAR:
		case CO_DTYPE_U8_PTR:
			retVal = coOdPutObj_u8(pObj->index, pObj->subIndex, *pObj->pData);
			break;

		case CO_DTYPE_U16_VAR:
		case CO_DTYPE_U16_PTR:
			memcpy(&u16, pObj->pData, sizeof(UNSIGNED16));
			retVal = coOdPutObj_u16(pObj->index, pObj->subIndex, u16);
			break;

		case CO_DTYPE_U32_VAR:
		case CO_DTYPE_U32_PTR:
			memcpy(&u32, pObj->pData, sizeof(UNSIGNED32));
			retVal = coOdPutObj_u32(pObj->index, pObj->subIndex, u32);
			break;

		case CO_DTYPE_I8_VAR:
		case CO_DTYPE_I8_PTR:
			memcpy(&i8, pObj->pData, 1);
			retVal = coOdPutObj_i8(pObj->index, pObj->subIndex, i8);
			break;

		case CO_DTYPE_I16_VAR:
		case CO_DTYPE_I16_PTR:
			memcpy(&i16, pObj->pData, sizeof(INTEGER16));
			retVal = coOdPutObj_i16(pObj->index, pObj->subIndex, i16);
			break;

		case CO_DTYPE_I32_VAR:
		case CO_DTYPE_I32_PTR:
			memcpy(&i32, pObj->pData, sizeof(INTEGER32));
			retVal = coOdPutObj_i32(pObj->index, pObj->subIndex, i32);
			break;

		case CO_DTYPE_R32_VAR:
		case CO_DTYPE_R32_PTR:
			memcpy(&r32, pObj->pData, sizeof(REAL32));
			retVal = coOdPutObj_r32(pObj->index, pObj->subIndex, r32);
			break;

		default:
			retVal = RET_INVALID_PARAMETER;
			break;
	}

	return(retVal);
}


/***************************************************************************/
/**
* \internal getObj - get object
*
* \brief
*
*/
static RET_T getObj(
		MBX_OBJ_T *pObj
	)
{
RET_T	retVal;
UNSIGNED16	u16;
UNSIGNED32	u32;
INTEGER8	i8;
INTEGER16	i16;
INTEGER32	i32;
REAL32		r32;

	switch (pObj->dType)  {
		case CO_DTYPE_U8_VAR:
		case CO_DTYPE_U8_PTR:
		case CO_DTYPE_U8_CONST:
			retVal = coOdGetObj_u8(pObj->index, pObj->subIndex, pObj->pData);
			break;

		case CO_DTYPE_U16_CONST:
		case CO_DTYPE_U16_VAR:
		case CO_DTYPE_U16_PTR:
			retVal = coOdGetObj_u16(pObj->index, pObj->subIndex, &u16);
			memcpy(pObj->pData, &u16, sizeof(UNSIGNED16));
			break;

		case CO_DTYPE_U32_CONST:
		case CO_DTYPE_U32_VAR:
		case CO_DTYPE_U32_PTR:
			retVal = coOdGetObj_u32(pObj->index, pObj->subIndex, &u32);
			memcpy(pObj->pData, &u32, sizeof(UNSIGNED32));
			break;

		case CO_DTYPE_I8_CONST:
		case CO_DTYPE_I8_VAR:
		case CO_DTYPE_I8_PTR:
			retVal = coOdGetObj_i8(pObj->index, pObj->subIndex, &i8);
			memcpy(pObj->pData, &i8, 1);
			break;

		case CO_DTYPE_I16_CONST:
		case CO_DTYPE_I16_VAR:
		case CO_DTYPE_I16_PTR:
			retVal = coOdGetObj_i16(pObj->index, pObj->subIndex, &i16);
			memcpy(pObj->pData, &i16, sizeof(INTEGER16));
			break;

		case CO_DTYPE_I32_CONST:
		case CO_DTYPE_I32_VAR:
		case CO_DTYPE_I32_PTR:
			retVal = coOdGetObj_i32(pObj->index, pObj->subIndex, &i32);
			memcpy(pObj->pData, &i32, sizeof(INTEGER32));
			break;

		case CO_DTYPE_R32_CONST:
		case CO_DTYPE_R32_VAR:
		case CO_DTYPE_R32_PTR:
			retVal = coOdGetObj_r32(pObj->index, pObj->subIndex, &r32);
			memcpy(pObj->pData, &r32, sizeof(REAL32));
			break;

		default:
			retVal = RET_INVALID_PARAMETER;
			break;
	}

	return(retVal);
}


/***************************************************************************/
/**
* \internal
*
* \brief drvCanMessage - transmit can message to command mailbox
*
* For some reasons it is possible, 
* to get the received can messages and put it to the command mailbox
*
*/
void drvCanMessage(
		CO_CAN_MSG_T	*canMsg			/* can message */
	)
{
MBX_COMMAND_T	msg;
int	ret;

	msg.respId = 0;
	msg.cmd = MBX_CMD_CAN;
	msg.answer = 0;
	memcpy(&msg.data.canMsg, canMsg, sizeof(CO_CAN_MSG_T));

	/* send can message to command mailbox */
	ret = Mbx_PostCommandMbx(mqCmd, &msg);
	if (ret == 0)  {
		;
	}

    return;
}


/***************************************************************************/
/**
* \internal
*
* \brief registerEvent - register event for respId
*
* Events are saved as bit-coded list
*
*/
static void registerEvent(
		QueueHandle_t respId,	/* response id for receiver */
		int event				/* event number */
	)
{
int idx;

	idx = getEventMboxIdx(respId);
	if (idx == 0xffff)  {
		return;
	}

	eventMboxId[idx].eventList |= (1 << event);
}


/***************************************************************************/
/**
* \internal
*
* \brief transmitEvent - transmit event
*
* transmit event for all mailbox, depending on event-bit at eventList
*
*/
static void transmitEvent(
		MBX_CANOPEN_EVENT_T coEvent,
		MBX_EVENT_T			*pEvent
	)
{
int i;
int	ret;

	/* standard values for event */
	pEvent->type = coEvent;

	/* for all registered response mailbox */
	for (i = 0; i < eventMbxCnt; i++)  {
		/* if event-bit is set ? */
		if ((eventMboxId[i].eventList & (1 << coEvent)) != 0)  {
			/* yes, transmit info */
			ret = Mbx_PostResponseMbx(eventMboxId[i].mqResponse, (void *)pEvent);
			if (ret != pdTRUE)  {
				/* error */
			}
		}
	}
}



/*********************************************************************/
/*
* initialize CANopen functionality including indication functions
*
*/
static int initCanopen(
		UNSIGNED16	bitRate
	)
{
	/* initialize hardware */
	codrvHardwareInit();

	/* initialize CAN */
	if (codrvCanInit(250) != RET_OK)  {
		return(1);
	}

	/* initialize CANopen stack */
	if (coCanOpenStackInit(NULL) != RET_OK)  {
		return(2);
	}

	/* OS specific setup */
	if (codrvOSConfig() != RET_OK)  {
		return(1);
	}

	/* register event functions */
#ifdef CO_EVENT_DYNAMIC_NMT
	if (coEventRegister_NMT(nmtInd) != RET_OK)  {
		return(3);
	}
#endif /* CO_EVENT_DYNAMIC_NMT */
#ifdef CO_EVENT_DYNAMIC_ERRCTRL
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		return(4);
	}
#endif /* CO_EVENT_DYNAMIC_ERRCTRL */
#ifdef CO_EVENT_SDO_SERVER_READ_CNT
	if (coEventRegister_SDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		return(5);
	}
#endif /* CO_EVENT_SDO_SERVER_READ_CNT */
#ifdef CO_EVENT_SDO_SERVER_WRITE_CNT
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		return(7);
	}
#endif /* CO_EVENT_SDO_SERVER_WRITE_CNT */
#ifdef CO_EVENT_PDO_CNT
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		return(8);
	}
#endif /* CO_EVENT_PDO_CNT */
#ifdef CO_EVENT_CAN_CNT
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		return(12);
	}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		return(13);
	}
#endif /* CO_EVENT_CAN_CNT */
#ifdef CO_EVENT_DYNAMIC_EMCY_CONSUMER
	if (coEventRegister_EMCY_CONSUMER(emcyConsumer) != RET_OK)  {
		return(14);
	}
#endif /* CO_EVENT_DYNAMIC_EMCY_CONSUMER */
#ifdef CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE
	if (coEventRegister_SDO_CLIENT_WRITE(sdoClientWriteInd) != RET_OK)  {
		return(15);
	}
#endif /* CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE */
#ifdef CO_EVENT_DYNAMIC_SDO_CLIENT_READ
	if (coEventRegister_SDO_CLIENT_READ(sdoClientReadInd) != RET_OK)  {
		return(16);
	}
#endif /* CO_EVENT_DYNAMIC_SDO_CLIENT_READ */
#ifdef CO_EVENT_DYNAMIC_LSS
	if (coEventRegister_LSS_MASTER(lssMasterEvent) != RET_OK)  {
		return(17);
	}
#endif /* CO_EVENT_DYNAMIC_LSS */
	return(0);
}


/*********************************************************************/
RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
MBX_EVENT_T	event;

	if (execute == CO_TRUE)  {
		event.event.nmt.newState = newState;

		transmitEvent(MBX_CANOPEN_EVENT_NMT, &event);
	}

	return(RET_OK);
}


/*********************************************************************/
void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
MBX_EVENT_T	event;

	event.event.hb.nodeId = nodeId;
	event.event.hb.state = state;
	event.event.hb.nmtState = nmtState;

	transmitEvent(MBX_CANOPEN_EVENT_HB, &event);

    return;
}


#ifdef CO_EVENT_PDO_CNT
/*********************************************************************/
void pdoInd(
		UNSIGNED16	pdoNr
	)
{
MBX_EVENT_T	pdo;

	pdo.event.pdo.pdoNr = pdoNr;

	transmitEvent(MBX_CANOPEN_EVENT_PDO, &pdo);
}
#endif /* CO_EVENT_PDO_CNT */

#ifdef CO_EVENT_SDO_SERVER_READ_CNT
/*********************************************************************/
static RET_T sdoServerReadInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
MBX_EVENT_T	sdo;

	if (execute == CO_TRUE)  {
		sdo.event.sdo.sdoNr = sdoNr;
		sdo.event.sdo.index = index;
		sdo.event.sdo.subIndex = subIndex;

		transmitEvent(MBX_CANOPEN_EVENT_SDO_SRV_READ, &sdo);
	}

	return(RET_OK);
}
#endif /* CO_EVENT_SDO_SERVER_READ_CNT */


#ifdef CO_EVENT_SDO_SERVER_WRITE_CNT
/*********************************************************************/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
MBX_EVENT_T	sdo;

	if (execute == CO_TRUE)  {
		sdo.event.sdo.sdoNr = sdoNr;
		sdo.event.sdo.index = index;
		sdo.event.sdo.subIndex = subIndex;

		transmitEvent(MBX_CANOPEN_EVENT_SDO_SRV_WRITE, &sdo);
	}

	return(RET_OK);
}
#endif /* CO_EVENT_SDO_SERVER_WRITE_CNT */


#ifdef CO_EVENT_CAN_CNT
/*********************************************************************/
static void canInd(
		CO_CAN_STATE_T	canState
	)
{
MBX_EVENT_T	can;

	can.event.can.state = canState;

	transmitEvent(MBX_CANOPEN_EVENT_CAN, &can);
}


/*********************************************************************/
static void commInd(
		CO_COMM_STATE_EVENT_T	commEvent
	)
{
MBX_EVENT_T	comm;

	comm.event.comm.event = commEvent;

	transmitEvent(MBX_CANOPEN_EVENT_COMM, &comm);
}
#endif /* CO_EVENT_CAN_CNT */


#ifdef CO_EVENT_DYNAMIC_EMCY_CONSUMER
/*********************************************************************/
static void emcyConsumer(
		UNSIGNED8       node,
		UNSIGNED16      emcyErrorCode,
		UNSIGNED8       errorRegister,
		UNSIGNED8       const *addErrorBytes
	)
{
MBX_EVENT_T	emcy;
                
	emcy.event.emcy.node = node;
	emcy.event.emcy.errCode = emcyErrorCode;
	emcy.event.emcy.errorRegister = errorRegister;
	memcpy(&emcy.event.emcy.addErrCode[0], addErrorBytes, 5);

	transmitEvent(MBX_CANOPEN_EVENT_EMCY, &emcy);
}
#endif /* CO_EVENT_DYNAMIC_EMCY_CONSUMER */


#ifdef CO_EVENT_DYNAMIC_SDO_CLIENT_READ
/*********************************************************************/
static void sdoClientReadInd(
		UNSIGNED8       sdoNr,
		UNSIGNED16      index,
		UNSIGNED8       subIndex,
		UNSIGNED32      size,
		UNSIGNED32      errorVal
	)
{
MBX_EVENT_T	sdo;

	sdo.event.sdo.sdoNr = sdoNr;
	sdo.event.sdo.index = index;
	sdo.event.sdo.subIndex = subIndex;
	sdo.event.sdo.dataLen = size;
	sdo.event.sdo.result = errorVal;

	transmitEvent(MBX_CANOPEN_EVENT_SDO_CL_READ, &sdo);
}
#endif /* CO_EVENT_DYNAMIC_SDO_CLIENT_READ */


#ifdef CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE
/*********************************************************************/
static void sdoClientWriteInd(
		UNSIGNED8       sdoNr,
		UNSIGNED16      index,
		UNSIGNED8       subIndex,
		UNSIGNED32      errorVal
	)
{
MBX_EVENT_T	sdo;

	sdo.event.sdo.sdoNr = sdoNr;
	sdo.event.sdo.index = index;
	sdo.event.sdo.subIndex = subIndex;
	sdo.event.sdo.result = errorVal;

	transmitEvent(MBX_CANOPEN_EVENT_SDO_CL_WRITE, &sdo);
}
#endif /* CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE */


#ifdef CO_EVENT_DYNAMIC_LSS
/*********************************************************************/
static void lssMasterEvent(
	CO_LSS_MASTER_SERVICE_T type,
	UNSIGNED16 errorCode,
	UNSIGNED8 errorSpec,
	UNSIGNED32 *pIdentity
	)
{
	MBX_EVENT_T event;
	event.event.lssMaster.type = type;
	event.event.lssMaster.errorCode = errorCode;
	event.event.lssMaster.errorSpec = errorSpec;
	event.event.lssMaster.pIdentity = pIdentity;

	transmitEvent(MBX_CANOPEN_EVENT_LSS_MASTER, &event);
	return;
}
#endif /* CO_EVENT_DYNAMIC_LSS */


/***********************************************************************/
/**
*
* \brief
*
* \return none
*
*/
void codrv_NewCanMsg(
		CO_CAN_MSG_T	*recMsg
	)
{
CO_CAN_MSG_T *pRecBuf;

	/* get receiveBuffer */
	pRecBuf = coQueueGetReceiveBuffer();
	if (pRecBuf == NULL)  {
		coCommStateEvent(CO_COMM_STATE_EVENT_REC_QUEUE_FULL);
		return;
	}

	pRecBuf = recMsg;

	/* set buffer filled */
	coQueueReceiveBufferIsFilled();


	coCommTaskSet(CO_COMMTASK_EVENT_MSG_AVAIL);
}
