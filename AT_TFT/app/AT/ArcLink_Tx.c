/*
 * ArcLink_Tx.c
 *
 *  Created on: 2017年5月5日
 *      Author: pli
 */

#include "canprocess.h"
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "parameter_idx.h"
#include "unit_parameter.h"



uint8_t AnswerReadAtr_Peer(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type)
{
	if(headmsg->service == 0)
	{
		uint16_t kk = (uint16_t)(msg->Data[0]*256 + msg->Data[1]);
		uint8_t peeradr_1 = headmsg->adr;
		uint8_t classid = 0;
		uint8_t locIndex1_ = 0;
		if(kk == 4)
		{
			classid	= (uint16_t)(msg->Data[4]*256 + msg->Data[5]);
			if(classid== 20)
			{
				_L_G.TRACTOR = peeradr_1	;
				_L_G.TRACTOR_Stabel =  0x20;
			}
			_L_G.answer_ret = 1;
			return 1;

		}
		else if(kk == 11)
		{
			for(locIndex1_=0;locIndex1_<_L_G.peernum;locIndex1_++)
			{
				if(allpeers[locIndex1_].peeradress == peeradr_1)
				{
					allpeers[locIndex1_].instance = msg->Data[4];
					_L_G.answer_ret = 1;
					break;
				}
			}
			return 1;
		}
		else if(kk == 10)
		{
			for(locIndex1_=0;locIndex1_<_L_G.peernum;locIndex1_++)
			{
				if(allpeers[locIndex1_].peeradress == peeradr_1)
				{
					allpeers[locIndex1_].groupid	= msg->Data[4];
					_L_G.answer_ret = 1;
					break;
				}
			}
			return 1;
		}
		return 1;

	}
	return 0;
}



static uint8_t Attr_WriteErr(CanRxMsgTypeDef* msg,AttributeType* atr,HeadMessage* headmsg,uint8_t code)
{
	CanTxMsgTypeDef sendmsg;
	HeadMessage headm = *headmsg;
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	sendmsg.IDE=CAN_ID_EXT;
	sendmsg.RTR = 0;
	headm.rre = 2;
	headm.fcbt = 0;
	headm.fcbc = 0;
	if(linshi == 0 || linshi == 2)
	{
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 0;
		headm.adr = _L_G.Peer;
		needadr = 0;
	}
	else if(linshi == 5 || linshi == 7)//Peer To Peer
	{
		headm.adr = _L_G.Peer;
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 1;
		needadr = 1;
	}
	headm.fcbt = 0;
	headm.fcbc = 0;
	sendmsg.DLC = 5+needadr;
	sendmsg.Data[0] = atr->id/256;
	sendmsg.Data[1] = atr->id%256;
	sendmsg.Data[2] = atr->stat;
	sendmsg.Data[3] = atr->propert;
	sendmsg.Data[4] = code;
	GetSendHead(&sendmsg,&headm);
	if(needadr == 1)
	{
		sendmsg.Data[5] =  headmsg->adr;
	}
	TSK_CAN_Transmit(&sendmsg);

	return 0;
}



