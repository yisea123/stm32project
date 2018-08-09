/*
* dynod_user.c - contains dynamic od handling example
*
* Copyright (c) 2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: $
*
*
*-------------------------------------------------------------------
*/

/********************************************************************/
/**
* \file
* \brief This file shows implementation of dynamic user objects
*
* Use this file to replace co_dynod.c to define own dynamic data
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>
#include <co_canopen.h>
#include "../../colib/src/ico_dynod.h"

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
UNSIGNED16 myVariable;
static CO_OBJECT_DESC_T objDesc;



/***************************************************************************/
/**
* \internal
*
* \brief icoDynOdGetObjDescPtr - get dynod obj description pointer
*
* \return RET_T
*
*/
RET_T icoDynOdGetObjDescPtr(
		UNSIGNED16 index,
		UNSIGNED8 subIndex,
		CO_CONST CO_OBJECT_DESC_T **pDescPtr
	)
{

	if ((index >= 0x2000) && (index < 0x2100))  {
		objDesc.subIndex = subIndex;
		objDesc.dType = CO_DTYPE_U16_PTR;
		objDesc.attr = CO_ATTR_READ
			|	CO_ATTR_WRITE
			|	CO_ATTR_NUM	
			|	CO_ATTR_MAP
			|	CO_ATTR_MAP_TR
			|	CO_ATTR_MAP_REC
			|	CO_ATTR_DYNOD;
		*pDescPtr = &objDesc;
		return(RET_OK);
	} else {
		return(RET_IDX_NOT_FOUND);
	}
}


/***************************************************************************/
/**
* \internal
*
* \brief icoDynOdGetObjAddr - return dynod object adresse
*
* \return RET_T
*
*/
UNSIGNED8 *icoDynOdGetObjAddr(
		CO_CONST CO_OBJECT_DESC_T	*pDesc  /* pointer for description index */
	)
{
	if (pDesc == &objDesc)  {
		return((UNSIGNED8 *)&myVariable);
	} else {
		return(NULL);
	}
}


/***************************************************************************/
/**
* \internal
*
* \brief icoDynOdGetObjSize - return size for dynamic object
*
* \return RET_T
*
*/

UNSIGNED32 icoDynOdGetObjSize(
		CO_CONST CO_OBJECT_DESC_T	*pDesc	/* pointer for description index */
	)
{
UNSIGNED32	size = 0;

	if (pDesc == &objDesc)  {
		return(sizeof(myVariable));
	} else {
		return(0u);
	}

	return(size);
}
