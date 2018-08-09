/*
* co_sdoqueue.c - sdo queue
*
* Copyright (c) 2013-2017 emtas GmbH
*-------------------------------------------------------------------
* $Id: co_sdoqueue.c 19914 2017-09-26 13:13:31Z boe $
*
*
*-------------------------------------------------------------------
* Changelog:
*
*
*/

/********************************************************************/
/**
* \brief SDO handling with queuing
*
* \file co_sdoqueue.c
*
*/


/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#ifdef CO_SDO_QUEUE
#include <co_commtask.h>
#include <co_drv.h>
#include <co_timer.h>
#include <co_sdo.h>
#include "ico_cobhandler.h"
#include "ico_queue.h"
#include "ico_event.h"
#include "ico_sdoclient.h"
# include "co_odaccess.h"
#ifdef CO_SDO_NETWORKING
# include "ico_sdoserver.h"
#endif /* CO_SDO_NETWORKING */
#include "ico_sdo.h"

/* constant definitions
---------------------------------------------------------------------------*/
typedef enum {
	CO_SDO_QUEUE_STATE_NEW,
	CO_SDO_QUEUE_STATE_TRANSMITTING,
	CO_SDO_QUEUE_STATE_FINISHED
} CO_SDO_QUEUE_STATE_T;


/* SDO queue entry */
typedef struct {
	UNSIGNED32	dataLen;			/* datalen */
	UNSIGNED8	*pData;				/* pointer of data */
	void		*pFctPara;			/* function parameter */
	CO_SDO_QUEUE_IND_T	pFct;		/* pointer to function */
	UNSIGNED16	remoteIndex;		/* remote index */
	UNSIGNED16	localIndex;			/* local index */
	UNSIGNED8	data[4];			/* data (saved for write access) */
	UNSIGNED8	sdoNr;				/* sdo number */
	UNSIGNED8	remoteSubIndex;		/* remote subindex */
	UNSIGNED8	localSubIndex;		/* local subindex */
	CO_SDO_QUEUE_STATE_T state;		/* internal state */
	BOOL_T		write;				/* read/write */
} CO_SDO_QUEUE_T;


/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T sdoQueueAddTransfer(BOOL_T write, UNSIGNED8 sdoNr,
		UNSIGNED16	remoteIndex, UNSIGNED8	remoteSubIndex,
		UNSIGNED16	localIndex, UNSIGNED8	localSubIndex,
		UNSIGNED8	*pData, UNSIGNED32	dataLen,
		CO_SDO_QUEUE_IND_T	pFct, void	*pFctPara);
static void startSdoTransfer(void *ptr);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static CO_SDO_QUEUE_T	sdoBuf[CO_SDO_QUEUE_LEN];
static UNSIGNED16	rdIdx = { 0u };
static UNSIGNED16	wrIdx = { 0u };
static const UNSIGNED32	timeOut = { 1000ul };
static CO_EVENT_T	startSdoEvent;




/***************************************************************************/
/**
*
* \brief coSdoQueueAddTransfer - add sdo transfer to sdo queue handler
*
* This function can be used to add sdo transfers to a queue.
* If a tranfer was finished, the next will start automatically.
* After each transfer, the given function with the parameter are called.
*
* Please note: Only allowed for expedited transfers with initialized sdo channel.
* Transmit data are saved internally.
*
* \return
*	RET_T
*/
RET_T coSdoQueueAddTransfer(
		BOOL_T		write,				/**< write/read access */
		UNSIGNED8	sdoNr,				/**< sdo number */
		UNSIGNED16	index,				/**< index */
		UNSIGNED8	subIndex,			/**< subIndex */
		UNSIGNED8	*pData,				/**< pointer to transfer data */
		UNSIGNED32	dataLen,			/**< len of transfer data */
		CO_SDO_QUEUE_IND_T	pFct,		/**< pointer to finish function */
		void		*pFctPara			/**< pointer to data field for finish function */
	)
{
RET_T	retVal;

	retVal = sdoQueueAddTransfer(write, sdoNr, index, subIndex, 0u, 0u,
		pData, dataLen, pFct, pFctPara);

	return(retVal);
}


