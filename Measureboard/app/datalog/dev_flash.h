/*
 * dev_flash.h
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
 *      Author: pli
 */

#ifndef DEV_FLASH_H_
#define DEV_FLASH_H_


//interface to HW level
uint16_t Flash_Init(void);
uint16_t Flash_WriteAdr(uint32_t adr, uint8_t* data, uint16_t len);
uint16_t Flash_ReadAdr(uint32_t adr, uint8_t* data, uint16_t len);




#endif /* DEV_FLASH_H_ */
