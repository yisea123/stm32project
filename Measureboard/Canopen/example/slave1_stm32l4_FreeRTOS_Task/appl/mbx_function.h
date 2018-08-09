/*
* mbx_function.h - contains defines for mbx access
*
* * Copyright (c) 2012-2017 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: mbx_function.h 18869 2017-06-27 12:29:08Z hil $

*-------------------------------------------------------------------
*
*
*
*/

/**
* \brief defines for mailbox access
*
* \file mbx_function.h - contains defines for mailbox access
*/

#ifndef CO_MBX_FUNCTION_H
#define CO_MBX_FUNCTION_H 1

#include <cmsis_os.h>

#include <co_canopen.h>

/* constant definitions */
#define MSG_QUEUE_CMD	"/mqCommand"	/* central command mailbox */
#define MSG_QUEUE_CAN	"/canMbx"		/* mailbox for transmit can messages */

/* defines */
//#define QueueHandle_t		mqd_t				/* handle data type */

#define MAX_DATA	4		/* max data size for get/put object */

/* data types */

/* commands for command mailbox */
typedef enum {
	MBX_CMD_RESP_MBOX,		/* name for response mailbox */
	MBX_CMD_EVENT_MBOX,		/* name for event mailbox */
	MBX_CMD_EVENT,			/* CANopen event */
	MBX_CMD_CAN,			/* can message */
	MBX_CMD_PDO_REQ,		/* PDO */
	MBX_CMD_EMCY_REQ,		/* emergency message */
	MBX_CMD_NMT_REQ,		/* NMT command */
	MBX_CMD_SDO_RD_REQ,		/* SDO read command */
	MBX_CMD_SDO_WR_REQ,		/* SDO write command */
	MBX_CMD_PUT_OBJ,		/* put object */
	MBX_CMD_GET_OBJ,		/* put object */
	MBX_CMD_SET_COBID,		/* set COB id */
	MBX_CMD_LSS_MASTER_REQ,
} MBX_CMD_T;

/* CANopen events */
typedef enum {
	MBX_CANOPEN_EVENT_NMT,
	MBX_CANOPEN_EVENT_HB,
	MBX_CANOPEN_EVENT_SDO_SRV_READ,
	MBX_CANOPEN_EVENT_SDO_SRV_WRITE,
	MBX_CANOPEN_EVENT_SDO_CL_READ,
	MBX_CANOPEN_EVENT_SDO_CL_WRITE,
	MBX_CANOPEN_EVENT_EMCY,
	MBX_CANOPEN_EVENT_PDO,
	MBX_CANOPEN_EVENT_CAN,
	MBX_CANOPEN_EVENT_COMM,
	MBX_CANOPEN_EVENT_LSS_MASTER
} MBX_CANOPEN_EVENT_T;


/* emergency data structure */
typedef struct {
	UNSIGNED16		errCode;
	UNSIGNED8		addErrCode[5];
	UNSIGNED8		node;
	UNSIGNED8		errorRegister;
} MBX_EMCY_T;

/* NMT state data structure */
typedef struct {
	CO_NMT_STATE_T	newState;
	UNSIGNED8		node;
	BOOL_T			master;
} MBX_NMT_T;

/* HB state event data structure */
typedef struct {
	UNSIGNED8		nodeId;
	CO_ERRCTRL_T	state;
	CO_NMT_STATE_T	nmtState;
} MBX_HB_T;

/* PDO event data structure */
typedef struct {
	UNSIGNED16	pdoNr;
	UNSIGNED8	flags;
} MBX_PDO_T;

/* SDO event data structure */
typedef struct {
	UNSIGNED8		sdoNr;
	UNSIGNED16		index;
	UNSIGNED8		subIndex;
	UNSIGNED8		*pData;
	UNSIGNED32		dataLen;
	UNSIGNED16		numeric;
	UNSIGNED32		timeout;
	UNSIGNED32		result;
} MBX_SDO_T;

/* CAN state event data structure */
typedef struct {
	CO_CAN_STATE_T	state;
} MBX_CAN_T;

/* COMM state event data structure */
typedef struct {
	CO_COMM_STATE_EVENT_T	event;
} MBX_COMM_T;

/* get object / put object /set COB id */
typedef struct {
		UNSIGNED16		index;
		UNSIGNED8		subIndex;
		CO_DATA_TYPE_T	dType;
		UNSIGNED8		*pData;
} MBX_OBJ_T;

/* LSS request: identify non-configured devices */
typedef struct {
	UNSIGNED16	timeOutVal;		/**< timeout value in milliseconds */
	UNSIGNED16	interval;		/**< interval in seconds */
} MBX_LSS_NON_CONFIG_SLAVES_T;

/* LSS request: set node id */
typedef struct {
	UNSIGNED8	nodeId;			/**< new node ID or 255 to reset */
	UNSIGNED16 timeOutVal;		/**< time out value in milliseconds */
} MBX_LSS_SET_NODE_ID_T;

/* LSS request: set bit rate */
typedef struct {
	UNSIGNED16 bitRate;			/**< new bit rate */
	UNSIGNED16 timeOutVal;		/**< time out value in milliseconds */
} MBX_LSS_SET_BIT_RATE_T;


/* LSS request: activate bit rate */
typedef struct {
	UNSIGNED16 switchDelay;		/**< delay until switch in milliseconds */
} MBX_LSS_ACTIVATE_BITRATE_T;


