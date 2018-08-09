#ifndef _CRC_COMMON_H
#define _CRC_COMMON_H




uint16_t InitCRC16_COMMON(void);
uint16_t InitCRC16_IIC(const uint16_t Lock_Code);
uint16_t CalcCRC16_COMMON(const uint8_t data, const uint16_t crc);
uint16_t CalcCrc16Mem_COMMON(uint8_t  * ptrData,uint16_t  crcInit ,uint16_t  length);


#endif

