/*
 * dev_eep.c
 *
 *  Created on: 2016濡ょ姷鍎戦幏锟�7闂佸搫鐗為幏锟�5闂佸搫鍠涢幏锟�
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
	SCL_L;            //濡府鎷�

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
		if (temp & 0x80)           //MSB闂侀潻璐熼崝灞炬櫠閿燂拷
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
	IicSendByte(0xA0);            //闁诲簼绲婚褍顭囬崘顔芥櫖閻忕偠妫勭徊娲⒑椤愶紕顦﹂柛鐔告崌瀹曘劑骞嬮幒鎾承�
	IicWaiteAck();
	adrByte = (adr>>8)&0xFF ;
	IicSendByte(adrByte);      //闁诲孩绋掗〃澶嬩繆椤撱垹鎹堕柡澶嬪缁诧拷 0
	IicWaiteAck();
	adrByte = adr&0xFF;
	IicSendByte(adrByte);      //闁诲孩绋掗〃澶嬩繆椤撱垹鎹堕柡澶嬪缁诧拷 1
	IicWaiteAck();
	for (uint8_t i = 0; i < len; i++)
	{
		IicSendByte(data[i]);            //闂佸憡鍔栭悷銉╂偩椤掍胶顩查柛鈩兠惁褰掓⒑閽樺澧曟慨妯稿姂瀵即顢涘☉妯兼▎闂佸湱顭堝ú顓㈠箯閿熺姵鍎嶉柛鏇ㄥ墮閻﹀綊鏌涘顓炵仴闁哥喐鎹囧顐︽偋閸繄銈�
		IicWaiteAck();
	}
	IicStop();
	return OK;
}



static uint16_t EEP_ReadPage(uint16_t adr,uint8_t* data, const uint16_t len)
{
	uint8_t adrByte;
	IicStart();
	IicSendByte(0xA0);        //闁诲簼绲婚褍顭囬崘顔芥櫖閻忕偠妫勭徊娲⒑椤愶紕顦﹂柛鐔告崌瀹曘劑骞嬮幒鎾承戦梺鎸庣☉閻楀棝宕㈠☉姘暫濞达絿顭堥弲鎼佹煙缁嬫寧鍠橀柟鍑ょ節閺佸秹鏁撻敓锟�
	IicWaiteAck();
	adrByte = (adr>>8)&0xFF;
	IicSendByte(adrByte);      //闁诲孩绋掗〃澶嬩繆椤撱垹鎹堕柡澶嬪缁诧拷 0
	IicWaiteAck();
	adrByte = adr&0xFF;
	IicSendByte(adrByte);      //闁诲孩绋掗〃澶嬩繆椤撱垹鎹堕柡澶嬪缁诧拷 1
	IicWaiteAck();
	IicStart();     //闂佸憡鍔曠粔鐢割敃婵傜鐭楅柟瀛樼箓濮ｅ鎮硅閸㈡煡锝為幇鏉跨骇闁炽儱寮堕锟�
	IicSendByte(0xA1);  //闁哄鏅滈悷锕傤敃婵傜鍙婃い鏍ㄥ嚬閸ゃ垽鏌涜閹风兘鏌涘鍐劮妞ゆ洑鍗冲畷銊╁箣閹烘挸袘
	for (uint16_t i = 0; i < len; i++)        //24LC婵炴潙鍚嬬喊宥夋偋閹绢喖绠叉い鏃傚帶閺呯鈹戦崒婊勬珪婵炲牊鍨块獮鎰板炊椤掍礁鐝遍梺鎸庣☉閼活垶鎯冮姀銈嗗剮缂佸娉曠�瑰鏌℃担鍝勵暭鐎规挷绶氬畷锝夊箣閿斿吋瀚瑰ù锝呮啞闊悞DA婵炴垶鎼幏锟�
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


uint16_t Trigger_EEPSave(uint8_t* adr, uint16_t len, uint8_t sync)
{
	if(len != 0)
	{
		uint32_t adrSegIdx = ((uint32_t)adr - EEP_RAM_WORKING)/EEP_DATA_PAGE_LEN;
		uint32_t adrSegMax = ((uint32_t)adr + len - EEP_RAM_WORKING)/EEP_DATA_PAGE_LEN;


		if(adrSegMax > EEP_SEG_NUM)
		{
			Dia_SetDiagnosis(EEPROM_ERROR, 1);
			TraceDBG(TSK_ID_EEP,"EEP storage Overrun: %d\n",adrSegMax);
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
