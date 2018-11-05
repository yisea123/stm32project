/*
 * t_data_obj.c
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
//----------------------------------------------------------------------------------------------------------
/*! definition of the initializer for objects of class T_DATA_OBJ

    T_DATA_OBJ has no business rules; the used data-type is uint8_t
*/
//----------------------------------------------------------------------------------------------------------
static  const uint8_t cmAryAttributeDataTypes[] = { ST_U8 };

const T_DATA_CLASS cgClassInitializer_TDATAOBJ =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ, // Check() not supported
	Get_DATAOBJ,
	Put_DATAOBJ,
	GetAttributeDescription_DATAOBJ,
	GetObjectDescription_DATAOBJ
};

//--------------------------------------------------------------------------------------------------
/*!
 \brief  dummy function; returns OK
 \param  me; T_DATA_OBJ itself
 \param  ptrValue; the nearest possible object or attribute value will be returned in ptrValue
 \param  attributeIndex; ==-1 --> Object; >=0 --> Attribute
 \return OK; error-code allways OK
 \warning the zero-pointer as a mark for not using Check is not loner allowed!
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t   Check_DATAOBJ(const struct _T_DATA_OBJ *me,
						 void * ptrValue,
						 int16_t attributeIndex)
{
	uint16_t result = OK;
	VIP_ASSERT(me);
	VIP_ASSERT(ptrValue);
	if(attributeIndex>=(int16_t)(me->numberOfAttributes) || attributeIndex<WHOLE_OBJECT)
	{
		result=ILLEGAL_ATTRIB_IDX;
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  put a new object or attribute value into ram, if no business rules are voilated
         Access to memory protected with a resource semaphore will be supported if the pointer to this
         semaphore is not zero.
 <pre>
         1. check storageQualifier
         2. check attributeIndex
         3. check business rules        (function call)
         4. transform to internal view (function call)
         5. copy data if no error until now
         6. transform to external view (function call)
         7. return
 </pre>
 \param  me; T_DATA_OBJ itself
 \param  ptrValue; the new object or attribute value; if return!=OK than return the
         nearest possible value in ptrValue
 \param  attributeIndex; <0 --> Object; >=0 --> Attribute
 \param  ptrDataSemaphore; pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
 \warning
*/
//--------------------------------------------------------------------------------------------------
uint16_t Put_DATAOBJ(const T_DATA_OBJ *me,
					 void * ptrValue,
					 int16_t attributeIndex,
					 OS_RSEMA * ptrDataSemaphore)
{
	uint16_t result;
	uint16_t length;
	int16_t   i;
	int32_t   attribPos;
	uint8_t  * ptrTarget;
	T_DATA_OBJ myself;
	// validate parameter
	VIP_ASSERT(me);
	VIP_ASSERT(ptrValue);
	// speed and code optimization
	myself = *me;
	if(myself.storageQualifier>=ROM)
	{
		result=READ_ONLY_ERR;
	}
	else if(attributeIndex>=(int16_t)(myself.numberOfAttributes) || attributeIndex<WHOLE_OBJECT)
	{
		result=ILLEGAL_ATTRIB_IDX;
	}
	else
	{
		// validate data-object
		assert(myself.numberOfAttributes); // divide by zero
		assert(myself.ptrValue); // pointer
		assert(myself.pClass);   // pointer
		// no check-method is not allowed!
		VIP_ASSERT(myself.pClass->Check);
		// call check-method
		result=myself.pClass->Check(me,ptrValue,attributeIndex);
		if(result<ERROR_CODES)
		{
			// the target is basicly the object-value
			ptrTarget = (uint8_t *)myself.ptrValue;
			// access to object
			if(attributeIndex<0)
			{
				// copy complete object or array of objects
				length = myself.objectLength;
			}
			else
			{
				switch(myself.typeQualifier)
				{
					case SIMPLE:
						length = myself.objectLength;
						break;
					case ARRAY:
						// error in data structure
						assert((myself.objectLength % myself.numberOfAttributes)==0);
						// calculate the length of one attribute
						length = myself.objectLength / myself.numberOfAttributes;
						// calculate the address offset of the attribute
						attribPos = attributeIndex;
						attribPos*= length;
						// validate pointer-arithmetic
						assert(myself.objectLength>attribPos);
						ptrTarget+=attribPos;
						break;
					case STRUCT:
						length = cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[attributeIndex]];
						attribPos=0;
						for(i=0; i<attributeIndex; i++)
						{
							attribPos+=cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[i]];
						}
						// validate pointer-arithmetic
						assert(myself.objectLength>attribPos);
						ptrTarget+=attribPos;
						break;
					default:
						length=0;
						//lint -e506 assert(0) is intentional
						assert(0);
				}
			}
			// use semaphore for protected memory
			if(ptrDataSemaphore)
			{
				OS_Use_Unit(ptrDataSemaphore);
			}
			// the  action depence on the storage-qualifier
			switch(myself.storageQualifier)
			{
				case RAM:
					memcpy(ptrTarget,ptrValue,length); // lint ok --> return value not from interest
					break;
				case NON_VOLATILE:
					memcpy(ptrTarget,ptrValue,length); // lint ok --> return value not from interest
					if(OK!=Trigger_EEPSave(ptrTarget,length, SYNC_IM))
					{
						result = FATAL_ERROR;
					}
					else { result = OK; }
					break;
				case ROM:
				case READONLY_RAM:
				case READONLY_NOV:
				default:
					result = READ_ONLY_ERR;
					//lint -e506 assert(0) is intentional
					assert(0);

			}
			if(ptrDataSemaphore)
			{
				OS_Unuse_Unit(ptrDataSemaphore);
			}
		}
	}
