/*
 * ArcLink_Data.c
 *
 *  Created on: 2016��9��23��
 *      Author: pli
 */


#include <string.h>
#include <stdint.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "parameter_idx.h"
#include "unit_parameter.h"
#include "CanProcess.h"
#include "main.h"


//ȫ�ֱ���
MODEINFO modeinfo ;
ALLNEEDVALUE allneedvalue ;
OnePeer 			allpeers[25];
//add local data
local_G 			_L_G;
NetAttr 			NetAttribute;
Gloabl_Value 		Can_G_V;
uint8_t 			rettype[8];			//mcb = 0
//local object;

uint32_t revu32(uint32_t dat)
{
	uint32_t data = 0;
	data = dat&0xFF;
	data<<=8;
	data += (dat&0xFF00)>>8;
	data<<=8;
	data += (dat&0xFF0000)>>16;
	data<<=8;
	data += (dat&0xFF000000)>>24;

	return data;
}
uint16_t revu16(uint16_t dat)
{
	uint16_t data = 0;
	data = dat&0xFF;
	data<<=8;
	data += (dat&0xFF00)>>8;

	return data;
}
int16_t revs16(int16_t dat)
{
	int16_t data = 0;
	data = dat&0xFF;
	data<<=8;
	data += (dat&0xFF00)>>8;

	return data;
}


void InitCan_GValue(void)
{
	_L_G.act520len = 0;
	_L_G.peernum = 11;
	_L_G.Process = NULL;

	_L_G.actionid = 0;
	_L_G.answer_ret = 0;


	_L_G.Peer = 0; //������ĵ�ַ
	_L_G.serialnum = 0xBBCC;
	_L_G.block514respond = 0x0;
	_L_G.atrid34value = 0;	//�Ƿ��Ѿ�mapped

	_L_G.isready = 0;


	_L_G.ismapped = 0; //�Ƿ��Ѿ�������нڵ�ĵ�ַmapping
	_L_G.isaddressed = 0; //�Ƿ񱻸����ַ��
	//ȫ�ֱ�������
	_L_G.TRACTOR	= 0x0A;
	_L_G.TRACTOR_Stabel	= 0x0;

	Can_G_V.controlkey = 0;
	Can_G_V.errcode_show = 0;
	Can_G_V.workpoint_real_520 = 0 ;
	Can_G_V.trimvalue_real_520 = 0;
	Can_G_V.hanjiestatus = 0;

	Can_G_V.modeindex = 0;
	Can_G_V.allmodenum = 0;

	//  to init the arclink map status
	InitNetAddribute(0);
}


void InitModeInfo(void)
{
	modeinfo.mode_set = 189;
	modeinfo.frequnce = 60;
	allneedvalue.trim_value.setvalue = 330;
	allneedvalue.work_point.setvalue = 450;
	allneedvalue.wave1.setvalue = 0;
	allneedvalue.wave2.setvalue	= 0;
	allneedvalue.wave3.setvalue	= 0;
	allneedvalue.wave4.setvalue	= 0;
}


