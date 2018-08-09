
//-------------------------------------------------------------------------------------------------
/*
<pre>
Copyright              Copyright ABB, 2007.
All rights reserved. Reproduction, modification,
use or disclosure to third parties without express
authority is forbidden.

System         Subsystem Parameter
Module
Description    Implementation of execute methods
for Subsystems based on T_UNIT.

Remarks
</pre>
*/
//-------------------------------------------------------------------------------------------------


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_dataclass.h"
#include "bsp.h"
#include "parameter_idx.h"
#include "unit_parameter.h"
#include "unit_rtc_cfg.h"
#include "rtc.h"
#include "dev_can.h"
#include "canprocess.h"

extern ALLNEEDVALUE allneedvalue;
static uint8_t objectData[32];
extern T_PARAMETER_WELD_SET_PARAMETER parameterWeldSetParameter ;




const T_UNIT * GetSubsystemPtr(uint8_t subsysIdx)
{
	const T_UNIT * unitPtr = 0;


	switch(subsysIdx)
	{
		//add one case if you implement a new subsystem
	case 0:
		unitPtr = &rtcCfg;
		break;
	case 1:
		unitPtr = &parameter;
		break;
	default:
		break;
	}

	return(unitPtr);
}

//--------------------------------------------------------------------------------------------------
/*!
\brief     Reads an object from a subsystem
Add type Cast (void *)ptrValue
\author    Stefan Tabelander,Eric-Jiping.li
\date      2004-07-27,2011-04-12
\param
<pre>
subsysIdx:       Subsystem to be addressed
objIdx:          Object index where to find the parameter
ptrValue:        data pointer
<\pre>
\return    for return code see T_DATA_OBJ_RETURNCODES
\warning
\test
test-date: 2008-04-29
\n by:     Marcel Schilg
\n environment: old Modultest from HART subsystem and HCF Data Linklayer Test scripts
\n intention: sight check
\n result  module test: OK
\n result  Lint Level 3: free // approved warnings and approved infos are suppressed
\n lint check o.k.: B. Tohermes; 30-08-2010
\bug
*/
//--------------------------------------------------------------------------------------------------

uint16_t GetObjects(const T_UNIT    *pSub, uint16_t objIdx, void *ptrValue)
{
	uint16_t            returnCode;
//	const T_UNIT    *pSub;          // Pointer to Subsystem

//	pSub = GetSubsystemPtr(subsysIdx);  // Get the pointer

	returnCode = pSub->Get(pSub,objIdx,WHOLE_OBJECT,(void *)ptrValue);

	return (returnCode);
}


//--------------------------------------------------------------------------------------------------
/*!
\brief     Reads an attribute from a subsystem
\author    Stefan Tabelander,Eric-Jiping.li
\date      2004-07-27
\param
<pre>
subsysIdx:       Subsystem to be addressed
objIdx:          Object index where to find the parameter
attribIdx:       subindex in object 'objIdx'
ptrValue:        data pointer
<\pre>
\return    for return code see T_DATA_OBJ_RETURNCODES
\warning
\test
test-date: 2008-04-29
\n by:     Marcel Schilg
\n environment: old Modultest from HART subsystem and HCF Data Linklayer Test scripts
\n intention: sight check
\n result  module test: OK
\n result  Lint Level 3: free // approved warnings and approved infos are suppressed
\n lint check o.k.: B. Tohermes; 30-08-2010
\bug
*/
//--------------------------------------------------------------------------------------------------

uint16_t GetAttribute(const T_UNIT    *pSub,  uint16_t objIdx, uint8_t attribIdx, void *ptrValue)
{
	uint16_t            returnCode;
//	T_UNIT const    *pSub;          // Pointer to Subsystem

//	pSub = GetSubsystemPtr(subsysIdx);  // Get the pointer

	returnCode = pSub->Get(pSub,objIdx,attribIdx,(void *)ptrValue);

	return (returnCode);
}



