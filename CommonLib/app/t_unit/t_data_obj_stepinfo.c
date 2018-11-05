/*
 * t_data_obj_stepinfo.c
 *
 *  Created on: 2017Äê9ÔÂ13ÈÕ
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_stepinfo.h"
//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::FLOWSTEP
*/
//----------------------------------------------------------------------------------------------------------
static const uint8_t cmAryAttributeDataTypes[] = { ST_U32, ST_T32, \
		ST_I32, ST_I32, ST_U32};

const T_DATA_CLASS cgClassInitializer_STEPINFO =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ,  // no business rule
	0,              // no internal view
	0,
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};


