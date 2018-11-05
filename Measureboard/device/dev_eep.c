/*
 * dev_eep.c
 *
 *  Created on: 2016婵°倗濮烽崕鎴﹀箯閿燂拷7闂備礁鎼悧鐐哄箯閿燂拷5闂備礁鎼崰娑㈠箯閿燂拷
 *      Author: pli
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"
#include "at24c64.h"
#include "dev_eep.h"
#include "unit_sys_diagnosis.h"
#include <string.h>




//definition
#define WORK_BANK_IDX		0
#define BACKUP_BANK_IDX		1


#define HEAD_ADR_ST			0
#define DATA_ADR_ST			512
#define MAX_DATA_LEN 		0x1E00
#define EEP_RAM_WORKING		0x20000000
#define EEP_DATA_PAGE_LEN	PAGE_LEN

#define BACKUP_ST_ADR		(0x2000u)
#define EEP_SEG_NUM  		(MAX_DATA_LEN/PAGE_LEN)
#define EEP_RAM_SHADOW		((uint32_t)(&ramBkpBuffEEP[0]))
#define EEP_HEAD_ALLOCATE	((uint16_t)32) // PAGE_LEN


static uint8_t ramBkpBuffEEP[MAX_DATA_LEN];
static uint8_t dataReadBk[EEP_DATA_PAGE_LEN];

//for external access
__IO uint16_t eepStatus[2] = {ERROR_NV_STORAGE, ERROR_NV_STORAGE};

static const uint16_t HeadStartAdr[2] = {HEAD_ADR_ST, HEAD_ADR_ST+BACKUP_ST_ADR};
static const uint16_t DataStartAdr[2] = {DATA_ADR_ST, DATA_ADR_ST+BACKUP_ST_ADR};
static const uint16_t HEAD_ADR_SEG_LEN = (DATA_ADR_ST/EEP_HEAD_ALLOCATE);//16
static const uint16_t eepSegNumMax = EEP_SEG_NUM;


static 	EEP_HEAD head;
static __IO uint16_t triggerSaveCnt = 0;
static uint16_t headAdr[2] = {HEAD_ADR_ST, HEAD_ADR_ST+BACKUP_ST_ADR};

static uint8_t durtySeg[EEP_SEG_NUM]; // EEP_SEG_NUM


static SemaphoreHandle_t eepLock = NULL;



#ifdef USE_GPIO

#define SDA_H		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET)
#define SDA_L		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET)

#define SCL_H		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET)
#define SCL_L		 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET)

static inline void I2cDelayus(uint16_t tick)
{
	tick *= 20;
	while(tick--)
	{
	}
}



static void IicStart(void)
{
	//prepare
	SDA_H;
	SCL_H;
	I2cDelayus(5);


	//sda low
	SDA_L;
	I2cDelayus(5);
	SCL_L;            //婵☆偄搴滈幏锟�

	I2cDelayus(5);      //
	SDA_H;
	I2cDelayus(5);
}

static void IicStop(void)
{
	SDA_L;
	SCL_L;
	I2cDelayus(2);

	SCL_H;
	I2cDelayus(2);
	SDA_H;            //
	I2cDelayus(2);
	SCL_L;

	I2cDelayus(2);        //
}
static void IicAck(void)  //
{
	SCL_L;
	SDA_L;
	I2cDelayus(5);

	SCL_H;
	I2cDelayus(5);   //
	SCL_L;

	I2cDelayus(2);
	SDA_H;
	I2cDelayus(2);
}

static void IicNack(void)  //
{
	SCL_L;
	SDA_H;
	I2cDelayus(5);

	SCL_H;
	I2cDelayus(5);   //
	SCL_L;

	I2cDelayus(5);
	SDA_H;
	I2cDelayus(5);
}
static void IicWaiteAck(void)
{
	SCL_L;
	SDA_H;
	I2cDelayus(5);

	SCL_H;
	I2cDelayus(5);
	SCL_L;

	I2cDelayus(5);
}
static void IicSendByte(uint8_t temp)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		SCL_L;
		I2cDelayus(5);
		if (temp & 0x80)           //MSB闂備線娼荤拹鐔煎礉鐏炵偓娅犻柨鐕傛嫹
			SDA_H;
		else
			SDA_L;
		SCL_H;
		I2cDelayus(5);
		temp <<= 1;
	}
}

static uint8_t IicReceiveByte(void)
{
	uint8_t i, temp = 0;

	I2cDelayus(5);
	SDA_H;                  //
	I2cDelayus(5);

	for (i = 0; i < 8; i++)
	{
		temp <<= 1;
		SCL_L;
		I2cDelayus(5);
		SCL_H;

		if (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5))
			temp = temp | 0x01;
		else
			temp = temp & 0xFE;
		I2cDelayus(5);
	}
	SCL_H;
	return temp;
}

static uint16_t EEP_WritePage(uint8_t adr, uint8_t* data, uint8_t len)
{
	uint8_t adrByte;
	IicStart();
	IicSendByte(0xA0);            //闂佽绨肩徊濠氼敋瑜嶉…鍥礃椤旇姤娅栭柣蹇曞仩濡嫮寰婂ú顏呪拺妞ゆ劧绱曢ˇ锕傛煕閻斿憡宕岀�规洏鍔戦獮瀣箳閹炬壙锟�
	IicWaiteAck();
	adrByte = (adr>>8)&0xFF ;
	IicSendByte(adrByte);      //闂佽瀛╃粙鎺椼�冩径瀣╃箚妞ゆ挶鍨归幑鍫曟煛婢跺顕滅紒璇ф嫹 0
	IicWaiteAck();
	adrByte = adr&0xFF;
	IicSendByte(adrByte);      //闂佽瀛╃粙鎺椼�冩径瀣╃箚妞ゆ挶鍨归幑鍫曟煛婢跺顕滅紒璇ф嫹 1
	IicWaiteAck();
	for (uint8_t i = 0; i < len; i++)
	{
		IicSendByte(data[i]);            //闂備礁鎲￠崝鏍偡閵夆晜鍋╂い鎺嶈兌椤╂煡鏌涢埄鍏狀亪鎯佽ぐ鎺撯拺闁芥ê顦辨晶鏇熸叏濡濮傜�殿喕鍗抽、娑樷槈濡吋鈻庨梻浣告贡椤牆煤椤撱垹绠柨鐔哄У閸庡秹鏌涢弴銊ュ闁伙箑缍婇弻娑橆潩椤撶偟浠撮梺鍝ュ枑閹瑰洤顕ｉ锔藉亱闁割偅绻勯妶锟�
		IicWaiteAck();
	}
	IicStop();
	return OK;
}



static uint16_t EEP_ReadPage(uint16_t adr,uint8_t* data, const uint16_t len)
{
	uint8_t adrByte;
	IicStart();
	IicSendByte(0xA0);        //闂佽绨肩徊濠氼敋瑜嶉…鍥礃椤旇姤娅栭柣蹇曞仩濡嫮寰婂ú顏呪拺妞ゆ劧绱曢ˇ锕傛煕閻斿憡宕岀�规洏鍔戦獮瀣箳閹炬壙鎴︽⒑閹稿海鈽夐柣妤�妫濆畷銏犫槈濮橆厼鏆繛杈剧悼椤牓寮查幖浣圭厵缂佸瀵ч崰姗�鏌熼崙銈囩瘈闁轰礁绉归弫鎾绘晸閿燂拷
	IicWaiteAck();
	adrByte = (adr>>8)&0xFF;
	IicSendByte(adrByte);      //闂佽瀛╃粙鎺椼�冩径瀣╃箚妞ゆ挶鍨归幑鍫曟煛婢跺顕滅紒璇ф嫹 0
	IicWaiteAck();
	adrByte = adr&0xFF;
	IicSendByte(adrByte);      //闂佽瀛╃粙鎺椼�冩径瀣╃箚妞ゆ挶鍨归幑鍫曟煛婢跺顕滅紒璇ф嫹 1
	IicWaiteAck();
	IicStart();     //闂備礁鎲￠崝鏇犵矓閻㈠壊鏁冨┑鍌滎焾閻鏌熺�涙绠撴慨锝咁樀閹顦查柛銏＄叀閿濈偤骞囬弶璺ㄩ獓闂佺偨鍎卞鍫曨敃閿燂拷
	IicSendByte(0xA1);  //闂佸搫顦弲婊堟偡閿曞偆鏁冨┑鍌滎焾閸欏﹥銇勯弽銊ュ毈闁搞們鍨介弻娑滎檪闁归鍏橀弻娑橆煥閸愵亞鍔銈嗘磻閸楀啿鐣烽妸鈺佺闁圭儤鎸歌
	for (uint16_t i = 0; i < len; i++)        //24LC濠电偞娼欓崥瀣枈瀹ュ鍋嬮柟缁㈠枛缁犲弶銇勯弮鍌氬付闁哄懐顭堥埞鎴﹀磼濠婂嫭鐝┑鐐茬墛閸ㄥ潡鐛幇鏉跨倞妞ゆ帊绀侀悵閬嶆⒑閹稿海鈽夐柤娲诲灦閹啴濮�閵堝棗鍓紓浣割儓濞夋洜锟界懓顦甸弻鈩冩媴閸濆嫷鏆悗瑙勬尫缁舵艾鐣烽敐澶婄闁挎柨鍚嬬�氱懓霉閿濆懏鍟為棅顒傛倿DA濠电偞鍨堕幖顐﹀箯閿燂拷
	{
		IicAck();
		data[i] = IicReceiveByte();
	}
	IicNack();
	IicStop();

	return OK;
}


#endif







static uint16_t CACL_CHKSUM(uint8_t* data, uint32_t len)
{
	return CalcCrc16Mem_COMMON(data, InitCRC16_COMMON(), len);
}
#ifdef USE_GPIO

static uint16_t EEP_READ_ADR( uint16_t adr,uint8_t* data, const uint16_t len)
{
	uint16_t ret = OK;
	uint16_t lenR = 0;
	uint16_t tempLen = 0;
	uint16_t adr_R = adr;
	assert(adr%PAGE_LEN == 0);
	assert(len>0);

	while(len > lenR )
	{
		uint16_t ret1;
		tempLen = (uint16_t)(len - lenR);
		if(tempLen > PAGE_LEN)
		{
			tempLen = PAGE_LEN;
		}
		ret1 = EEP_ReadPage(adr_R, data, (uint16_t)tempLen );

		lenR = (uint16_t)(lenR + tempLen);
		adr_R = (uint16_t)(adr_R + tempLen);
		data += tempLen;

		if(ret1 != OK )
		{
			ret = FATAL_ERROR;
		}
		else
		{
			ret = OK;
		}
	}
	return ret;
}

static uint16_t EEP_WRITE_ADR(uint16_t adr, uint8_t* data, const uint16_t len)
{
	uint16_t ret = OK;
	uint16_t lenW = 0;
	uint16_t tempLen = 0;
	uint16_t adr_W = adr;
	assert(adr%PAGE_LEN == 0);
	assert(len>0);

	while(len > lenW )
	{
		uint16_t ret1;
		tempLen = (uint16_t)(len - lenW);
		if(tempLen > PAGE_LEN)
		{
			tempLen = PAGE_LEN;
		}
		ret1 = EEP_WritePage(adr_W, data, tempLen );

		lenW = (uint16_t)(lenW + tempLen);
		adr_W = (uint16_t)(adr_W + tempLen);
		data += tempLen;
		if(ret1 != OK )
		{
			ret = FATAL_ERROR;
		}
		else
		{
			ret = OK;
		}
	}
	return ret;

}

#else



#endif

static uint16_t EEP_WRITE_HEAD(uint8_t* data, uint16_t len, uint16_t bankSel)
{
	headAdr[bankSel] = (uint16_t)(headAdr[bankSel] + EEP_HEAD_ALLOCATE);
	if(headAdr[bankSel] >= DataStartAdr[bankSel])
	{
		headAdr[bankSel] = HeadStartAdr[bankSel];
	}
	return EEP_WRITE_ADR(headAdr[bankSel],data, len);
}

static uint16_t EEP_CLEAR_HEAD(uint16_t bankSel)
{
	uint16_t adr = HeadStartAdr[bankSel];
	uint8_t data[EEP_HEAD_ALLOCATE] = {0,0,};
	for(;;)
	{

		if(adr >= DataStartAdr[bankSel])
		{
			break;
		}
		EEP_WRITE_ADR(adr,data, EEP_HEAD_ALLOCATE);
		adr = (uint16_t)(adr + EEP_HEAD_ALLOCATE);
	}
	return OK;
}

static uint16_t EEP_WRITE_ALL_DATA(uint8_t* data, uint16_t len, uint16_t bankSel)
{
	return EEP_WRITE_ADR(DataStartAdr[bankSel],data, len);
}

#ifdef USE_GPIO

static uint16_t EEP_CheckHead(uint8_t* _headAdr)
{
	uint16_t ret = FATAL_ERROR;
	uint16_t adr = 0;
	uint32_t count = 0;
	EEP_HEAD headLoc;
	headLoc.wrCount = 0;

	for(uint16_t idx=0; idx<HEAD_ADR_SEG_LEN; idx++)
	{
		adr = (uint16_t)(HEAD_ADR + idx*PAGE_LEN);
		uint16_t ret1 = EEP_READ_ADR(adr,(void*)&headLoc,PAGE_LEN);
		if(ret1 == OK)
		{
			uint32_t checkSum = CACL_CHKSUM((void*)&headLoc,PAGE_LEN-CHK_SUM_LEN);
			if(checkSum != headLoc.checkSum)
			{
			}
			else if(EEP_VERSION != headLoc.version)
			{
			}
			else
			{
				if(count <= headLoc.wrCount)
				{
					count = headLoc.wrCount;
					ret = OK;
					headAdr = adr;
					count = headLoc.wrCount;
					memcpy( (void*)_headAdr, (void*)&headLoc,PAGE_LEN);
				}
			}
		}
	}

	return ret;
}

#else

static uint16_t EEP_CheckHead(uint8_t* _headData, uint16_t bankSel)
{
	uint16_t ret = FATAL_ERROR;
	uint16_t adr = 0;
	uint32_t count = 0;
	EEP_HEAD headLoc;
	for(uint16_t idx=0; idx<HEAD_ADR_SEG_LEN; idx++)
	{
		adr = (uint16_t)(HeadStartAdr[bankSel] + idx*EEP_HEAD_ALLOCATE);
		ret = EEP_READ_ADR(adr,(void*)&headLoc,sizeof(headLoc));
		if(ret == OK)
		{
			uint16_t checkSum = CACL_CHKSUM((void*)&headLoc,sizeof(EEP_HEAD)-CHK_SUM_LEN);
			if(checkSum != headLoc.crc)
			{
			}
			else if(EEP_VERSION != headLoc.version)
			{
			}
			else
			{
				if(count <= headLoc.wrCount)
				{
					count = headLoc.wrCount;
					ret = OK;
					headAdr[bankSel] = adr;
					count = headLoc.wrCount;
					memcpy( (void*)_headData, (void*)&headLoc,sizeof(headLoc));
				}
			}
		}
	}

	return ret;
}

#endif

static uint16_t EEP_Init(EEP_HEAD* ptrEepHead, uint8_t* data, uint16_t len, uint16_t bankSel)
{
	uint16_t  ret = ERROR_NV_STORAGE;


	if(EEP_CheckHead((uint8_t*)ptrEepHead, bankSel) == OK)
	{
		if(len == ptrEepHead->validLen)
		{
			ret = EEP_READ_ADR(DataStartAdr[bankSel], data, (uint16_t)ptrEepHead->validLen);
		}
		else
		{
			ret = ERROR_NV_STORAGE;
		}
		if(ret == OK)
		{
			uint16_t checkSum = CACL_CHKSUM(data,len);
			if(checkSum != ptrEepHead->dataCrc)
			{
				ret = ERROR_NV_STORAGE;
			}
			else
			{
				ret = OK;
			}

		}
	}
	else
	{
		ptrEepHead->version = EEP_VERSION;
		ptrEepHead->wrCount = 0;
		ptrEepHead->validLen = len;
	}

	return ret;
}

static void EEP_NewHead(EEP_HEAD* ptrHead)
{
	ptrHead->version = EEP_VERSION;
	ptrHead->wrCount++;
	ptrHead->crc = CACL_CHKSUM((uint8_t*)ptrHead,sizeof(EEP_HEAD)-CHK_SUM_LEN);
}

static uint16_t EEP_PrepareHead(uint8_t* adr)
{
	uint16_t  ret = OK;
	uint16_t checkSum = CACL_CHKSUM(adr,MAX_DATA_LEN);
	head.dataCrc = checkSum;
	head.validLen = MAX_DATA_LEN;
	EEP_NewHead(&head);
	return ret;
}
static uint16_t EEP_WriteHead(uint16_t bankSel)
{
	return EEP_WRITE_HEAD((uint8_t*)&head,sizeof(head),bankSel);
}

static uint16_t EEP_Sync(uint8_t* adr, uint16_t len, uint16_t bankSel)
{
	uint16_t  ret = FATAL_ERROR;
	EEP_PrepareHead(adr);
	ret = EEP_WriteHead(bankSel);
	ret |= EEP_WRITE_ALL_DATA(adr,len,bankSel);
	eepStatus[1] = ret;

	return ret;
}



uint16_t Init_EEPData(void)
{
#ifndef DEBUG_TEST
	uint16_t bankError = 0;
	uint8_t* adr = (uint8_t*)EEP_RAM_WORKING;
	void* dst = (void*)EEP_RAM_SHADOW;
	uint16_t len = MAX_DATA_LEN;
	uint16_t ret;
	EEP_HEAD eepHead;
#ifdef IIC_GPIO_SIMU
	Iic_LowInit();
#endif
	(void)memset(durtySeg, 0, sizeof(durtySeg));

	eepLock = OS_CreateSemaphore();
	OS_Use(eepLock);
	ret = EEP_Init(&eepHead, adr,len, WORK_BANK_IDX);
	if(ret == OK)
	{
		memcpy((void*)&head,(void*)&eepHead, sizeof(head));
		ret = EEP_READ_ADR(DataStartAdr[BACKUP_BANK_IDX], dst, (uint16_t)head.validLen);
		if(ret == OK)
		{
			uint16_t checkSum = CACL_CHKSUM(dst,head.validLen);
			if(checkSum != head.dataCrc)
			{
				bankError |= 0xF0;
			}
		}
	}
	else
	{
		bankError |= 0x0F;
		ret = EEP_Init(&eepHead, adr,len, BACKUP_BANK_IDX);
		if(ret != OK)
			bankError |= 0xF0;

	}
	OS_Unuse(eepLock);
	eepStatus[0] = ret;
	if(ret != OK)
	{
		head = eepHead;
		LoadDefaultCfg(0xFFFF);
		EEP_CLEAR_HEAD(WORK_BANK_IDX);
		EEP_CLEAR_HEAD(BACKUP_BANK_IDX);
		memcpy(dst, adr, len);
		EEP_Sync(adr,len,WORK_BANK_IDX);
		EEP_Sync(adr,len,BACKUP_BANK_IDX);
		TraceDBG(TSK_ID_EEP,"EEP storage is wrong \n");
	}
	else
	{
		eepStatus[1] = OK;
		memcpy(dst, adr, len);
		//WORK_BANK error
		if(bankError & 0x0F)
		{
			EEP_Sync(adr,len,WORK_BANK_IDX);
		}
		if(bankError & 0xF0)
		{
			EEP_Sync(adr,len,BACKUP_BANK_IDX);
		}
		TracePrint(TSK_ID_EEP,"EEP init OK! \n");
	}
	Dia_UpdateDiagnosis(EEPROM_ERROR,(ret != OK) );
#else
	uint16_t ret = LoadDefaultCfg();
	lock = OS_CreateSemaphore();
#endif
	return ret;
}



uint16_t Save_EEPData(void)
{
	uint8_t* adr = (uint8_t*)EEP_RAM_SHADOW;
	uint16_t ret = OK;
	uint16_t dataIsUpdated = 0;
#ifdef NV_TEST
	for(uint16_t idx=0;idx<EEP_SEG_NUM;idx++)
	{
		//if(durtySeg[idx])
		{
			ret = EEP_READ_ADR((uint16_t)(DATA_ADR+idx*PAGE_LEN),dataBk ,PAGE_LEN);
			if(memcmp(adr+idx*PAGE_LEN, dataBk, PAGE_LEN) != 0)
			{
				if(durtySeg[idx] == 0x0)
				{
					durtySeg[idx] |= 0x01;
				}
			}
		}
	}
#endif

	if(!triggerSaveCnt)
	{
		return OK;
	}

	OS_Use(eepLock);
	for(uint16_t idx=0;idx<eepSegNumMax;idx++)
	{
		if(durtySeg[idx] & 0x0F)
		{
			ret = EEP_READ_ADR((uint16_t)(DataStartAdr[WORK_BANK_IDX]+idx*EEP_DATA_PAGE_LEN),dataReadBk ,EEP_DATA_PAGE_LEN);
			if(memcmp(adr+idx*EEP_DATA_PAGE_LEN, dataReadBk, EEP_DATA_PAGE_LEN) != 0)
			{
				ret = EEP_WRITE_ADR((uint16_t)(DataStartAdr[WORK_BANK_IDX]+idx*EEP_DATA_PAGE_LEN),adr+idx*EEP_DATA_PAGE_LEN,EEP_DATA_PAGE_LEN);
				dataIsUpdated++;
			}

			if(ret==OK)
				durtySeg[idx] &= 0xF0;
		}
	}
	if(dataIsUpdated)
	{
		EEP_PrepareHead((void*)EEP_RAM_SHADOW);
		ret |= EEP_WriteHead(WORK_BANK_IDX);
	}


	dataIsUpdated = 0;
	for(uint16_t idx=0;idx<eepSegNumMax;idx++)
	{
		if(durtySeg[idx] & 0xF0)
		{
			ret = EEP_READ_ADR((uint16_t)(DataStartAdr[BACKUP_BANK_IDX]+idx*EEP_DATA_PAGE_LEN),dataReadBk ,EEP_DATA_PAGE_LEN);
			if(memcmp(adr+idx*EEP_DATA_PAGE_LEN, dataReadBk, EEP_DATA_PAGE_LEN) != 0)
			{
				ret = EEP_WRITE_ADR((uint16_t)(DataStartAdr[BACKUP_BANK_IDX]+idx*EEP_DATA_PAGE_LEN),adr+idx*EEP_DATA_PAGE_LEN,EEP_DATA_PAGE_LEN);
				dataIsUpdated++;
			}
			if(ret==OK)
				durtySeg[idx] &= 0x0F;
		}
	}
	if(dataIsUpdated)	ret |= EEP_WriteHead(BACKUP_BANK_IDX);
	OS_Unuse(eepLock);
	eepStatus[1] = ret;

	if(ret == OK)
	{
		OS_Use(eepLock);
		triggerSaveCnt=0;
		OS_Unuse(eepLock);
		TracePrint(TSK_ID_EEP,"EEP task saved once  \n");
	}
	else
	{
		TraceDBG(TSK_ID_EEP,"EEP task saved failed \n");
	}
	Dia_UpdateDiagnosis(EEPROM_ERROR, (ret != OK));
	return ret;
}


uint16_t Trigger_EEPSaveInst(uint8_t* adr, uint16_t len, uint8_t sync, uint32_t _line)
{
	if(len != 0)
	{
		uint32_t adrSegIdx = ((uint32_t)adr - EEP_RAM_WORKING)/EEP_DATA_PAGE_LEN;
		uint32_t adrSegMax = ((uint32_t)adr + len - EEP_RAM_WORKING)/EEP_DATA_PAGE_LEN;


		if(adrSegMax > EEP_SEG_NUM)
		{
			Dia_SetDiagnosis(EEPROM_ERROR, 1);
			TraceDBG(TSK_ID_EEP,"EEP storage Overrun: %d, %x, line: %d\n",adrSegMax,adrSegMax, _line);
		}
		OS_Use(eepLock);
		void* dst = (void*)(EEP_RAM_SHADOW + ((uint32_t)adr - EEP_RAM_WORKING));
		if( 0!= memcmp(dst, adr, len))
		{
			memcpy(dst, adr, len);
			triggerSaveCnt++;
			if((adrSegIdx <= adrSegMax) && (adrSegMax < eepSegNumMax))
			{
				for(;adrSegIdx<=adrSegMax;adrSegIdx++)
				{
					durtySeg[adrSegIdx] |= 0xFF;
				}
			}
			if((SYNC_IM == sync) && (eepTaskHandle!= NULL))
				SigPush(eepTaskHandle, EEP_SIGNAL_SAVE);
		}
		OS_Unuse(eepLock);

	}
	return OK;
}





#if 0

uint16_t EEP_ReadAdr(uint8_t* data, uint32_t adr, uint32_t len)
{
	EEP_HEAD head;
	uint16_t  ret = FATAL_ERROR;
	if(adr + len <= head.validLen)
	{
		ret = EEP_READ_ADR(data, adr, len);
	}

	return ret;
}

#endif
