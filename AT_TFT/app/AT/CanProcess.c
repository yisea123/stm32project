#include <stdint.h>

#include "CanProcess.h"
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "parameter_idx.h"
#include "unit_parameter.h"
#include "dev_can.h"
#include "main.h"

#define CAN_RX_MSG_ID		CAN_RX_ID1
#define UNKNOW				100
#define DELAY_MILISEC 		5


uint16_t 		ALLMODES[256];

uint8_t 			Group[10]; //���е���
OnePara 			oneparasavetofile;






static uint8_t ModeExist(uint16_t mode)
{
	uint8_t 	locIndex1_ = 0;

	if(mode == 0x0)
		return 0;
	for(locIndex1_ = 0;locIndex1_<255;locIndex1_++)
	{
		if(ALLMODES[locIndex1_] == mode)
			return 1;
	}
	return 0;
}





static void DelayXms(unsigned int x)
{
	osDelay(x);
}
static uint8_t Act19_ResPond(void)
{
	CanTxMsgTypeDef sendmsg;
	uint32_t adr =  _L_G.Peer;
	adr= adr<<13;
	sendmsg.RTR=CAN_RTR_DATA;
	sendmsg.IDE= CAN_ID_EXT;
	sendmsg.ExtId = 0x1F400900 | adr;
	sendmsg.Data[0] = 0x00;
	sendmsg.Data[1] = 0x13;
	sendmsg.Data[2] = 0x00;
	sendmsg.DLC = 3;
	TSK_CAN_Transmit(&sendmsg);
	return 0;
}

















static uint8_t FragResPondErr(uint8_t* data,uint8_t errcode,HeadMessage* headmsg)
{
	CanTxMsgTypeDef sendmsg;
	HeadMessage headm = *headmsg;
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	uint8_t locIndex1_ = 0;
	uint8_t len_to;
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
	if(headmsg->service<2)
		len_to = 4;
	else
		len_to = 2;
	headm.fcbt = 0;
	headm.fcbc = 0;
	sendmsg.DLC = len_to+needadr+1;
	for(locIndex1_=0;locIndex1_<len_to;locIndex1_++)
	{
		sendmsg.Data[locIndex1_] = data[locIndex1_];
	}
	sendmsg.Data[len_to] = errcode;
	if(needadr == 1)
	{
		sendmsg.Data[len_to+1] =  headmsg->adr;
	}
	GetSendHead(&sendmsg,&headm);

	TSK_CAN_Transmit(&sendmsg);

	return 0;
}



static uint8_t ActionResPondErr(uint16_t id,uint8_t errcode,HeadMessage* headmsg)
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

	{
		sendmsg.DLC = 3+needadr;
		sendmsg.Data[0] = id/256;
		sendmsg.Data[1] = id%256;
		sendmsg.Data[2] = errcode;
		GetSendHead(&sendmsg,&headm);
		if(needadr == 1)
		{
			sendmsg.Data[3] =  headmsg->adr;
		}

	}
	TSK_CAN_Transmit(&sendmsg);

	return 0;
}