static uint8_t SaveAttribute(AttributeType* atr,uint8_t* data,uint8_t len)
{
	switch(atr->id)
	{
	case 0:
		NetAttribute.vendorid=data[0]*256+data[1];
		break;
	case 1:
		memset(NetAttribute.vendorname,0,10);
		if(len>10)
			len =10;
		memcpy(NetAttribute.vendorname,data,len);
		break;
	case 2:
		memset(NetAttribute.modelnum,0,10);
		if(len >10)
			len =10;
		memcpy(NetAttribute.modelnum,data,len);
		break;
	case 3:
		memset(NetAttribute.modulename,0,10);
		if(len >10)
			len =10;
		memcpy(NetAttribute.modulename,data,len);
		break;
	case 4:
		NetAttribute.classid = (uint16_t)(data[0]*256+data[1]);
		break;
	case 5:
		memset(NetAttribute.classname,0,10);
		if(len >10)
			len =10;
		(void)memcpy(NetAttribute.classname,data,len);
		break;
	case 6:
		NetAttribute.classrevision = (uint16_t)(data[0]*256+data[1]);
		break;
	case 7:
		NetAttribute.classtype = data[0];
		break;
	case 8:
		NetAttribute.groupswitch = data[0];
		break;
	case 9:
		NetAttribute.instanceswitch = data[0];
		break;
	case 10:
		NetAttribute.groupassign = data[0];
		break;
	case 11:
		NetAttribute.instanceassign = data[0];
		break;
	case 12:
		NetAttribute.arclinkrevision = (uint16_t)(data[0]*256+data[1]);
		break;
	case 13:
		NetAttribute.readystatus = data[0];//bit7����ready
		break;
	case 14:
		NetAttribute.fault = data[0];
		break;
	case 16:
		NetAttribute.ssn = (uint16_t)(data[0]*256+data[1]);
		break;
	case 17:
		NetAttribute.neo = data[0];
		break;
	case 18:
		NetAttribute.eon = data[0];
		break;
	case 19:
		memset(NetAttribute.softversion,0,10);
		if(len >10)
			len =10;
		memcpy(NetAttribute.softversion,data,10);
		break;
	case 20:
		memset(NetAttribute.hardversion,0,10);
		if(len >10)
			len =10;
		memcpy(NetAttribute.hardversion,data,len);
		break;
	case 21:
		NetAttribute.diagstatus = data[0]*4095*256+data[1]*4096+data[2]*256+data[3];
		break;
	case 22:
		ReInitArcLink();
		break;
	case 23:
		_L_G.ismapped = 1;
		NetAttribute.busavailable = data[0];
		if(data[0]==1)
			NetAttribute.reset = 0;
		break;
	case 24:
		NetAttribute.bootstrap = data[0];
		break;
	case 25:
		NetAttribute.objectindecator = data[0];
		break;
	case 26:
		NetAttribute.numoftimemaped = data[0];
		break;
	case 27:
		NetAttribute.modulebaseaddr = data[0];
		break;
	case 28:
		NetAttribute.numobjects =data[0];
		break;
	case 29:
		NetAttribute.hbdis = data[0];
		break;
	default:
		break;
	}
	return 0;
}



static uint8_t Attr_WriteOk(CanRxMsgTypeDef* msg,AttributeType* atr,HeadMessage* headmsg)
{
	CanTxMsgTypeDef sendmsg;
	HeadMessage headm = *headmsg;
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	sendmsg.IDE=CAN_ID_EXT;
	sendmsg.RTR = 0;
	headm.rre = 1;
	headm.fcbt = 0;
	headm.fcbc = 0;
	if(linshi == 0 || linshi == 2)
	{
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 0;
		headm.adr = _L_G.Peer;
		needadr = 0;
	}
	else if(linshi == 5 || linshi == 7)//Peer To Peer
	{
		headm.adr = _L_G.Peer;
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 1;
		needadr = 1;
	}
	headm.fcbt = 0;
	headm.fcbc = 0;
	sendmsg.DLC = 4+needadr;
	sendmsg.Data[0] = atr->id/256;
	sendmsg.Data[1] = atr->id%256;
	sendmsg.Data[2] = atr->stat;
	sendmsg.Data[3] = atr->propert;
	GetSendHead(&sendmsg,&headm);
	if(atr->id == 22)
		return 0;
	if(needadr == 1)
	{
		sendmsg.Data[4] =  headmsg->adr;
	}
	TSK_CAN_Transmit(&sendmsg);

	return 0;
}


