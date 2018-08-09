#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "stdint.h"

#define SKHIONUM        4  //��32��IO



typedef struct {
uint8_t baoliu1;	  //����
uint8_t baoliu2; //����
uint32_t time_real;// =	309440690+100 ;
uint16_t IOPRESSED;
uint8_t  controlkey;
uint8_t  errcode;
uint16_t block514respond;//��˿��λ���
int16_t workpoint_set;
int16_t trim_set;
int16_t speed;
uint16_t volt_feedback ;	  //�����ĵ�����ѹ
uint16_t curr_feedback ;

uint8_t end1;
uint8_t end2;
}OnePara;


uint32_t revu32(uint32_t dat);
uint16_t revu16(uint16_t dat);
int16_t revs16(int16_t dat1);
void ChangeOnePara(OnePara* one1);
void InitSpeedData(void);
void InitMachineAD(void);
int GetMachieOut(int* dat,int setd);
int Get456Out(int outdata);
int GetSongsiOut(int outdata);
void CRCVerify (uint8_t *puchMsg1,int len);
uint8_t CRCCheck(uint8_t* inbufback,int len);
uint8_t ProcessReqType(uint8_t* dat);
void ProcessComm(uint8_t* inbufback, uint8_t datalen,int comindex);
void InitMvPara(void);
void RenewHanjiePara(void);
#endif