//--------------------------------------------------------------------------------------------------
/*!
\brief     Writes an object to a subsystem
Add type cast (void *)ptrValue
\author    Stefan Tabelander,Eric-Jiping.li
\date      2004-07-27,2011-04-12
\param
<pre>
subsysIdx:       Subsystem to be addressed
objIdx:          Object index where to find the parameter
ptrValue:        data pointer
<\pre>
\return    for return code see T_DATA_OBJ_RETURNCODES
\warning
\test
test-date: 2008-04-29
\n by:     Marcel Schilg
\n environment: old Modultest from HART subsystem and HCF Data Linklayer Test scripts
\n intention: sight check
\n result  module test: OK
\n result  Lint Level 3: free // approved warnings and approved infos are suppressed
\n lint check o.k.: B. Tohermes; 30-08-2010
\bug
*/
//--------------------------------------------------------------------------------------------------

uint16_t PutObjects(uint8_t subsysIdx, uint16_t objIdx, void *ptrValue)
{
	uint16_t            returnCode;
	T_UNIT const    *pSub;          // Pointer to Subsystem

	pSub = GetSubsystemPtr(subsysIdx);  // Get the pointer

	returnCode = pSub->Put(pSub,objIdx,WHOLE_OBJECT,(void *)ptrValue);

	return (returnCode);
}


//--------------------------------------------------------------------------------------------------
/*!
\brief     Writes an attribute to a subsystem
Add type cast (void *)ptrValue
\author    Stefan Tabelander,Eric-Jiping.li
\date      2004-07-27,2011-04-12
\param
<pre>
subsysIdx:       Subsystem to be addressed
objIdx:          Object index where to find the parameter
attribIdx:       subindex in object 'objIdx'
ptrValue:        data pointer
<\pre>
\return    for return code see T_DATA_OBJ_RETURNCODES
\warning
\test
test-date: 2008-04-29
\n by:     Marcel Schilg
\n environment: old Modultest from HART subsystem and HCF Data Linklayer Test scripts
\n intention: sight check
\n result  module test: OK
\n result  Lint Level 3: free // approved warnings and approved infos are suppressed
\n lint check o.k.: B. Tohermes; 30-08-2010
\bug
*/
//--------------------------------------------------------------------------------------------------

uint16_t PutAttribute(uint8_t subsysIdx, uint16_t objIdx, uint8_t attribIdx, void *ptrValue)
{

	uint16_t            returnCode;
	T_UNIT  const   *pSub;          // Pointer to Subsystem

	pSub = GetSubsystemPtr(subsysIdx);  // Get the pointer

	returnCode = pSub->Put(pSub,objIdx,attribIdx,(void *)ptrValue);

	return (returnCode);
}



//--------------------------------------------------------------------------------------------------
/*!
\brief     return the siez (in byte) of the object
\author    Martin Dahl
\date      2006-11-23
\param     subsystemPtr = pointer to subsystem
\param     objectIndex = object index.
\param     attributeIndex = attribute index.
\warning
\test
test-date: 2008-05-07
\n by: Marcel Schilg
\n environment: PD30F / Modultest "Modultest_HART_overload"
\n intention:  instruction coverage
\n result  modul test: ok
\n result  Lint Level 3: ok, 2008-05-07
\n lint check o.k.: B. Tohermes; 30-08-2010
\bug
*/
//--------------------------------------------------------------------------------------------------
static uint16_t GetObjectSize(const T_UNIT  *subsystemPtr,uint16_t objectIndex, int16_t attributeIndex)
{
	T_DO_OBJECT_DESCRIPTION objDescriptor;
	T_DO_ATTRIB_RANGE attrDescriptor;
	if (attributeIndex == WHOLE_OBJECT)
	{
		//lint -e{522} Marcel Schilg :return value shall not be evaluated
		subsystemPtr->GetObjectDescription(subsystemPtr,objectIndex,&objDescriptor);
		return objDescriptor.objectLength;
	}
	else
	{
		//lint -e{522} Marcel Schilg :return value shall not be evaluated
		//lint -e{732} Marcel Schilg : loss of sign ok
		subsystemPtr->GetAttributeDescription(subsystemPtr,objectIndex,attributeIndex,&attrDescriptor);
		return cgSimpleTypeLength[attrDescriptor.dataType];
	}
}