//	assert(result<DEBUG_ERROR);
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  get the actual object or attribute value.
         Access to memory protected with a resource semaphore will be supported if the pointer to this
         semaphore is not zero.
         Objects resided in ROM are not supported.

 <pre>
         1. check attributeIndex
         2. copy data
         3. transform to external view (function call)
         4. return
 </pre>

 \param  me; T_DATA_OBJ itself
 \param  ptrValue; the new object or attribute value will be returned in ptrValue
 \param  attributeIndex ==-1 --> Object; >=0 --> Attribute
 \param  ptrDataSemaphore pointer to the resource-semaphore of the used data-memory; 0-ptr-->not protected
 \return error-code
 <pre>
     OK                    operation was successful
     LESSTHAN_RANGEMIN     at least one value is less than its minimum value
     GREATERTHAN_RANGEMAX  at least one value is greater than its maximum
     SPAN_TO_SMALL         then span between two values is to small
     NOT_ON_GRID           at least one value is not a multiple of its increment
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
     FATAL_ERROR           object outside nv-mem range
 </pre>
 \warning
 \test
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t Get_DATAOBJ(const T_DATA_OBJ *me,
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
	// speed and code optimization
	myself = *me;
	assert(myself.numberOfAttributes); // divide by zero
	assert(myself.ptrValue); // pointer
	assert(myself.pClass);   // pointer
//	assert(myself.storageQualifier!=ROM); // this method does not support ROM
	if(attributeIndex>=(int16_t)(myself.numberOfAttributes) || attributeIndex<WHOLE_OBJECT)
	{
		return ILLEGAL_ATTRIB_IDX;
	}
	else
	{
		// the source is basicly the object-value
		ptrSource = (uint8_t *)myself.ptrValue;
		// access to object
		if(attributeIndex<0)
		{
			// copy complete object or array of objects
			length = myself.objectLength;
		}
		else
		{
			switch(myself.typeQualifier)
			{
				case SIMPLE:
					length = myself.objectLength;
					break;
				case ARRAY:
					// error in data structure
					assert((myself.objectLength % myself.numberOfAttributes)==0);
					// calculate the length of one attribute
					length = myself.objectLength / myself.numberOfAttributes;
					// calculate the address offset of the attribute
					attribPos = attributeIndex;
					attribPos*= length;
					// validate pointer-arithmetic
					assert(myself.objectLength>attribPos);
					ptrSource+=attribPos;
					break;
				case STRUCT:
					length = cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[attributeIndex]];
					attribPos=0;
					for(i=0; i<attributeIndex; i++)
					{
						attribPos+=cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[i]];
					}
					// validate pointer-arithmetic
					assert(myself.objectLength>attribPos);
					ptrSource+=attribPos;
					break;
				default:
					length=0;
					//lint -e506 assert(0) is intentional
					assert(0);
			}
		}
		// use semaphore for protected memory
		if(ptrDataSemaphore)
		{
			OS_Use_Unit(ptrDataSemaphore);  // lint ok --> return value not from interest
		}
		// the get action depence on the storage-qualifier
		switch(myself.storageQualifier)
		{
			case READONLY_RAM:
			case RAM:
				memcpy(ptrValue,ptrSource,length);  // lint ok --> return value not from interest
				result = OK;
				break;
			case READONLY_NOV:
			case NON_VOLATILE:
				memcpy(ptrValue,ptrSource,length);  // lint ok --> return value not from interest
				result = OK;

				break;
			case ROM:
				memcpy(ptrValue,ptrSource,length);  // lint ok --> return value not from interest
				result = OK;
				break;
			default:
				result = FATAL_ERROR;
				//lint -e506 assert(0) is intentional
				assert(0);
				break;
		}
		if(ptrDataSemaphore)
		{
			OS_Unuse_Unit(ptrDataSemaphore);
		}
	}
