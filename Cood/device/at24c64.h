/*
 * I2c_Simu.h
 *
 *  Created on: 2016Äê10ÔÂ14ÈÕ
 *      Author: pli
 */

#ifndef AT24C64_H_
#define AT24C64_H_

#define PAGE_LEN 			32

void Iic_LowInit(void);

void At24WriteStr(uint16_t addr, uint8_t *dat, uint8_t length);
void At24ReadStr(uint16_t addr, uint8_t *dat, uint8_t len);



uint16_t EEP_READ_ADR( uint16_t adr,uint8_t* data, const uint16_t len);
uint16_t EEP_WRITE_ADR(uint16_t adr, uint8_t* data, const uint16_t len);
#endif /* AT24C64_H_ */