void SendDataProxy(const MessageData msgData, uint32_t id)
{
	CanTxMsgTypeDef TxMessage;
	TxMessage.StdId = id;
	TxMessage.RTR = 0;
	TxMessage.DLC = 8;
	TxMessage.IDE = 0;
	memcpy(&TxMessage.Data[0], (uint8_t*)&msgData,8);
	TSK_CAN_Transmit1(&TxMessage);

}

HeadBit Convert(uint8_t head)
{
	HeadBit dbit;
	dbit.cmd = (head>>5)&0x07;
	dbit.type = (head>>3)&0x03;
	dbit.status = (head>>2)&0x01;
	dbit.length = (head>>0)&0x03;
	return dbit;
}

uint8_t ConvertB(HeadBit dbit)
{
	uint8_t head = 0;
	head |= ((dbit.cmd&0x07) <<5);
	head |= ((dbit.type&0x03)<<3);
	head |= ((dbit.status&0x01)<<2);
	head |= ((dbit.length&0x03)<<0);
	return head;
}


void SendDataBack(HeadBit head,MessageData msgData,const uint8_t* objData, int16_t len,uint32_t stdId)
{
	int16_t idx = 0;

	(void)memset(msgData.data,0,4);
	if(len <= 4 )
	{
		idx = 0;
		head.type = WHOLE_FRAME;
		if(len > 0)
			head.length = len-1;//len -1
		else
			head.length = 0;//len -1

		memcpy(msgData.data,&objData[idx],len);
		msgData.head = ConvertB(head);
		SendDataProxy(msgData,stdId);
	}
	else
	{
		idx = 0;
		head.type = FIRST_FRAME;
		head.length = 3;//len-1
		memcpy(msgData.data,&objData[idx],4);
		SendDataProxy(msgData,stdId);
		len -= 4;
		idx += 4;
		while(len >4 )
		{
			osDelay(5);

			head.type = MID_FRAME;
			head.length = 3;// len -1
			memcpy(msgData.data,&objData[idx],4);
			msgData.head = ConvertB(head);
			SendDataProxy(msgData,stdId);
			len -= 4;
			idx += 4;
		}
		osDelay(5);
		head.type = FIRST_FRAME;
		head.length = len - 1;//len -1
		memcpy(msgData.data,&objData[idx],4);
		msgData.head = ConvertB(head);
		SendDataProxy(msgData,stdId);
	}
}


