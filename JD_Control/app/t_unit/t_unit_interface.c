/*
 * t_unit_interface.c
 *
 *  Created on: 2019Äê3ÔÂ4ÈÕ
 *      Author: pli
 */

#include <stdio.h>
#include <string.h>

#include "t_unit_head.h"

uint16_t GetObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		OBJ_DATA* inst)
{
	if(inst)
	{
		const T_UNIT* subPtr = GetSubsystem(subID);
		T_DO_ATTRIB_RANGE atrDesp;
		if(subPtr)
		{
			subPtr->GetAttributeDescription(subPtr,objectIndex,(uint16_t)attributeIndex, &atrDesp);
			if(atrDesp.dataType)
			{
				inst->type = atrDesp.dataType;
				return subPtr->Get(subPtr,objectIndex,attributeIndex,inst->data);
			}
		}
	}
	return FATAL_ERROR;

}

uint16_t GetAdrInfo(uint16_t subId, uint16_t objId,int16_t atrId, uint32_t* adr, uint16_t* len)
{
	uint16_t ret = ILLEGAL_SUB_IDX;
	int16_t  atrId1 = atrId;

	{
		const T_UNIT* subPtr = GetSubsystem(subId);

		if(subPtr)
		{
			if(atrId == WHOLE_OBJECT)
			{
				T_DO_OBJECT_DESCRIPTION objDesp;
				ret = subPtr->GetObjectDescription(subPtr,objId,&objDesp);
				if(ret == OK)
					*len = objDesp.objectLength;
				atrId1 = 0;
			}
			else
			{
				atrId1 = atrId;
				ret = OK;
			}
			if(ret == OK)
			{
				T_DO_ATTRIB_RANGE atrDesp;
				ret = subPtr->GetAttributeDescription(subPtr,objId,(uint16_t)atrId1, &atrDesp);

				if(ret == OK)
				{
					*adr = atrDesp.adr;
					if(atrId != WHOLE_OBJECT)
					{
						*len = atrDesp.len;
					}
				}
			}
		}
	}
	return ret;
}
uint16_t GetName(uint16_t subID,  uint16_t objectIndex,OBJ_DATA* inst)
{
	if(inst)
	{
		const T_UNIT* subPtr = GetSubsystem(subID);
		if(subPtr)
		{
			return subPtr->GetObjectName(subPtr,objectIndex, inst->data,&inst->len);
		}
	}
	return FATAL_ERROR;

}


uint16_t GetObjectType(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		uint16_t* type)
{
//	if(subID < IDX_SUB_MAX)
	{
		const T_UNIT* subPtr = GetSubsystem(subID);
		T_DO_ATTRIB_RANGE atrDesp;
		if(subPtr)
		{
			subPtr->GetAttributeDescription(subPtr,objectIndex,(uint16_t)attributeIndex, &atrDesp);
			*type = atrDesp.dataType;
			return OK;
		}
	}
	return FATAL_ERROR;
}

uint16_t PutObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		void* data)
{
	if(data)// && (subID < IDX_SUB_MAX))
	{

		const T_UNIT* subPtr = GetSubsystem(subID);
	//	T_DO_ATTRIB_RANGE atrDesp;
		if(subPtr)
		{

			return subPtr->Put(subPtr,objectIndex,attributeIndex,data);
		}
	}
	return FATAL_ERROR;
}