/***************************************************************************/
/**
*
* \brief coSdoQueueAddOdTransfer - add sdo transfer to sdo queue handler
*
* This function can be used to add sdo transfers 
* from local object dicationary to remote dictionary to a queue.
* If a tranfer was finished, the next will start automatically.
* After each transfer, the given function with the parameter are called.
*
* Please note: Only allowed for expedited transfers with initialized sdo channel.
* Transmit data are saved internally.
*
* \return
*	RET_T
*/
RET_T coSdoQueueAddOdTransfer(
		BOOL_T		write,			/**< write/read access */
		UNSIGNED8	sdoNr,			/**< sdo number */
		UNSIGNED16	remoteIndex,	/**< remote index */
		UNSIGNED8	remoteSubIndex,	/**< remote subIndex */
		UNSIGNED16	localIndex,		/**< local index */
		UNSIGNED8	localSubIndex,	/**< local subindex */
		CO_SDO_QUEUE_IND_T	pFct,	/**< pointer to finish function */
		void		*pFctPara		/**< pointer to data field for finish function */
	)
{
RET_T	retVal;
UNSIGNED8	*pData;
UNSIGNED32	dataLen;
CO_CONST CO_OBJECT_DESC_T *pDescPtr;

	pData = coOdGetObjAddr(localIndex, localSubIndex);
	if (pData == NULL)  {
		return(RET_OD_ACCESS_ERROR);
	}

	retVal = coOdGetObjDescPtr(localIndex, localSubIndex, &pDescPtr);
	if (retVal != RET_OK)  {
		return(retVal);
	}

	/* only main datatypes are supported */
	switch(pDescPtr->dType)  {
		case CO_DTYPE_BOOL_VAR:
		case CO_DTYPE_BOOL_PTR:
		case CO_DTYPE_U8_VAR:
		case CO_DTYPE_U8_PTR:
		case CO_DTYPE_U16_VAR:
		case CO_DTYPE_U16_PTR:
		case CO_DTYPE_U32_VAR:
		case CO_DTYPE_U32_PTR:
		case CO_DTYPE_I8_VAR:
		case CO_DTYPE_I8_PTR:
		case CO_DTYPE_I16_VAR:
		case CO_DTYPE_I16_PTR:
		case CO_DTYPE_I32_VAR:
		case CO_DTYPE_I32_PTR:
			break;
		default:
			return(RET_PARAMETER_INCOMPATIBLE);
	}

	dataLen = coOdGetObjSize(pDescPtr);

	retVal = sdoQueueAddTransfer(write, sdoNr,
		remoteIndex, remoteSubIndex,
		localIndex, localSubIndex,
		pData, dataLen, pFct, pFctPara);

	return(retVal);
}


/***************************************************************************/
/**
* \internal
*
* \brief sdoQueueAddTransfer - add sdo transfer to sdo queue handler
*
* \return
*	RET_T
*/
static RET_T sdoQueueAddTransfer(
		BOOL_T		write,				/* write/read access */
		UNSIGNED8	sdoNr,				/* sdo number */
		UNSIGNED16	remoteIndex,		/* index */
		UNSIGNED8	remoteSubIndex,		/* subIndex */
		UNSIGNED16	localIndex,			/* index */
		UNSIGNED8	localSubIndex,		/* subIndex */
		UNSIGNED8	*pData,				/* pointer to transfer data */
		UNSIGNED32	dataLen,			/* len of transfer data */
		CO_SDO_QUEUE_IND_T	pFct,		/* pointer to finish function */
		void		*pFctPara			/* pointer to data field for finish function */
	)
{
CO_SDO_QUEUE_T	*pSdoBuf;
UNSIGNED16	idx;

	CO_DEBUG4("coSdoAddTransfer: write: %d, sdoNr: %d, idx: %x:%d, ",
		write, sdoNr, index, subIndex);
	CO_DEBUG4("data %x %x %x %x\n",
		pData[0], pData[1], pData[2], pData[3]);
	CO_DEBUG2("coSdoAddTransfer start: rd: %d wr:%d\n", rdIdx, wrIdx);

	if ((sdoNr < 1u) || (sdoNr > 128u) || (dataLen > 4u))  {
		return(RET_INVALID_PARAMETER);
	}

	idx = wrIdx + 1u;
	if (idx == CO_SDO_QUEUE_LEN)  {
		idx = 0u;
	}
	if (idx == rdIdx)  {
		return(RET_OUT_OF_MEMORY);
	}

	pSdoBuf = &sdoBuf[wrIdx];
	pSdoBuf->write = write;
	pSdoBuf->sdoNr = sdoNr;
	pSdoBuf->remoteIndex = remoteIndex;
	pSdoBuf->remoteSubIndex = remoteSubIndex;
	pSdoBuf->localIndex = localIndex;
	pSdoBuf->localSubIndex = localSubIndex;
	if (write == CO_TRUE)  {
		(void)coNumMemcpy(&pSdoBuf->data[0], (CO_CONST void *)pData,
			(UNSIGNED32)dataLen, 1u); /*lint !e960 Note: pointer arithmetic other than array indexing used */
		pSdoBuf->pData = pSdoBuf->data;
	} else {
		if (localIndex != 0u)  {
			pSdoBuf->pData = &pSdoBuf->data[0];/*lint !e960 Note: pointer arithmetic other than array indexing used */
		} else {
			pSdoBuf->pData = pData;
		}
	}
	pSdoBuf->dataLen = dataLen;
	pSdoBuf->pFct = pFct;
	pSdoBuf->pFctPara = pFctPara;
	pSdoBuf->state = CO_SDO_QUEUE_STATE_NEW;

	wrIdx = idx;

	CO_DEBUG2("ebSdoAddTransfer end: rd: %d wr:%d\n", rdIdx, wrIdx);

	/* try to start transfer */
	startSdoTransfer(NULL);

	return(RET_OK);
}


