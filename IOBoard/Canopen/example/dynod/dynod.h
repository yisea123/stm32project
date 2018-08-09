/*
* dynod.h - contains definitions for dynamic OD handling
*
* Copyright (c) 2014-2015 emtas GmbH
*
*-------------------------------------------------------------------
*-------------------------------------------------------------------
*
* Changelog:
*
*/

/**
* \brief defines for dynamic OD handling
*
* \file dynod.h - contains defines for SDO service
*/

#ifndef CO_DYNOD_H
#define CO_DYNOD_H 1

#include <co_datatype.h>

RET_T coDynOdInit(
		UNSIGNED16  objCnt,		/**< number of new objects */
		UNSIGNED16  u8Cnt,		/**< number of U8 vars */
		UNSIGNED16  u16Cnt,		/**< number of U16 vars */
		UNSIGNED16  u32Cnt,		/**< number of U32 vars */
		UNSIGNED16  i8Cnt,		/**< number of i8 vars */
		UNSIGNED16  i16Cnt,		/**< number of i16 vars */
		UNSIGNED16  i32Cnt		/**< number of i32 vars */
    );

RET_T coDynOdAddIndex(
		UNSIGNED16  index,		/**< index */
		UNSIGNED8   nrOfSubs,		/**< number of subindex */
		CO_ODTYPE_T	odType		/**< variable, array, struct */
	);

RET_T coDynOdAddSubIndex(
		UNSIGNED16  index,		/**< index */
		UNSIGNED8   subIndex,		/**< number of subindex */
		CO_DATA_TYPE_T  dataType,	/**< data type */
		UNSIGNED16  attr,		/**< attribute */
		void        *pVar		/**< pointer to variable */
	);


#endif /* CO_DYNOD_H */

