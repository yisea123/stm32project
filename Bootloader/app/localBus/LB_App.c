/*
 * LB_App.c
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */

#include <string.h>
#include "main.h"
#include "LB_Layer7.h"
#include "LB_App.h"


static FrameHandler SendFrames = NULL;

uint16_t LB_APP_Init(FrameHandler rxHandle, FrameHandler resp)
{
	SendFrames = resp;
	return LB_Layer7_Init(rxHandle);
}


static void SendBackrame(MsgFrame* ptrMsgFrame,uint16_t idx ,uint16_t ret)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);
	if(ret == OK)
	{
		ret = deviceStatus;
	}
	if(ret != OK)
	{
		frame->St.length = (uint16_t)(idx+1);
		frame->St.function |= ERROR_BIT;
		frame->St.dataBuff[idx] = (uint8_t)ret;

	}
	if(SendFrames)
	{
		SendFrames(ptrMsgFrame);
	}
}


#ifndef BOOTLOADER

static uint16_t GetObjectLength(uint16_t subId, uint16_t objId, int16_t atrId, uint16_t* ptrLen)
{
	uint16_t ret = FATAL_ERROR;
	*ptrLen = 0;
	if(subId < IDX_SUB_MAX)
	{
		const T_UNIT* subPtr = subSystem[subId];
		if(atrId == WHOLE_OBJECT)
		{
			T_DO_OBJECT_DESCRIPTION objDesp;
			ret = subPtr->GetObjectDescription(subPtr,objId,&objDesp);
			if(ret == OK)
			{
				*ptrLen = (uint16_t)objDesp.objectLength;
			}
		}
		else
		{
			T_DO_ATTRIB_RANGE otrDesp;
			ret = subPtr->GetAttributeDescription(subPtr,objId, (uint16_t)atrId,&otrDesp);

			if(ret == OK)
			{
				*ptrLen = cgSimpleTypeLength[otrDesp.dataType];
			}
		}
	}

	return ret;
}
uint16_t LB_APP_ReadLength(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
	int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = ILLEGAL_SUB_IDX;
	ret = GetObjectLength(subId,objId,atrId,(uint16_t*)&frame->St.dataBuff[3]);
	frame->St.length = 5;
	SendBackrame(ptrMsgFrame, OBJ_CMD_HEAD_DATA_IDX, ret);

	return ret;
}

uint16_t LB_APP_ReadName(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
//	int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = ILLEGAL_SUB_IDX;
	if(subId < IDX_SUB_MAX)
	{
		const T_UNIT* subPtr = subSystem[subId];
		uint16_t length = 240;
		ret = subPtr->GetObjectName(subPtr,objId,&frame->St.dataBuff[3],&length);
		if(ret == OK)
		{
			frame->St.length = (uint16_t)(length + 3);
		}
	}
	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);

	return ret;
}

uint16_t LB_APP_ReadAtrNum(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);
	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
//	int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = ILLEGAL_SUB_IDX;
	if(subId < IDX_SUB_MAX)
	{
		T_DO_OBJECT_DESCRIPTION objDesp;
		const T_UNIT* subPtr = subSystem[subId];
		ret = subPtr->GetObjectDescription(subPtr,objId,&objDesp);
		if(ret == OK)
		{
			frame->St.dataBuff[3] = (uint8_t)( (objDesp.numberOfAttributes)& 0xFF);
			frame->St.dataBuff[4] =  (uint8_t)((objDesp.numberOfAttributes>>8)& 0xFF);
		}
	}
	frame->St.length = 5;
	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);
	return ret;
}

uint16_t LB_APP_ReadObj(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);
	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
	int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = ILLEGAL_SUB_IDX;
	uint16_t objLen = 0;
	if(subId < IDX_SUB_MAX)
	{
		ret = GetObjectLength(subId,objId,atrId,&objLen);

		if(ret == OK)
		{
			const T_UNIT* subPtr = subSystem[subId];
			ret |= subPtr->Get(subPtr,objId,atrId,&frame->St.dataBuff[3]);
		}
	}
	frame->St.length = (uint16_t)(objLen + 3);

	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);
	return ret;
}


uint16_t LB_APP_WriteObj(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
	int16_t atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = ILLEGAL_SUB_IDX;
	uint16_t objLen = 0;
	if(subId < IDX_SUB_MAX)
	{
		ret = GetObjectLength(subId,objId,atrId,&objLen);
		if(ret == OK)
		{
			if(objLen != (frame->St.length - 3))
			{
				ret = ERROR_DATA_LENGTH;
			}
			else
			{
				ret = PutObject(subId,objId, atrId, (void*)&frame->St.dataBuff[3]);
			}
		}
	}
	frame->St.length = 4;
	frame->St.dataBuff[3] = (uint8_t)ret;
	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);

	return ret;
}


uint16_t LB_APP_ReadType(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);
	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
	int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = ILLEGAL_SUB_IDX;
	if(subId < IDX_SUB_MAX)
	{
		const T_UNIT* subPtr = subSystem[subId];
		if(atrId == WHOLE_OBJECT)
		{
			T_DO_OBJECT_DESCRIPTION objDesp;
			ret = subPtr->GetObjectDescription(subPtr,objId,&objDesp);
			if(ret == OK)
			{
				frame->St.dataBuff[3] = (uint8_t)(objDesp.typeQualifier);
			}
		}
		else
		{
			T_DO_ATTRIB_RANGE otrDesp;
			ret = subPtr->GetAttributeDescription(subPtr,objId, (uint16_t)atrId,&otrDesp);

			if(ret == OK)
			{
				frame->St.dataBuff[3] = otrDesp.dataType;
			}
		}
	}
	frame->St.length = 4;

	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);
	return ret;
}


#endif

