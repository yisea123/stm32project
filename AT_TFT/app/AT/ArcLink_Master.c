/*
 * ArcLink_Master.c
 *
 *  Created on: 2017年5月4日
 *      Author: pli
 */


#include "canprocess.h"
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "parameter_idx.h"
#include "unit_parameter.h"

AttributeType 	reqatr;
OneMaxMinValue 	allwavevalue[5];
AtrDescription 	action515data;  //action515
HANJIEPARA 		hanjiepara;
uint8_t 		Act520Data[20];
void delay_ms(uint16_t num)
{
	osDelay(num);
}



static void GetSetHanjiePara(void)
{
	hanjiepara.strikespeed = parameterWeldSetParameter.strikeInch;
	hanjiepara.striketrim = parameterWeldSetParameter.strikeTrim;
	hanjiepara.strikeworkpoint = parameterWeldSetParameter.strikeWorkpoint;
	hanjiepara.starttrim = parameterWeldSetParameter.startTrim;
	hanjiepara.startworkpoint = parameterWeldSetParameter.startWorkpoint ;

	hanjiepara.cratertrim = parameterWeldSetParameter.craterTrim;
	hanjiepara.craterworkpoint = parameterWeldSetParameter.craterWorkpoint;


	hanjiepara.starttime = parameterWeldSetParameter.startTime;
	hanjiepara.upslopetime = parameterWeldSetParameter.upslopeTime;
	hanjiepara.downslopetime = parameterWeldSetParameter.downSlopeTime;
	hanjiepara.cratertime = parameterWeldSetParameter.craterTime;
	hanjiepara.stopmovedealy = parameterWeldSetParameter.stopMoveDelay;
	hanjiepara.startmovedelay = parameterWeldSetParameter.startMoveDelay;


	hanjiepara.coldinchSpeed = parameterWeldSetParameter.codeInchSpeed;
	hanjiepara.weldmode = parameterWeldSetParameter.setMode;
}


void GetSendHead(CanTxMsgTypeDef* msg,HeadMessage* headmsg)
{
	msg->RTR=CAN_RTR_DATA;
	msg->IDE= CAN_ID_EXT;
	msg->ExtId = (uint32_t)(headmsg->conid) <<25 | (uint32_t)(headmsg->di) <<8 | (uint32_t)(headmsg->ds) <<24 | (uint32_t)(headmsg->gr) <<21 |
		(uint32_t)(headmsg->adr) <<13 | (uint32_t)(headmsg->rre) <<22 | (uint32_t)(headmsg->service) <<9	|
		(uint32_t)(headmsg->fcbt) <<6 | (uint32_t)(headmsg->fcbc) <<3 | (uint32_t)(headmsg->mcb) <<0;
}


static uint8_t SendData_Can(HeadMessage* headm,uint8_t* data,uint8_t len,uint8_t needadr,uint8_t adress)
{
	uint8_t lensend = len;
	CanTxMsgTypeDef sendmsg;
	uint8_t locIndex1_ = 0;
	uint8_t fcbc = 0;
	uint8_t len0 = 0;
	sendmsg.IDE = CAN_ID_EXT;
	sendmsg.RTR = 0;

	if(lensend + needadr <=8)
	{
		sendmsg.DLC = lensend+needadr;
		headm->fcbt =0;
		headm->fcbc = 0;
		GetSendHead(&sendmsg,headm);
		for(locIndex1_=0;locIndex1_ <lensend;locIndex1_++)
			sendmsg.Data[locIndex1_] =  data[locIndex1_];
		if(needadr == 1)
		{
			sendmsg.Data[locIndex1_] =  adress;
		}
		sendmsg.DLC =lensend+needadr;
		TSK_CAN_Transmit(&sendmsg);
	}
	else
	{
		headm->fcbt = 1;
		headm->fcbc = 0;
		GetSendHead(&sendmsg,headm);
		for(locIndex1_=0;locIndex1_ <8-needadr;locIndex1_++)
			sendmsg.Data[locIndex1_] =  data[locIndex1_];
		if(needadr == 1)
		{
			sendmsg.Data[locIndex1_] =  adress;
		}
		sendmsg.DLC =8;
		locIndex1_ = 8-needadr;
		TSK_CAN_Transmit(&sendmsg);
		delay_ms(2);
		while(lensend-locIndex1_ + needadr >8)
		{
			fcbc++;
			if(fcbc >=8)
				fcbc=0;
			headm->fcbt = 2;
			headm->fcbc = fcbc;
			GetSendHead(&sendmsg,headm);
			sendmsg.DLC =8;
			for(len0=0;len0<8-needadr;len0++)
			{
				sendmsg.Data[len0] =  data[len0+locIndex1_];
			}
			if(needadr==1)
			{
				locIndex1_+=	7;
				sendmsg.Data[7] =  adress;
			}
			else
			{
				locIndex1_+=8;
			}
			TSK_CAN_Transmit(&sendmsg);
			delay_ms(2);
		}
		fcbc++;
		if(fcbc >=8)
			fcbc=0;
		headm->fcbt = 3;
		headm->fcbc = fcbc;
		sendmsg.DLC =lensend - locIndex1_ + needadr;
		for(len0=0;len0<lensend - locIndex1_ ;len0++)
		{
			sendmsg.Data[len0] =  data[len0+locIndex1_];
		}
		if(needadr==1)
		{
			sendmsg.Data[len0] =  adress;
		}
		GetSendHead(&sendmsg,headm);
		TSK_CAN_Transmit(&sendmsg);
		delay_ms(2);
	}
	return 1;
}





uint8_t AnswerReadAtr(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type)
{
	static uint8_t fragment[255];
	static uint8_t datalen;
	static uint8_t errnum = 0;
	uint8_t errcode = 0;
	uint32_t timefromsd1000 = 0;
	if(type == 1)
	{
		errcode = msg->Data[4];
		errnum++;
		if( GetErroCode(errcode)==1)
		{
			return 1;
		}
		else if(errnum <10)
			return 0;
		else
		{
			return 1;
		}
	}
	if(headmsg->service == 0)
	{
		errnum = 0;
		if(headmsg->fcbt == 0)
		{
			uint16_t kk = msg->Data[0]*256 + msg->Data[1];
			if( reqatr.id == kk &&
				reqatr.stat == msg->Data[2] && reqatr.propert == msg->Data[3])
			{
				if(kk == 34)
					_L_G.atrid34value = msg->Data[4];
				else if(kk==527)
				{
					if(msg->Data[3]==0)
					{
						allneedvalue.inchdata.setvalue = msg->Data[4]*256+msg->Data[5];
					}
					else if(msg->Data[3]==6)
					{
						allneedvalue.inchdata.minvalue = msg->Data[4]*256+msg->Data[5];
					}
					else if(msg->Data[3]==5)
					{
						allneedvalue.inchdata.maxvalue = msg->Data[4]*256+msg->Data[5];
					}
					else if(msg->Data[3]==7)
					{
						allneedvalue.inchdata.decinum = msg->Data[4];
					}
					else if(msg->Data[3]==4)
					{
						allneedvalue.inchdata.unitcode = msg->Data[4];
					}
				}
				return 1;
				///�洢��Щ��ȡ�����ݣ���
			}

		}
		else if(headmsg->fcbt != 0)
		{
			uint8_t ret = GetFragMent(msg,headmsg,0,fragment,&datalen);
			if(ret == 1)
			{
				uint16_t kk = fragment[0]*256 + fragment[1];
				if( reqatr.id == kk &&
					reqatr.stat == fragment[2] && reqatr.propert == fragment[3])
				{
					if(28740 == kk)
					{
						timefromsd1000 = fragment[4]<<24 | fragment[5]<<16 | fragment[6]<<8 |fragment[7];// could be used to set STM32 RTC time
						timefromsd1000 = timefromsd1000 - 410263200;
//						UpdateSetSDTime(timefromsd1000);
					}
					return 1;
				}
			}
			else
			{
				return 2;
			}

		}

	}
	return 0;
}


