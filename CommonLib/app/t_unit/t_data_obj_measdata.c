/*
 * t_data_obj_measdata.c
 *
 *  Created on: 2017骞�3鏈�13鏃�
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_measdata.h"


//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::MEASDATA
*/
//----------------------------------------------------------------------------------------------------------
static const uint8_t cmAryAttributeDataTypes1[] = { ST_T32, \
													ST_FLOAT,ST_FLOAT,ST_FLOAT,ST_FLOAT,\
													ST_FLOAT,\
													ST_FLOAT,\
													ST_FLOAT,\
													ST_FLOAT,\
													ST_FLOAT,\
													ST_FLOAT,\
													ST_I16,ST_I16,ST_I16,ST_I16,\
													ST_U16,\
													ST_U16,\
													ST_T32,\
													};

const T_DATA_CLASS cgClassInitializer_MeasData =
{
	cmAryAttributeDataTypes1,
	Check_DATAOBJ,  // no business rule
	0,              // no internal view
	0,
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};


static const uint8_t cmAryAttributeDataTypes2[] = { ST_T32, \
													ST_FLOAT, ST_FLOAT,\
													ST_FLOAT, ST_FLOAT,\
													ST_U16,ST_U16,/* 7 attributes */ \
													ST_T32,ST_FLOAT,ST_FLOAT,ST_FLOAT,ST_FLOAT,ST_I16,ST_I16,ST_I16,ST_I16, /*9 attributes*/ \
													ST_T32,ST_FLOAT,ST_FLOAT,ST_FLOAT,ST_FLOAT,ST_I16,ST_I16,ST_I16,ST_I16,\
													ST_U16,		\
													ST_U16,		\
													};

const T_DATA_CLASS cgClassInitializer_CaliData =
{
	cmAryAttributeDataTypes2,
	Check_DATAOBJ,  // no business rule
	0,              // no internal view
	0,
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};



static const uint8_t cmAryAttributeDataTypes3[] = { ST_T32, \
													ST_FLOAT, ST_FLOAT,ST_FLOAT, ST_FLOAT,\
													ST_I16,ST_I16,ST_I16,ST_I16,/* 9 attributes */ \
													ST_U16,		\
													};

const T_DATA_CLASS cgClassInitializer_MeasDataRaw =
{
	cmAryAttributeDataTypes3,
	Check_DATAOBJ,  // no business rule
	0,              // no internal view
	0,
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};

static const uint8_t cmAryAttributeDataTypes4[] = { ST_T32, \
		ST_U32,ST_U8,ST_U8,ST_U8,ST_U8,ST_U8,ST_U8,};

const T_DATA_CLASS cgClassInitializer_EventData =
{
	cmAryAttributeDataTypes4,
	Check_DATAOBJ,  // no business rule
	0,              // no internal view
	0,
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};





