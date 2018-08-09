#ifndef  __APP_CAN_PROCESS_H__
#define  __APP_CAN_PROCESS_H__
#include "stdint.h"
#include "stm32f4xx.h"
#include "database.h"


#define DELAY_MILISEC 		5

typedef struct{

	uint32_t modedesp;
	uint32_t mask514;
	int16_t mode_set;
	int16_t workpointmax;
	int16_t workpointmin;
	int16_t frequnce;
	int16_t processtype;
	uint8_t wiresize;
	uint8_t wiretype;
	uint8_t gastype;
	int8_t modedescription[17];
}MODEINFO;	   //38

typedef struct{
	int16_t maxvalue;
	int16_t minvalue;
	int16_t setvalue;
	uint8_t decinum;
	uint8_t unitcode;
	int8_t name[17];
}ONEVALUEINFO;	//31

typedef struct{
	ONEVALUEINFO inchdata;
	ONEVALUEINFO work_point;
	ONEVALUEINFO trim_value;
	ONEVALUEINFO wave1;
	ONEVALUEINFO wave2;
	ONEVALUEINFO wave3;
	ONEVALUEINFO wave4;
}ALLNEEDVALUE;		//31*7=217

typedef struct {
	uint16_t  strikespeed ;
	uint16_t  starttime;
	uint16_t  upslopetime;
	uint16_t  downslopetime;
	uint16_t  cratertime;
	// 	uint16_t  burnbacktime;
	uint16_t  startmovedelay;
	uint16_t  stopmovedealy;
	//	uint16_t  strikedelay;  // ������ʱ�����ڰ������º�೤ʱ�俪ʼ����
	//	uint16_t  enddelay;	   // �ջ���ʱ�����ڰ������º�೤ʱ�俪ʼ�ջ�

	uint16_t  striketrim;
	uint16_t  strikeworkpoint;
	uint16_t  starttrim;
	uint16_t  startworkpoint;
	uint16_t  cratertrim;
	uint16_t  craterworkpoint;


	uint16_t  weldmode;
	uint16_t  coldinchSpeed;
	uint16_t  weldTrim;
	uint16_t  weldWorkpoint;
	//	int16_t  burnbacktrim;
}HANJIEPARA;	   //15*2 =28









extern MODEINFO modeinfo;
extern ALLNEEDVALUE allneedvalue;

typedef struct {
	uint16_t	vendorid;
	uint8_t	vendorname[10];
	uint8_t	modelnum[10];
	uint8_t	modulename[10];
	uint16_t	classid;
	uint8_t	classname[10];
	uint16_t	classrevision;
	uint8_t	classtype;
	uint8_t	groupswitch;
	uint8_t	instanceswitch;
	uint8_t	groupassign;
	///10
	uint8_t	instanceassign;
	uint16_t	arclinkrevision;
	uint8_t	readystatus;
	uint8_t	fault;
	uint16_t	ssn;
	uint8_t	neo;
	uint8_t	eon;

	uint8_t	softversion[10];
	uint8_t	hardversion[10];

	uint32_t	diagstatus;
	uint8_t	reset;
	uint8_t	busavailable;
	uint8_t	bootstrap;
	uint8_t	objectindecator;
	uint8_t	numoftimemaped;
	uint8_t	modulebaseaddr;
	uint8_t	numobjects;
	uint8_t	hbdis;
}NetAttr;
//���� ����



typedef struct {
	uint8_t non:3;
	uint8_t conid:4;
	uint8_t ds:1;
	uint8_t rre:2;
	uint8_t gr:1;
	uint8_t adr:8;
	uint8_t service:4;
	uint8_t di:1;
	uint8_t fcbt:2;
	uint8_t fcbc:3;
	uint8_t mcb:3;
	uint8_t fromadr;
	uint8_t toadr;
}HeadMessage;



typedef union
{
	HeadMessage msg;
	uint32_t head;
}HeadMsg;



//canͷ�ļ� ID
typedef struct {
	uint16_t id ;
	uint8_t stat ;
	uint8_t propert;
}AttributeType;