static uint8_t GetAtrdata(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t* data, uint8_t* len)
{
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	if(linshi == 0 || linshi == 2)
	{
		needadr = 0;
	}
	else if(linshi == 5 || linshi == 7)//Peer To Peer
	{
		needadr = 1;
	}
	if(headmsg->fcbt==1)
	{
		memcpy(data,msg->Data+4,4-needadr);
		*len = 4-needadr;
		return 0;
	}
	else if(headmsg->fcbt==2)
	{
		memcpy(data,msg->Data,8-needadr);
		*len += 8-needadr;
		return 0;
	}
	else if(headmsg->fcbt==3)
	{
		memcpy(data,msg->Data,msg->DLC-needadr);
		*len += msg->DLC-needadr;
		return 1;
	}
	return 0;
}



static uint8_t RespondWriteAtr(CanRxMsgTypeDef* msg,AttributeType* atr,HeadMessage* headmsg)
{
	static uint8_t fcbnum;
	uint8_t fcbnum_7;
	static uint8_t data[255];
	static uint8_t len=0;
	uint8_t lenall=0;
	uint8_t dataall[8];
	uint8_t locIndex1_;
	if(atr->id == 22)
	{
		//reset??
		_L_G.TRACTOR_Stabel = 0x0;
	}
	if(atr->id <=29)
	{
		if(headmsg->fcbt==1)
		{
			fcbnum =0;
		}
		else if(headmsg->fcbt==2)
		{
			fcbnum_7 = fcbnum++;
			while(fcbnum_7>=8)
			{
				fcbnum_7-=8;
			}
			if(fcbnum_7 != headmsg->fcbc)
			{
				return Attr_WriteErr( msg, atr,headmsg,27);
			}
		}
		else if(headmsg->fcbt==0)
		{
			lenall = msg->DLC-4;
			for(locIndex1_=0;locIndex1_<msg->DLC-4;locIndex1_++)
			{
				dataall[locIndex1_] = msg->Data[4+locIndex1_];
			}
			lenall = msg->DLC-4;
			SaveAttribute(atr,dataall,lenall);
			return Attr_WriteOk(msg,atr,headmsg);
		}

		if(GetAtrdata(msg,headmsg,data,&len))
		{
			SaveAttribute(atr,data,len);
			return Attr_WriteOk(msg,atr,headmsg);
		}
	}
	else
	{
		if(atr->id == 28740)
		{
			if(headmsg->fcbt!=1)
			{
				return Attr_WriteOk(msg,atr,headmsg);
			}
		}
		else if(headmsg->fcbt!=1)
			return Attr_WriteErr( msg, atr,headmsg,1);
	}
	return 0;
}


static uint8_t Atr_SendErrRead(CanRxMsgTypeDef* msg,AttributeType* atr ,HeadMessage* headmsg,uint8_t code)
{
	CanTxMsgTypeDef sendmsg;
	HeadMessage headm = *headmsg;
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	sendmsg.RTR = 0;
	headm.rre = 2;
	sendmsg.IDE=CAN_ID_EXT;
	if(linshi == 0 || linshi == 2)
	{
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 0;
		headm.adr = _L_G.Peer;
		needadr = 0;
	}
	else if(linshi == 5 || linshi == 7)//Peer To Peer
	{
		headm.adr = _L_G.Peer;
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 1;
		needadr = 1;
	}
	headm.fcbt = 0;
	headm.fcbc = 0;
	sendmsg.DLC = 5+needadr;
	sendmsg.Data[0] = atr->id/256;
	sendmsg.Data[1] = atr->id%256;
	sendmsg.Data[2] = atr->stat;
	sendmsg.Data[3] = atr->propert;
	sendmsg.Data[4] = code;
	GetSendHead(&sendmsg,&headm);
	if(needadr == 1)
	{
		sendmsg.Data[5] =  headmsg->adr;
	}
	TSK_CAN_Transmit(&sendmsg);

	return 0;
}