uint16_t LB_APP_WriteMem(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t ret = OK;
	uint32_t adr = frame->St.dataBuff[3];
	adr = adr*256 + frame->St.dataBuff[2];
	adr = adr*256 + frame->St.dataBuff[1];
	adr = adr*256 + frame->St.dataBuff[0];

	uint16_t len =  *(uint16_t*)&frame->St.dataBuff[MEM_CMD_HEAD_LEN_IDX];
	if(len <= MAX_MEM_BUFF_LEN)
	{
		if(CheckAdrRWStatus(adr,len,WRITE_ACCESS) != OK)
		{
			ret = RULE_VIOLATION_ERR;
		}
		else
		{
			Flash2Rom(adr,len,&frame->St.dataBuff[MEM_CMD_HEAD_LEN_IDX+2]);
		}
	}
	else
	{
		ret = RULE_VIOLATION_ERR;
	}
	frame->St.length = 7;
	frame->St.dataBuff[6] = (uint8_t)ret;

	SendBackrame(ptrMsgFrame,MEM_CMD_HEAD_DATA_IDX, ret);
	return ret;
}

extern  __IO uint16_t resetRequest;
uint16_t LB_APP_WriteObj(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
	uint16_t ret = ILLEGAL_SUB_IDX;
	if((subId == 1) && (objId == 11) )
	{
		uint16_t data = frame->St.dataBuff[4];
		data *= 256;
		data += frame->St.dataBuff[3];

		if(data == 6000)
		{
			//reset with bootloader
			validPrintMsg = VALID_RST_MSG;
		}
		else if(data)
		{
			//reset
			validPrintMsg = 0;
		}

		if(data)
		{

			resetRequest = 0x01;
		}
		ret = OK;

	}
	frame->St.length = 4;
	frame->St.dataBuff[3] = (uint8_t)ret;
	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);

	return ret;
}

uint16_t LB_APP_ReadMem(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t ret = OK;
	uint32_t adr = frame->St.dataBuff[3];
	adr = adr*256 + frame->St.dataBuff[2];
	adr = adr*256 + frame->St.dataBuff[1];
	adr = adr*256 + frame->St.dataBuff[0];

	uint16_t len =  *(uint16_t*)&frame->St.dataBuff[MEM_CMD_HEAD_LEN_IDX];
	if(len <= MAX_MEM_BUFF_LEN)
	{
		frame->St.length = (uint16_t)(len+MEM_CMD_HEAD_DATA_IDX);
		if(CheckAdrRWStatus(adr,len,READ_ACCESS) != OK)
		{
			ret = RULE_VIOLATION_ERR;
		}
		else
		{
			(void)memcpy((void*)&frame->St.dataBuff[MEM_CMD_HEAD_DATA_IDX], (void*)adr, len);
		}
	}
	else
	{
		ret = RULE_VIOLATION_ERR;
	}
	SendBackrame(ptrMsgFrame,MEM_CMD_HEAD_DATA_IDX, ret);

	return ret;
}


uint16_t LB_APP_Poll(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t ret = OK;
//	frame->data[0] = 4;
	if(frame->St.length >= 2 )
	{
		uint16_t id = frame->St.dataBuff[1];
		id = (uint16_t)(id<<8 | frame->St.dataBuff[0]);

		if(id == 0x0000)
		{
			frame->St.length = 12;
			memcpy((void*)&frame->St.dataBuff[2],(void*)&_sysInfo.DeviceID,sizeof(_sysInfo.DeviceID));
			memcpy((void*)&frame->St.dataBuff[4],(void*)&deviceStatus,sizeof(deviceStatus));
			memcpy((void*)&frame->St.dataBuff[6],(void*)&_sysInfo.VersionCode,sizeof(_sysInfo.VersionCode));
			memcpy((void*)&frame->St.dataBuff[10],(void*)&burstLength,sizeof(burstLength));
		}
		else if(0x0001 == id)
		{
			frame->St.length = sizeof(_sysInfo.DeviceType) + 2;
			memcpy((void*)&frame->St.dataBuff[2],(void*)&_sysInfo.DeviceType,sizeof(_sysInfo.DeviceType));
		}
		else if(0x0002 == id)
		{
			frame->St.length = 12;
			memcpy((void*)&frame->St.dataBuff[2],(void*)&sysInfoApp.DeviceID,sizeof(sysInfoApp.DeviceID));
			memcpy((void*)&frame->St.dataBuff[4],(void*)&deviceStatus,sizeof(deviceStatus));
			memcpy((void*)&frame->St.dataBuff[6],(void*)&appVersion,sizeof(appVersion));
			memcpy((void*)&frame->St.dataBuff[10],(void*)&burstLength,sizeof(burstLength));

		}
		else if( (0x1000 <= id) && (burstLength+0x1000) >= id)
		{
			frame->St.length = 2+5;
			uint16_t id1 = id - 0x1000;
			frame->St.dataBuff[2] = 0x0;
			frame->St.dataBuff[3] = 0x0;
			frame->St.dataBuff[4] = 0x0;
			frame->St.dataBuff[5] = 0x0;
			frame->St.dataBuff[6] = 0;
		}
		else
		{
			frame->St.length = 2 + 1;
			ret = RULE_VIOLATION_ERR;
			frame->St.dataBuff[2] = ret;
		}
	}
	else
	{
		ret = ERROR_DATA_LENGTH;
	}
	SendBackrame(ptrMsgFrame,POLL_CMD_HEAD_DATA_IDX, ret);
	return ret;
}
uint16_t LB_APP_UnSupport(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t ret = OK;

	SendBackrame(ptrMsgFrame,UNSUPPORT_HEAD_DATA_IDX, RULE_VIOLATION_ERR);

	return ret;
}


uint16_t LB_APP_Burst(void)
{
	return OK;
}


