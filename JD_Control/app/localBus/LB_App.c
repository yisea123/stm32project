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
#include "unit_sys_info.h"
#include "t_unit_head.h"


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

	if(ret != OK)
	{
		frame->St.length = (uint16_t)(idx+1);
		frame->St.function |= ERROR_BIT;
		frame->St.dataBuff[idx] = (uint8_t)ret;
		TraceMsg(TSK_ID_LOCAL_BUS, "Error Feedback: frame: 0x%08x; ret: %d\n",ptrMsgFrame, ret );
	}
	else
	{
		if(frame->St.function & ERROR_BIT)
			TraceMsg(TSK_ID_LOCAL_BUS, "Error Error Feedback: frame: 0x%08x; ret: %d\n",ptrMsgFrame, ret );

	}



	if(SendFrames)
	{
		SendFrames(ptrMsgFrame);
	}
}

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
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "Read length: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
	}
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
	frame->St.length = (uint16_t)(2 + 3);
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
	if(ret != OK)
	{
		int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
		TraceMsg(TSK_ID_LOCAL_BUS, "Read name: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
	}
	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);

	return ret;
}


uint16_t LB_APP_ReadAdrInfo(MsgFrame* ptrMsgFrame)
{
	assert(ptrMsgFrame);
	Layer2Frame* frame = &ptrMsgFrame->frame;
	assert(frame);

	uint16_t subId = (uint16_t)frame->St.dataBuff[0];
	uint16_t objId = (uint16_t)frame->St.dataBuff[1];
	int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
	uint16_t ret = GetAdrInfo(subId, objId, atrId,(uint32_t*)&frame->St.dataBuff[3],(uint16_t*)&frame->St.dataBuff[7]);
	frame->St.length = (uint16_t)(6 + 3);
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "Read adr info: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
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
	if(ret != OK)
	{
		int16_t  atrId = (frame->St.dataBuff[2]==0xFF)?WHOLE_OBJECT:(int16_t)frame->St.dataBuff[2];
		TraceMsg(TSK_ID_LOCAL_BUS, "Read atr num: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
	}
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
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "Read obj: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
	}
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
				if(ret == OK)
				{
					uint8_t dataEvent[6];
					uint8_t* data = &frame->St.dataBuff[3];
					dataEvent[0] = (uint8_t)objId;
					dataEvent[1] = (uint8_t)atrId;
					dataEvent[2] = ((uint8_t*)data)[0];
					dataEvent[3] = ((uint8_t*)data)[1];
					dataEvent[4] = ((uint8_t*)data)[2];
					dataEvent[5] = ((uint8_t*)data)[3];
				//	if((subId !=8) && (subId !=9))
				//		NewEventLog((EV_CMD_PUT_OBJ|(uint32_t)subId), dataEvent);
				}
			}
		}
	}
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "write obj: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
	}
	else
	{
		uint32_t val = 0;
		if(objLen == 1)
		{
			val = frame->St.dataBuff[3];
		}
		else if(objLen == 2)
		{
			val = frame->St.dataBuff[4];
			val *=256;
			val += frame->St.dataBuff[3];
		}
		else if(objLen == 4)
		{
			val = frame->St.dataBuff[6];
			val *=256;
			val += frame->St.dataBuff[5];
			val *=256;
			val += frame->St.dataBuff[4];
			val *=256;
			val += frame->St.dataBuff[3];
		}
		memcpy((void*)&val,(void*)(&frame->St.dataBuff[3]), objLen);
		if(objLen == 4)
		{
			float valF = 0;
			memcpy((void*)&valF,(void*)(&frame->St.dataBuff[3]), objLen);
			TraceMsg(TSK_ID_LOCAL_BUS, "UI_Put %d %d %d->len:%d, value: %d, %x or Float: %.2f, ret: %d\n",subId,objId, (uint16_t)atrId,objLen,val,val, valF,ret );
		}
		else
			TraceMsg(TSK_ID_LOCAL_BUS, "UI_Put %d %d %d-> %d, %x ret: %d\n",subId,objId, (uint16_t)atrId,val,val, ret );
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
			else
			{
				//TraceDBG(TSK_ID_LOCAL_BUS, "Read Type: %d-subId>%d->%d: ret: %d\n",subPtr,objId, (uint16_t)atrId,ret );
			}
		}
	}
	frame->St.length = 4;
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "read type: %d->%d->%d: ret: %d\n",subId,objId, (uint16_t)atrId,ret );
	}
	SendBackrame(ptrMsgFrame,OBJ_CMD_HEAD_DATA_IDX, ret);
	return ret;
}



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
			//NewEventLog(EV_CMD_PUT_MEM, (void*)&frame->St.dataBuff[0]);
			(void)memcpy((void*)adr, (void*)&frame->St.dataBuff[MEM_CMD_HEAD_DATA_IDX], len);
		}
	}
	else
	{
		ret = RULE_VIOLATION_ERR;
	}
	frame->St.length = 7;
	frame->St.dataBuff[6] = (uint8_t)ret;
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "write mem: %d->%d: ret: %d\n",adr,len,ret );
	}
	SendBackrame(ptrMsgFrame,MEM_CMD_HEAD_DATA_IDX, ret);
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
	if(ret != OK)
	{
		TraceMsg(TSK_ID_LOCAL_BUS, "read mem: %d->%d: ret: %d\n",adr,len,ret );
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
		else if( (0x1000 <= id) && (burstLength+0x1000) >= id)
		{
			frame->St.length = 2 + 5;
			uint16_t id1 = (uint16_t)(id - 0x1000);
			memcpy((void*)&frame->St.dataBuff[2],(void*)&burstCfg[id1].adr, sizeof(burstCfg[id1].adr));
			frame->St.dataBuff[6] = burstCfg[id1].len;
		}
		else
		{
			frame->St.length = 2 + 1;
			ret = RULE_VIOLATION_ERR;
			frame->St.dataBuff[2] = (uint8_t)ret;
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