typedef struct{
	uint8_t peeradress;
	uint16_t classid;
	uint8_t  groupid;
	uint8_t  instance;
}OnePeer;



//ÿ���ڵ�Ļ�����Ϣ
typedef struct {
	int16_t max_value;
	int16_t min_value;
	uint8_t  unit_code;
	uint8_t  deci_num;
	int8_t name[17];
}AtrDescription;



//����ID�Ļ�����Ϣ
typedef struct {
	uint32_t modedesp;
	uint32_t seqmask;
	uint16_t fuctionatrmask;
	uint16_t processtype;
	uint16_t synergicrec;
	uint8_t trimattach;
	uint8_t wave1;
	uint8_t wave2;
	uint8_t wave3;
	uint8_t wave4;
	uint8_t nodata;
	uint16_t synergicatrid;
	uint16_t synergicatrmask;
	uint16_t synergicclass;
	uint16_t synergicinsytance;
}Action514Data;
//action514 �ķ���ֵ



typedef struct{
	int16_t minvalue;
	int16_t maxvalue;
	int16_t normvalue;
}OneMaxMinValue;

typedef uint8_t (*CanDataProcess)(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type ) ;



typedef struct
{
	uint8_t act520len;
	uint8_t peernum;
	CanDataProcess Process;
	uint16_t actionid;
	uint8_t answer_ret;


	uint8_t Peer; //������ĵ�ַ
	uint16_t serialnum;
	uint16_t block514respond;
	uint8_t atrid34value;	//�Ƿ��Ѿ�mapped

	uint8_t isready;

	uint8_t ismapped; //�Ƿ��Ѿ�������нڵ�ĵ�ַmapping
	uint8_t isaddressed; //�Ƿ񱻸����ַ��
	//ȫ�ֱ�������
	uint8_t TRACTOR;
	uint8_t TRACTOR_Stabel;
	uint8_t  act520mask;
	uint8_t  synergic_status  ;
	int16_t synergic_speed  ;
}local_G;


typedef struct
{
	uint8_t errcode_show ;
	int16_t workpoint_real_520 ;
	int16_t trimvalue_real_520;
	uint8_t hanjiestatus;
	uint8_t modeindex;
	uint8_t allmodenum;
	uint8_t controlkey;
}Gloabl_Value;


// for menu


typedef struct __Postion
{
	uint8_t  sel;
	uint16_t yPos;
	uint16_t xPosStart;
	uint16_t xPosEnd;
}numPosition;



//typedef void (ChangePara1*)(int16_t* pValue);

typedef struct __dat
{
	uint8_t function;
	int8_t* pStr;
	int16_t* pValue;
	int8_t	 point;
	uint8_t  len;
	uint8_t  lineindex;// contain line index and row index;
	//	void (ChangePara*)(int16_t* pValue);
	numPosition* pos;

}DisplayStr;

#define yHold  5
#define xHold  10
#define SIZE_FONT 16
typedef struct _xyPos
{
	int16_t xPos;
	int16_t yPos;
}xyPos;




// public interface

//really public function
void NewMessageArrive(void);

uint8_t GetNewMessage(CanRxMsgTypeDef* ptrMess);
void CanRxTXTask(void);





void GetNewLog(uint8_t type,uint8_t* str);
uint16_t revu16(uint16_t dat);
int16_t revs16(int16_t dat1);
void GetUnitCode(uint8_t unitcode,int8_t* showdata);
void GetWireSize(uint8_t wiresize,int8_t* showdata);
void GetWireType(uint8_t wiretype,int8_t* showdata);
void GetProcessType(uint16_t processype,int8_t* showdata);
void GetGasType(uint8_t gastype,int8_t* showdata);


void InitNetAddribute(uint8_t);

void CanProcess(CanRxMsgTypeDef* msg)  ;


