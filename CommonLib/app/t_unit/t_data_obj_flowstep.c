/*
 * t_data_obj_flowstep.c
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"



//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::FLOWSTEP
*/
//----------------------------------------------------------------------------------------------------------
static const uint8_t cmAryAttributeDataTypes[] = { ST_BIT16, \
													ST_U8, ST_U8, ST_U16, ST_U32,\
													ST_U8, ST_U8, ST_U16, ST_U32,\
													ST_U8, ST_U8, ST_U16, ST_U32,\
													};

const T_DATA_CLASS cgClassInitializer_FLOWSTEP =
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
