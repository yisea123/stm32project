/*
 * dev_eep.h
 *
 *  Created on: 2016��7��5��
 *      Author: pli
 */

#ifndef DEV_EEP_H_
#define DEV_EEP_H_
#pragma pack(push)
#pragma pack(1)

typedef struct
{
	uint16_t version;
	uint16_t validLen;
	uint32_t wrCount;
	uint32_t rev;
	uint16_t dataCrc;
	uint16_t crc;
} EEP_HEAD;

#define CHK_SUM_LEN		2u
#pragma pack(pop)


#define EEP_STORAGE_TIME	(60000)//1 minutes

#ifdef __cplusplus
extern "C"
{
#endif






#define eepIIC				hi2c2
#define MAX_RETRY			5
#define EEP_VERSION			0x3B
#define EEP_ADR				0xA0

#define eepTimeOut(x)  		(uint32_t)((x)+20)



//local data
#define EEP_SIGNAL_RESET_SAVE	0x02
#define EEP_SIGNAL_SAVE		0x01

#define EEP_SIGNAL			(EEP_SIGNAL_SAVE | EEP_SIGNAL_RESET_SAVE)


//call when device startup with no valid data;
//this function make copy the all eep data to ram
uint16_t Init_EEPData(void);
//this function stores all dirty data to eep with updated crc checksum
uint16_t Save_EEPData(void);


#ifdef __cplusplus
}
#endif

#endif /* DEV_EEP_H_ */
