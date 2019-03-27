/*
 * t_data_obj_segweld.c
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"

#include "t_data_obj_segweld.h"

//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::USIGN8
*/
//----------------------------------------------------------------------------------------------------------
static  const uint8_t cmAryAttributeDataTypes[] = { ST_FLOAT,ST_FLOAT,ST_FLOAT,
		ST_U16,ST_U16,ST_U16,ST_U16,};

static  const uint8_t cmAryAttributeDataTypes2[] = { ST_FLOAT,ST_U32,ST_U32};


const T_DATA_CLASS cgClassInitializer_SegWeldData =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ, // no business rules
	Get_DATAOBJ,                // overload
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // inherit
	GetObjectDescription_DATAOBJ // inherit
};

const T_DATA_CLASS cgClassInitializer_CaliPoint =
{
	cmAryAttributeDataTypes2,
	Check_DATAOBJ, // no business rules
	Get_DATAOBJ,                // overload
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // inherit
	GetObjectDescription_DATAOBJ // inherit
};