/***************************************************************************/
/**
* \internal
*
* \brief startSdoTransfer - try to start next sdo transfer
*
*
* \return
*	void
*/
static void startSdoTransfer(
		void	*ptr
	)
{
RET_T	retVal;
CO_SDO_QUEUE_T	*pSdoBuf;
(void)ptr;

#ifdef ___DEBUG__
{
UNSIGNED8	rd = rdIdx, wr = wrIdx;

	while (rd != wr)  {
		EB_DEBUG("buf %d, state %d, idx %x:%d\n", rd,
			sdoBuf[rd].state,
			sdoBuf[rd].remoteIndex,
			sdoBuf[rd].remoteSubIndex);
		rd++;
		if (rd == CO_SDO_QUEUE_LEN)  {
			rd = 0u;
		}
	}
}
#endif /* __DEBUG__ */

	CO_DEBUG2("startSdoTransfer start: rd: %d wr:%d\n", rdIdx, wrIdx);

	/* data to transmit ? */
	if (rdIdx != wrIdx)  {
		/* buffer waiting ? */
		pSdoBuf = &sdoBuf[rdIdx];
		if (pSdoBuf->state == CO_SDO_QUEUE_STATE_NEW)  {
			if (pSdoBuf->write == CO_TRUE)  {
				/* write */
				CO_DEBUG3("startSdoTransfer: write sdoNr: %d, idx: %x:%d",
					pSdoBuf->sdoNr, pSdoBuf->remoteIndex,
					pSdoBuf->remoteSubIndex);
				CO_DEBUG4(", data %x %x %x %x\n",
					pSdoBuf->pData[0], pSdoBuf->pData[1],
					pSdoBuf->pData[2], pSdoBuf->pData[3]);

				retVal = coSdoWrite(pSdoBuf->sdoNr,
					pSdoBuf->remoteIndex, pSdoBuf->remoteSubIndex,
					&pSdoBuf->data[0], pSdoBuf->dataLen, 1u, timeOut);
			} else {
				/* read */
				CO_DEBUG3("startSdoTransfer: read sdoNr: %d, idx: %x:%d\n",
					pSdoBuf->sdoNr, pSdoBuf->remoteIndex,
					pSdoBuf->remoteSubIndex);
				/* check for internal transfers */
				retVal = coSdoRead(pSdoBuf->sdoNr,
					pSdoBuf->remoteIndex, pSdoBuf->remoteSubIndex,
					pSdoBuf->pData, pSdoBuf->dataLen, 1u, timeOut);
			}
			if (retVal != RET_OK)  {
				/* start function again */
				(void)icoEventStart(&startSdoEvent,
					startSdoTransfer, NULL);
				return;
			}
			pSdoBuf->state = CO_SDO_QUEUE_STATE_TRANSMITTING;
		}
	}

	CO_DEBUG2("startSdoTransfer end: rd: %d wr:%d\n", rdIdx, wrIdx);
}