void GetUnitCode(uint8_t unitcode,int8_t* showdata)
{
	memset(showdata,0,12);
	switch(unitcode)
	{
		case 0:
		case 1: memcpy(showdata," None  " ,7); break;
		case 2: memcpy(showdata," M     " ,7); break;
		case 3: memcpy(showdata," Kg    " ,7); break;
		case 4: memcpy(showdata," S     " ,7); break;
		case 5: memcpy(showdata," AmpS  " ,7); break;
		case 6: memcpy(showdata," K     " ,7); break;
		case 7: memcpy(showdata," Moles " ,7); break;
		case 8: memcpy(showdata,"Candela" ,7); break;
		case 9: memcpy(showdata,"Farads " ,7); break;
		case 10: memcpy(showdata," Hertz " ,7); break;
		case 11: memcpy(showdata,"Joules " ,7); break;
		case 12: memcpy(showdata,"Newtons" ,7); break;
		case 13: memcpy(showdata," Ohms  " ,7); break;
		case 14: memcpy(showdata,"Pascal " ,7); break;
		case 15: memcpy(showdata," Volts " ,7); break;
		case 16: memcpy(showdata," Watts " ,7); break;
 		case 17: memcpy(showdata,"  MS   " ,7); break;
		case 18: memcpy(showdata,"Minutes" ,7); break;
		case 19: memcpy(showdata," Hours " ,7); break;
		case 20: memcpy(showdata," Days  " ,7); break;
		case 21: memcpy(showdata,"Months " ,7); break;
		case 22: memcpy(showdata," Years " ,7); break;
		case 23: memcpy(showdata," CM    " ,7); break;
		case 24: memcpy(showdata," KM    " ,7); break;
		case 25: memcpy(showdata," Inch  " ,7); break;
		case 26: memcpy(showdata," Feet  " ,7); break;
		case 27: memcpy(showdata," Yard  " ,7); break;
		case 28: memcpy(showdata," Mile  " ,7); break;
		case 29: memcpy(showdata," CMPS  " ,7); break;
 		case 30: memcpy(showdata," CMPM  " ,7); break;
 		case 31: memcpy(showdata," MPS   " ,7); break;
 		case 32: memcpy(showdata," MPM   " ,7); break;
 		case 33: memcpy(showdata," IPS   " ,7); break;
 		case 34: memcpy(showdata," IPM   " ,7); break;
 		case 35: memcpy(showdata," FPS   " ,7); break;
 		case 36: memcpy(showdata," FPM   " ,7); break;
 		case 37: memcpy(showdata," Bars  " ,7); break;
 		case 38: memcpy(showdata," Ppsf  " ,7); break;
 		case 39: memcpy(showdata," HP    " ,7); break;
 		case 40: memcpy(showdata," KW    " ,7); break;
 		case 41: memcpy(showdata,"Degree " ,7); break;
 		case 42: memcpy(showdata,"APM    " ,7); break;
 		case 43: memcpy(showdata," VPM    " ,7); break;
 		case 44: memcpy(showdata," KWPM    " ,7); break;
 		case 45: memcpy(showdata," A*M    " ,7); break;
 		case 46: memcpy(showdata," V*M    " ,7); break;
 		case 47: memcpy(showdata," KW*M    " ,7); break;
 		case 48: memcpy(showdata,"Percent" ,7); break;
 		case 49: memcpy(showdata," CMPS   " ,7); break;
 		case 50: memcpy(showdata," LPM    " ,7); break;
 		case 51: memcpy(showdata," KP    " ,7); break;
 		case 52: memcpy(showdata," GPL    " ,7); break;
 		case 53: memcpy(showdata," N*M    " ,7); break;
 		case 54: memcpy(showdata," H      " ,7); break;
		default: memcpy(showdata,"        " ,7); break;
	};
}


