/*
* dynod.c - contains dynamic od handling for UNSIGNED8 .. 32 and INTEGER8 .. 32 objects
*
* Copyright (c) 2014-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: dynod.c 8720 2015-03-06 10:47:13Z oe $
*
*
*-------------------------------------------------------------------
*/

/********************************************************************/
/**
* \file
* \brief This file implements a dynamic object dictionary for objects 
*        => 0x2000.
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#define coOdInitOdPtr(p1, p2, p3, p4, p5, p6)	coDynOdInitOdPtr()
#include "gen_objdict.c"
#undef coOdInitOdPtr

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/
extern void coOdInitOdPtr(
		const CO_OD_ASSIGN_T	 *pOdAssing,
		UNSIGNED16		 odCnt,
		const CO_OBJECT_DESC_T *pObjdesc,
		UNSIGNED16		 descCnt,
		CO_EVENT_OBJECT_CHANGED_FCT_T	*pEventPtr,
		const CO_OD_DATA_VARIABLES_T *pOdVarPointers
);


/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static CO_OD_ASSIGN_T *dynod_pOdAssign;
static CO_OBJECT_DESC_T *dynod_pDesc;
static CO_OD_DATA_VARIABLES_T dynodDataVariables;
static UNSIGNED16 dynod_odAssignCnt;
static UNSIGNED16 dynod_odAssignMax;
static UNSIGNED16 dynod_descCnt;
static UNSIGNED16 dynod_descMax;
static UNSIGNED16 dynod_U8cnt;
static UNSIGNED16 dynod_U8max;
static UNSIGNED16 dynod_U16cnt;
static UNSIGNED16 dynod_U16max;
static UNSIGNED16 dynod_U32cnt;
static UNSIGNED16 dynod_U32max;
static UNSIGNED16 dynod_I8cnt;
static UNSIGNED16 dynod_I8max;
static UNSIGNED16 dynod_I16cnt;
static UNSIGNED16 dynod_I16max;
static UNSIGNED16 dynod_I32cnt;
static UNSIGNED16 dynod_I32max;
static UNSIGNED8  **dynod_pU8;
static UNSIGNED16 **dynod_pU16;
static UNSIGNED32 **dynod_pU32;
static INTEGER8  **dynod_pI8;
static INTEGER16 **dynod_pI16;
static INTEGER32 **dynod_pI32;



/***************************************************************************/
/**
* \brief coDynOdInit - init dynamic object dictionary
*
* \retval RET_OK
*	initialisation OK
* \retval RET_EVENT_NO_RESSOURCE
*	error at malloc()
*/