static uint8_t ActionResPond(uint16_t id,uint8_t* adr,uint8_t len,HeadMessage* headmsg)
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
	sendlen = needadr + len +2;
	if(sendlen <=8)
	{
		headm.fcbt = 0;
		headm.fcbc = 0;
		sendmsg.DLC = sendlen;
		sendmsg.Data[0] = id/256;
		sendmsg.Data[1] = id%256;
		for(locIndex1_=0;locIndex1_<len;locIndex1_++)
		{
			sendmsg.Data[2+locIndex1_] = adr[locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[2+len] =  headmsg->adr;
		}
		GetSendHead(&sendmsg,&headm);
		TSK_CAN_Transmit(&sendmsg);
	}
	else
	{
		headm.fcbt = 1;
		headm.fcbc = 0;
		sendmsg.DLC = 8;
		sendmsg.Data[0] = id/256;
		sendmsg.Data[1] = id%256;
		for(locIndex1_=0;locIndex1_<6-needadr;locIndex1_++)
		{
			sendmsg.Data[2+locIndex1_] = adr[locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[7] =  headmsg->adr;
		}
		sendnum = 6-needadr;
		GetSendHead(&sendmsg,&headm);
		TSK_CAN_Transmit(&sendmsg);
		DelayXms(1);
		while(len-sendnum+needadr>8)
		{
			headm.fcbt = 2;
			(headm.fcbc)++;
			/*lint -e650*/
			if(headm.fcbc>=8)
				headm.fcbc = 0;
			GetSendHead(&sendmsg,&headm);
			for(locIndex1_=0;locIndex1_<8-needadr;locIndex1_++)
			{
				sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
			}
			if(needadr==1)
			{
				sendmsg.Data[7] = headmsg->adr;
				sendnum+=7;
			}
			else
			{
				sendnum+=8;
			}
			sendmsg.DLC = 8;
			TSK_CAN_Transmit(&sendmsg);
			DelayXms(1);
		}
		headm.fcbt = 3;
		(headm.fcbc)++;
		/*lint -e650*/
		if(headm.fcbc>=8)
			headm.fcbc = 0;
		GetSendHead(&sendmsg,&headm);
		sendmsg.DLC = len-sendnum+needadr;
		for(locIndex1_=0;locIndex1_<len-sendnum;locIndex1_++)
		{
			sendmsg.Data[locIndex1_] = adr[sendnum+locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[len-sendnum] = headmsg->adr;
		}
		TSK_CAN_Transmit(&sendmsg);

	}
	return 0;
}

uint8_t GetFragMent(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t len,uint8_t* data, uint8_t* datalen )
{
	uint8_t linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t needadr = 0;
	uint8_t locIndex1_;
	static uint8_t fcbc=0;
	static uint8_t fromadr = 0xef;
	if(linshi == 0 || linshi == 2)
	{
		needadr = 0;
		if(fromadr == 0xef)
			fromadr = 0xff;
		else if(fromadr!=0xff)
		{
			FragResPondErr(msg->Data,27,headmsg);
		}
	}
	else if(linshi == 5 || linshi == 7)//Peer To Peer
	{
		needadr = 1;
	}
	if(headmsg->fcbt == 1)
	{
		*datalen=0;
		for(locIndex1_=0;locIndex1_<8-len-needadr;locIndex1_++)
		{
			data[locIndex1_+*datalen] = msg->Data[locIndex1_+len];
		}
		*datalen+= 8-len-needadr;
		fcbc = 0;
		return 0;
	}
	else if(headmsg->fcbt == 2)
	{
		fcbc++;
		if(fcbc >=8)
			fcbc = 0;
		if(fcbc == headmsg->fcbc)
		{
			for(locIndex1_=0;locIndex1_<8-needadr;locIndex1_++)
			{
				data[locIndex1_+*datalen]	= msg->Data[locIndex1_];
			}
			*datalen+= 8-needadr;
		}
		else
		{
			FragResPondErr(data,27,headmsg);
			return 0xff;
		}
		return 0;
	}
	else if(headmsg->fcbt == 3)
	{
		fcbc++;
		if(fcbc >=8)
			fcbc = 0;
		if(fcbc == headmsg->fcbc)
		{
			for(locIndex1_=0;locIndex1_<msg->DLC-needadr;locIndex1_++)
			{
				data[locIndex1_+*datalen]	= msg->Data[locIndex1_];
			}
			*datalen+= msg->DLC-needadr;
		}
		else
		{
			FragResPondErr(data,27,headmsg);
			return 0xff;
		}
		return 1;
	}
	return 3;
}
void GetAtridStatMask(uint16_t class_id, uint8_t* data, uint8_t locIndex1_, uint16_t atrid)
{
	switch(class_id)
	{
	case 0:
		{
			switch(atrid)
			{
			case 0x020d:
				data[locIndex1_]   = 0x10;
				data[locIndex1_+1] = 0x0a;
				break;
			case 0x020e:
				data[locIndex1_]   = 0x10;
				data[locIndex1_+1] = 0x0a;
				break;
			case 0x020f:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			case 0x0210:
				data[locIndex1_]   = 0x3f;
				data[locIndex1_+1] = 0xff;
				break;
			default:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			}
		}
		break;
	case 1:
		{
			switch(atrid)
			{
			case 0x0200:
				data[locIndex1_]   = 0xD7;
				data[locIndex1_+1] = 0x7A;
				break;
			case 0x0205:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x48;
				break;
			case 0x0206:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x40;
				break;
			case 0x0207:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x40;
				break;
			case 0x0208:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x40;
				break;
			case 0x0209:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x40;
				break;
			case 0x020A:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x40;
				break;

			default:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			}
			break;
		}
	case 2:
		{
			switch(atrid)
			{
			case 0x0200:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			case 0x0201:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;

			default:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			}
			break;
		}
	case 4:
		{
			switch(atrid)
			{
			case 0x0200:
				data[locIndex1_]   = 0xDF;
				data[locIndex1_+1] = 0x7C;
				break;


			default:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			}
		}
		break;
	case 5:
		{
			switch(atrid)
			{
			default:
				data[locIndex1_]   = 0x00;
				data[locIndex1_+1] = 0x00;
				break;
			}
		}
		break;
	default:
		data[locIndex1_]   = 0x00;
		data[locIndex1_+1] = 0x00;
		break;

	}
}
void ProcessAction32(uint8_t* data, uint8_t* len)
{
	uint16_t class_id = (uint16_t)(data[1]+data[0]*256);
	uint8_t locIndex1_;
	uint16_t atrid;

	*len = (uint8_t)(*len-6);
	for(locIndex1_=0;locIndex1_<*len;locIndex1_++,locIndex1_++)
	{
		atrid = (uint16_t)(data[6+locIndex1_]*256+data[7+locIndex1_]);
		GetAtridStatMask(class_id,data,locIndex1_,atrid);
	}


}
uint8_t ActionProcess(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	static uint8_t actiondata[255];
	static uint8_t datalen;
	switch(id)
	{
	case 0://modual;
		{
			uint8_t data[10];
			if(_L_G.isaddressed==1)
			{
				return 0;
			}
			data[0] = NetAttribute.vendorid/256;
			data[1] = NetAttribute.vendorid%256;
			data[2] = _L_G.serialnum/256;
			data[3] = _L_G.serialnum%256;
			data[4] = 1;
			ActionResPond(id,data,5,headmsg);
			break;
		}
	case 1:
		{
			uint8_t data[10];
			if( msg->Data[2] == NetAttribute.vendorid/256 &&
				msg->Data[3] == NetAttribute.vendorid%256 &&
				msg->Data[4] == _L_G.serialnum/256 &&
				msg->Data[5] == _L_G.serialnum%256)
			{
				_L_G.Peer = msg->Data[6];
				_L_G.isaddressed = 1;
				return ActionResPond(id,data,0,headmsg);
			}
			break;
		}
	case 5:
		{
			uint8_t i=0;
			uint8_t data[2];
			for(i=0;i<10;i++)
			{
				if(Group[i] ==0)
				{
					Group[i] = msg->Data[2];
					break;
				}
			}
			ActionResPond(id,data,0,headmsg);
			break;
		}
	case 11:
		{
			uint8_t data[2];
			uint8_t ret = GetFragMent(msg,headmsg,2,actiondata,&datalen);
			if(ret== 1)
			{
				ActionResPond(id,data,0,headmsg);
			}
			else
			{
				return 2;
			}
			break;
		}
	case 19:
		{
			Act19_ResPond();
			break;
		}
	case 32://��ַ�����
		{
			uint8_t ret = GetFragMent(msg,headmsg,2,actiondata,&datalen);
			if(ret== 1)
			{
				ProcessAction32(actiondata,&datalen);
				ActionResPond(id,actiondata,datalen,headmsg);
			}
			else
			{
				return 2;
			}

			break;
		}
	default:
		ActionResPondErr(id,21,headmsg); //action not found
		break;
	}
	return 0;
}
uint8_t RdBlockResPond(uint16_t id,uint8_t* adr, uint8_t len,HeadMessage* headmsg)
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
	sendlen = needadr + len +2;
	if(sendlen <=8)
	{
		headm.fcbt = 0;
		headm.fcbc = 0;
		sendmsg.DLC = sendlen;
		sendmsg.Data[0] = id/256;
		sendmsg.Data[1] = id%256;
		for(locIndex1_=0;locIndex1_<len;locIndex1_++)
		{
			sendmsg.Data[2+locIndex1_] = adr[locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[2+len] =  headmsg->adr;
		}
		GetSendHead(&sendmsg,&headm);
		TSK_CAN_Transmit(&sendmsg);
	}
	else
	{
		headm.fcbt = 1;
		headm.fcbc = 0;
		sendmsg.DLC = 8;
		sendmsg.Data[0] = id/256;
		sendmsg.Data[1] = id%256;
		for(locIndex1_=0;locIndex1_<len;locIndex1_++)
		{
			sendmsg.Data[2+locIndex1_] = adr[locIndex1_];
		}
		if(needadr == 1)
		{
			sendmsg.Data[2+len] =  headmsg->adr;
		}
		sendnum = 6;
		GetSendHead(&sendmsg,&headm);
		TSK_CAN_Transmit(&sendmsg);
		DelayXms(1);
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
				DelayXms(1);
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
				DelayXms(1);
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
uint8_t RdBlockResPondErr(uint16_t id,uint8_t errcode,HeadMessage* headmsg)
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
	sendmsg.DLC = 3+needadr;
	sendmsg.Data[0] = id/256;
	sendmsg.Data[1] = id%256;
	sendmsg.Data[2] = errcode;
	GetSendHead(&sendmsg,&headm);
	if(needadr == 1)
	{
		sendmsg.Data[3] =  headmsg->adr;
	}
	TSK_CAN_Transmit(&sendmsg);

	return 0;
}
uint8_t RdBlockProcess(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	switch(id)
	{
	case 0://modual;
		{
			uint8_t data[10];
			data[0] = NetAttribute.classid/256;
			data[1] = NetAttribute.classid%256;
			data[2] = NetAttribute.groupassign;
			data[3] = NetAttribute.instanceassign;
			data[4] = NetAttribute.classtype;
			return RdBlockResPond(id,data,5,headmsg);
		}
	}
	RdBlockResPondErr(id,20,headmsg);
	return 0;
}
uint8_t ConEvRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	static	uint8_t data[20];
	static uint8_t datalen = 0;

	switch(id)
	{
	case 0://modual;
		{
			uint8_t _data[10];
			_data[0] = NetAttribute.classid/256;
			_data[1] = NetAttribute.classid%256;
			_data[2] = NetAttribute.groupassign;
			_data[3] = NetAttribute.instanceassign;
			_data[4] = NetAttribute.classtype;
			return RdBlockResPond(id,_data,5,headmsg);
		}
	case 2:		 //alarm code
		{
			uint8_t ret = GetFragMent(msg,headmsg,0,data,&datalen);
			if(ret ==1)
			{
				Can_G_V.errcode_show = data[2]*256+data[3];
			}
			break;
		}
	}
	RdBlockResPondErr(id,1,headmsg);
	return 0;
}




uint8_t ConEvBlockRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	static uint8_t blockdata[100];
	static uint8_t datalen = 0;
	uint16_t val_1 = 0;
	uint16_t val_2 = 0;
	switch(id)
	{
	case 512://modual;
		{
			uint8_t ret = GetFragMent(msg,headmsg,2,blockdata,&datalen);
			if(ret== 1)
			{
				if(blockdata[1]==0x07)
				{
					val_1 = (uint16_t)(blockdata[2]*256+ blockdata[3]);
				 	val_2 = (uint16_t)(blockdata[4]*256+ blockdata[5]);
					if(val_1 >= 0x10)
						parameter_Put(PARA_IDX_weldCurrent,WHOLE_OBJECT, &val_1);
					if(val_2 >= 0x30)
						parameter_Put(PARA_IDX_weldWorkPoint,WHOLE_OBJECT, &val_2);
					parameter_Put(PARA_IDX_can_validatenum,WHOLE_OBJECT, &id);
				}
			}
			break;
		}
	case 520:
		{
			uint8_t adrfrom = msg->Data[msg->DLC-1];
			if(adrfrom == GetPeerByClass(0x02))
			{
				val_1 = (uint16_t)(msg->Data[2]*256+ msg->Data[3]);
			 	val_2 = (uint16_t)(msg->Data[4]*256+ msg->Data[5]);
				parameter_Put(PARA_IDX_inchSpeed,WHOLE_OBJECT, &val_1);
				parameter_Put(PARA_IDX_inchCurrent,WHOLE_OBJECT, &val_2);
			}
			break;
		}
	}
	return 0;
}

uint8_t UnConEvBlockRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg)
{
	static uint8_t blockdata[50];
	static uint8_t datalen = 0;
	uint16_t val_1 = 0;
	uint16_t val_2 = 0;
	switch(id)
	{
	case 512://modual;
		{
			uint8_t ret = GetFragMent(msg,headmsg,2,blockdata,&datalen);
			if(ret== 1)
			{
				if(blockdata[1]==0x07)
				{
					val_1 = (uint16_t)(blockdata[2]*256+ blockdata[3]);
				 	val_2 = (uint16_t)(blockdata[4]*256+ blockdata[5]);
					if(val_1 >= 0x20)
						parameter_Put(PARA_IDX_weldCurrent,WHOLE_OBJECT, &val_1);
					if(val_2 >= 0x30)
						parameter_Put(PARA_IDX_weldWorkPoint,WHOLE_OBJECT, &val_2);
					parameter_Put(PARA_IDX_can_validatenum,WHOLE_OBJECT, &id);
				}
			}
			break;
		}
	case 516:  //unknown
	{
		uint8_t ret = GetFragMent(msg,headmsg,2,blockdata,&datalen);
		if(ret== 1)
		{
		}//unknown
	}
	break;
	case 520:
		{
			uint8_t adrfrom = msg->Data[msg->DLC-1];
			if(adrfrom == GetPeerByClass(0x02))
			{
				val_1 = (uint16_t)(msg->Data[2]*256 + msg->Data[3]);
			 	val_2 = (uint16_t)(msg->Data[4]*256 + msg->Data[5]);
				parameter_Put(PARA_IDX_inchSpeed,WHOLE_OBJECT, &val_1);
				parameter_Put(PARA_IDX_inchCurrent,WHOLE_OBJECT, &val_2);
			}
		}
		break;
	}
	return 0;
}