void GetWireSize(uint8_t wiresize,int8_t* showdata)
{
	memset(showdata,0,12);
	switch(wiresize)
	{
	case 0: memcpy(showdata," -----",5);break;
	case 1: memcpy(showdata,"N/A",3);;break;
	case 2: memcpy(showdata,"0.025",5);break;
	case 3: memcpy(showdata,"0.030",5);break;
	case 4: memcpy(showdata,"0.035",5);break;
	case 5: memcpy(showdata,"0.045",5);break;
	case 6: memcpy(showdata,"3/64",4);	break;
	case 7: memcpy(showdata,"0.052",5);break;
	case 8: memcpy(showdata,"1/16",4);;break;
	case 9: memcpy(showdata,"0.068",5);break;
	case 10: memcpy(showdata,"0.072",5);break;
	case 11: memcpy(showdata,"5/64",4);break;
	case 12: memcpy(showdata,"3/32",4);break;
	case 13: memcpy(showdata,"7/64",4);break;
	case 14: memcpy(showdata,"0.120",5);break;
	case 15: memcpy(showdata,"1/8",3);break;
	case 16: memcpy(showdata,"5/32",4);break;
	case 17: memcpy(showdata,"3/16",4);break;
	case 18: memcpy(showdata,"7/32",4);break;
	case 19: memcpy(showdata,"1/4",3);break;
	case 20: memcpy(showdata,"5/16",4);break;
	case 21: memcpy(showdata,"3/8",3);break;
	case 22: memcpy(showdata,"0.8mm",5);break;
	case 23: memcpy(showdata,"0.9mm",5);break;
	case 24: memcpy(showdata,"1.0mm",5);break;
	case 25: memcpy(showdata,"1.2mm",5);break;
	case 26: memcpy(showdata,"1.4mm",5);break;
	case 27: memcpy(showdata,"1.6mm",5);break;
	case 28: memcpy(showdata,"2.0mm",5);break;
	case 29: memcpy(showdata,"2.4mm",5);break;
	case 30: memcpy(showdata,"2.5mm",5);break;
	case 31: memcpy(showdata,"3.2mm",5);break;
	case 32: memcpy(showdata,"4.0mm",5);break;
	case 33: memcpy(showdata,"5.0mm",5);break;
	case 34: memcpy(showdata,"5.6mm",5);break;
	case 35: memcpy(showdata,"6.0mm",5);break;
	case 36: memcpy(showdata,"6.3mm",5);break;
	default: memcpy(showdata," -----",5);break;
	};
}