void HandleCommunicationSRV(const uint8_t* ptrData,uint32_t stdId)
{
	MessageData msgData;
	uint16_t result = FATAL_ERROR;
	uint16_t objectSize = 0;
	(void)memcpy(&msgData,ptrData,8);
	HeadBit head = Convert(msgData.head);
	const T_UNIT *subsystemPtr = GetSubsystemPtr(msgData.subID);
	static uint8_t idx = 0;
	if(subsystemPtr == NULL)
	{
		//send error data;
		return;
	}
	else
	{
		if(head.type == WHOLE_FRAME)
		{
			if(msgData.frameID == 0xff)
			{
				objectSize = GetObjectSize(subsystemPtr,msgData.objID, WHOLE_OBJECT);
			}
			else
			{
				objectSize = GetObjectSize(subsystemPtr,msgData.objID, msgData.frameID);
			}
		}
		else
		{
			objectSize = GetObjectSize(subsystemPtr,msgData.objID, WHOLE_OBJECT);
		}
	}
	switch(head.cmd)
	{
	case READ_OBJ://
		if (objectSize <= 32)
		{
			if( msgData.frameID == 0xff)
				result = GetObjects(subsystemPtr,msgData.objID,&objectData[0]);
			else
				result = GetAttribute(subsystemPtr,msgData.objID,msgData.frameID,&objectData[0]);
			if(result == OK)
			{
				head.status = 0;
			}
			else
			{
				head.status = 1;
			}
			SendDataBack(head, msgData,objectData,objectSize,stdId);
		}
		break;
	case WRITE_OBJ://
		if (objectSize <= 32)
		{
			if(head.type == WHOLE_FRAME)
			{
				uint8_t data_[4];
				memcpy(data_, msgData.data, head.length+1);//len +1
				if(msgData.frameID == 0xff)
				{
					result =  subsystemPtr->Put(subsystemPtr,msgData.objID,WHOLE_OBJECT,(void *)&data_[0]);;
				}
				else
				{
					result =  subsystemPtr->Put(subsystemPtr,msgData.objID,msgData.frameID,(void *)&data_[0]);;
				}
				if(result == OK)
				{
					head.status = 0;
					SendDataBack(head, msgData,data_,head.length+1,stdId);//len +1
				}
				else
				{
					head.status = 1;
					SendDataBack(head, msgData,(uint8_t*)&result,2,stdId);//len +1
				}
			}
			else
			{
				if(head.type == FIRST_FRAME)
				{
					memcpy(&objectData[0],&msgData.data[0],4);
				}
				else if(head.type == MID_FRAME)
				{
					idx = msgData.frameID;
					if(idx < 8 && idx > 0)
						memcpy(&objectData[idx*4],&msgData.data[0],4);
					else
						result = FATAL_ERROR;
				}
				else
				{
					idx = msgData.frameID;
					if(idx < 8 && idx > 0)
					{
						memcpy(&objectData[idx*4],&msgData.data[0],head.length+1);//len +1
						result =  subsystemPtr->Put(subsystemPtr,msgData.objID,WHOLE_OBJECT,(void *)&objectData[0]);;

					}
					else
						result = FATAL_ERROR;
					if(result == OK)
					{
						head.status = 0;
					}
					else
					{
						head.status = 1;
					}
					head.type = WHOLE_FRAME;
					SendDataBack(head, msgData,(uint8_t*)&result,2,stdId);//len +1
				}

			}

		}
		break;

	case GET_DESCRIPTION://
		result = FATAL_ERROR;
		head.type = WHOLE_FRAME;
		if(result == OK)
		{
			head.status = 0;
		}
		else
		{
			head.status = 1;
		}
		SendDataBack(head, msgData,(uint8_t*)&result,2,stdId);//len +1

		break;
	}


}


void RenewHanjieParaSRV(void)
{
	parameterWeldSetParameter.strikeInch = (uint16_t)(allneedvalue.inchdata.minvalue);
	parameterWeldSetParameter.codeInchSpeed = (uint16_t)(allneedvalue.inchdata.minvalue);
	parameterWeldSetParameter.strikeTrim = (uint16_t)(allneedvalue.trim_value.maxvalue);
	parameterWeldSetParameter.strikeWorkpoint = (uint16_t)(allneedvalue.work_point.minvalue);
	parameterWeldSetParameter.startTrim = (uint16_t)(allneedvalue.trim_value.setvalue*2/3);
	parameterWeldSetParameter.startWorkpoint = (uint16_t)(allneedvalue.work_point.setvalue*2/3);

	parameterWeldSetParameter.craterTrim = (uint16_t)(allneedvalue.trim_value.setvalue*2/3);
	parameterWeldSetParameter.craterWorkpoint = (uint16_t)(allneedvalue.work_point.setvalue*2/3);

}



void UpdateExe(void)
{
}

void CheckSimuAction(void )
{
	uint16_t simuNum = 41;
	parameter_Put(PARA_IDX_simulateSave,1,&simuNum);
}

/*

This section defines templates for the SubSystem CodeGenerator.
Do not modify or remove this section


//@SubGen template start@ <METHOD>
//-------------------------------------------------------------------------------------------------
@SubGen OpenComment@!
\if @SubGen start@ <METHODHEADER TEMPLATE> \endif
\if @SubGen end@ <METHODHEADER TEMPLATE> \endif
\author
\date
\param void
\return void
\warning
\test
test-date: 2004-MM-DD
\n by: pp ss
\n environment:
\n intention:
\n result module test:
\n result Lint Level 3:
\bug
@SubGen CloseComment@
//-------------------------------------------------------------------------------------------------
//@SubGen start@<METHOD TEMPLATE>
//@SubGen end@<METHOD TEMPLATE>
{
}
//@SubGen template end@ <METHOD>

*/