uint8_t AnswerWritAtr(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type)
{
	uint8_t errcode = 0;
	static uint8_t errnum = 0;
	if(type == 1)
	{
		errcode = msg->Data[4];
		if( GetErroCode(errcode)==1)
		{
			errnum = 0;
			return 1;
		}
		else if(errnum++<10)
			return 0;
		errnum = 0;
		return 1;
	}
	else if(headmsg->service == 1)
	{
		uint16_t kk = msg->Data[0]*256 + msg->Data[1];
		errnum = 0;
		if(kk == reqatr.id)
		{
			return 1;
		}
	}
	return 0;
}


static uint8_t IsSynergic(void)
{
	/*lint -e514*/
	if(0xff & (modeinfo.modedesp & 0x01) != 0)
		return 1;
	else
		return 0;
}



static void Get520Mask(uint8_t msak)
{
	_L_G.synergic_status = IsSynergic();
	_L_G.act520mask = (0x78&(msak>>1)) | (0x06&(msak<<1)) | 0x01;
	if(_L_G.synergic_status !=0 )
		_L_G.act520mask =_L_G.act520mask + 0x80;
}



uint8_t AnswerAction(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type)
{
	static uint8_t fragment[256];
	static uint8_t datalen;
	static uint8_t errnum = 0;

	uint8_t 	actiondata517[2];
	uint8_t 	errcode = 0;

	if(type == 1)
	{
		errcode = msg->Data[2];
		errnum++;
		if( GetErroCode(errcode)==1)
		{
			return 1;
		}
		else if(errnum <10)
			return 0;
		else
		{
			return 1;
		}
	}
	else if(headmsg->service == 4)
	{
		errnum = 0;
		if(headmsg->fcbt == 0)
		{
			uint16_t kk = msg->Data[0]*256 + msg->Data[1];
			if( _L_G.actionid == kk )
			{
				if(_L_G.actionid == 517)
				{
					actiondata517[0] = msg->Data[3];
					actiondata517[1] = msg->Data[4];
				}
				///�洢��Щ��ȡ�����ݣ���
				return 1;
			}
		}
		else if(headmsg->fcbt != 0)
		{
			uint8_t ret = GetFragMent(msg,headmsg,0,fragment,&datalen);
			if(ret == 1)
			{
				uint16_t kk = fragment[0]*256 + fragment[1];
				if( _L_G.actionid == kk )
				{
					if(kk == 10)
					{
						for(ret =0;ret<datalen-2 && ret <20;ret++)
						{
							allpeers[ret].peeradress = fragment[2+ret];
						}
						_L_G.peernum = datalen-2;
						if(_L_G.peernum>20)
							_L_G.peernum =20;
					}
					else if(kk == 519)
					{
						memcpy((uint8_t*)&allwavevalue[0],fragment+2,30);
					}
					else if(kk == 517)
					{
						actiondata517[0] = fragment[3];
						actiondata517[1] = fragment[4];
					}
					else if(kk == 515)	//����
					{
						memcpy(&action515data,fragment+2,23);
					}
					else if(kk == 513)
					{
						memcpy(modeinfo.modedescription,fragment+17,17);
						memcpy((uint8_t*)&allneedvalue.work_point.maxvalue,fragment+10,2);
						memcpy((uint8_t*)&allneedvalue.work_point.minvalue,fragment+8,2);
						allneedvalue.work_point.maxvalue = revs16(allneedvalue.work_point.maxvalue);
						allneedvalue.work_point.minvalue = revs16(allneedvalue.work_point.minvalue);
						if(allneedvalue.work_point.setvalue < allneedvalue.work_point.minvalue)
							allneedvalue.work_point.setvalue = allneedvalue.work_point.minvalue;
						else if(allneedvalue.work_point.setvalue > allneedvalue.work_point.maxvalue)
							allneedvalue.work_point.setvalue = allneedvalue.work_point.maxvalue;
						modeinfo.wiretype = fragment[14];
						modeinfo.wiresize = fragment[15];
						modeinfo.gastype = fragment[16];
						modeinfo.processtype = fragment[6]*256 + fragment[7];
					}
					else if(kk == 514)
					{
						uint32_t dat = fragment[2];
						modeinfo.modedesp = fragment[5];
						modeinfo.modedesp += dat<<24;
						dat = fragment[3];
						modeinfo.modedesp += dat<<16;
						dat = fragment[4];
						modeinfo.modedesp += dat<<8;

 						modeinfo.mask514 = fragment[9];
						dat = fragment[6];
						modeinfo.mask514 += dat<<24;
						dat = fragment[7];
						modeinfo.mask514 += dat<<16;
						dat = fragment[8];
						modeinfo.mask514 += dat<<8;

						Get520Mask(fragment[9]);
//						memcpy((uint8_t*)&act514data,fragment+2,28);
					}
					else if(kk == 512)
					{
						memcpy((uint8_t*)ALLMODES,fragment+2,datalen-2);
						Can_G_V.allmodenum = datalen/2-1;
					}

					///�洢��Щ��ȡ�����ݣ���
					return 1;
				}
				else
					return 0;
			}
			else
				return 2;
		}
	}
	return 0;

}