//	assert(result<DEBUG_ERROR);
	return result;
}






//--------------------------------------------------------------------------------------------------
/*!
 \brief  Reads the addressed attribute range-information

 \param  me; T_DATA_OBJ itself
 \param  ptrDescriptor range-information
 \param  attributeIndex ==-1 --> putObject; >=0 --> putAttribute
 \return error-code
 <pre>
     OK                    operation was successful
     ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
 </pre>
 \warning
 \test
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetAttributeDescription_DATAOBJ(const T_DATA_OBJ *me,
		T_DO_ATTRIB_RANGE * ptrDescriptor,
		uint16_t attributeIndex)
{
	T_DATA_OBJ myself;
	uint16_t result;
	// debug zero pointer and illegal values
	VIP_ASSERT(me);        // lint ??
	VIP_ASSERT(ptrDescriptor);
	// speed and code optimization
	myself = *me;
	assert(myself.pClass);   // pointer
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
				ptrDescriptor->len = myself.objectLength;
				ptrDescriptor->dataType=myself.pClass->aryAttribDataTypes[0];
				ptrDescriptor->adr = ptrSource;
				result = OK;
				break;
			case ARRAY:
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
				ptrDescriptor->dataType=myself.pClass->aryAttribDataTypes[0];
				ptrDescriptor->adr = ptrSource;
				result = OK;
				break;
			case STRUCT:
				ptrDescriptor->len = cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[attributeIndex]];
				attribPos=0;
				for(int16_t i=0; i<attributeIndex; i++)
				{
					attribPos+=cgSimpleTypeLength[myself.pClass->aryAttribDataTypes[i]];
				}
				// validate pointer-arithmetic
				assert(myself.objectLength>attribPos);
				ptrSource+=attribPos;
				ptrDescriptor->dataType=myself.pClass->aryAttribDataTypes[attributeIndex];
				ptrDescriptor->adr = ptrSource;
				result = OK;
				break;
			default:
				result = FATAL_ERROR;
				//lint -e506 assert(0) is intentional
				assert(0);
		}

	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  Read object description

 \param  me; T_DATA_OBJ itself
 \param  ptrDescriptor object description
 \return error-code
 <pre>
     OK                    operation was successful
 </pre>
 \warning
 \test
 \bug
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetObjectDescription_DATAOBJ(const T_DATA_OBJ *me,
									  T_DO_OBJECT_DESCRIPTION * ptrDescriptor)
{
	T_DO_OBJECT_DESCRIPTION objDesc;
	T_DATA_OBJ myself;
	// debug zero pointer and illegal values
	VIP_ASSERT(me);
	VIP_ASSERT(ptrDescriptor);
	// speed and code optimization
	myself = *me;
	assert(myself.pClass);   // pointer
	objDesc.objectLength=myself.objectLength;
	objDesc.numberOfAttributes=myself.numberOfAttributes;
	objDesc.typeQualifier=myself.typeQualifier;
	objDesc.aryAttribDataTypes=myself.pClass->aryAttribDataTypes;
	*ptrDescriptor=objDesc;
	return OK;
}

