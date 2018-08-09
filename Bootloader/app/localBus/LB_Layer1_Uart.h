/*
 * LB_Driver.h
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */

#ifndef LOCALBUS_LB_LAYER1_UART_H_
#define LOCALBUS_LB_LAYER1_UART_H_

typedef enum
{
	SET_BAUDRATE,
	SET_RX_CALLBACK,
	SET_ADDR,
}CHN_VAL;


typedef uint16_t (*RxCallBack)(uint8_t* data, uint16_t len);

uint16_t LB_Layer1_Uart_Init(RxCallBack callBack);
uint16_t LB_Layer1_Uart_Tx(uint8_t* data, uint16_t len);
uint16_t LB_Layer1_Uart_Rx(uint8_t data);

void LB_Layer1_StartTimer(void);
void LB_Layer1_TimerOut(void);


#endif /* LOCALBUS_LB_LAYER1_UART_H_ */