//local function
#if 0
uint8_t Act19_ResPond(void);
uint8_t SendData_Can(HeadMessage* headm,uint8_t* data,uint8_t len,uint8_t needadr,uint8_t adress);
uint8_t IsNewMessage(CanRxMsgTypeDef* msg,HeadMessage* headmsg );
uint8_t Attr_WriteErr(CanRxMsgTypeDef* msg,AttributeType* atr,HeadMessage* headmsg,uint8_t code);
uint8_t Attr_WriteOk(CanRxMsgTypeDef* msg,AttributeType* atr,HeadMessage* headmsg);
uint8_t SaveAttribute(AttributeType* atr,uint8_t* data,uint8_t len);
uint8_t GetAtrdata(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t* data, uint8_t* len);
uint8_t RespondWriteAtr(CanRxMsgTypeDef* msg,AttributeType* atr,HeadMessage* headmsg);
uint8_t Atr_SendSucRead(CanRxMsgTypeDef* msg,AttributeType* atr ,HeadMessage* headmsg,uint8_t* adr, uint8_t len);
uint8_t Atr_SendErrRead(CanRxMsgTypeDef* msg,AttributeType* atr ,HeadMessage* headmsg,uint8_t code);
uint8_t RespondReadAtr(CanRxMsgTypeDef* msg,AttributeType* atr ,HeadMessage* headmsg);
uint8_t FragResPondErr(uint8_t* data,uint8_t errcode,HeadMessage* headmsg);
uint8_t ActionResPondErr(uint16_t id,uint8_t errcode,HeadMessage* headmsg);
uint8_t ActionResPond(uint16_t id,uint8_t* adr,uint8_t len,HeadMessage* headmsg);
uint8_t GetFragMent(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t len,uint8_t* data, uint8_t* datalen );

void ProcessAction32(uint8_t* data, uint8_t* len);
uint8_t ActionProcess(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t RdBlockResPond(uint16_t id,uint8_t* adr, uint8_t len,HeadMessage* headmsg);
uint8_t RdBlockResPondErr(uint16_t id,uint8_t errcode,HeadMessage* headmsg);
uint8_t RdBlockProcess(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t WRBlockProcess(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t ConEvRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t UNConEvRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t ConEvBlockRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t UnConEvBlockRP(CanRxMsgTypeDef* msg,uint16_t id,HeadMessage* headmsg);
uint8_t SolvServiceCode(CanRxMsgTypeDef* msg,HeadMessage* headmsg );
uint8_t GetErroCode(uint8_t errcode);
uint8_t SendReadAtr(AttributeType* atr,uint8_t adress,uint8_t mcb);
uint8_t SendWriteAtr(AttributeType* atr,uint8_t* data1, uint8_t len,uint8_t adress,uint8_t mcb);
uint8_t AnswerReadAtr_Peer(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);
uint8_t AnswerReadAtr(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);
uint8_t AnswerWritAtr(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);
uint16_t GetModeIndex(uint8_t* index);
uint8_t AnswerAction(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);
void GetAllPeerInfo(void);

uint8_t GetPeerByClass(uint16_t cla);
void ChangeSpeed(void);
uint8_t AnswerRCERP(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);
void FakeIni(void);
void SendReqUER(uint16_t id,uint8_t* dat,uint8_t len,uint8_t adr, uint8_t mcb);
void GetAllPeersInfo(void);
void ChangeToBig_Act515(void);
void WeldingConfiguration(uint16_t);
void GetAllModes(void);
void ProcessCommand(void);
void InchUP(uint8_t dat);
uint8_t CheckSongSi(void);


void HanjieProcess(void) ;
void SendErrCode(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t* data,uint8_t code);
void CreateReportEvent(void);


void Get520Mask(uint8_t);
uint8_t AnswerAction520(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);
int16_t GetDuringNum(int16_t max1,int16_t min1,uint8_t step1,uint8_t stepnum);


#endif




void ChangeWorkpoint(void);

void InitCan_GValue(void);



uint8_t AnswerReadAtr_Peer(CanRxMsgTypeDef* msg,HeadMessage* headmsg,uint8_t type);

extern Gloabl_Value 	Can_G_V;
extern local_G 			_L_G;
extern NetAttr 			NetAttribute;
extern uint8_t 			rettype[8];			//mcb = 0
extern uint8_t 			Group[10];
extern uint16_t 		ALLMODES[256];
extern OnePara 			oneparasavetofile;
extern OnePeer 			allpeers[25];
#endif

