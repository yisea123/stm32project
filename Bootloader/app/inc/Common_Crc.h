#ifndef _CRC_COMMON_H
#define _CRC_COMMON_H


#define CRC_LEN		2U

uint16_t InitCRC16_COMMON(void);
uint16_t InitCRC16_IIC(const uint16_t Lock_Code);
uint16_t CalcCRC16_COMMON(const uint8_t data, const uint16_t crc);
uint16_t CalcCrc16Mem_COMMON(uint8_t  * ptrData,uint16_t  crcInit ,uint32_t length);
uint16_t crc16_ccitt(const void *buf, uint32_t len);

#endif

