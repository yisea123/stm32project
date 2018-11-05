/*
 * t_data_obj_pid.c
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_pid.h"


//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::PID
*/
//----------------------------------------------------------------------------------------------------------
static const uint8_t cmAryAttributeDataTypes[] = { ST_FLOAT, ST_FLOAT, \
		ST_FLOAT, ST_U16, ST_U16};

const T_DATA_CLASS cgClassInitializer_PID =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ,  // no business rule
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};