RET_T coDynOdInit(
		UNSIGNED16  objCnt,		/**< number of new objects */
		UNSIGNED16  u8Cnt,		/**< number of U8 vars */
		UNSIGNED16  u16Cnt,		/**< number of U16 vars */
		UNSIGNED16  u32Cnt,		/**< number of U32 vars */
		UNSIGNED16  i8Cnt,		/**< number of i8 vars */
		UNSIGNED16  i16Cnt,		/**< number of i16 vars */
		UNSIGNED16  i32Cnt		/**< number of i32 vars */
    )
{
	/* get memory for od assign */
	dynod_odAssignCnt = CO_OD_ASSIGN_CNT;
	dynod_odAssignMax = CO_OD_ASSIGN_CNT + objCnt;
	dynod_pOdAssign = (CO_OD_ASSIGN_T*)malloc(dynod_odAssignMax * sizeof(CO_OD_ASSIGN_T));
	if (dynod_pOdAssign == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	/* copy original OD there */
	memcpy(dynod_pOdAssign, &od_assign, sizeof(od_assign));


	/* get memory for subindex description */
	dynod_descCnt = CO_OBJ_DESC_CNT;
	dynod_descMax = CO_OBJ_DESC_CNT + u8Cnt + u16Cnt + u32Cnt;
	dynod_pDesc = (CO_OBJECT_DESC_T*)malloc(dynod_descMax * sizeof(CO_OBJECT_DESC_T));
	if (dynod_pDesc == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	/* copy original OD there */
	memcpy(dynod_pDesc, &od_description, sizeof(od_description));

	/* get memory for pointer table U8 */
	dynod_pU8 = (UNSIGNED8**)malloc(u8Cnt * sizeof(UNSIGNED8 *));
	if (dynod_pU8 == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	dynod_U8cnt = 0;
	dynod_U8max = u8Cnt;

	/* get memory for pointer table U16 */
	dynod_pU16 = (UNSIGNED16**)malloc(u16Cnt * sizeof(UNSIGNED16 *));
	if (dynod_pU16 == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	dynod_U16cnt = 0;
	dynod_U16max = u16Cnt;

	/* get memory for pointer table U32 */
	dynod_pU32 = (UNSIGNED32**)malloc(u32Cnt * sizeof(UNSIGNED32 *));
	if (dynod_pU32 == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	dynod_U32cnt = 0;
	dynod_U32max = u32Cnt;

	/* get memory for pointer table I8 */
	dynod_pI8 = (INTEGER8**)malloc(i8Cnt * sizeof(INTEGER8 *));
	if (dynod_pI8 == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	dynod_I8cnt = 0;
	dynod_I8max = u8Cnt;

	/* get memory for pointer table I16 */
	dynod_pI16 = (INTEGER16**)malloc(i16Cnt * sizeof(INTEGER16 *));
	if (dynod_pI16 == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	dynod_I16cnt = 0;
	dynod_I16max = i16Cnt;

	/* get memory for pointer table I32 */
	dynod_pI32 = (INTEGER32**)malloc(i32Cnt * sizeof(INTEGER32 *));
	if (dynod_pI32 == NULL)  {
		return(RET_EVENT_NO_RESSOURCE);
	}
	dynod_I32cnt = 0;
	dynod_I32max = i32Cnt;


	/* copy od variable tables */
	memcpy(&dynodDataVariables, &od_data_variables,
			sizeof(CO_OD_DATA_VARIABLES_T)); 

	/* and exchange with the new one */
	dynodDataVariables.odPtr_u8 = dynod_pU8;
	dynodDataVariables.odPtr_u16 = dynod_pU16;
	dynodDataVariables.odPtr_u32 = dynod_pU32;
	dynodDataVariables.odPtr_i8 = dynod_pI8;
	dynodDataVariables.odPtr_i16 = dynod_pI16;
	dynodDataVariables.odPtr_i32 = dynod_pI32;
	return(RET_OK);
}


/***************************************************************************/
/**
* \brief coDynOdAddIndex - add a new object index
*
* \retval RET_IDX_NOT_FOUND
*	index < 0x2000 are not allowed
* \retval RET_INVALID_PARAMETER
*	index already exist
* \retval RET_EVENT_NO_RESSOURCE
*	no resource available
*/

RET_T coDynOdAddIndex(
		UNSIGNED16  index,			/**< index */
		UNSIGNED8   nrOfSubs,		/**< number of subindex */
		CO_ODTYPE_T	odType			/**< variable, array, struct */
	)
{
UNSIGNED16  idx = 0xffff;
UNSIGNED16	i;

	
	/* only index >= 0x2000 are allowed */
	if (index < 0x2000)  {
		return(RET_IDX_NOT_FOUND);
	}

	/* check, if index is already listed */
	for (i = 0; i < dynod_odAssignCnt; i++)  {
		if (dynod_pOdAssign[i].index == index)  {
			idx = i;
			break;
		}
	}
	/* if it already exist, abort */
	if (idx != 0xffff)  {
		return(RET_INVALID_PARAMETER);
	}

	/* check for free entry */
	if (dynod_odAssignCnt == dynod_odAssignMax)  {
		return(RET_EVENT_NO_RESSOURCE);
	}

	/* not listed, add it */
	for (idx = dynod_odAssignCnt; idx > 0; idx--)  {
		if (index < dynod_pOdAssign[idx - 1].index)  {
			memcpy(&dynod_pOdAssign[idx].index, &dynod_pOdAssign[idx - 1],
				   sizeof(CO_OD_ASSIGN_T));
		} else {
			break;
		}
	}
	dynod_pOdAssign[idx].index = index;
	dynod_pOdAssign[idx].numberOfSubs = nrOfSubs;
	dynod_pOdAssign[idx].highestSub = 0u;
	dynod_pOdAssign[idx].odType = odType;
	dynod_pOdAssign[idx].odDescIdx = dynod_descCnt;
	dynod_pDesc[dynod_descCnt].subIndex = 0xff;
	/* enough desc list entries available ? */
	if ((dynod_descCnt + nrOfSubs) > dynod_descMax)  {
		/* no, abort */
		dynod_pOdAssign[idx].index = 0;
		return(RET_EVENT_NO_RESSOURCE);
	}

	dynod_descCnt += nrOfSubs;
	dynod_odAssignCnt ++;

	return(RET_OK);
}


/***************************************************************************/
/**
* \brief coDynOdAddSubIndex - add new subindex
*
* no check for to many data or duplicate subindex
*
* \retval RET_DATA_TYPE_MISMATCH
*	data type not supported (only U8, U16, U32, I8, I16, I32 allowed)
* \retval RET_IDX_NOT_FOUND
*	index not found
*/

RET_T coDynOdAddSubIndex(
		UNSIGNED16  index,			/**< index */
		UNSIGNED8   subIndex,		/**< number of subindex */
		CO_DATA_TYPE_T  dataType,	/**< data type */
		UNSIGNED16  attr,			/**< attribute */
		void        *pVar			/**< pointer to variable */
	)
{
UNSIGNED16  idx = 0xffff;
UNSIGNED16	i;
UNSIGNED16	offs;
UNSIGNED16	*pCnt, *pMax;

	/* check if pointer is != 0 */
    if (pVar == NULL) {
		return(RET_EVENT_NO_RESSOURCE);
	}

	/* check for allowed data types */
	switch (dataType)  {
		case CO_DTYPE_U8_PTR:
			pCnt = &dynod_U8cnt;
			pMax = &dynod_U8max;
			break;
		case CO_DTYPE_U16_PTR:
			pCnt = &dynod_U16cnt;
			pMax = &dynod_U16max;
			break;
		case CO_DTYPE_U32_PTR:
			pCnt = &dynod_U32cnt;
			pMax = &dynod_U32max;
			break;
		case CO_DTYPE_I8_PTR:
			pCnt = &dynod_I8cnt;
			pMax = &dynod_I8max;
			break;
		case CO_DTYPE_I16_PTR:
			pCnt = &dynod_I16cnt;
			pMax = &dynod_I16max;
			break;
		case CO_DTYPE_I32_PTR:
			pCnt = &dynod_I32cnt;
			pMax = &dynod_I32max;
			break;
		default:
			return(RET_DATA_TYPE_MISMATCH);
	}

	/* free entry for data pointer available ? */
	if (*pCnt >= *pMax)  {
		return(RET_EVENT_NO_RESSOURCE);
	}

	/* search index */
	for (i = 0; i < dynod_odAssignCnt; i++)  {
		if (dynod_pOdAssign[i].index == index)  {
			idx = i;
			break;
		}
	}
	if (idx == 0xffff)  {
		return(RET_IDX_NOT_FOUND);
	}

	/* set offset for start subindex desc */
	offs = dynod_pOdAssign[idx].odDescIdx;

    /* search subindex */
	for (i = 0; i < dynod_pOdAssign[idx].numberOfSubs; i++)  {
		if (subIndex < dynod_pDesc[offs + i].subIndex)  {
			/* move all subs one to the end - if possible */
			if (i < (dynod_pOdAssign[idx].numberOfSubs - 1))  {
				memmove(&dynod_pDesc[offs + i + 1], &dynod_pDesc[offs + i],
					((dynod_pOdAssign[idx].numberOfSubs - i - 1) * sizeof(CO_OBJECT_DESC_T)));
			}
			/* save here my data */
			dynod_pDesc[offs + i].subIndex = subIndex;
			dynod_pDesc[offs + i].dType = dataType;
			dynod_pDesc[offs + i].attr = attr;

			switch (dataType)  {
				case CO_DTYPE_U8_PTR:
					dynod_pU8[dynod_U8cnt] = (UNSIGNED8*)pVar;
					dynod_pDesc[offs + i].tableIdx = dynod_U8cnt;
					dynod_U8cnt ++;
				break;
			case CO_DTYPE_U16_PTR:
					dynod_pU16[dynod_U16cnt] = (UNSIGNED16*)pVar;
					dynod_pDesc[offs + i].tableIdx = dynod_U16cnt;
					dynod_U16cnt ++;
				break;
			case CO_DTYPE_U32_PTR:
					dynod_pU32[dynod_U32cnt] = (UNSIGNED32*)pVar;
					dynod_pDesc[offs + i].tableIdx = dynod_U32cnt;
					dynod_U32cnt ++;
				break;
			case CO_DTYPE_I8_PTR:
					dynod_pI8[dynod_I8cnt] = (INTEGER8*)pVar;
					dynod_pDesc[offs + i].tableIdx = dynod_I8cnt;
					dynod_I8cnt ++;
				break;
			case CO_DTYPE_I16_PTR:
					dynod_pI16[dynod_I16cnt] = (INTEGER16*)pVar;
					dynod_pDesc[offs + i].tableIdx = dynod_I16cnt;
					dynod_I16cnt ++;
				break;
			case CO_DTYPE_I32_PTR:
					dynod_pI32[dynod_I32cnt] = (INTEGER32*)pVar;
					dynod_pDesc[offs + i].tableIdx = dynod_I32cnt;
					dynod_U32cnt ++;
				break;
			default:
				break;
			}

			/* setup highest subindex */
			if (subIndex > dynod_pOdAssign[idx].highestSub)  {
				dynod_pOdAssign[idx].highestSub = subIndex;
			}
			break;
		}
	}

	return(RET_OK);
}


void coDynOdInitOdPtr(
	)
{
	coOdInitOdPtr(dynod_pOdAssign, dynod_odAssignCnt, dynod_pDesc, dynod_descCnt, NULL, &dynodDataVariables);

}
