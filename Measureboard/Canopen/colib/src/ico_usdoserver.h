/*
* ico_sdoserver.h - contains internal defines for SDO
*
* Copyright (c) 2013-2016 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: ico_usdoserver.h 16982 2017-02-27 09:23:30Z phi $

*-------------------------------------------------------------------
*
*
*
*/

/**
* \file
* \brief dataty type header
*/

#ifndef ICO_USDO_SERVER_H
#define ICO_USDO_SERVER_H 1


/* constants */
#define USDO_MAX_SAVE_DATA	64u


/* datatypes */

typedef enum {
	CO_USDO_STATE_FREE,
	CO_USDO_STATE_UPLOAD,
	CO_USDO_STATE_DOWNLOAD,
	CO_USDO_STATE_UPLOAD_SEGMENT,
	CO_USDO_STATE_DOWNLOAD_SEGMENT,
	CO_USDO_STATE_SPLIT_INDICATION,
	CO_USDO_STATE_REMOTE_UPLOAD,
	CO_USDO_STATE_DOWNLOAD_BULK
} CO_USDO_STATE_T;

typedef struct co_usdo_server_con_t {
	struct co_usdo_server_con_t* pNext; /* pointer to next active connection */
	struct co_usdo_server_con_t* pPrev; /* pointer to prev active connection */
	
	CO_USDO_STATE_T	state;			/* sdo state */
	UNSIGNED8		sessionId;		/* session Id of the client */
	BOOL_T			changed;		/* object was changed */
	
	UNSIGNED16		index;			/* index */
	UNSIGNED8		subIndex;		/* sub index */
	UNSIGNED8		clientNode;		/* node id of client */
	
	CO_CONST CO_OBJECT_DESC_T *pObjDesc;		/* object description pointer */

	UNSIGNED32		objSize;		/* object size */
	UNSIGNED32		transferedSize;	/* transfered size */

	UNSIGNED16		connRef;
	UNSIGNED8		saveData[USDO_MAX_SAVE_DATA];
	UNSIGNED8		savedSize;
	UNSIGNED8		segCnt;
	CO_TIMER_T		toTimer;
# ifdef CO_EVENT_SUSDO_DOMAIN_WRITE
	BOOL_T			domainTransfer;	/* object with domain transfer */
	UNSIGNED32		domainTransferedSize;	/* overall transfered size */
# endif /* CO_EVENT_SUSDO_DOMAIN_WRITE */
# ifdef CO_SDO_NETWORKING
# endif /* CO_SDO_NETWORKING */
} CO_USDO_SERVER_CON_T;






/* function prototypes */

void	icoUsdoServerVarInit(void);

void icoUsdoServerAbort(
	CO_USDO_SERVER_CON_T	**ppConn,			/* pointer to sdo */
	RET_T					errorReason,	/* error reason */
	BOOL_T					fromClient		/* called from client */
	);


void icoUsdoServerReset(void);	
#if 0
RET_T	icoSdoCheckUserWriteInd(const CO_SDO_SERVER_T *pSdo);
RET_T	icoSdoCheckUserReadInd(UNSIGNED8 sdoNr, UNSIGNED16 index,
			UNSIGNED8 subIndex);
RET_T	icoSdoCheckUserCheckWriteInd(UNSIGNED8 sdoNr,
			UNSIGNED16 index, UNSIGNED8 subIndex, const UNSIGNED8 *pData);
void	icoSdoDomainUserWriteInd(const CO_SDO_SERVER_T *pSdo);
void	icoSdoServerAbort(CO_SDO_SERVER_T *pSdo, RET_T	errorReason,
			BOOL_T fromClient);

# ifdef CO_SDO_BLOCK
RET_T	icoSdoServerBlockReadInit(CO_SDO_SERVER_T	*pSdo,
			const CO_CAN_MSG_T	*pRecData);
RET_T	icoSdoServerBlockRead(CO_SDO_SERVER_T	*pSdo);
RET_T	icoSdoServerBlockReadCon(CO_SDO_SERVER_T	*pSdo,
			const CO_CAN_MSG_T	*pRecData);
RET_T	icoSdoServerBlockWriteInit(CO_SDO_SERVER_T *pSdo,
			const CO_CAN_MSG_T	*pRecData);
RET_T	icoSdoServerBlockWrite(CO_SDO_SERVER_T *pSdo,
			const CO_CAN_MSG_T *pRecData);
RET_T	icoSdoServerBlockWriteEnd(CO_SDO_SERVER_T *pSdo,
			const CO_CAN_MSG_T	*pRecData);
# endif /* CO_SDO_BLOCK */

#ifdef CO_SDO_NETWORKING
RET_T	icoSdoServerNetworkReq(CO_SDO_SERVER_T *pSdo, const CO_CAN_MSG_T *pRecData);
void	icoSdoServerNetwork(CO_SDO_SERVER_T *pSdo, const CO_CAN_MSG_T *pRecData);
RET_T	icoSdoServerLocalResp(	CO_SDO_SERVER_T	*pSdo,
			const CO_TRANS_DATA_T	*pTrData);
#endif /* CO_SDO_NETWORKING */

CO_SDO_SERVER_T	*icoSdoServerPtr(UNSIGNED16 sdoNr);
#endif /* 0 */

#endif /* ICO_USDO_SERVER_H */
