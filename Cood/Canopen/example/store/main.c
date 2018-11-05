/*
* main.c - enthält programm main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 1633 2012-08-30 15:10:43Z  $
*
*
*-------------------------------------------------------------------
* Changelog:
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_canopen.h>

/* constant definitions
---------------------------------------------------------------------------*/
typedef struct {
	UNSIGNED16	index;
	UNSIGNED8	subIndex;
	UNSIGNED32	size;
	UNSIGNED32	val;
} SAVE_OD_T;

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern void coInitObjDictionary();

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static RET_T loadPara(UNSIGNED8	subIndex);
static RET_T savePara(UNSIGNED8	subIndex);
static RET_T clearPara(UNSIGNED8	subIndex);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED16	test;

/* local defined variables
---------------------------------------------------------------------------*/
static BOOL_T		saveOdValid = CO_FALSE;
static SAVE_OD_T	savedData[20];
static UNSIGNED16	savedCnt;
#ifdef USE_SAVE_TABLE
static SAVE_OD_T	saveObj[] = {
	{ 0x1005u, 0u, 4,	0 },
	{ 0x1017u, 0u, 2,	0 },
	{ 0x1029u, 1u, 1,	0 },
	{ 0x1029u, 2u, 1,	0 },
	{ 0x1800u, 3u, 2,	0 },
	{ 0x1800u, 5u, 2,	0 },
};
#endif /* USE_SAVE_TABLE */




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
	/* hardware initialization */
	codrvHardwareInit();

	if (codrvCanInit(250) != RET_OK)  {
		exit(1);
	}
    if (coCanOpenStackInit(loadPara) != RET_OK)  {
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
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		exit(4);
	}
	if (coEventRegister_SAVE_PARA(savePara) != RET_OK)  {
		exit(5);
	}
	if (coEventRegister_CLEAR_PARA(clearPara) != RET_OK)  {
		exit(6);
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
void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	printf("hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

    return;
}


/*********************************************************************/
static RET_T loadPara(
		UNSIGNED8	sIndex
	)
{
UNSIGNED16	i, startIdx, lastIdx;
RET_T	retVal;
const CO_OBJECT_DESC_T *pDesc;
void	*pObj;

	printf("load para indication %d\n", sIndex);

	if (saveOdValid != CO_TRUE)  {
		return(RET_OK);
	}

	switch (sIndex)  {
		case 1: /* all */
			startIdx = 0x1000;
			lastIdx = 0xffff;
			break;
		case 2: /* comm */
			startIdx = 0x1000;
			lastIdx = 0x1fff;
			break;
		default:
			startIdx = 0x1000;
			lastIdx = 0x1000;
			break;
	}

	for (i = 0; i < savedCnt; i++)  {	
		if ((savedData[i].index >= startIdx)
		 && (savedData[i].index <= lastIdx))  {

			/* get object description */
			retVal = coOdGetObjDescPtr(savedData[i].index, savedData[i].subIndex, &pDesc);
			if (retVal != RET_OK)  {
				return(retVal);
			}

			/* get pointer to object */
			pObj = coOdGetObjAddr(savedData[i].index, savedData[i].subIndex);
			if (pObj == NULL)  {
				return(RET_IDX_NOT_FOUND);
			}

printf("Load %x:%d, size %d\n", savedData[i].index, savedData[i].subIndex, savedData[i].size);
			/* get data type of this index */
			switch (savedData[i].size)  {
				case 1:
				case 2:
				case 4:
					coNumMemcpy(pObj, &savedData[i].val, savedData[i].size, CO_ATTR_NUM);
					break;
				default:
					return(RET_SDO_INVALID_VALUE);
			}
		}
	}

	return(RET_OK);
}


/*********************************************************************/
static RET_T savePara(
		UNSIGNED8	sIndex
	)
{
UNSIGNED16	i, startIdx, lastIdx;
RET_T	retVal;
UNSIGNED32	nrOfObj;
const CO_OBJECT_DESC_T *pDesc;
UNSIGNED32	size;
UNSIGNED16	index;
UNSIGNED8	subIndex;
void	*pObj;

	printf("save para indication %d\n", sIndex);

	switch (sIndex)  {
		case 1: /* all */
			startIdx = 0x1000;
			lastIdx = 0xffff;
			break;
		case 2: /* comm */
			startIdx = 0x1000;
			lastIdx = 0x1fff;
			break;
		default:
			startIdx = 0x1000;
			lastIdx = 0x1000;
			break;
	}

	/* get number of saved objects */
#ifdef USE_SAVE_TABLE
	nrOfObj = sizeof(saveObj) / sizeof(SAVE_OD_T);
#else /* USE_SAVE_TABLE */
	nrOfObj = coOdGetObjStoreFlagCnt();
#endif /* USE_SAVE_TABLE */

	savedCnt = 0;

	/* for all saved objects */
	for (i = 0; i < nrOfObj; i++)  {	

#ifdef USE_SAVE_TABLE
		index = saveObj[i].index;
		subIndex = saveObj[i].subIndex;
#else /* USE_SAVE_TABLE */

		/* get next index/subindex */
		retVal = coOdGetObjStoreFlag(i + 1, &index, &subIndex);
		if (retVal != RET_OK)  {
			return(retVal);
		}
#endif /* USE_SAVE_TABLE */

		if ((index >= startIdx)
		 && (index <= lastIdx))  {
			/* get object description */
			retVal = coOdGetObjDescPtr(index, subIndex, &pDesc);
			if (retVal != RET_OK)  {
				return(retVal);
			}
			/* get size info */
			size = coOdGetObjSize(pDesc);
			/* get pointer to object */
			pObj = coOdGetObjAddr(index, subIndex);
			if (pObj == NULL)  {
				return(RET_IDX_NOT_FOUND);
			}

printf("Store %x:%d, size %d\n", index, subIndex, size);
			/* get data type of this index */
			switch (size)  {
				case 1:
				case 2:
				case 4:
					savedData[savedCnt].index = index;
					savedData[savedCnt].subIndex = subIndex;
					savedData[savedCnt].size = size;
					coNumMemcpy(&savedData[savedCnt].val, pObj, size, CO_ATTR_NUM);
					savedCnt++;
					break;
				default:
					return(RET_SDO_INVALID_VALUE);
			}
		}
	}

	saveOdValid = CO_TRUE;
	return(RET_OK);
}


/*********************************************************************/
static RET_T clearPara(
		UNSIGNED8	subIndex
	)
{
	printf("clear para indication %d\n", subIndex);

	if (subIndex == 3)  {
		return(RET_ERROR_STORE);
	}
	saveOdValid = CO_FALSE;

	return(RET_OK);
}