void GetWireType(uint8_t wiretype,int8_t* showdata)
{
	memset(showdata,0,12);
	switch(wiretype)
	{
	case 0:
		memcpy(showdata,"------",6);
		break;
	case  1:
		memcpy(showdata,"N/A",3);
		break;
	case  2:
		memcpy(showdata,"Steel",5);
		break;
	case  3:
		memcpy(showdata,"Stainless",9);
		break;
	case  4:
		memcpy(showdata,"Aluminum",8);
		break;
	case  5:
		memcpy(showdata,"CrNi Alloy",10);
		break;
	case  6:
		memcpy(showdata,"Ni Alloy",8);
		break;
	case  7:
		memcpy(showdata,"NiCr Alloy",10);
		break;
	case  8:
		memcpy(showdata,"Copper Ni",9);
		break;
	case  9:
		memcpy(showdata,"Si Bronze",9);
		break;
	case  10:
		memcpy(showdata,"Al Bronze",9);
		break;
	case  11:
		memcpy(showdata,"Stick",5);
		break;
	case  12:
		memcpy(showdata,"Cored Wire",10);
		break;
	case  13:
		memcpy(showdata,"Metal Core",10);
		break;
	case  14:
		memcpy(showdata,"SelfShield",10);
		break;
	case  15:
		memcpy(showdata,"Gas Shield",10);
		break;
	case  16:
		memcpy(showdata,"Carbon Arc",10);
		break;
	case  17:
		memcpy(showdata,"L-50/L-56",9);
		break;
	case  18:
		memcpy(showdata,"L-50",4);
		break;
	case  19:
		memcpy(showdata,"L-54",4);
		break;
	case  20:
		memcpy(showdata,"L-56",4);
		break;
	case  21:
		memcpy(showdata,"L-60",4);
		break;
	case  22:
		memcpy(showdata,"L-70",4);
		break;
	case  23:
		memcpy(showdata,"L-50 Steel",10);
		break;
	case  24:
		memcpy(showdata,"L-54 Steel",10);
		break;
	case  25:
		memcpy(showdata,"L-56 Steel",10);
		break;
	case  26:
		memcpy(showdata,"LA-71",5);
		break;
	case  27:
		memcpy(showdata,"LA-75",5);
		break;
	case  28:
		memcpy(showdata,"LA-81",5);
		break;
	case  29:
		memcpy(showdata,"LA-90",5);
		break;
	case  30:
		memcpy(showdata,"LA-100",6);
		break;
	case  31:
		memcpy(showdata,"ER70S-X",7);
		break;
	case  32:
		memcpy(showdata,"ER70S-3",7);
		break;
	case  33:
		memcpy(showdata,"ER70S-4",7);
		break;
	case  34:
		memcpy(showdata,"ER70S-6",7);
		break;
	case  35:
		memcpy(showdata,"Steel SG-1",10);
		break;
	case  36:
		memcpy(showdata,"Steel SG-2",10);
		break;
	case  37:
		memcpy(showdata,"Steel SG-3",10);
		break;
	case  38:
		memcpy(showdata,"2xx Stnles",10);
		break;
	case  39:
		memcpy(showdata,"3xx Stnles",10);
		break;
	case  40:
		memcpy(showdata,"4xx Stnles",10);
		break;
	case  41:
		memcpy(showdata,"Soft Alum",9);
		break;
	case  42:
		memcpy(showdata,"Hard Alum",9);
		break;
	case  43:
		memcpy(showdata,"1100 Al",7);
		break;
	case  44:
		memcpy(showdata,"2319 Al",7);
		break;
	case  45:
		memcpy(showdata,"4043 Al",7);
		break;
	case  46:
		memcpy(showdata,"4047 Al",7);
		break;
	case  47:
		memcpy(showdata,"4145 Al",7);
		break;
	case  48:
		memcpy(showdata,"5183 Al",7);
		break;
	case  49:
		memcpy(showdata,"5356 Al",7);
		break;
	case  50:
		memcpy(showdata,"AlMg 5",6);
		break;
	case  51:
		memcpy(showdata,"AMg4.5Mn",8);
		break;
	case  52:
		memcpy(showdata,"AlMg 3",6);
		break;
	case  53:
		memcpy(showdata,"AlSi 5",6);
		break;
	case  54:
		memcpy(showdata,"Al 99.5",7);
		break;
	case  55:
		memcpy(showdata,"NiMo-1",6);
		break;
	case  56:
		memcpy(showdata,"NiMo-2",6);
		break;
	case  57:
		memcpy(showdata,"NiMo-3",6);
		break;
	case  58:
		memcpy(showdata,"NiMo-7",6);
		break;
	case  59:
		memcpy(showdata,"NiCrMo-1",8);
		break;
	case  60:
		memcpy(showdata,"NiCrMo-2",8);
		break;
	case  61:
		memcpy(showdata,"NiCrMo-3",8);
		break;
	case  62:
		memcpy(showdata,"NiCrMo-4",8);
		break;
	case  63:
		memcpy(showdata,"NiCrMo-7",8);
		break;
	case  64:
		memcpy(showdata,"NiCrMo-8",8);
		break;
	case  65:
		memcpy(showdata,"NiCrMo-9",8);
		break;
	case  66:
		memcpy(showdata,"C-276",5);
		break;
	case  67:
		memcpy(showdata,"Copper",6);
		break;
	case  68:
		memcpy(showdata,"CuSi",4);
		break;
	case  69:
		memcpy(showdata,"CuSn",4);
		break;
	case  70:
		memcpy(showdata,"CuNi",4);
		break;
	case  71:
		memcpy(showdata,"CuAl",4);
		break;
	case  72:
		memcpy(showdata,"NR-5",4);
		break;
	case  73:
		memcpy(showdata,"NR-131",6);
		break;
	case  74:
		memcpy(showdata,"NR-151",6);
		break;
	case  75:
		memcpy(showdata,"NR-152",6);
		break;
	case  76:
		memcpy(showdata,"NR-202",6);
		break;
	case  77:
		memcpy(showdata,"NR-203 M",8);
		break;
	case  78:
		memcpy(showdata,"NR-203 Ni",9);
		break;
	case  79:
		memcpy(showdata,"NR-203NiC",9);
		break;
	case  80:
		memcpy(showdata,"NR-204",6);
		break;
	case  81:
		memcpy(showdata,"NR-207",6);
		break;
	case  82:
		memcpy(showdata,"NR-211 MP",9);
		break;
	case  83:
		memcpy(showdata,"NR-232",6);
		break;
	case  84:
		memcpy(showdata,"NR-305",6);
		break;
	case  85:
		memcpy(showdata,"NR-311",6);
		break;
	case  86:
		memcpy(showdata,"NR-311 Ni",9);
		break;
	case  87:
		memcpy(showdata,"NR-314",6);
		break;
	case  88:
		memcpy(showdata,"NR-400",6);
		break;
	case  89:
		memcpy(showdata,"NS-3M",5);
		break;
	case  90:
		memcpy(showdata,"OS-70",5);
		break;
	case  91:
		memcpy(showdata,"OS HD-70",8);
		break;
	case  92:
		memcpy(showdata,"OS-71",5);
		break;
	case  93:
		memcpy(showdata,"OS-71M",6);
		break;
	case  94:
		memcpy(showdata,"OS 81B2-H",9);
		break;
	case  95:
		memcpy(showdata,"OS 81K2-H",9);
		break;
	case  96:
		memcpy(showdata,"OS 81Nil-H",10);
		break;
	case  97:
		memcpy(showdata,"OS 91K2-H",9);
		break;
	case  98:
		memcpy(showdata,"MC-100",6);
		break;
	case  99:
		memcpy(showdata,"MC-120-55",9);
		break;
	case  100:
		memcpy(showdata,"MC-409",6);
		break;
	case  101:
		memcpy(showdata,"MC702",5);
		break;
	case  102:
		memcpy(showdata,"MC710",5);
		break;
	case  103:
		memcpy(showdata,"MC-710-H",8);
		break;
	case  104:
		memcpy(showdata,"Other",5);
		break;
	default:
		memcpy(showdata,"UNKNOWN",7);
		break;
	};
}



