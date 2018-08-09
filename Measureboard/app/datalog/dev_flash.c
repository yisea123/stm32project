/*
 * dev_flash.c
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
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
#include "dev_log_sp.h"
#include "dev_logdata.h"
#include "t_data_obj_time.h"
#include "unit_rtc_cfg.h"
#include "dev_flash_fat.h"
#include "dev_spiflash.h"
#include "dev_flash.h"
#include <string.h>

#define FLASH_VERIFY		1

#ifdef FLASH_VERIFY

static uint8_t verifyData[FLASH_PAGE_SIZE];

#endif


uint16_t Flash_Init(void)
{
	return Dev_SpiFlash_Init();
}

static uint16_t Flash_WriteAdr_Loc(uint32_t adr, uint8_t* buf, uint16_t len)
{
	uint16_t ret = OK;
	uint32_t newSector = adr%FF_SECTOR_SZ;
	uint32_t sec1 = adr / FF_SECTOR_SZ;
	uint32_t sec2 = (adr+len-1) / FF_SECTOR_SZ;

	if(newSector == 0)
	{
		//erase first;
		for( uint16_t retry = 0; retry < 10; retry++)
		{
			ret |= Dev_SpiFlash_EraseBlk(sec1);
			memset(verifyData, 0, len);
			Dev_SpiFlash_ReadWrite(NULL, verifyData, adr, len);
			for(uint16_t i=0;i<len;i++)
			{
				if(verifyData[i] != 0xFF)
				{
					ret = FATAL_ERROR;
					TraceDBG(TSK_ID_DATA_LOG, "Default value Error: erase adr:0x%x, sec%d!\n",adr,sec1);
				}
			}
			if(ret == OK)
				break;
		}
		if(ret != OK)
		{
			TraceDBG(TSK_ID_DATA_LOG, "Error: erase adr:0x%x, sec%d!\n",adr,sec1);
		}
		else
		{
			TracePrint(TSK_ID_DATA_LOG, "OK: erase adr:0x%x, sec%d!\n",adr,sec1);
		}
	}
	if(sec2 != sec1)
	{
		ret |= Dev_SpiFlash_EraseBlk(sec2);
		if(ret != OK)
		{
			TraceDBG(TSK_ID_DATA_LOG, "Error: erase adr:0x%x, sec%d!\n",adr,sec2);
		}
		else
		{
			TracePrint(TSK_ID_DATA_LOG, "OK: erase adr:0x%x, sec%d!\n",adr,sec2);
		}
	}
	if(ret == OK)
	{
		uint16_t cmpRet = 0;
		//then write new data
		if(len <= FLASH_PAGE_SIZE )
		{
			ret |= Dev_SpiFlash_ReadWrite(buf, NULL, adr, len);
	#ifdef FLASH_VERIFY
			memset(verifyData, 0, len);
			Dev_SpiFlash_ReadWrite(NULL, verifyData, adr, len);
			cmpRet = (uint16_t)memcmp((void*)(verifyData),(void*)buf, len );
			if(cmpRet != 0)
			{
				ret |= FATAL_ERROR;
				if(ret != OK)
				{
					TraceDBG(TSK_ID_DATA_LOG, "compare error: adr:0x%x, len%d!\n",adr,len);
				}
			}
	#endif
		}
		else
		{
			uint16_t wrLen = 0;
			for(uint16_t idx =0;idx<len;)
			{
				if((uint16_t)(len-idx) > FLASH_PAGE_SIZE)
					wrLen = FLASH_PAGE_SIZE;
				else
					wrLen = (uint16_t)(len-idx);
				ret |= Dev_SpiFlash_ReadWrite(buf+idx, NULL, adr+idx, wrLen);
	#ifdef FLASH_VERIFY
				Dev_SpiFlash_ReadWrite(NULL, verifyData, adr+idx, wrLen);
				cmpRet = (uint16_t)memcmp((void*)(verifyData),(void*)(buf+idx), wrLen );
				if(cmpRet != 0)
				{
					ret |= FATAL_ERROR;
					if(ret != OK)
					{
						TraceDBG(TSK_ID_DATA_LOG, "compare error: adr:0x%x, len%d!\n",adr,len);
					}
				}
	#endif
				idx = (uint16_t)(idx + wrLen);
				if(ret)
				{
					break;
				}
			}
		}
	}

	if(ret == OK)
	{
		TraceMsg(TSK_ID_DATA_LOG, "Write File OK: adr:0x%x, len%d!\n",adr,len);
	}
	return ret;

}



uint16_t Flash_WriteAdr(uint32_t adr, uint8_t* buf, uint16_t len)
{
	uint16_t ret = Flash_WriteAdr_Loc(adr,buf,len);
	uint32_t newSector = adr%FF_SECTOR_SZ;

	if(newSector == 0)
	{
		uint16_t count = 4;
		while(ret != OK && count != 0)
		{
			count--;
			ret = Flash_WriteAdr_Loc(adr,buf,len);
		}
	}
	return ret;

}



uint16_t Flash_ReadAdr(uint32_t adr, uint8_t* buf, uint16_t len)
{
	uint16_t ret = OK;

#define BYTES_EACH_TIME		FLASH_PAGE_SIZE

	//then write new data
	if(len <= BYTES_EACH_TIME )
	{
		ret |= Dev_SpiFlash_ReadWrite(NULL, buf, adr, len);
	}
	else
	{
		uint32_t rdLen = 0;
		for(uint32_t idx =0;idx<len;)
		{
			if(len-idx > BYTES_EACH_TIME)
				rdLen = BYTES_EACH_TIME;
			else
				rdLen = len-idx;
			ret |= Dev_SpiFlash_ReadWrite(NULL, &buf[idx], (uint32_t)(adr+idx), (uint16_t)rdLen);

			idx += rdLen;
			if(ret)
			{
				break;
			}
		}
	}
	return ret;
}

