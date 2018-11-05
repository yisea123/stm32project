/*
 * t_data_obj_simple.c
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"

//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::INT16
*/
//----------------------------------------------------------------------------------------------------------
static  const uint8_t cmAryAttributeDataTypes[] = { ST_U8 };

const T_DATA_CLASS cgClassInitializer_SIMPLE =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ,  // no businessrules
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_SIMPLE,    // overload
	GetObjectDescription_SIMPLE  // overload
};


//--------------------------------------------------------------------------------------------------
/*!
 \brief  Reads the addressed attribute range-information
 \param  me pointer of T_DATA_OBJ
 \param  ptrDescriptor object description
 \param  attributeIndex ==-1 --> putObject; >=0 --> putAttribute
 \return error-code
 <pre>
     OK                    operation was successful
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetAttributeDescription_SIMPLE(const T_DATA_OBJ *me,
										T_DO_ATTRIB_RANGE * ptrDescriptor,
										uint16_t attributeIndex)
{
	T_DATA_OBJ myself;
	uint16_t result;
	// debug zero pointer and illegal values
	assert(me);
	assert(ptrDescriptor);
	// speed and code optimization
	myself = *me;
	assert(myself.pClass);       // pointer
	assert(myself.objectLength); // check length != 0
	assert((uint32_t)myself.ptrDescription > (uint32_t)ST_NIL);  // check valid enumeration type
	assert(myself.ptrDescription < (const void *)
		   ST_NUMBER_OF_SIMPLE_TYPES); // check valid enumeration type
	if(attributeIndex>=myself.numberOfAttributes)
	{
		result = ILLEGAL_ATTRIB_IDX;
	}
	else
	{
		ptrDescriptor->rulesAreActive=false;
		// the source is basicly the object-value
		uint32_t ptrSource = (uint32_t)myself.ptrValue;
		uint32_t attribPos = 0;
		switch(myself.typeQualifier)
		{
			case SIMPLE:
				ptrDescriptor->dataType=(uint8_t)(myself.ptrDescription);
				ptrDescriptor->len = myself.objectLength;
				ptrDescriptor->adr = ptrSource;
				result = OK;
				break;
			case ARRAY:
				ptrDescriptor->dataType=(uint8_t)(myself.ptrDescription);
				// error in data structure
				assert((myself.objectLength % myself.numberOfAttributes)==0);
				// calculate the length of one attribute
				ptrDescriptor->len = myself.objectLength / myself.numberOfAttributes;
				// calculate the address offset of the attribute
				attribPos = attributeIndex;
				attribPos*= ptrDescriptor->len;
				// validate pointer-arithmetic
				assert(myself.objectLength>attribPos);
				ptrSource+=attribPos;
				ptrDescriptor->adr = ptrSource;
				result = OK;
				break;
			case STRUCT:   // structs are not allowed
			default:
				result = FATAL_ERROR;
				//lint -e506 assert(0) is intentional
				assert(0);
				break;
		}
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  Read object description
 \param  me pointer of T_DATA_OBJ
 \param  ptrDescriptor object description
 \return error-code
 <pre>
     OK                    operation was successful
 </pre>
 \warning
 \test
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetObjectDescription_SIMPLE(const struct _T_DATA_OBJ *me,
									 T_DO_OBJECT_DESCRIPTION * ptrDescriptor)
{
	T_DO_OBJECT_DESCRIPTION objDesc;
	T_DATA_OBJ myself;
	// debug zero pointer and illegal values
	assert(me);
	assert(ptrDescriptor);
	// speed and code optimization
	myself = *me;
	assert(myself.pClass);   // pointer
	assert(myself.objectLength); // check length != 0
	assert((uint32_t)myself.ptrDescription > (uint32_t)ST_NIL);  // check valid enumeration type
	assert(myself.ptrDescription < (const void *)
		   ST_NUMBER_OF_SIMPLE_TYPES); // check valid enumeration type
	objDesc.objectLength=myself.objectLength;
	objDesc.numberOfAttributes=myself.numberOfAttributes;
	objDesc.typeQualifier=myself.typeQualifier;
	objDesc.aryAttribDataTypes=(const uint8_t *)(&myself.ptrDescription);
	*ptrDescriptor=objDesc;
	return OK;
}

