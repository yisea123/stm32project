/*
 * dev_can.h
 *
 *  Created on: 2016��8��23��
 *      Author: pli
 */

#ifndef DEV_CAN_H_
#define DEV_CAN_H_


#define SLAVE

#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"



#define Message_Initializer {0,0,0,{0,0,0,0,0,0,0,0}}

#define ERR_CAN_1			0x0001
#define ERR_CAN_2			0x0002
#define CAN_ERR_ALL			(ERR_CAN_1|ERR_CAN_2)



#define RX_CAN_BUF1			0x01
#define TX_CAN_BUF1			0x01

void HW_CanRXMsg(CAN_HandleTypeDef* hcan, uint16_t FIFONumber);
//sw interface
void TSK_CAN_Transmit(const CanTxMsgTypeDef* sendmsg);

void TSK_CAN_Transmit1(const CanTxMsgTypeDef* sendmsg);
void StartCanRXTask(void const * argument);
void StartCanTXTask(void const * argument);
void StartCanErrTask(void const * argument);

void SimuCan(void);


#endif /* DEV_CAN_H_ */