static uint8_t AnswerAction520(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type)
{
	static uint8_t fragment[20];
	static uint8_t datalen;
	static uint8_t errnum = 0;
	uint8_t errcode = 0;
	if(type == 1)
	{
		errcode = msg->Data[2];
		errnum++;
		if( GetErroCode(errcode)==1)
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
	else if(headmsg->service == 4)
	{
		errnum = 0;
		if(headmsg->fcbt == 0)
		{
			uint16_t kk = msg->Data[0]*256 + msg->Data[1];
			if( kk == 520 )
			{
				_L_G.synergic_speed = fragment[2]*256 + fragment[3];
				/*lint -e514*/
				if(fragment[4]& 0x7f != 0)
				{
					 Can_G_V.errcode_show = fragment[5];
				}
				else
					 Can_G_V.errcode_show = 0;
				return 1;
			}
		}
		else if(headmsg->fcbt != 0)
		{
			uint8_t ret = GetFragMent(msg,headmsg,0,fragment,&datalen);
			if(ret == 1)
			{
				uint16_t kk = fragment[0]*256 + fragment[1];
				if( 520 == kk )
				{
					_L_G.synergic_speed = fragment[2]*256 + fragment[3];
					/*lint -e514*/
					if(fragment[4]& 0x7f != 0)
					{
						 Can_G_V.errcode_show = fragment[5];
					}
					else
						 Can_G_V.errcode_show = 0;
					return 1;
				}
			}
			return 2;
		}
	}
	return 0;

}


uint8_t Get520MaskData(int16_t work1,int16_t trim1)
{
	Can_G_V.workpoint_real_520 = work1;
	Can_G_V.trimvalue_real_520 = trim1;

	if((_L_G.act520mask & 0x02) != 0)
	{
		Act520Data[5] = work1/256;//
		Act520Data[6] = work1%256;   //
	}


	if((_L_G.act520mask & 0x04) != 0)
	{
		Act520Data[7] = trim1/256;//
		Act520Data[8] = trim1%256;   //
	}

	return 0;
}

uint8_t SendWriteAtr(AttributeType* atr,uint8_t* data1, uint8_t len,uint8_t adress,uint8_t mcb)//peer to peer;
{
	HeadMessage	headmessage;
	uint8_t data[255];
	uint8_t locIndex1_;
	headmessage.rre = 0;
	headmessage.conid = 15;
	headmessage.mcb = mcb;
	headmessage.fcbt = 0;
	headmessage.fcbc =0;
	headmessage.service = 1;
	headmessage.gr = 0;
	headmessage.di = 1;
	headmessage.ds = 1;
	headmessage.adr = _L_G.Peer;
	data[0] = atr->id/256;
	data[1] = atr->id%256;
	data[2] = atr->stat;
	data[3] = atr->propert;
	for(locIndex1_=0;locIndex1_<len;locIndex1_++)
	{
		data[locIndex1_+4] = data1[locIndex1_];
	}
	return SendData_Can(&headmessage,data,len+4,1,adress);
}

static uint8_t Act520Process(int16_t work1,int16_t trim1)
{
	uint8_t data[5];
	HeadMessage	headmessage;
	uint8_t reconnectnum = 0;
	uint8_t mappingsteps = 0xff;
	const uint8_t mcb = 2;
	rettype[mcb] = 1;
	do{
		if(_L_G.ismapped != 1)
			return 0;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
			case 0:
			{
				headmessage.rre = 0;
				headmessage.conid = 2;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				Get520MaskData(work1,trim1);
				SendData_Can(&headmessage,Act520Data,_L_G.act520len,1,1);//adrress = 9;
				_L_G.Process = AnswerAction520;
				break;

			}
			case 1:
			{
				if(_L_G.synergic_status != 0)
				{
					mappingsteps = mappingsteps+1;
					break;
				}
				reqatr.id = 513;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = _L_G.synergic_speed/256;;
				data[1] = _L_G.synergic_speed%256;
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<=1 && _L_G.ismapped==1);
	return 1;
}

static void ChangeCurrVoltData(void)
{
	if(Can_G_V.hanjiestatus == 80)
		Act520Process(allneedvalue.work_point.setvalue,allneedvalue.trim_value.setvalue) ;
}




void ChangeWorkpoint(void)
{
	uint8_t data[20];
	uint8_t mappingsteps = 0xff;
	HeadMessage	headmessage;
	uint8_t reconnectnum = 0;
	const uint8_t mcb = 0;
	rettype[mcb] = 1;

	if(allneedvalue.work_point.setvalue > allneedvalue.work_point.maxvalue)
		allneedvalue.work_point.setvalue = allneedvalue.work_point.maxvalue;
	else if(allneedvalue.work_point.setvalue < allneedvalue.work_point.minvalue)
		allneedvalue.work_point.setvalue = allneedvalue.work_point.minvalue;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;

		switch(mappingsteps)
		{
		case 0:
			{
				_L_G.actionid = 519;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;
				data[2] = allneedvalue.work_point.setvalue/256;   //10 01 00 00 00   00 23 00
				data[3] = allneedvalue.work_point.setvalue%256;
				data[4] = modeinfo.mode_set/256;
				data[5] = modeinfo.mode_set%256;

				SendData_Can(&headmessage,data,6,1,1);//adrress = 9;
				_L_G.Process = AnswerAction;
				delay_ms(10);
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<1 && _L_G.ismapped==1);


	allneedvalue.trim_value.maxvalue	= revs16(allwavevalue[0].maxvalue);
	allneedvalue.trim_value.minvalue	= revs16(allwavevalue[0].minvalue);
	if(allneedvalue.trim_value.setvalue < allneedvalue.trim_value.minvalue || allneedvalue.trim_value.setvalue > allneedvalue.trim_value.maxvalue)
		allneedvalue.trim_value.setvalue	= revs16(allwavevalue[0].normvalue);

	allneedvalue.wave1.maxvalue	= revs16(allwavevalue[1].maxvalue);
	allneedvalue.wave1.minvalue	= revs16(allwavevalue[1].minvalue);
	if(allneedvalue.wave1.setvalue < allneedvalue.wave1.minvalue || allneedvalue.wave1.setvalue > allneedvalue.wave1.maxvalue)
		allneedvalue.wave1.setvalue	= revs16(allwavevalue[1].normvalue);


	allneedvalue.wave2.maxvalue	= revs16(allwavevalue[2].maxvalue);
	allneedvalue.wave2.minvalue	= revs16(allwavevalue[2].minvalue);
	if(allneedvalue.wave2.setvalue < allneedvalue.wave2.minvalue || allneedvalue.wave2.setvalue > allneedvalue.wave2.maxvalue)
		allneedvalue.wave2.setvalue	= revs16(allwavevalue[2].normvalue);

	allneedvalue.wave3.maxvalue	= revs16(allwavevalue[3].maxvalue);
	allneedvalue.wave3.minvalue	= revs16(allwavevalue[3].minvalue);
	if(allneedvalue.wave3.setvalue < allneedvalue.wave3.minvalue || allneedvalue.wave3.setvalue > allneedvalue.wave3.maxvalue)
		allneedvalue.wave3.setvalue	= revs16(allwavevalue[3].normvalue);

	allneedvalue.wave4.maxvalue	= revs16(allwavevalue[4].maxvalue);
	allneedvalue.wave4.minvalue	= revs16(allwavevalue[4].minvalue);
	if(allneedvalue.wave4.setvalue < allneedvalue.wave4.minvalue || allneedvalue.wave4.setvalue > allneedvalue.wave4.maxvalue)
		allneedvalue.wave4.setvalue	= revs16(allwavevalue[4].normvalue);
}



void WeldingConfiguration(uint16_t mode_num)
{
	uint8_t 	data[10];
	uint16_t 	mode_old = (uint16_t)modeinfo.mode_set;
	HeadMessage	headmessage;
	uint8_t 	reconnectnum = 0;
	uint8_t 	mappingsteps = 0xff;
	const uint8_t mcb = 0;


	modeinfo.mode_set = mode_num;
	if(modeinfo.mode_set == 0)
	{
		modeinfo.mode_set = mode_old;
		return;
	}
	rettype[mcb] = 1;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
		case 0:
			{
				_L_G.actionid = 513;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				headmessage.mcb = mcb;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;
				data[2] = modeinfo.mode_set/256;
				data[3] = modeinfo.mode_set%256;//action 10;

				SendData_Can(&headmessage,data,4,1,1);//adrress = 9;
				_L_G.Process = AnswerAction;
				delay_ms(10);
				break;
			}
		case 1:
			{
				_L_G.actionid = 514;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				headmessage.mcb = mcb;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;
				data[2] = modeinfo.mode_set/256;
				data[3] = modeinfo.mode_set%256;//action 10;

				SendData_Can(&headmessage,data,4,1,1);//adrress = 9;
				_L_G.Process = AnswerAction;
				delay_ms(10);
				break;
			}
		case 2:
			{
				if(0xff & (modeinfo.mask514 & 0x01) ==0)
				{
					rettype[mcb] = 1;
				}
				else
				{
					_L_G.actionid = 515;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 517/256;
					data[3] = 517%256;//action 10;
					data[4] = modeinfo.mode_set/256;
					data[5] = modeinfo.mode_set%256;//action 10;

					SendData_Can(&headmessage,data,6,1,1);//adrress = 9;
					_L_G.Process = AnswerAction;
					delay_ms(10);
				}
				break;
			}
		case 3:
			{
				if(0xff & (modeinfo.mask514 & 0x01) !=0)
				{
					allneedvalue.work_point.maxvalue =  revs16(action515data.max_value) ;
					allneedvalue.work_point.minvalue =  revs16(action515data.min_value) ;
					allneedvalue.work_point.decinum =  revs16(action515data.unit_code)	 ;
					allneedvalue.work_point.unitcode =  revs16(action515data.deci_num)	 ;
					memcpy(allneedvalue.work_point.name, action515data.name,17) ;
				}
				if(0xff & (modeinfo.mask514 & 0x02) ==0)
				{
					rettype[mcb] = 1;
				}
				else
				{
					_L_G.actionid = 515;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 518/256;
					data[3] = 518%256;//action 10;
					data[4] = modeinfo.mode_set/256;
					data[5] = modeinfo.mode_set%256;//action 10;

					SendData_Can(&headmessage,data,6,1,1);//adrress = 1;
					_L_G.Process = AnswerAction;
					delay_ms(10);
				}
				break;
			}
		case 4:
			{
				if(0xff & (modeinfo.mask514 & 0x02) !=0)
				{
					allneedvalue.trim_value.maxvalue =  revs16(action515data.max_value) ;
					allneedvalue.trim_value.minvalue =  revs16(action515data.min_value) ;
					allneedvalue.trim_value.decinum =  revs16(action515data.unit_code)	 ;
					allneedvalue.trim_value.unitcode =  revs16(action515data.deci_num)	 ;
					memcpy(allneedvalue.trim_value.name, action515data.name,17) ;
				}
				if(0xff & (modeinfo.mask514 & 0x10) ==0)
				{
					rettype[mcb] = 1;
				}
				else
				{
					_L_G.actionid = 515;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 519/256;
					data[3] = 519%256;//action 10;
					data[4] = modeinfo.mode_set/256;
					data[5] = modeinfo.mode_set%256;//action 10;

					SendData_Can(&headmessage,data,6,1,1);//adrress = 1;
					_L_G.Process = AnswerAction;
					delay_ms(10);
				}
				break;
			}
		case 5:
			{
				if(0xff & (modeinfo.mask514 & 0x10) !=0)
				{
					allneedvalue.wave1.maxvalue =  revs16(action515data.max_value) ;
					allneedvalue.wave1.minvalue =  revs16(action515data.min_value) ;
					allneedvalue.wave1.decinum =  revs16(action515data.unit_code)	 ;
					allneedvalue.wave1.unitcode =  revs16(action515data.deci_num)	 ;
					memcpy(allneedvalue.wave1.name, action515data.name,17) ;
				}
				if(0xff & (modeinfo.mask514 & 0x20) ==0)
				{
					rettype[mcb] = 1;
				}
				else
				{
					_L_G.actionid = 515;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 520/256;
					data[3] = 520%256;//action 10;
					data[4] = modeinfo.mode_set/256;
					data[5] = modeinfo.mode_set%256;//action 10;

					SendData_Can(&headmessage,data,6,1,1);//adrress = 1;
					_L_G.Process = AnswerAction;
					delay_ms(10);
				}
				break;
			}
		case 6:
			{
				if(0xff & (modeinfo.mask514 & 0x20) !=0)
				{
					allneedvalue.wave2.maxvalue =  revs16(action515data.max_value) ;
					allneedvalue.wave2.minvalue =  revs16(action515data.min_value) ;
					allneedvalue.wave2.decinum =  revs16(action515data.unit_code)	 ;
					allneedvalue.wave2.unitcode =  revs16(action515data.deci_num)	 ;
					memcpy(allneedvalue.wave2.name, action515data.name,17) ;
				}
				if(0xff & (modeinfo.mask514 & 0x40) ==0)
				{
					rettype[mcb] = 1;
				}
				else
				{
					_L_G.actionid = 515;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 521/256;
					data[3] = 521%256;//action 10;
					data[4] = modeinfo.mode_set/256;
					data[5] = modeinfo.mode_set%256;//action 10;

					SendData_Can(&headmessage,data,6,1,1);//adrress = 1;
					_L_G.Process = AnswerAction;
					delay_ms(10);
				}
				break;
			}
		case 7:
			{
				if(0xff & (modeinfo.mask514 & 0x40) !=0)
				{
					allneedvalue.wave3.maxvalue =  revs16(action515data.max_value) ;
					allneedvalue.wave3.minvalue =  revs16(action515data.min_value) ;
					allneedvalue.wave3.decinum =  revs16(action515data.unit_code)	 ;
					allneedvalue.wave3.unitcode =  revs16(action515data.deci_num)	 ;
					memcpy(allneedvalue.wave3.name, action515data.name,17) ;
				}
				if(0xff & (modeinfo.mask514 & 0x80) ==0)
				{
					rettype[mcb] = 1;
				}
				else
				{
					_L_G.actionid = 515;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 522/256;
					data[3] = 522%256;//action 10;
					data[4] = modeinfo.mode_set/256;
					data[5] = modeinfo.mode_set%256;//action 10;

					SendData_Can(&headmessage,data,6,1,1);//adrress = 1;
					_L_G.Process = AnswerAction;
					delay_ms(10);
				}
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<=7 && _L_G.ismapped==1);
	if(0xff & (modeinfo.mask514 & 0x80) !=0)
	{
		allneedvalue.wave4.maxvalue =  revs16(action515data.max_value) ;
		allneedvalue.wave4.minvalue =  revs16(action515data.min_value) ;
		allneedvalue.wave4.decinum =  revs16(action515data.unit_code)	 ;
		allneedvalue.wave4.unitcode =  revs16(action515data.deci_num)	 ;
		memcpy(allneedvalue.wave4.name, action515data.name,17) ;
	}
	ChangeWorkpoint();
	RenewHanjieParaSRV();
}


uint8_t SendReadAtr(AttributeType* atr,uint8_t adress,uint8_t mcb)//peer to peer;
{
	HeadMessage	headmessage;
	uint8_t data[10];
	headmessage.rre = 0;
	headmessage.conid = 15;
	headmessage.fcbt = 0;
	headmessage.fcbc =0;
	headmessage.service = 0;
	headmessage.gr = 0;
	headmessage.di = 1;
	headmessage.ds = 1;
	headmessage.adr = _L_G.Peer;
	headmessage.mcb = mcb;
	data[0] = atr->id/256;
	data[1] = atr->id%256;
	data[2] = atr->stat;
	data[3] = atr->propert;
	return SendData_Can(&headmessage,data,4,1,adress);
}


static void CreateReportEvent(void)
{
	uint8_t 	data[10];
	HeadMessage	headmessage;
	uint8_t 	reconnectnum = 0;
	uint8_t 	mappingsteps = 0xff;
	const uint8_t mcb = 0;


	rettype[mcb] = 1;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
		case 0:
			{
				_L_G.actionid = 11;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;

				data[2] = 14/256;
				data[3] = 14%256;//atr id ;
				data[4] = 0x00;
				data[5] = 0x00;//action 10;
				data[6] = 0x60;
				data[7] = 0x00;//atr id ;

				SendData_Can(&headmessage,data,8,1,0);//adrress = 9;
				_L_G.Process = AnswerAction;
				break;
			}
		case 1:
			{
				_L_G.actionid = 11;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;

				data[2] = 14/256;
				data[3] = 14%256;//atr id ;
				data[4] = 0x00;
				data[5] = 0x00;//action 10;
				data[6] = 0x60;
				data[7] = 0x00;//atr id ;

				SendData_Can(&headmessage,data,8,1,GetPeerByClass(0x02));//adrress = 9;
				_L_G.Process = AnswerAction;
				break;
			}
		case 2:
			{
				_L_G.actionid = 11;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;

				data[2] = 13/256;
				data[3] = 13%256;//atr id ;
				data[4] = 0x00;
				data[5] = 0x00;//action 10;
				data[6] = 0x60;
				data[7] = 0x00;//atr id ;

				SendData_Can(&headmessage,data,8,1,GetPeerByClass(0x02));//adrress = 9;
				_L_G.Process = AnswerAction;
				break;
			}
		case 3:
			{
				_L_G.actionid = 11;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;

				data[2] = 13/256;
				data[3] = 13%256;//atr id ;
				data[4] = 0x00;
				data[5] = 0x00;//action 10;
				data[6] = 0x60;
				data[7] = 0x00;//atr id ;

				SendData_Can(&headmessage,data,8,1,0);//adrress = 9;
				_L_G.Process = AnswerAction;
				break;
			}
		case 4:
			{
				reqatr.id = 512;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x05),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 5:
			{
				reqatr.id = 30770;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;
				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 6:
			{
				reqatr.id = 30781;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x0A;
				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 7:
			{
				reqatr.id = 30786;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x0A;
				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				mappingsteps++;
				break;
			}
		case 8:
			{
				reqatr.id = 30758;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;
				data[1] = 0x90;
				SendWriteAtr(&reqatr,data,2,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 9:
			{
				reqatr.id = 30781;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x0A;
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 10:
			{
				reqatr.id = 30;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x00;
				data[1] = 0x14;
				SendWriteAtr(&reqatr,data,2,0,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 11:
			{
				reqatr.id = 28740;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				_L_G.Process = (AnswerReadAtr) ;
				SendReadAtr(&reqatr,0,0);
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<=11 && _L_G.ismapped==1);
}


static uint8_t CheckSongSi(void)
{
	uint8_t reconnectnum =0;
	uint8_t data[4];
	const uint8_t mcb = 6;
	uint8_t mappingsteps = 0xff;
	rettype[mcb] = 1;
	if((_L_G.block514respond & 0x0002)== 0x0002)
	{
		do{
			if(_L_G.ismapped != 1)
				return 0;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;
			reconnectnum=0;
			rettype[mcb] = 0;
			switch(mappingsteps)
			{
			case 0:
				reqatr.id = 545;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x00;

				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			case 1:
				reqatr.id = 536;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x00;
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			default:
				break;
			}
			delay_ms(10);
		}while(mappingsteps<2 && _L_G.ismapped==1);
		return 2;
	}
	return 1;
}

static void InchUP(uint8_t dat)
{
	uint8_t data[3];
	uint8_t reconnectnum = 0;
	uint8_t mappingsteps = 0xff;
	const uint8_t mcb = 7;
	rettype[mcb] = 1;
	if(dat == 1)
	{
		mappingsteps = 0xff;
		do{
			if(_L_G.ismapped != 1)
				return;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;

			rettype[mcb] = 0;
			reconnectnum=0;
			switch(mappingsteps)
			{
			case 0:
				{
					reqatr.id = 527;
					reqatr.stat = 0;
					reqatr.propert = 0;//value
					data[0] = allneedvalue.inchdata.setvalue/256;
					data[1] = allneedvalue.inchdata.setvalue%256;
					SendWriteAtr(&reqatr,data,2,GetPeerByClass(0x02),mcb);
					_L_G.Process = (AnswerWritAtr) ;
					break;
				}
			case 1:
				{
					reqatr.id = 537;
					reqatr.stat = 0;
					reqatr.propert = 0;//value
					data[0] = 0x01;

					SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
					_L_G.Process = (AnswerWritAtr) ;
					break;
				}
			default:
				break;
			}
			delay_ms(DELAY_MILISEC);
		}while(mappingsteps<=1 && _L_G.ismapped==1);
	}
	else
	{
		mappingsteps = 0xff;
		rettype[mcb] = 1;
		do{
			if(_L_G.ismapped != 1)
				return;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;
			reconnectnum=0;
			rettype[mcb] = 0;
			switch(mappingsteps)
			{
			case 0:
				reqatr.id = 537;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x00;

				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			default:
				break;
			}
			delay_ms(DELAY_MILISEC);
		}while(mappingsteps<1 && _L_G.ismapped==1);
	}
}

void InchDown(uint8_t dat)
{
	uint8_t data[3];
	uint8_t reconnectnum = 0;
	const uint8_t mcb = 5;
	uint8_t mappingsteps = 0xff;
	rettype[mcb] = 1;
	if(dat == 1)
	{
		mappingsteps = 0xff;
		do{
			if(_L_G.ismapped != 1)
				return;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;
			rettype[mcb] = 0;
			reconnectnum=0;
			switch(mappingsteps)
			{
			case 0:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = allneedvalue.inchdata.setvalue/256;
				data[1] = allneedvalue.inchdata.setvalue%256;
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
			case 1:
			{
				reqatr.id = 536;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;

				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
			case 2:
			{
				reqatr.id = 545;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;

				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
			}
			delay_ms(DELAY_MILISEC);
		}while(mappingsteps<3 && _L_G.ismapped==1);
	}
	else
	{
		mappingsteps = 0xff;
		do{
			if(_L_G.ismapped != 1)
				return;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;
			reconnectnum=0;
			rettype[mcb] = 0;
			switch(mappingsteps)
			{
			case 0:
				reqatr.id = 536;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x00;

				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			case 1:
				reqatr.id = 545;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x00;

				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
			delay_ms(DELAY_MILISEC);
		}while(mappingsteps<2 && _L_G.ismapped==1);
	}
}

void GetFirst520String(void)
{
	Act520Data[0] = 520/256;
	Act520Data[1] = 520%256;
	Act520Data[2] = _L_G.act520mask;
	Act520Data[3] = modeinfo.mode_set/256;//action 10;
	Act520Data[4] = modeinfo.mode_set%256;   //10 01 00 00 00   00 23 00
	_L_G.act520len = 5;
	Can_G_V.workpoint_real_520 = allneedvalue.work_point.setvalue;
	Can_G_V.trimvalue_real_520 = allneedvalue.trim_value.setvalue;

	if((_L_G.act520mask & 0x02) != 0)
	{
		Act520Data[_L_G.act520len++] = allneedvalue.work_point.setvalue/256;//
		Act520Data[_L_G.act520len++] = allneedvalue.work_point.setvalue%256;   //
	}


	if((_L_G.act520mask & 0x04) != 0)
	{
		Act520Data[_L_G.act520len++] = allneedvalue.trim_value.setvalue/256;//
		Act520Data[_L_G.act520len++] = allneedvalue.trim_value.setvalue%256;   //
	}


	if((_L_G.act520mask & 0x08) != 0)
	{
		Act520Data[_L_G.act520len++] = allneedvalue.wave1.setvalue/256;
		Act520Data[_L_G.act520len++] = allneedvalue.wave1.setvalue%256;
	}


	if((_L_G.act520mask & 0x10) != 0)
	{
		Act520Data[_L_G.act520len++] = allneedvalue.wave2.setvalue/256;//action 10;
		Act520Data[_L_G.act520len++] = allneedvalue.wave2.setvalue%256;   //10 01 00 00 00   00 23 00
	}


	if((_L_G.act520mask & 0x20) != 0)
	{
		Act520Data[_L_G.act520len++] = allneedvalue.wave3.setvalue/256;
		Act520Data[_L_G.act520len++] = allneedvalue.wave3.setvalue%256;
	}

	if((_L_G.act520mask & 0x40) != 0)
	{
		Act520Data[_L_G.act520len++] = allneedvalue.wave4.setvalue/256;
		Act520Data[_L_G.act520len++] = allneedvalue.wave4.setvalue%256;
	}
}





static void ProcessInchValue(void)
{
	uint8_t 	reconnectnum = 0;
	uint8_t 	mappingsteps = 0xff;
	const uint8_t mcb = 0;

	rettype[mcb]= 1;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
			case 0:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				_L_G.Process = (AnswerReadAtr) ;
				SendReadAtr(&reqatr,GetPeerByClass(0x02),mcb);
			}
			break;
			case 1:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 4;//value
				_L_G.Process = (AnswerReadAtr) ;
				SendReadAtr(&reqatr,GetPeerByClass(0x02),mcb);
			}
			break;
			case 2:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 5;//value
				_L_G.Process = (AnswerReadAtr) ;
				SendReadAtr(&reqatr,GetPeerByClass(0x02),mcb);
			}
			break;
			case 3:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 6;//value
				_L_G.Process = (AnswerReadAtr) ;
				SendReadAtr(&reqatr,GetPeerByClass(0x02),mcb);
			}
			break;
			case 4:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 7;//value
				_L_G.Process = (AnswerReadAtr) ;
				SendReadAtr(&reqatr,GetPeerByClass(0x02),mcb);
			}
			break;

		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<=4 && _L_G.ismapped==1);
}



static void GetAllPeersInfo(void)
{
	uint8_t data[15];
	uint8_t mappingsteps = 0xff;
	HeadMessage	headmessage;
	uint8_t reconnectnum = 0;
	const uint8_t mcb = 0;
	rettype[mcb] = 1;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
		case 0:
			{
				reqatr.id = 34;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				_L_G.atrid34value = 0;
				SendReadAtr(&reqatr,0,0);
				_L_G.Process = (AnswerReadAtr) ;
			}
			break;
		case 1:
			{
				if(_L_G.atrid34value != 0x01)
				{
					rettype[mcb] = 0;
					reconnectnum = 5;
					mappingsteps = 0;
				}
				else
				{
					_L_G.actionid = 10;
					headmessage.rre = 0;
					headmessage.conid = 15;
					headmessage.mcb = mcb;
					headmessage.fcbt = 0;
					headmessage.fcbc =0;
					headmessage.service = 4;
					headmessage.gr = 0;
					headmessage.di = 1;
					headmessage.ds = 1;
					headmessage.adr = _L_G.Peer;
					data[0] = _L_G.actionid/256;
					data[1] = _L_G.actionid%256;//action 10;
					data[2] = 0x00;   //10 01 00 00 00   00 23 00
					data[3] = 0x01;
					data[4] = 0x00;
					data[5] = 0x00;
					data[6] = 0x00;

					data[7] = 0x22;
					data[8] = 0x40;
					data[9] = 0x00;
					SendData_Can(&headmessage,data,10,1,0);//adrress = 0;
					_L_G.Process = AnswerAction;
				}
				break;
			}
		case 2:
			GetAllPeerInfo();
			break;
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<=2 && _L_G.ismapped==1);
}

void GetAllPeerInfo(void)
{
	uint8_t locIndex1_;
	uint8_t recon = 0;
	uint8_t idll = 0;
	_L_G.Process = AnswerReadAtr_Peer;
	locIndex1_ = _L_G.peernum;


	for(locIndex1_ =0;locIndex1_ <_L_G.peernum;locIndex1_++)
	{
		if(allpeers[locIndex1_].peeradress != _L_G.Peer)	  //
		{
			recon = 0;
			idll = 0;
			_L_G.answer_ret = 0;
			while(_L_G.answer_ret != 1 && idll <15)
			{
				if(recon==0)
				{
					reqatr.id = 4;
					reqatr.stat = 0;
					reqatr.propert = 0;
					SendReadAtr(&reqatr,allpeers[locIndex1_].peeradress,0);//peer to peer;
				}
				recon++;
				delay_ms(DELAY_MILISEC);
				if(recon== 4)
				{
					idll++;
					recon = 0;
				}

			}
		}
	}

	for(locIndex1_ =0;locIndex1_ <_L_G.peernum;locIndex1_++)
	{
		if(allpeers[locIndex1_].peeradress != _L_G.Peer)	  //
		{
			recon = 0;
			idll = 0;
			_L_G.answer_ret = 0;
			while(_L_G.answer_ret != 1 && idll <5)
			{
				if(recon==0)
				{
					reqatr.id = 11;
					reqatr.stat = 0;
					reqatr.propert = 0;
					SendReadAtr(&reqatr,allpeers[locIndex1_].peeradress,0);//peer to peer;
				}
				recon++;
				delay_ms(DELAY_MILISEC);
				if(recon== 5)
				{
					idll++;
					recon = 0;
				}

			}
		}
	}
	for(locIndex1_ =0;locIndex1_ <_L_G.peernum;locIndex1_++)
	{
		if(allpeers[locIndex1_].peeradress != _L_G.Peer)	  //
		{
			recon = 0;
			idll = 0;
			_L_G.answer_ret = 0;
			while(_L_G.answer_ret != 1 && idll <5)
			{
				if(recon==0)
				{
					reqatr.id = 10;
					reqatr.stat = 0;
					reqatr.propert = 0;
					SendReadAtr(&reqatr,allpeers[locIndex1_].peeradress,0);//peer to peer;
				}
				recon++;
				delay_ms(DELAY_MILISEC);
				if(recon== 5)
				{
					idll++;
					recon = 0;
				}

			}
		}
	}
	rettype[0] = 1;
}


static void GetAllModes(void)
{
	uint8_t data[20];
	HeadMessage	headmessage;
	uint8_t reconnectnum = 0;
	uint8_t mappingsteps = 0xff;
	const uint8_t mcb = 0;
	rettype[mcb] = 1;

	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;

		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
		case 0:
			{

				_L_G.actionid = 512;
				headmessage.rre = 0;
				headmessage.conid = 15;
				headmessage.mcb = mcb;
				headmessage.fcbt = 0;
				headmessage.fcbc =0;
				headmessage.service = 4;
				headmessage.gr = 0;
				headmessage.di = 1;
				headmessage.ds = 1;
				headmessage.adr = _L_G.Peer;
				data[0] = _L_G.actionid/256;
				data[1] = _L_G.actionid%256;//action 10;
				data[2] = 0x00;   //10 01 00 00 00   00 23 00
				data[3] = 0x00;
				data[4] = 0x00;
				data[5] = 0x00;
				data[6] = 0x00;

				data[7] = 0x02;
				data[8] = 0x00;
				data[9] = 0x80;
				data[10] = 0x00;
				data[11] = 0x00;
				data[12] = 0x7E;
				data[13] = 0xD3;

				data[14] = 0xD4;
				data[15] = 0xD4;
				data[16] = 0xD4;
				data[17] = 0x00;
				SendData_Can(&headmessage,data,18,1,1);//adrress = 9;
				_L_G.Process = AnswerAction;
				delay_ms(10);
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<1 && _L_G.ismapped==1);
}


static void SetPhaseGenerator(void)
{
	uint8_t 	data[5];
	uint8_t 	reconnectnum = 0;
	uint8_t 	mappingsteps = 0xff;
	const uint8_t mcb = 0;
	rettype[mcb] = 1;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		reconnectnum=0;
		rettype[mcb] = 0;
		switch(mappingsteps)
		{
		case 0:
			{
				reqatr.id = 517;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = modeinfo.frequnce/256;	   //ģʽ
				data[1] = modeinfo.frequnce%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 1:
			{
				reqatr.id = 516;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 1;	   //ģʽ
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 2:
			{
				reqatr.id = 513;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 90/256;	   //ģʽ
				data[1] = 90%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 3:
			{
				reqatr.id = 514;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 180/256;	   //ģʽ
				data[1] = 180%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 4:
			{
				reqatr.id = 515;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 270/256;	   //ģʽ
				data[1] = 270%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 5:
			{
				reqatr.id = 530;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0/256;	   //ģʽ
				data[1] = 0%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 6:
			{
				reqatr.id = 533;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 90/256;	   //ģʽ
				data[1] = 90%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 7:
			{
				reqatr.id = 521;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 50;	   //ģʽ
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 8:
			{
				reqatr.id = 522;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 50;	   //ģʽ
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 9:
			{
				reqatr.id = 523;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 50;	   //ģʽ
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 10:
			{
				reqatr.id = 532;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 50;	   //ģʽ
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 11:
			{
				reqatr.id = 535;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 50;	   //ģʽ
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(11),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<=11  && _L_G.ismapped==1);
}







void ChangeSpeed(void)
{
	uint8_t data[5];
	uint8_t reconnectnum = 0;
	uint8_t mappingsteps = 0xff;
	const uint8_t mcb = 4;
	rettype[mcb] = 1;
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		reconnectnum=0;
		rettype[mcb] = 0;
		switch(mappingsteps)
		{
		case 0:
			{
				reqatr.id = 527;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = allneedvalue.inchdata.setvalue/256;	   //ģʽ
				data[1] = allneedvalue.inchdata.setvalue%256;	   //ģʽ
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<1  && _L_G.ismapped==1);
}


int16_t GetDuringNum(int16_t max1,int16_t min1,uint8_t step1,uint8_t stepnum)
{
	int16_t ll =  stepnum;
	ll = (max1-min1)/stepnum;
	ll*=step1;
	ll+=min1;
	return ll;
}



static void DownSlopeWeld(void)
{
	uint8_t 	maxsteps = hanjiepara.downslopetime/25;
	uint8_t 	mappingsteps = 0;

	if(Can_G_V.hanjiestatus<=10 || maxsteps<1)
	{
		Can_G_V.hanjiestatus = 100;
		return;//no curr;
	}
	Can_G_V.hanjiestatus = 90;
	do{
		if(_L_G.ismapped != 1)
			return;
		Act520Process(GetDuringNum(hanjiepara.craterworkpoint,allneedvalue.work_point.setvalue,mappingsteps,maxsteps),
				GetDuringNum(hanjiepara.cratertrim,allneedvalue.trim_value.setvalue,mappingsteps,maxsteps));
		mappingsteps=mappingsteps+1;
		delay_ms(20);
	}while(mappingsteps<maxsteps && _L_G.ismapped==1);
	Act520Process(hanjiepara.craterworkpoint,hanjiepara.cratertrim);
	delay_ms(hanjiepara.cratertime);
	Can_G_V.hanjiestatus = 100;
}



static void UpSlopeWeld(void)
{
	uint8_t 	maxsteps = hanjiepara.upslopetime/25;
	uint8_t 	mappingsteps = 0;

	if((_L_G.block514respond & 0x0002) != 0x0002)//Curr detected;
	{
		return ;
	}
	if(maxsteps<1)
	{
		Can_G_V.hanjiestatus = 80;
		return;//no curr;
	}
	Can_G_V.hanjiestatus = 20;
	Act520Process(hanjiepara.startworkpoint,hanjiepara.starttrim);
	delay_ms(hanjiepara.starttime);
	Can_G_V.hanjiestatus = 40;
	do{
		Act520Process(GetDuringNum(allneedvalue.work_point.setvalue,hanjiepara.startworkpoint ,mappingsteps,maxsteps),GetDuringNum(allneedvalue.trim_value.setvalue,hanjiepara.starttrim ,mappingsteps,maxsteps));
		mappingsteps=mappingsteps+1;
		delay_ms(20);
	}while(mappingsteps<maxsteps && _L_G.ismapped==1);
	Can_G_V.hanjiestatus = 80;
}



static void StartWeld(void)
{
	uint8_t 	data[4];
	uint8_t 	reconnectnum = 0;
	const uint8_t mcb = 1;
	uint8_t 	mappingsteps = 0xff;

	rettype[mcb] = 1;
	GetFirst520String();
	do{
		if(_L_G.ismapped != 1)
			return;
		while (rettype[mcb] == 2 && _L_G.ismapped==1)
		{
			delay_ms(DELAY_MILISEC);
		}
		while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
		{
			delay_ms(DELAY_MILISEC);
			reconnectnum++;
		}
		if(rettype[mcb] == 1)
			mappingsteps++;
		rettype[mcb] = 0;
		reconnectnum=0;
		switch(mappingsteps)
		{
		case 0:
			{
				rettype[mcb] = Act520Process(hanjiepara.strikeworkpoint,hanjiepara.striketrim);
				break;
			}
		case 1:
			{
				reqatr.id = 513;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = hanjiepara.strikespeed/256;
				data[1] = hanjiepara.strikespeed%256;
				SendWriteAtr(&reqatr,data,2,GetPeerByClass(0x02),mcb);
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		case 2:
			{
				reqatr.id = 512;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;
				SendWriteAtr(&reqatr,data,1,1,mcb);
				_L_G.Process = (AnswerWritAtr) ;
				Can_G_V.hanjiestatus = 1;
				break;
			}
		case 3:
			{
				reqatr.id = 512;
				reqatr.stat = 0;
				reqatr.propert = 0;//value
				data[0] = 0x01;
				SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
				Can_G_V.hanjiestatus = 10;
				_L_G.Process = (AnswerWritAtr) ;
				break;
			}
		}
		delay_ms(DELAY_MILISEC);
	}while(mappingsteps<4 && _L_G.ismapped==1);
}




static void ProcessCommand(void)
{
	GetAllPeersInfo();	 //mcb = 0;
	GetAllModes();	   //�õ����е�ģʽ,mcb = 0
	ProcessInchValue();	//�õ�Cold Inch ������// mcb = 0
	WeldingConfiguration((uint16_t)modeinfo.mode_set);	 //�õ�ģʽ����	 // mcb = 1;
	CreateReportEvent();   // ����report ����//mcb = 0;
	//SetRealTime();
	SetPhaseGenerator();
}


static void EndWeld(void)
{
	uint8_t data[3];
	uint8_t reconnectnum = 0;
	const uint8_t mcb = 3;
	uint8_t mappingsteps = 0xff;
	rettype[mcb] = 1;
	if(Can_G_V.hanjiestatus <= 10)
	{
		mappingsteps = 0xff;
		rettype[mcb] = 1;
		do{
			if(_L_G.ismapped != 1)
				return;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;
			rettype[mcb] = 0;
			reconnectnum=0;
			switch(mappingsteps)
			{
				case 0:
				{
					reqatr.id = 512;
					reqatr.stat = 0;
					reqatr.propert = 0;//value
					data[0] = 0x00;

					SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
					_L_G.Process = (AnswerWritAtr) ;
					break;
				}
				case 1:
				{
					reqatr.id = 512;
					reqatr.stat = 0;
					reqatr.propert = 0;//value
					data[0] = 0x00;

					SendWriteAtr(&reqatr,data,1,1,mcb);
					_L_G.Process = (AnswerWritAtr) ;
					break;
				}
			}
			delay_ms(DELAY_MILISEC);
		}while(mappingsteps<2 && _L_G.ismapped==1);
	}
	else
	{
		DownSlopeWeld();
		mappingsteps = 0xff;
		rettype[mcb] = 1;
		do{
			if(_L_G.ismapped != 1)
				return;
			while (rettype[mcb] == 2 && _L_G.ismapped==1)
			{
				delay_ms(DELAY_MILISEC);
			}
			while(rettype[mcb] == 0 && _L_G.ismapped==1 && reconnectnum<5)
			{
				delay_ms(DELAY_MILISEC);
				reconnectnum++;
			}
			if(rettype[mcb] == 1)
				mappingsteps++;
			rettype[mcb] = 0;
			reconnectnum=0;
			switch(mappingsteps)
			{
				case 0:
				{
					rettype[mcb] = Act520Process(allneedvalue.work_point.minvalue,hanjiepara.cratertrim);
					break;
				}
				case 1:
				{
					reqatr.id = 512;
					reqatr.stat = 0;
					reqatr.propert = 0;//value
					data[0] = 0x00;
					SendWriteAtr(&reqatr,data,1,GetPeerByClass(0x02),mcb);
					_L_G.Process = (AnswerWritAtr) ;
					delay_ms(5);
					//delay_ms(hanjiepara.burnbacktime);
					break;
				}
				case 2:
				{
					reqatr.id = 512;
					reqatr.stat = 0;
					reqatr.propert = 0;//value
					data[0] = 0x00;
					SendWriteAtr(&reqatr,data,1,1,mcb);
					_L_G.Process = (AnswerWritAtr) ;
					break;
				}
			}
			delay_ms(DELAY_MILISEC);
		}while(mappingsteps<3 && _L_G.ismapped==1);
//		SaveParaMeter(1);
	}
	Can_G_V.hanjiestatus = 0;
}


static void HanjieProcess(void)
{
	if((Can_G_V.controlkey == 0x10) && Can_G_V.hanjiestatus ==0)
	{
		StartWeld();
	}
	else if((Can_G_V.controlkey ==0x20) && (Can_G_V.hanjiestatus !=0 || _L_G.block514respond != 0))
	{
		EndWeld();
	}
	else if(Can_G_V.hanjiestatus == 80)
	{
		ChangeCurrVoltData();
		delay_ms(130);
	}
	else
	{
		while(Can_G_V.controlkey != 0x20 && Can_G_V.hanjiestatus != 0 && Can_G_V.hanjiestatus != 80)
		{
			delay_ms(5);
			UpSlopeWeld();
		}
	}
}



void ArcLinkProcess(void)
{
	static uint8_t istuisipressed = 0;
	static uint8_t issongsipressed = 0;
	if(_L_G.ismapped==1 && _L_G.isready==0)
	{
		ProcessCommand();
		_L_G.isready	= 1;
	}
	else if(_L_G.ismapped!=1)
	{
		_L_G.isready = 0;
	}
	GetSetHanjiePara();
	while(_L_G.isready == 1)
	{
		osDelay(20);
		if(istuisipressed == 0 && issongsipressed == 0)
		{
			HanjieProcess();
		}
		if(Can_G_V.hanjiestatus == 0)
		{
			if((Can_G_V.controlkey & 0x80) !=0x00 && (Can_G_V.controlkey& 0x40) !=0x00)
			{
				Can_G_V.controlkey = 0;
				_L_G.ismapped = 0;
				istuisipressed = 0;
				issongsipressed = 0;
				osDelay(50);
			}
			if(issongsipressed==0 && (Can_G_V.controlkey& 0x80) !=0x00 && istuisipressed ==0)
			{
				InchUP(1);
				istuisipressed = 1;
			}
			else if((Can_G_V.controlkey& 0x80) ==0x00 && istuisipressed!=0)
			{
				InchUP(0);
				istuisipressed=0;

			}

			if(istuisipressed==0 && (Can_G_V.controlkey& 0x40) !=0x00  && issongsipressed ==0)
			{
				InchDown(1);
				issongsipressed = 1;
			}
			else if((Can_G_V.controlkey& 0x40) !=0x00  && issongsipressed ==1)
			{
				issongsipressed = CheckSongSi();
			}
			else if((Can_G_V.controlkey& 0x40) ==0x00 && issongsipressed!=0)
			{
				InchDown(0);
				issongsipressed=0;
			}
		}
	}
}
