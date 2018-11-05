/*
 * t_data_obj_test.c
 *
 *  Created on: 2017年2月20日
 *      Author: pli
 */
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_test.h"


//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::TEST
*/
//----------------------------------------------------------------------------------------------------------
static const uint8_t cmAryAttributeDataTypes[] = { ST_U8, ST_E8, ST_BIT8, ST_CHAR,\
		ST_U16, ST_I16,

		ST_WIDECHAR, ST_BIT16,

		ST_E16,ST_U16,

		ST_U32, ST_I32,ST_T32,
		ST_FLOAT,ST_DOUBLE,
		ST_I64,	ST_U64,
};

const T_DATA_CLASS cgClassInitializer_TEST =
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



