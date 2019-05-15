/*
 * sdio.h
 *
 *  Created on: 2019��5��15��
 *      Author: pli
 */

#ifndef INC_SDIO_H_
#define INC_SDIO_H_

#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//SD����������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#define SD_TIMEOUT ((uint32_t)100000000)            //��ʱʱ��

#define SD_DMA_MODE    		0	//1��DMAģʽ��0����ѯģʽ

extern SD_HandleTypeDef        SDCARD_Handler;     //SD�����
extern HAL_SD_CardInfoTypeDef  SDCardInfo;         //SD����Ϣ�ṹ��

u8 SD_Init(void);
u8 SD_GetCardInfo(HAL_SD_CardInfoTypeDef *cardinfo);
u8 SD_ReadDisk(u8* buf,u32 sector,u32 cnt);
u8 SD_WriteDisk(u8 *buf,u32 sector,u32 cnt);
u8 SD_ReadBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt);
u8 SD_WriteBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt);


#endif /* INC_SDIO_H_ */