/***************************************************************************/
/**
* \internal
*
* \brief icoSdoClientQueueInd
*
*
* \return
*	void
*/
void icoSdoClientQueueInd(
		UNSIGNED8		sdoNr,			/* sdo number */
		UNSIGNED16		index,			/* index */
		UNSIGNED8		subIndex,		/* subindex */
		UNSIGNED32		result			/* result of transfer */
	)
{
CO_CONST CO_OBJECT_DESC_T *pDescPtr;
CO_SDO_QUEUE_T	*pSdoBuf;
UNSIGNED16	u16;
UNSIGNED32	u32;
INTEGER16	i16;
INTEGER32	i32;

	pSdoBuf = &sdoBuf[rdIdx];

	CO_DEBUG2("coSdoClientWriteInd start: rd: %d wr:%d\n", rdIdx, wrIdx);
	CO_DEBUG4("coSdoClientWriteInd: sdoNr: %d, idx: %x:%d, result: %x\n",
		sdoNr, index, subIndex, result);

	CO_DEBUG4("coSdoClientWriteInd: sdoBuf[%d].sdoNr: %d sdoBuf[%d].index: %x\n",
		rdIdx, sdoBuf[rdIdx].sdoNr, rdIdx, sdoBuf[rdIdx].remoteIndex);

	if ((sdoNr == pSdoBuf->sdoNr)
	 && (index == pSdoBuf->remoteIndex)
	 && (subIndex == pSdoBuf->remoteSubIndex))  {
		CO_DEBUG1("coSdoClientWriteInd: state: %d\n", pSdoBuf->state);

		if (pSdoBuf->state == CO_SDO_QUEUE_STATE_TRANSMITTING)  {
			/* only for read access to local objects */
			if ((result == 0u)
			 && (pSdoBuf->write == CO_FALSE)
			 && (pSdoBuf->localIndex != 0u))  {
				/* for coPutObj we need type of data for signed or unsigned */
				(void)coOdGetObjDescPtr(
					pSdoBuf->localIndex, pSdoBuf->localSubIndex, &pDescPtr);

				/* putObj() depends on data type */
				/* so the object indication is called also */
				switch(pDescPtr->dType)  {
					case CO_DTYPE_BOOL_VAR:
					case CO_DTYPE_BOOL_PTR:
					case CO_DTYPE_U8_VAR:
					case CO_DTYPE_U8_PTR:
						(void)coOdPutObj_u8(pSdoBuf->localIndex,
							pSdoBuf->localSubIndex, pSdoBuf->pData[0]);/*lint !e960 Note: pointer arithmetic other than array indexing used */
						break;
					case CO_DTYPE_U16_VAR:
					case CO_DTYPE_U16_PTR:
						(void)coNumMemcpy(&u16, &pSdoBuf->pData[0], 2u, 1u);/*lint !e960 Note: pointer arithmetic other than array indexing used */
						(void)coOdPutObj_u16(pSdoBuf->localIndex,
							pSdoBuf->localSubIndex, u16);
						break;
					case CO_DTYPE_U32_VAR:
					case CO_DTYPE_U32_PTR:
						(void)coNumMemcpy(&u32, &pSdoBuf->pData[0], 4u, 1u);/*lint !e960 Note: pointer arithmetic other than array indexing used */
						(void)coOdPutObj_u32(pSdoBuf->localIndex,
							pSdoBuf->localSubIndex, u32);
						break;
					case CO_DTYPE_I8_VAR:
					case CO_DTYPE_I8_PTR:
						(void)coOdPutObj_i8(pSdoBuf->localIndex,
							pSdoBuf->localSubIndex, pSdoBuf->pData[0]);/*lint !e960 Note: pointer arithmetic other than array indexing used */
						break;
					case CO_DTYPE_I16_VAR:
					case CO_DTYPE_I16_PTR:
						(void)coNumMemcpy(&i16, &pSdoBuf->pData[0], 2u, 1u);/*lint !e960 Note: pointer arithmetic other than array indexing used */
						(void)coOdPutObj_i16(pSdoBuf->localIndex,
							pSdoBuf->localSubIndex, i16);
						break;
					case CO_DTYPE_I32_VAR:
					case CO_DTYPE_I32_PTR:
						(void)coNumMemcpy(&i32, &pSdoBuf->pData[0], 4u, 1u);/*lint !e960 Note: pointer arithmetic other than array indexing used */
						(void)coOdPutObj_i32(pSdoBuf->localIndex,
							pSdoBuf->localSubIndex, i32);
						break;
					default:
						result = 0xffff000u;
				}
			}

			pSdoBuf->state = CO_SDO_QUEUE_STATE_FINISHED;

			/* call indication */
			if (pSdoBuf->pFct != NULL)  {
				pSdoBuf->pFct(pSdoBuf->pFctPara, result);
			}

			rdIdx++;
			if (rdIdx == CO_SDO_QUEUE_LEN)  {
				rdIdx = 0u;
			}
		}
	}
	CO_DEBUG2("coSdoClientWriteInd end: rd: %d wr:%d\n", rdIdx, wrIdx);

	/* start next transfer */
	startSdoTransfer(NULL);
}


/***************************************************************************/
/*
* \brief icoSdoClientVarInit - init sdo client variables
*
*/
void icoSdoQueueVarInit(
		void
	)
{

	{
		rdIdx = 0u;
		wrIdx = 0u;
	}
}


#endif /* CO_SDO_QUEUE */
