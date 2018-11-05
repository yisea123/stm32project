/*
* ico_usdoclient.h - contains internal defines for SDO
*
* Copyright (c) 2013-2016 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: ico_usdoclient.h 16982 2017-02-27 09:23:30Z phi $

*-------------------------------------------------------------------
*
*
*
*/

/**
* \file
* \brief dataty type header
*/

#ifndef ICO_USDO_CLIENT_H
#define ICO_USDO_CLIENT_H 1


/* constants */

/* datatypes */
typedef enum {
	CO_USDO_CLIENT_STATE_FREE,
	CO_USDO_CLIENT_STATE_UPLOAD_INIT,
	CO_USDO_CLIENT_STATE_UPLOAD_SEGMENT,
	CO_USDO_CLIENT_STATE_DOWNLOAD_INIT,
	CO_USDO_CLIENT_STATE_DOWNLOAD_SEGMENT,
	CO_USDO_CLIENT_STATE_DOWNLOAD_BULK_INIT,
	CO_USDO_CLIENT_STATE_DOWNLOAD_BULK_CONT,
	CO_USDO_CLIENT_STATE_DOWNLOAD_BULK_LAST
} CO_USDO_CLIENT_STATE_T;



typedef struct co_usdo_client_t {
	UNSIGNED16		index;			/* index */
	UNSIGNED8		subIndex;		/* sub index */
	UNSIGNED8		sessionId;

	UNSIGNED8		nodeId;
	UNSIGNED8		responseId;
	UNSIGNED8		dataType;
	UNSIGNED32		size;
	UNSIGNED32		restSize;

	UNSIGNED32		timeOutVal;
	UNSIGNED32		timeBulk;
	CO_TIMER_T		timer;
	CO_TIMER_T		bulkTimer;
	UNSIGNED8		*pData;
	UNSIGNED8		seqCnt;
	UNSIGNED32		maxData;
	BOOL_T			bulkStart;
	CO_USDO_CLIENT_STATE_T state;
} CO_USDO_CLIENT_T;

/* function prototypes */

void icoUsdoClientVarInit(void);
void icoUsdoClientReset(void);
void icoUsdoClientTimeOut(void *pData);
void icoUsdoClientAbort(CO_USDO_CLIENT_T *pUsdo, RET_T errorReason);

#endif /* ICO_USDO_CLIENT_H */
