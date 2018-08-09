/*
 * dev_hdc1080.h
 *
 *  Created on: 2017Äê7ÔÂ17ÈÕ
 *      Author: pli
 */

#ifndef DEV_HDC1080_H_
#define DEV_HDC1080_H_

uint16_t Init_hdc1080(void);
uint16_t Start_hdc1080(void);
uint16_t Read_hdc1080(uint16_t* ptrHumidity, uint16_t* ptrTemp);

uint16_t UpdateTH(void);


#endif /* DEV_HDC1080_H_ */
