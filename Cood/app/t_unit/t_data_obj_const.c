/*
 * t_data_obj_const.c
 *
 *  Created on: 2016��11��1��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_const.h"

//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ::USIGN8
*/
//----------------------------------------------------------------------------------------------------------
static  const uint8_t cmAryAttributeDataTypes[] = { ST_U8 };

const T_DATA_CLASS cgClassInitializer_CONST =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ, // no business rules
	Get_CONST,                // overload
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // inherit
	GetObjectDescription_DATAOBJ // inherit
};


//--------------------------------------------------------------------------------------------------
/*!
 \brief  Reads the addressed attribute or the whole object out of ROM or RAM.
         This method doesn't support objects resided in non-volatile areas!

 \param  ptrValue the new object or attribute value
 \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
 \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
 \return error-code
 <pre>
     OK                    operation was successful
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
     METHOD_NOT_SUPPORTED  RAM and NON_VOLATILE are not supported
 </pre>
 */
//--------------------------------------------------------------------------------------------------
uint16_t Get_CONST(const T_DATA_OBJ *me,
					void * ptrValue,
					int16_t attributeIndex,
					OS_RSEMA * ptrDataSemaphore)
{
	T_DATA_OBJ myself;
	uint16_t result;
	uint16_t length;
	int16_t   i;
	int32_t   attribPos;
	uint8_t  * ptrSource;

	// debug zero pointer and illegal values
	VIP_ASSERT(me);        // lint ??
	VIP_ASSERT(ptrValue);
	VIP_ASSERT(ptrDataSemaphore);

	// speed and code optimization
	myself = *me;

	assert(myself.numberOfAttributes);      // divide by zero
	assert(myself.pClass);                  // pointer
	assert( myself.storageQualifier==ROM ); // this method support only ROM!

	if( (attributeIndex>=(int16_t)(myself.numberOfAttributes)) || (attributeIndex<WHOLE_OBJECT) )
	{
		result=ILLEGAL_ATTRIB_IDX;
	}
	else
	{
		// the source is basicly the object-value (for T_DATA_OBJ::CONST it is the description-ptr)
		ptrSource = (uint8_t *)myself.ptrDescription;

		// access to object
		if( attributeIndex<0  )
		{
			// copy complete object or array of objects
			length = myself.objectLength;
			result = OK;
		}
		else
		{
			switch( myself.typeQualifier )
			{
			case SIMPLE:
				length = cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[0]];
				result = OK;
				break;

			case ARRAY:
				// error in data structure
				assert( (myself.objectLength % myself.numberOfAttributes)==0 );

				// calculate the length of one attribute
				length = myself.objectLength / myself.numberOfAttributes;

				// calculate the address offset of the attribute
				attribPos = attributeIndex;
				attribPos*= length;

				// validate pointer-arithmetic
				assert( myself.objectLength>attribPos );

				ptrSource+=attribPos;
				result = OK;
				break;

			case STRUCT:
				length = cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[attributeIndex]];

				attribPos=0;
				for( i=0; i<attributeIndex; i++)
				{
					attribPos+=cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[i]];
				}

				// validate pointer-arithmetic
				assert( myself.objectLength>attribPos );

				ptrSource+=attribPos;
				result = OK;
				break;

			default:
				length=0;
				result = FATAL_ERROR;
				//lint -e{506} -e{774}
				assert(0);
			}
		}

		//lint -e{619} loss of precision (arg. no. 2) (pointer to pointer) is OK
		(void)memcpy(ptrValue,ptrSource,length);  // return value not of interest

	}

	assert( (result<(uint16_t)DEBUG_ERROR) );

	return result;
}