/* LSS request: switch mode global */
typedef struct {
	UNSIGNED8 mode;				/**< mode for switch global */
} MBX_LSS_SWITCH_GLOBAL_T;

typedef struct {
	UNSIGNED32 vendorId;
	UNSIGNED32 productCode;
	UNSIGNED32 versionNr;
	UNSIGNED32 serNr;
	UNSIGNED16 timeOutVal;		/**< time out value in milliseconds */
}  MBX_LSS_SWITCH_SELECTIVE_T;

/* LSS request: store configuration */
typedef struct {
	UNSIGNED16 timeOutVal;		/**< time out value in milliseconds */
} MBX_LSS_STORE_CONFIG_T;

/* lss master service request */
typedef struct {
	CO_LSS_MASTER_SERVICE_T type;		/**< type of LSS master service */
	union {
		MBX_LSS_NON_CONFIG_SLAVES_T nonConfigSlaves;
		MBX_LSS_SET_NODE_ID_T       setNodeId;
		MBX_LSS_SET_BIT_RATE_T		setBitRate;
		MBX_LSS_ACTIVATE_BITRATE_T	activeBitrate;
		MBX_LSS_SWITCH_GLOBAL_T		switchGlobal;
		MBX_LSS_SWITCH_SELECTIVE_T	switchSelective;
		MBX_LSS_STORE_CONFIG_T		storeConfig;
	} data;
} MBX_LSS_MASTER_SERVICE_T;


/* LSS master event type */
typedef struct {
	CO_LSS_MASTER_SERVICE_T type;
	UNSIGNED16 errorCode;
	UNSIGNED8 errorSpec;
	UNSIGNED32 *pIdentity;
} MBX_LSS_MASTER_T;


/* mailbox command */
typedef struct {
	QueueHandle_t		respId;		/* response mailbox id */
	MBX_CMD_T	cmd;				/* command */
	UNSIGNED32	ident;				/* command identifier - sent as answer */
	unsigned char	answer;			/* answer expected */
	union {
		char	respMboxName[50];	/* name for response mailbox */
		CO_CAN_MSG_T	canMsg;		/* can message */
		MBX_CANOPEN_EVENT_T	event;	/* CANopen event */
		MBX_EMCY_T	emcyReq;		/* request emergency */
		MBX_PDO_T	pdoReq;			/* request PDO */
		MBX_NMT_T	nmtReq;			/* request NMT */
		MBX_SDO_T	sdoReq;			/* request SDO */
		MBX_OBJ_T	object;			/* get object / put object /set COB id */
		MBX_LSS_MASTER_SERVICE_T   lssReq;         /* LSS master services */
	} data;
} MBX_COMMAND_T;


/* mailbox response */
typedef struct {
	MBX_CMD_T	cmd;				/* answer for command */
	UNSIGNED32	ident;				/* command identifier - sent as answer */
	RET_T		retVal;				/* return value for command */
	MBX_OBJ_T	object;				/* get object / put object /set COB id */
} MBX_RESPONSE_T;


/* mailbox event */
typedef struct {
	MBX_CANOPEN_EVENT_T	type;			/* CANopen event type */
	union answer {
		MBX_NMT_T	nmt;				/* NMT data */
		MBX_HB_T	hb;					/* heart beat data */
		MBX_PDO_T	pdo;				/* PDO data */
		MBX_SDO_T	sdo;				/* SDO data */
		MBX_CAN_T	can;				/* CAN data */
		MBX_COMM_T	comm;				/* communication data */
		MBX_EMCY_T	emcy;				/* emergency data */
		MBX_LSS_MASTER_T   lssMaster;	/* LSS master services */
	} event;
} MBX_EVENT_T;


/* c variables */
extern SemaphoreHandle_t coOdMutex;		/* semaphore for object dictionary */


/* prototypes */

int				Mbx_Init_CAN(				UNSIGNED16 		bitRate);
int				Mbx_DeInit_CAN(void);
QueueHandle_t	Mbx_Init_CmdMailBox(void);
QueueHandle_t	Mbx_Init_ResponseMailBox(QueueHandle_t mqCmd);
QueueHandle_t	Mbx_Init_EventMailBox(	QueueHandle_t mqCmd);
UNSIGNED32		Mbx_PostCommandMbx(		QueueHandle_t mqCmd,
		MBX_COMMAND_T *pCmd);
BaseType_t		Mbx_PostResponseMbx(QueueHandle_t mqCmd,
		MBX_RESPONSE_T *pCmd);
BaseType_t		Mbx_Init_CANopen_Event(QueueHandle_t mqCmd,
		QueueHandle_t mqResp, MBX_CANOPEN_EVENT_T coEvent);
BaseType_t		Mbx_WaitForResponseMbx(QueueHandle_t mqResp,
		MBX_RESPONSE_T	*pResponse);
BaseType_t		Mbx_WaitForEventMbx(QueueHandle_t mqResp,
		MBX_EVENT_T *pEvent);
BaseType_t		Mbx_WaitForCommandMbx(QueueHandle_t	mqCmd,
		MBX_COMMAND_T *pNewCmd);
void			Mbx_Lock_objDict(void);
void			Mbx_Unlock_objDict(void);

void			canopenTask(void *ptr);

#endif /* CO_MBX_FUNCTION_H */