uint8_t GetErroCode(uint8_t errcode)
{
	uint8_t ret = 0;
	switch(errcode)
	{
	case 0:
		//"Success ";
		ret = 1;
		break;
	case 1:
		//"Attribute not found ";
		ret = 1;
		break;
	case 2:
		//"Read only ";
		ret = 1;
		break;
	case 3:
		//"General limit error ";
		ret = 1;
		break;
	case 4:
		//"High limit error ";
		ret = 1;
		break;
	case 5:
		//"Low limit error ";
		ret = 1;
		break;
	case 6:
		//"Schedule not found ";
		ret = 1;
		break;
	case 7:
		//"Group not found ";
		ret = 1;
		break;
	case 8:
		//"Object not found ";
		ret = 1;
		break;
	case 9:
		//"State not found ";
		ret = 1;
		break;
	case 10:
		//"Attribute typecode invalid ";
		ret = 1;
		break;
	case 11:
		//"Property not applicable ";
		ret = 1;
		break;
	case 12:
		//"Property not supported ";
		ret = 1;
		break;
	case 13:
		//"Property read only ";
		ret = 1;
		break;
	case 14:
		//"Property invalid ";
		ret = 1;
		break;
	case 15:
		//"Proxy object unavailable ";
		ret = 1;
		break;
	case 16:
		//"Proxy busy ";
		ret = 0;
		break;
	case 17:
		//"Proxy aborted ";
		ret = 1;
		break;
	case 18:
		//"Proxy failed ";
		ret = 1;
		break;
	case 19:
		//"Proxy object not mapped ";
		ret = 1;
		break;
	case 20:
		//"Block ID not found ";
		ret = 1;
		break;
	case 21:
		//"Action not found ";
		ret = 1;
		break;
	case 22:
		//"Group address not found ";
		ret = 1;
		break;
	case 23:
		//"Event not found ";
		ret = 1;
		break;
	case 24:
		//"Service not supported ";
		ret = 1;
		break;
	case 25:
		//"Service unavailable ";
		ret = 1;
		break;
	case 26:
		//"Fragment buffer busy ";
		ret = 0;
		break;
	case 27:
		//"Fragment sequence error ";
		ret = 0;
		break;
	case 28:
		//"Fragment timeout ";
		ret = 0;
		break;
	case 29:
		//"Fragment change ";
		ret = 0;
		break;
	case 30:
		//"Request size invalid ";
		ret = 1;
		break;
	case 31:
		//"Response size invalid ";
		ret = 1;
		break;
	case 32:
		//"Proxy source unavailable ";
		ret = 1;
		break;
	case 33:
		//"No response from proxy ";
		ret = 1;
		break;
	case 34:
		//"Write failed ";
		ret = 0;
		break;
	case 35:
		//"Read failed ";
		ret = 0;
		break;
	case 36:
		//"Invalid selection ";
		ret = 1;
		break;
	case 37:
		//"Busy ";
		ret = 0;
		break;
	case 38:
		//"Idle state only ";
		ret = 1;
		break;
	default:
		//"ID="<<num<<" ";
		ret = 1;
		break;

	}
	return ret;

}