static uint8_t Atr_SendSucRead(CanRxMsgTypeDef* msg,AttributeType* atr ,HeadMessage* headmsg,uint8_t* adr, uint8_t len)
{
	CanTxMsgTypeDef sendmsg;
	HeadMessage headm = *headmsg;
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	uint8_t sendlen;
	uint8_t locIndex1_;
	uint8_t sendnum;
	sendmsg.RTR = 0;
	sendmsg.IDE=CAN_ID_EXT;
	headm.rre = 1;
	if(linshi == 0 || linshi == 2)
	{
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 0;
		headm.adr = _L_G.Peer;
		needadr = 0;
	}
	else if(linshi == 5 || linshi == 7)//Peer To Peer
	{
		headm.adr = _L_G.Peer;
		headm.ds = 1;
		headm.gr = 0;
		headm.di = 1;
		needadr = 1;
	}
	sendlen = needadr + len +4;
	if(sendlen <=8)
	{
		/*uint16_t id ;
		uint8_t stat ;
		uint8_t propert;
		*/
		headm.fcbt = 0;
		headm.fcbc = 0;
		sendmsg.DLC = sendlen;
		sendmsg.Data[0] = atr->id/256;
		sendmsg.Data[1] = atr->id%256;
		sendmsg.Data[2] = atr->stat;
		sendmsg.Data[3] = atr->propert;
		for(locIndex1_=0;locIndex1_<len;locIndex1_++)
		{
			sendmsg.Data[4+locIndex1_] = adr[locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[4+len] =  headmsg->adr;
		}
		GetSendHead(&sendmsg,&headm);
		TSK_CAN_Transmit(&sendmsg);
	}
	else
	{
		headm.fcbt = 1;
		headm.fcbc = 0;
		sendmsg.DLC = 8;
		sendmsg.Data[0] = atr->id/256;
		sendmsg.Data[1] = atr->id%256;
		sendmsg.Data[2] = atr->stat;
		sendmsg.Data[3] = atr->propert;

		for(locIndex1_=0;locIndex1_<4;locIndex1_++)
		{
			sendmsg.Data[4+locIndex1_] = adr[locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[7] =  headmsg->adr;
			sendnum = 3;
		}
		else
		{
			sendnum = 4;
		}
		GetSendHead(&sendmsg,&headm);
		TSK_CAN_Transmit(&sendmsg);
		delay_ms(1);
		if(needadr==1)
		{
			while(len-sendnum>7)
			{
				headm.fcbt = 2;
				(headm.fcbc)++;
				/*lint -e650*/
				if(headm.fcbc>=8)
					headm.fcbc = 0;
				GetSendHead(&sendmsg,&headm);
				for(locIndex1_=0;locIndex1_<6;locIndex1_++)
				{
					sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
				}
				if(needadr==1)
				{
					sendmsg.Data[locIndex1_] = headmsg->adr;
					sendnum+=7;
				}
				else
				{
					sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
					sendnum+=8;
				}
				sendmsg.DLC = 8;
				TSK_CAN_Transmit(&sendmsg);
				delay_ms(1);
			}
			headm.fcbt = 3;
			(headm.fcbc)++;
			/*lint -e650*/
			if(headm.fcbc>=8) 	headm.fcbc = 0;
			GetSendHead(&sendmsg,&headm);
			sendmsg.DLC = len-sendnum+1;
			for(locIndex1_=0;locIndex1_<len-sendnum;locIndex1_++)
			{
				sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
			}
			sendmsg.Data[locIndex1_] = headmsg->adr;
			TSK_CAN_Transmit(&sendmsg);
		}
		else
		{
			while(len-sendnum>8)
			{
				headm.fcbt = 2;
				(headm.fcbc)++;
				/*lint -e650*/
				if(headm.fcbc>=8)
					headm.fcbc = 0;
				GetSendHead(&sendmsg,&headm);
				for(locIndex1_=0;locIndex1_<7;locIndex1_++)
				{
					sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
				}

				sendmsg.DLC = 8;
				TSK_CAN_Transmit(&sendmsg);
				delay_ms(1);
			}
			headm.fcbt = 3;
			(headm.fcbc)++;
			/*lint -e650*/
			if(headm.fcbc>=8)
				headm.fcbc = 0;
			GetSendHead(&sendmsg,&headm);
			sendmsg.DLC = len-sendnum+1;
			for(locIndex1_=0;locIndex1_<len-sendnum;locIndex1_++)
			{
				sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
			}
			TSK_CAN_Transmit(&sendmsg);
		}
	}
	return 0;
}



static uint8_t RespondReadAtr(CanRxMsgTypeDef* msg,AttributeType* atr ,HeadMessage* headmsg)
{
	uint8_t senddata[10];
	if(atr->id <=29)
	{
		switch(atr->id)
		{
		case 0:
			senddata[0] = NetAttribute.vendorid/256;
			senddata[0] = NetAttribute.vendorid%256;
			return Atr_SendSucRead(msg,atr,headmsg,senddata,2);
		case 1:
			return Atr_SendSucRead(msg,atr,headmsg,(NetAttribute.vendorname),10);
		case 2:
			return Atr_SendSucRead(msg,atr,headmsg,(NetAttribute.modelnum),10);
		case 3:
			return Atr_SendSucRead(msg,atr,headmsg,(NetAttribute.modulename),10);
		case 4:
			senddata[0] = NetAttribute.classid/256;
			senddata[1] = NetAttribute.classid%256;
			return Atr_SendSucRead(msg,atr,headmsg,senddata,2);
		case 5:
			return Atr_SendSucRead(msg,atr,headmsg,(NetAttribute.classname),10);
		case 6:
			return Atr_SendSucRead(msg,atr,headmsg,(uint8_t*)&(NetAttribute.classrevision),2);
		case 7:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.classtype),1);
		case 8:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.groupswitch),1);
		case 9:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.instanceassign),1);
		case 10:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.groupassign),1);


		case 11:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.instanceassign),1);
		case 12:
			return Atr_SendSucRead(msg,atr,headmsg,(uint8_t*)&(NetAttribute.arclinkrevision),2);
		case 13:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.readystatus),1);
		case 14:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.fault),1);

		case 15:
			return Atr_SendErrRead(msg,atr,headmsg,0x01);
		case 16:
			return Atr_SendSucRead(msg,atr,headmsg,(uint8_t*)&(NetAttribute.ssn),2);
		case 17:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.neo),1);
		case 18:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.eon),1);


		case 19:
			return Atr_SendSucRead(msg,atr,headmsg,(NetAttribute.softversion),10);
		case 20:
			return Atr_SendSucRead(msg,atr,headmsg,(NetAttribute.hardversion),10);


		case 21:
			return Atr_SendSucRead(msg,atr,headmsg,(uint8_t*)&(NetAttribute.diagstatus),4);


		case 22:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.reset),1);
		case 23:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.busavailable),1);
		case 24:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.bootstrap),1);
		case 25:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.objectindecator),1);

		case 26:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.numoftimemaped),1);
		case 27:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.modulebaseaddr),1);
		case 28:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.numobjects),1);
		case 29:
			return Atr_SendSucRead(msg,atr,headmsg,&(NetAttribute.hbdis),1);

		}
	}
	else
	{
		if(atr->id == 28676)
		{
			uint8_t data_o[17]={0x31,0x36,0x33,0x38,0x38,0x32,0x30,0x30,0x34,0x31,0x39,0x39,0x30,0x30,0x33,0x38,0x00,};
			return Atr_SendSucRead(msg,atr,headmsg,data_o,17);
		}
	}
	return Atr_SendErrRead(msg,atr,headmsg,0x01);
}