void GetProcessType(uint16_t processype,int8_t* showdata)
{
	memset(showdata,0,7);
	switch(processype)
	{
	case  0 :
		memcpy(showdata,"SMAW",4);
		break;
	case  1 :
		memcpy(showdata,"SSFCAW",6);
		break;
	case  2 :
		memcpy(showdata,"GTAW",4);
		break;
	case  3 :
		memcpy(showdata,"GMAW",4);
		break;
	case  4 :
		memcpy(showdata,"GOUGE",5);
		break;
	case  5 :
		memcpy(showdata,"PAW",3);
		break;
	case  6 :
		memcpy(showdata,"SAW",3);
		break;
	case  7 :
		memcpy(showdata,"TSENSE",6);
		break;
	};
}



void GetGasType(uint8_t gastype,int8_t* showdata)
{
	memset(showdata,0,12);
	switch(gastype)
	{
	case  0 :
		memcpy(showdata,"No Gas",6);
		break;
	case  1 :
		memcpy(showdata,"M3",2);
		break;
	case  2 :
		memcpy(showdata,"M2",2);
		break;
	case  4 :
		memcpy(showdata,"M1",2);
		break;
	case  8 :
		memcpy(showdata,"HE",2);
		break;
	case  16 :
		memcpy(showdata,"H",1);
		break;
	case  32 :
		memcpy(showdata,"CO2",3);
		break;
	case  64 :
		memcpy(showdata,"O2",2);
		break;
	case  128 :
		memcpy(showdata,"AR",2);
		break;
	};

}