uint16_t GetModeIndex(uint8_t* locIndex1_)
{
	uint16_t ret = 1;
	if(*locIndex1_ > 254)
		*locIndex1_ = 0;
	ret = revu16(ALLMODES[*locIndex1_]);
	if(ret == 0)
	{
		*locIndex1_ = 0;
		ret = revu16(ALLMODES[*locIndex1_]);
	}
	return ret;
}


static uint8_t AnswerRCERP(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type)
{
	uint8_t errcode1 = 0;
	static uint8_t errnum=0;
	if(type == 1)
	{
		errcode1 = msg->Data[2];
		errnum++;
		if( GetErroCode(errcode1)==1)
		{
			return 1;
		}
		else if(errnum<10)
			return 0;
		else
		{
			return 1;
		}
	}
	if(headmsg->service == 2)
	{
		errnum = 0;
		return 1;
	}
	return 0;
}


static void SendReqUER(uint16_t id,uint8_t* dat,uint8_t len,uint8_t adr, uint8_t mcb)
{
	CanTxMsgTypeDef TxMessage;
	uint8_t locIndex1_ = 0;
	uint32_t adr1 = _L_G.Peer;
	TxMessage.StdId=0x00;
	TxMessage.ExtId=0x1F200700 | (adr1<<13);
	TxMessage.IDE=CAN_ID_EXT;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.Data[0] = id/256;
	TxMessage.Data[1] = id%256;
	for(locIndex1_=0;locIndex1_<len;locIndex1_++)
		TxMessage.Data[locIndex1_+2]=dat[locIndex1_];
	TxMessage.Data[len+2] = adr;
	TxMessage.DLC = len+3;
	TSK_CAN_Transmit(&TxMessage);
}





static void SendErrCode(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t* data,uint8_t code)
{
	uint8_t 	locIndex1_=0;
	CanTxMsgTypeDef 	sendmsg;
	HeadMessage headm = *headmsg;
	uint8_t 	linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	uint8_t 	needadr = 0;

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
	switch(headmsg->service)
	{
	case 0:
	case 1:
		sendmsg.Data[0] = data[0];
		sendmsg.Data[1] = data[1];
		sendmsg.Data[2] = data[2];
		sendmsg.Data[3] = data[3];
		sendmsg.Data[4] = code;
		locIndex1_ = 5;
		break;
	default:
		sendmsg.Data[0] = data[0];
		sendmsg.Data[1] = data[1];
		sendmsg.Data[2] = code;
		locIndex1_ = 3;
		break;
	}
	if(needadr == 1)
	{
		sendmsg.Data[locIndex1_] = headmsg->adr;
	}
	sendmsg.DLC = locIndex1_+needadr;
	GetSendHead(&sendmsg,&headm);
	TSK_CAN_Transmit(&sendmsg);
}

















