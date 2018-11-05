/*
 * t_data_obj_time.c
 *
 *  Created on: 2016��11��1��
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#pragma   pack(1)

//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::TIME
*/
//----------------------------------------------------------------------------------------------------------
static const uint8_t cmAryAttributeDataTypes[] = { ST_U16,ST_U8,ST_U8,\
													ST_U8, ST_U8, \
													ST_U8};

const T_DATA_CLASS cgClassInitializer_TIME =
{
	cmAryAttributeDataTypes,
	Check_TIME,  // no business rule
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};


#pragma   pack()

uint16_t   Check_TIME(const struct _T_DATA_OBJ *me,
                         void * ptrValue,
                         int16_t attributeIndex )
{
	uint16_t result;
	uint16_t val;

	// debug zero pointer and illegal values
	VIP_ASSERT(me);
	VIP_ASSERT(ptrValue);


	// validate data-object

	result = OK;
	switch(attributeIndex)
	{
		case 0: //year
			val = *(uint16_t*) ptrValue;
			if (val <= 2015)
				result = LESSTHAN_RANGEMIN_ERR;
			break;
		case 1:
			val = *(uint8_t*) ptrValue;
			if (val > 12)
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case 2:
			val = *(uint8_t*) ptrValue;
			if (val > 31)
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case 3: //hour
			val = *(uint8_t*) ptrValue;
			if (val > 23)
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case 4:
			val = *(uint8_t*) ptrValue;
			if (val > 59)
				result = GREATERTHAN_RANGEMAX_ERR;
			break;
		case 5:
			val = *(uint8_t*) ptrValue;
			if (val > 59)
				result = GREATERTHAN_RANGEMAX_ERR;
			break;

		default:
			result = ILLEGAL_ATTRIB_IDX;
			break;

	}
	return result;
}
