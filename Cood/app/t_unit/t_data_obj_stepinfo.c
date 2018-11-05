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

#pragma   pack(1)

static const uint8_t cmAryAttributeDataTypes[] =  	 { ST_T32,ST_U32,ST_U32};
const T_DATA_CLASS cgClassInitializer_STEPRUN =
 {
	cmAryAttributeDataTypes,
 	0,  // no business rule
 	Get_DATAOBJ,              // inherit
 	Put_DATAOBJ,              // inherit
 	GetAttributeDescription_DATAOBJ,   // overload
 	GetObjectDescription_DATAOBJ // inherit
 };

static const uint8_t cmAryAttributeDataTypes1[] =  	 { ST_U16,ST_U16,ST_U16,ST_U16,ST_U16};
const T_DATA_CLASS cgClassInitializer_STEPINFO =
 {
	cmAryAttributeDataTypes1,
 	0,  // no business rule
 	Get_DATAOBJ,              // inherit
 	Put_DATAOBJ,              // inherit
 	GetAttributeDescription_DATAOBJ,   // overload
 	GetObjectDescription_DATAOBJ // inherit
 };


#pragma   pack()