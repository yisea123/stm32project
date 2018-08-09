/*
 * t_unit.c
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */
#include <stdio.h>
#include <string.h>

#include "t_data_obj.h"
#include "t_unit.h"


//--------------------------------------------------------------------------------------------------
/*!
 \brief  initialize the data-semaphore of the subsystem; This method must be overloaded
 \param  me; pointer of T_UNIT itself
 \param  typeOfStartUp
 <pre>
   INIT_HARDWARE   =0x80  initialize hardware used by the subsystem
   INIT_TASKS      =0x40  initialize tasks, semaphores, timer ....
   INIT_DATA       =0x20  initialize data and data structures
   INIT_CALCULATION=0x10  if a subsystem needs onle once a preparation for calculating, then
 </pre>
 \return error-code = OK
*/
//--------------------------------------------------------------------------------------------------
uint16_t Initialize_T_UNIT(const T_UNIT *me, uint8_t typeOfStartUp)
{
	//lint -e{613} all pointers are check by asserts
	uint16_t * ptrState;
	// check for zero-pointers
	VIP_ASSERT(me);
	assert(me->ptrState);
	assert(me->ptrDataSemaphore);
	ptrState = me->ptrState;
	if(*ptrState<INITIALIZED && (typeOfStartUp & INIT_TASKS) != 0)
	{
		*me->ptrDataSemaphore = OS_CREATERSEMA();
		*ptrState=INITIALIZED;
	}
	/*
	else TODO in overload
	{
	    if( (typeOfStartUp & INIT_HARDWARE) != 0)
	    {
	    }

	    if( (typeOfStartUp & INIT_DATA) != 0)
	    {
	    }

	    if( (typeOfStartUp & INIT_CALCULATION) != 0)
	    {
	    }
	}
	*/
	return OK;
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief  That method calls LoadRomDefaults of the dataClass[dataClassIndex]
 <pre>
	in mode "NOT_INITIALIZED" the mode will not be changed.
	for all other modes the state will be switched to "INACTIVE" or "ERROR_STATE"
	usually the function will be called in situations before the subsystem will be initialized
	thus the semaphore is not initialized and could not be used.
	It should not be a big problem cause of the usage in special situations.

 </pre>

 \param  me; pointer of T_UNIT itself
 \param  dataClassIndex
 <pre>
	-1 > dataClassIndex >=numOfDataClasses --> ILLEGAL_DATACLASS_INDEX
	-1 --> all data-classes of the subsystem will be called
	0 - numOfDataClasses-1 --> dataClass[dataClassIndex] will be called
 </pre>
 \return error-code
 <pre>
   OK                       operation was successful
   DECLINE                  DEBUG, operation isn't permitted in the actual unit-state
   ILLEGAL_DATACLASS_INDEX  -1 > dataClassIndex >=numOfDataClasses
 </pre>
 */
//--------------------------------------------------------------------------------------------------
uint16_t LoadRomDefaults_T_UNIT(const T_UNIT *me, int16_t dataClassIndex)
{
	uint16_t * ptrState;
	uint16_t result,errCnt;
	int16_t   i, number;
	const T_DATACLASS * pDataClass;
	// check for zero-pointers
	VIP_ASSERT(me);
	assert(me->ptrState);
	assert(me->ptrDataClasses);
	ptrState = me->ptrState;
	// check if index is allowed
	if(dataClassIndex<ALL_DATACLASSES || dataClassIndex>=(int16_t)me->numOfDataClasses)
	{
		result = ILLEGAL_DATACLASS_INDEX;
		//lint -e506 assert(0) is intentional
		assert(0); // an illegal index should not occur!
	}
	else
	{
		// prepare action
		if(dataClassIndex==ALL_DATACLASSES)
		{
			dataClassIndex=0;
			number= (int16_t)me->numOfDataClasses;
		}
		else
		{
			number = 1;
		}
		// call LoadRomDefaults of the addressed data-classes
		errCnt = 0;
		for(i=0; i<number; i++)
		{
			pDataClass = me->ptrDataClasses+dataClassIndex+i;
				assert(pDataClass);
			// errors will not stop the loop, but they will not be forgotten
			if(pDataClass->LoadRomDefaults(pDataClass) != OK) { errCnt++; }
		}
		// map nv-results to T_UNIT-error-code and state handling
		if(errCnt > 0)
		{
			result = ERROR_RAM_STORAGE;
			// in state INITIALIZED a state-change is necessary, in NOT_INITIALIZED it is not allowed
			if(*ptrState>=INITIALIZED)
			{
				*ptrState = ERROR_STATE;
			}
		}
		else
		{
			result = OK;
			// in state INITIALIZED a state-change is necessary, in NOT_INITIALIZED it is not allowed
			if(*ptrState>=INITIALIZED)
			{
				*ptrState = INACTIVE;
			}
		}
	}
	return result;
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief  That method calls ResetToDefault of the dataClass[dataClassIndex]
 <pre>
    not allowed in mode "NOT_INITIALIZED"
    the state will be switched to "INACTIVE" or "ERROR_STATE"
 </pre>

 \param  me; pointer of T_UNIT itself
 \param  dataClassIndex
 <pre>
    -1 > dataClassIndex >=numOfDataClasses --> ILLEGAL_DATACLASS_INDEX
    -1 --> all data-classes of the subsystem will be called
    0 - numOfDataClasses-1 --> dataClass[dataClassIndex] will be called
 </pre>
 \return error-code
 <pre>
   OK                       operation was successful
   DECLINE                  DEBUG, operation isn't permitted in the aktual unit-state
   ILLEGAL_DATACLASS_INDEX  -1 > dataClassIndex >=numOfDataClasses
 </pre>
 */
//--------------------------------------------------------------------------------------------------
uint16_t ResetToDefault_T_UNIT(const T_UNIT *me, int16_t dataClassIndex)
{
	uint16_t * ptrState;
	uint16_t result,errCnt;
	int16_t   i, number;
	const T_DATACLASS * pDataClass;
	// check for zero-pointers
	VIP_ASSERT(me);
	assert(me->ptrState);
	assert(me->ptrDataSemaphore);
	assert(me->ptrDataClasses);
	ptrState = me->ptrState;
	// method is allowed only if the subsystem is well initialized
	VIP_ASSERT(*ptrState>=INITIALIZED);
	// check if index is allowed
	if(dataClassIndex<ALL_DATACLASSES || dataClassIndex>=(int16_t)me->numOfDataClasses)
	{

		result = ILLEGAL_DATACLASS_INDEX;
		//lint -e{506,774} assert(0) is intentional
		assert(0); // an illegal index should not occur!
	}
	else
	{
		// prepare action
		if(dataClassIndex==ALL_DATACLASSES)
		{
			dataClassIndex=0;
			number= (int16_t)me->numOfDataClasses;
		}
		else
		{
			number = 1;
		}
		OS_Use_Unit(me->ptrDataSemaphore);
		// call LoadRomDefaults of the addressed data-classes
		errCnt = 0;
		for(i=0; i<number; i++)
		{
			pDataClass = me->ptrDataClasses+dataClassIndex+i;
			//lint -e{774} assert(pDataClass) is OK
			assert(pDataClass);
			// errors will not stop the loop, but they will not be forgotten
			if(pDataClass->ResetToDefault(pDataClass) != OK) { errCnt++; }
		}
		OS_Unuse_Unit(me->ptrDataSemaphore);
		// map nv-results to T_UNIT-error-code and state handling
		if(errCnt > 0)
		{
			result = ERROR_RAM_STORAGE;
			*ptrState = ERROR_STATE;
		}
		else
		{
			result = OK;
			*ptrState = INACTIVE;
		}
	}
	return result;
}

//--------------------------------------------------------------------------------------------------
/*!
\brief  That method calls SaveAsDefault of the dataClass[dataClassIndex]
<pre>
   not allowed in mode "NOT_INITIALIZED"
   the state will be switched to "INACTIVE" or "ERROR_STATE"
</pre>
\param  me; pointer of T_UNIT itself
\param  dataClassIndex
<pre>
   -1 > dataClassIndex >=numOfDataClasses --> ILLEGAL_DATACLASS_INDEX
   -1 --> all data-classes of the subsystem will be called
   0 - numOfDataClasses-1 --> dataClass[dataClassIndex] will be called
</pre>
\return error-code
<pre>
  OK                       operation was successful
  DECLINE                  DEBUG, operation isn't permitted in the aktual unit-state
  ILLEGAL_DATACLASS_INDEX  -1 > dataClassIndex >=numOfDataClasses
</pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t SaveAsDefault_T_UNIT(const T_UNIT *me, int16_t dataClassIndex)
{
	uint16_t * ptrState;
	uint16_t result,errCnt;
	int16_t   i, number;
	const T_DATACLASS *pDataClass;
	// check for zero-pointers
	VIP_ASSERT(me);
	assert(me->ptrState);
	assert(me->ptrDataSemaphore);
	assert(me->ptrDataClasses);
	ptrState = me->ptrState;
	// method is allowed only if the subsystem is well initialized
	VIP_ASSERT(*ptrState>=INITIALIZED);
	// check if index is allowed
	if(dataClassIndex<ALL_DATACLASSES || dataClassIndex>=(int16_t)me->numOfDataClasses)
	{
		//lint -e{506,774} assert(0) is intentional
		result = ILLEGAL_DATACLASS_INDEX;
		assert(0); // an illegal index should not occur!
	}
	else
	{
		// prepare action
		if(dataClassIndex==ALL_DATACLASSES)
		{
			dataClassIndex=0;
			number= (int16_t)me->numOfDataClasses;
		}
		else
		{
			number = 1;
		}
		OS_Use_Unit(me->ptrDataSemaphore);
		// call LoadRomDefaults of the addressed data-classes
		errCnt = 0;
		for(i=0; i<number; i++)
		{
			pDataClass = me->ptrDataClasses+dataClassIndex+i;
			assert(pDataClass);
			// errors will not stop the loop, but they will not be forgotten
			if(pDataClass->SaveAsDefault(pDataClass) != OK) { errCnt++; }
		}
		OS_Unuse_Unit(me->ptrDataSemaphore);
		// map nv-results to T_UNIT-error-code and state handling
		if(errCnt > 0)
		{
			result = ERROR_RAM_STORAGE;
			*ptrState = ERROR_STATE;
		}
		else
		{
			result = OK;
			//*ptrState = INACTIVE;
		}
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  gets attributeIndex of the data-object objectIndex; only in "INITIALIZED"
 \param  me; pointer of T_UNIT itself
 \param  objectIndex    object index
 \param  attributeIndex attribute index; attributeIndex<0 --> get whole object
 \param  ptrValue  pointer for return-Value
 \return error-code
 <pre>
   OK                      operation was successful
   DECLINE                 DEBUG, operation isn't permitted in the actual unit-state
   ILLEGAL_OBJ_IDX         DEBUG, unknown Object
   ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t Get_T_UNIT(const T_UNIT *me,
					uint16_t objectIndex, int16_t attributeIndex, void * ptrValue)
{
	const T_DATA_OBJ * ptrObj;
	uint16_t result;
	// check for zeropointer
	VIP_ASSERT(me);
	VIP_ASSERT(ptrValue);
	assert(me->ptrObjectList);
	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
	}
	else
	{
		// handmade code optimizer
		ptrObj = &(me->ptrObjectList[objectIndex]);
		result = ptrObj->pClass->Get(ptrObj, ptrValue, attributeIndex, me->ptrDataSemaphore);
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  put the attribute attributeIndex of the data-object objectIndex ; only in "INITIALIZED"
 \param  me; pointer of T_UNIT itself
 \param  objectIndex    object index
 \param  attributeIndex attribute index; <0 --> put whole object
 \param  ptrValue  pointer to attribute-value and return-Value
 \return error-code
 <pre>
   OK                      operation was successful
   LESSTHAN_RANGEMIN       at least one value is less than its minimum value
   GREATERTHAN_RANGEMAX    at least one value is greater than its maximum
   SPAN_TO_SMALL           then span between two values is to small
   NOT_ON_GRID             at least one value is not a multiple of its increment
   RULE_VIOLATION          this shows a violation of one or more businessrules
   DECLINE                 DEBUG, operation isn't permitted in the actual unit-state
   ILLEGAL_OBJ_IDX         DEBUG, unknown Object
   ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t Put_T_UNIT(const T_UNIT *me,
					uint16_t objectIndex, int16_t attributeIndex, void * ptrValue)
{
	const T_DATA_OBJ * ptrObj;
	uint16_t result;
	// check for zeropointer
	VIP_ASSERT(me);
	VIP_ASSERT(ptrValue);
	assert(me->ptrObjectList);
	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
	}
	else
	{
		// handmade code optimizer
		ptrObj = &(me->ptrObjectList[objectIndex]);
		result = ptrObj->pClass->Put(ptrObj, ptrValue, attributeIndex, me->ptrDataSemaphore);
	}
	return result;
}

//--------------------------------------------------------------------------------------------------
/*!
 \brief  put the attribute attributeIndex of the data-object objectIndex ; only in "INITIALIZED"
 \param  me; pointer of T_UNIT itself
 \param  objectIndex    object index
 \param  attributeIndex attribute index; <0 --> put whole object
 \param  ptrValue  pointer to attribute-value and return-Value
 \return error-code
 <pre>
   OK                      operation was successful
   LESSTHAN_RANGEMIN       at least one value is less than its minimum value
   GREATERTHAN_RANGEMAX    at least one value is greater than its maximum
   SPAN_TO_SMALL           then span between two values is to small
   NOT_ON_GRID             at least one value is not a multiple of its increment
   RULE_VIOLATION          this shows a violation of one or more businessrules
   DECLINE                 DEBUG, operation isn't permitted in the actual unit-state
   ILLEGAL_OBJ_IDX         DEBUG, unknown Object
   ILLEGAL_ATTRIB_IDX      DEBUG, unknown Attribute
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t Check_T_UNIT(const T_UNIT *me,
					  uint16_t objectIndex, int16_t attributeIndex, void * ptrValue)
{
	const T_DATA_OBJ * ptrObj;
	uint16_t result;
	// check for zeropointer
	VIP_ASSERT(me);
	VIP_ASSERT(ptrValue);
	assert(me->ptrObjectList);
	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
	}
	else
	{
		// handmade code optimizer
		ptrObj = &(me->ptrObjectList[objectIndex]);
		result = ptrObj->pClass->Check(ptrObj, ptrValue, attributeIndex);
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  Get the description of the addressed attribute
 \param  me; pointer of T_UNIT itself
 \param  objectIndex; object index
 \param  ptrDescriptor range-information
   <pre>
		  ptrDescriptor->rulesAreActive ==eFALSE--> this attribute will not be validated --> no range-info
		  ptrDescriptor->dataType       :  type info for the following union
		  ptrDescriptor->range          : (union) Container for range-information

		  example for simple type ST_INT16 (refer to simple_type.h unit T_DATA_OBJ)

			ptrDescriptor->range.i16.min       : minimum
			ptrDescriptor->range.i16.max       : maximum
			ptrDescriptor->range.i16.increment : increment=5 --> data value must be a multiple of 5

		  for ST_E8 (tabled enumeration)
			ptrDescriptor->range.e8.numberOfCodes                  : as the name says
			ptrDescriptor->range.e8.aryCodeTab[0..numberOfCodes-1] :  ""
   </pre>
 \param  attributeIndex ==-1 --> putObject; >=0 --> putAttribute
 \return error-code
 <pre>
	 OK                    operation was successful
	 ILLEGAL_ATTRIB_IDX    DEBUG, unknown Attribute
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetAttributeDescription_T_UNIT(const struct _T_UNIT *me,
										uint16_t objectIndex,
										uint16_t attributeIndex,
										T_DO_ATTRIB_RANGE * ptrDescriptor)
{
	const T_DATA_OBJ * ptrObj;
	uint16_t result;
	// check for zeropointer
	VIP_ASSERT(me);
	VIP_ASSERT(ptrDescriptor);
	assert(me->ptrObjectList);
	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
	}
	else
	{
		// handmade code optimizer
		ptrObj = &(me->ptrObjectList[objectIndex]);
		result = ptrObj->pClass->GetAttributeDescription(ptrObj, ptrDescriptor, attributeIndex);
	}
	return result;
}


//--------------------------------------------------------------------------------------------------
/*!
 \brief  get information about the addressed object
 \param  me; T_UNIT itself
 \param  ptrDescriptor; the object description will be returned in ptrObjDescriptor
   <pre>
     ptrDescriptor->objectLength         : byte length of the whole object
     ptrDescriptor->numberOfAttributes   : SIMPLE -> 1
                                           ARRAY  -> ARRAY-length
                                           STRUCT -> number of struct member
     ptrDescriptor->aryAttribDataTypes[] : for SIMPLE and ARRAY only [0];
                                           for STRUCT [0-numberOfAttributes-1]
     ptrDescriptor->typeQualifier        : SIMPLE, ARRAY, STRUCT
   </pre>
 \param  objectIndex; object index
 \return error-code
 <pre>
     OK                    operation was successful
 </pre>
*/
//--------------------------------------------------------------------------------------------------
uint16_t GetObjectDescription_T_UNIT(const struct _T_UNIT *me,
									 uint16_t objectIndex,
									 T_DO_OBJECT_DESCRIPTION * ptrDescriptor)
{
	const T_DATA_OBJ * ptrObj;
	uint16_t result;
	// check for zeropointer
	VIP_ASSERT(me);
	VIP_ASSERT(ptrDescriptor);
	assert(me->ptrObjectList);
	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
	}
	else
	{
		// handmade code optimizer
		ptrObj = &(me->ptrObjectList[objectIndex]);
		result = ptrObj->pClass->GetObjectDescription(ptrObj, ptrDescriptor);
	}
	return result;
}



uint16_t GetObjectName_T_UNIT(const T_UNIT *me,
                                     uint16_t objectIndex,
                                     uint8_t * ptrDescriptor,
									 uint16_t* ptrLength)
{
	const T_DATA_OBJ * ptrObj;
	uint16_t result = FATAL_ERROR;
	// check for zeropointer
	VIP_ASSERT(me);
	VIP_ASSERT(ptrDescriptor);
	assert(me->ptrObjectList);
	// exception if not initialized
	VIP_ASSERT(*(me->ptrState)>=INITIALIZED);
	// unknown object
	if(objectIndex>me->maxIdx)
	{
		result = ILLEGAL_OBJ_IDX;
	}
	else
	{
		// handmade code optimizer
		ptrObj = &(me->ptrObjectList[objectIndex]);
		const uint8_t* adrName = ptrObj->nameDesp;
		if(adrName)
		{
			if(adrName[0] == '&')
			{
				adrName = adrName + 1;
			}
			uint8_t i = 0;
			for( i = 0; i < *ptrLength; i++)
			{
				if(adrName[i] != '\0')
				{
					ptrDescriptor[i] = adrName[i];
				}
				else
				{
					*ptrLength = i;
					break;
				}
			}
			result = OK;
		}

		//result = ptrObj->pClass->GetObjectDescription(ptrObj, ptrDescriptor);
	}
	return result;
}