void InitNetAddribute(uint8_t kk)
{
	NetAttribute.vendorid=0x0001;
	memset(NetAttribute.vendorname,0,10);
	memcpy(NetAttribute.vendorname,"JUST For Test ",9);
	memset(NetAttribute.modelnum,0,6);
	memcpy(NetAttribute.modelnum,"STM 32 can 0.1",5);

	memset(NetAttribute.modulename,0,10);
	memcpy(NetAttribute.modulename," ALL-FIT                                  ",9);
	NetAttribute.classid = 0x000A;
	memset(NetAttribute.classname,0,10);
	memcpy(NetAttribute.classname,"interface 0.101000",9);

	NetAttribute.classrevision = 0x0010;
	NetAttribute.classtype = 0x12;
	NetAttribute.groupswitch = 0;
	NetAttribute.instanceswitch = 0;
	NetAttribute.groupassign = 0x01;

	///10
	NetAttribute.instanceassign = 0;
	NetAttribute.arclinkrevision = 0x0001;
	NetAttribute.readystatus = 0xFF;//bit7����ready
	NetAttribute.fault = 0x00;
	NetAttribute.ssn = 0x1020;
	NetAttribute.neo = 1;
	NetAttribute.eon = 0;

	memset(NetAttribute.softversion,0,10);
	memcpy(NetAttribute.softversion,"All-time 0.001",9);

	memset(NetAttribute.hardversion,0,10);
	memcpy(NetAttribute.hardversion,"STM32 CAN    ",9);



	NetAttribute.diagstatus = 0x0e0d0f00;
	NetAttribute.reset = 0;
	NetAttribute.busavailable = 0;
	NetAttribute.bootstrap = 0;
	NetAttribute.objectindecator = 0;
	NetAttribute.numoftimemaped = 0;
	NetAttribute.modulebaseaddr = 0;
	NetAttribute.numobjects =1;
	NetAttribute.hbdis = 0;

	_L_G.Peer = 0;
	memset((void*)Group,0,10);
	Group[0] = 255;
	Group[1] = 254;
	memset((uint8_t*)ALLMODES,0,512);
	Can_G_V.allmodenum = 0;
//					  extern OnePara oneparasavetofile;
	memset((uint8_t*)&oneparasavetofile,0,sizeof(OnePara));
	oneparasavetofile.baoliu1 = 0xff;
	oneparasavetofile.baoliu2 = 0xff;
	oneparasavetofile.end1 = '\r';
	oneparasavetofile.end2 = '\n';
	if(kk == 0)
		InitModeInfo();
}



void ReInitArcLink(void)
{
	Can_G_V.hanjiestatus = 0;
	_L_G.ismapped = 0;
	_L_G.isaddressed = 0;
	NetAttribute.reset = 1;
	_L_G.Peer =0;
	NetAttribute.busavailable = 0;
	InitNetAddribute(1);
}




uint8_t GetPeerByClass(uint16_t cla)
{
	uint8_t index = 0;
	for(index=0;index<_L_G.peernum;index++)
	{
		if(allpeers[index].classid == cla)
			return allpeers[index].peeradress;
	}
	return 0;

}


void FakeIni(void)
{
	int index = 0;
	_L_G.Peer = 2;
	Group[0] = 11;
	_L_G.ismapped =1;
	_L_G.peernum = 20;
	for(index = 0;index <_L_G.peernum; index++)
	{
		allpeers[index].peeradress = index;
	}

	/*

	allpeers[0].classid = 0;
	allpeers[0].groupid = 1;
	allpeers[0].instance = 0;

	allpeers[1].classid = 1;
	allpeers[1].groupid = 0;
	allpeers[1].instance = 0;

	allpeers[2].classid = 2;
	allpeers[2].groupid = 1;
	allpeers[2].instance = 0;

	allpeers[3].classid = 4;
	allpeers[3].groupid = 1;
	allpeers[3].instance = 0;

	allpeers[4].classid = 5;
	allpeers[4].groupid = 1;
	allpeers[4].instance = 0;

	allpeers[5].classid = 8;
	allpeers[5].groupid = 1;
	allpeers[5].instance = 0;

	allpeers[6].classid = 6;
	allpeers[6].groupid = 1;
	allpeers[6].instance = 0;

	allpeers[7].classid = 17;
	allpeers[7].groupid = 1;
	allpeers[7].instance = 0;

	allpeers[8].classid = 12;
	allpeers[8].groupid = 1;
	allpeers[8].instance = 0;

	allpeers[9].classid = 11;
	allpeers[9].groupid = 1;
	allpeers[9].instance = 0;

	allpeers[10].classid = 3;
	allpeers[10].groupid = 1;
	allpeers[10].instance = 0;
	*/

}
