/*
 * LB_Layer2_Uart.h
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */

#ifndef LOCALBUS_LB_LAYER2_UART_H_
#define LOCALBUS_LB_LAYER2_UART_H_

#include "LB_Layer1_Uart.h"
#include "LB_Layer_Data.h"


void LB_Layer2_RX_TimeOut(void);


uint16_t LB_Layer2_Uart_Init(FrameHandler _ptrHandle);
uint16_t LB_Layer2_Uart_Rx(uint8_t* data, uint16_t len);
uint16_t LB_Layer2_Uart_Tx(MsgFrame* data);


uint16_t StartUartTimer(void);


#endif /* LOCALBUS_LB_LAYER2_UART_H_ */
