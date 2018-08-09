/*
 * dev_ad7172.h
 *
 *  Created on: 2016Äê7ÔÂ25ÈÕ
 *      Author: pli
 */

#ifndef DEV_AD7172_H_
#define DEV_AD7172_H_

typedef enum
{
	AD7172_MEA_ID,
	AD7172_REF_ID,
	DEV_7172_MAX,

}AD_IDX;

#define AD_CHN_SHORT		0
#define AD_CHN_LONG			1


#ifdef __cplusplus
 extern "C" {
#endif

extern AD717X_DEVICE 			AD717x[DEV_7172_MAX];

void DevAD_Init(void);
uint16_t DevAD_ReadData(uint8_t device, int32_t* data);
uint16_t DevAD_SelectChn(uint8_t slave_id, uint8_t chnl);

uint16_t DevAD_ReadReg(uint8_t device, uint8_t addr, uint32_t* value);
uint16_t DevAD_WriteReg(uint8_t device, uint8_t addr,const uint32_t value);

#ifdef __cplusplus
 }
#endif

#endif /* DEV_AD7172_H_ */