uint8_t WRBlockProcess(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	switch(id)
	{
	default:
		RdBlockResPondErr(id,20,headmsg);
		break;
	}
	return 0;
}

uint8_t UNConEvRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	switch(id)
	{
	case 514://modual;
		{
			uint16_t count = 0;
			_L_G.block514respond = (uint16_t)(msg->Data[2]*256+msg->Data[3]);
			if(_L_G.block514respond == 0x02 && Can_G_V.hanjiestatus != 0)//volt present
			{
				parameter_Put(PARA_IDX_shortCount,WHOLE_OBJECT, &count);
			}
			else if(_L_G.block514respond == 0x01 && Can_G_V.hanjiestatus != 0)//cur present
			{
				parameter_Put(PARA_IDX_loseArcNum,WHOLE_OBJECT, &count);
			}

			return 0;
		}
	}
	return 0;
}

uint8_t SolvServiceCode(CanRxMsgTypeDef* msg,HeadMessage* headmsg ) //��Ҫ�𸴵��ź�
{
	static AttributeType  atrrtyperead;
	static AttributeType  atrrtypewrit;
	static uint16_t			actionid;
	static uint16_t			rdblockid;
	static uint16_t			wrblockid;
	static uint16_t			ConFEVRPID;
	static uint16_t			UNConFEVRPID;
	static uint16_t			UNConFEVBRPID;
	static uint16_t			ConFEVBRPID;
	uint8_t type=0;
	switch(headmsg->service)
	{
	case 0:	  			//Read Atribute ";
		{
			AttributeType attr;
			if(headmsg->fcbt==0)
			{
				attr.id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				attr.stat = (msg->Data[2]);
				attr.propert = (msg->Data[3]);
				return RespondReadAtr(msg,&attr,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				atrrtyperead.id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				atrrtyperead.stat = (msg->Data[2]);
				atrrtyperead.propert = (msg->Data[3]);
			}
			RespondReadAtr(msg,&atrrtyperead,headmsg);
		}
		break;
	case 1:			//		  "Write Attibute ";
		{
			AttributeType attr;
			if(headmsg->fcbt==0)
			{
				attr.id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				attr.stat = (msg->Data[2]);
				attr.propert = (msg->Data[3]);
				return RespondWriteAtr(msg,&attr,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				atrrtypewrit.id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				atrrtypewrit.stat = (msg->Data[2]);
				atrrtypewrit.propert = (msg->Data[3]);
			}
			RespondWriteAtr(msg,&atrrtypewrit,headmsg);
		}
		break;
	case 4://action
		{
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return ActionProcess(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				actionid = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
			}
			return ActionProcess(msg,actionid,headmsg);

		}
	case 5://read block
		{
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return RdBlockProcess(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				rdblockid = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
			}
			return RdBlockProcess(msg,rdblockid,headmsg);
		}

	case 2:			//		  Confirmed Event Report;
		{
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return ConEvRP(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				ConFEVRPID = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
			}
			return ConEvRP(msg,ConFEVRPID,headmsg);
		}
	case 3:			//		  Unconfirmed Event Report;
		{
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return UNConEvRP(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				UNConFEVRPID = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
			}
			return UNConEvRP(msg,UNConFEVRPID,headmsg);
		}


	case 6:
		{
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return WRBlockProcess(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				wrblockid = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
			}
			return WRBlockProcess(msg,wrblockid,headmsg);
		}
	case 7:
		//			out1<<"Confirmed Event Block Report ";
		{
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return ConEvBlockRP(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				ConFEVBRPID = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
			}
			return ConEvBlockRP(msg,ConFEVBRPID,headmsg);
		}
	case 8:
		{
			//	out1<<"Unconfirmed Event  Block Report ";
			if(headmsg->fcbt==0)
			{
				uint16_t id = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				return UnConEvBlockRP(msg,id,headmsg);
			}
			else if(headmsg->fcbt==1)
			{
				UNConFEVBRPID = (uint16_t)((msg->Data[0])*256+msg->Data[1]);
				if(UNConFEVBRPID == 516)
				{
					UNConFEVBRPID = 516;
				}
			}
			return UnConEvBlockRP(msg,UNConFEVBRPID,headmsg);
		}
	default:
		type = 1;
		break;
	}
	return type;

}

