/*
 * ArcLink_Rx.c
 *
 *  Created on: 2016��9��23��
 *      Author: pli
 */

#include "canprocess.h"
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "parameter_idx.h"
#include "unit_parameter.h"

void TractorHandle(CanRxMsgTypeDef* msg,HeadMessage* headmsg )
{
	uint16_t 	id ;
	uint8_t 	propert;

#ifdef USE_MAX10
	uint16_t	speed;
#endif

	if(	headmsg->service == 0)	//Read Atribute ";
	{
		id = (uint16_t)((msg->Data[0])*256 +msg->Data[1]);
		propert = (msg->Data[3]);
		if(headmsg->rre == 1)
		{
			if(headmsg->fromadr == _L_G.TRACTOR)	//tractor response
			{
				if(propert == 0)
				{
					if(id == 513 )
					{
			//			parameter_Put(PARA_IDX_can_validatenum,WHOLE_OBJECT, &id);
#ifdef USE_MAX10
						speed =  (uint16_t)(msg->Data[4]) *256 + msg->Data[5];
						parameter_Put(PARA_IDX_weldSpeed,WHOLE_OBJECT, &speed );
#endif
					}
					if(id == 514 )
					{
			//			parameter_Put(PARA_IDX_can_validatenum,WHOLE_OBJECT, &id);
#ifdef USE_MAX10
						speed =	(uint16_t)(msg->Data[4]) *256 + msg->Data[5];
						parameter_Put(PARA_IDX_weldSpeed,WHOLE_OBJECT, &speed );
#endif
					}
				}
			}
		}
	}
	else if(headmsg->service == 1)	//		  "Write Attibute ";
	{
		if((headmsg->rre == 0) && (headmsg->toadr  == _L_G.TRACTOR))
		{
			id = (uint16_t)((uint16_t)(msg->Data[0] *256)+msg->Data[1]);
			propert = (msg->Data[3]);
			if(id == 513)
			{
			//	parameter_Put(PARA_IDX_can_validatenum,WHOLE_OBJECT, &id);
#ifdef USE_MAX10
				speed =  (uint16_t)(msg->Data[4]) *256 + msg->Data[5];
				parameter_Put(PARA_IDX_weldSpeed,WHOLE_OBJECT, &speed );
#endif
			}
		}
	}
}

uint8_t IsNewMessage(CanRxMsgTypeDef* msg,HeadMessage* headmsg )
{
	uint8_t adrget;
	uint8_t linshi,locIndex1_;
	uint32_t head= msg->ExtId;
	if(msg->IDE == CAN_ID_STD || msg->RTR != CAN_RTR_DATA)
	{
		if(msg->IDE == CAN_ID_STD)
			HandleCommunicationSRV(msg->Data,msg->StdId);
		return 0;
	}

	headmsg->ds =(uint8_t) ((head>>24)&0x1);
	headmsg->gr =(uint8_t) ((head>>21)&0x1);
	headmsg->di =(uint8_t) ((head>>8)&0x1);
	headmsg->adr	=(uint8_t) ((head>>13)&0xff);
	headmsg->conid =(uint8_t) ((head>>25)&0x0f);
	headmsg->rre =(uint8_t) ((head>>22)&0x03);
	headmsg->service =(uint8_t) ((head>>9)&0x0f);
	headmsg->mcb =(uint8_t) ((head>>0)&0x07);
	headmsg->fcbt =(uint8_t) ((head>>6)&0x03);
	headmsg->fcbc =(uint8_t) ((head>>3)&0x07);

	linshi = headmsg->ds*4+headmsg->gr*2+headmsg->di;
	if(linshi == 5)
	{
		if(msg->DLC<=1)
			return 0 ;
		adrget = msg->Data[msg->DLC-1];
		headmsg->fromadr = headmsg->adr;
		headmsg->toadr = adrget;
		if(	(_L_G.TRACTOR_Stabel == 0 )&& (headmsg->service == 0))	//Read Atribute ";
		{
			AnswerReadAtr_Peer(msg, headmsg,0	);
		}

		if((headmsg->toadr == _L_G.TRACTOR) || (headmsg->fromadr == _L_G.TRACTOR) )
		{
			TractorHandle(msg,headmsg);
			return 0;
		}
		else if((adrget == _L_G.Peer) )
		{
			return 11;		// peer to peer
		}

	}
	else if(linshi==0 && (headmsg->adr == _L_G.Peer || headmsg->adr ==255 ))  // 10 tractor
	{
		return 1;				// master to slave;
	}
	else if(linshi == 2)
	{
		for(locIndex1_=0;locIndex1_<10;locIndex1_++)
		{
			if(headmsg->adr == Group[locIndex1_])
			{
				return 2; //master to slaves
			}
		}
		return 0;
	}
	else if(linshi==7)
	{
		if(msg->DLC<=1)
			return 0 ;
		adrget = msg->Data[msg->DLC-1];
		for(locIndex1_=0;locIndex1_<10;locIndex1_++)
		{
			if(adrget == Group[locIndex1_])
			{
				return 12; //peer to peers;
			}
		}
	}
	return 0;
}


//interfaces to handle the rx message
void CanProcess(CanRxMsgTypeDef* msg)
{
	HeadMessage headmessage;
	CanRxMsgTypeDef rxmsg;
	uint8_t  ret = IsNewMessage(msg,&headmessage);
	uint16_t count = 1;
	parameter_Put(PARA_IDX_can_innum,WHOLE_OBJECT,&count);
	if(ret == 0)
		return;
	rxmsg = *msg;

	if(headmessage.rre == 0)	   //req
	{
		SolvServiceCode(&rxmsg, &headmessage);
	}
	else if(headmessage.rre == 1)	   //rep
	{
		if(_L_G.Process !=0)
		{
			rettype[headmessage.mcb] = (_L_G.Process)(&rxmsg, &headmessage,0);
		}
	}
	else if(headmessage.rre == 2)//err
	{
		if(_L_G.Process !=0)
		{
			rettype[headmessage.mcb] = (_L_G.Process)(&rxmsg, &headmessage,1);
		}
	}


}
