/*
 * LB_Layer2_Can.h
 *
 *  Created on: 2017骞�4鏈�14鏃�
 *      Author: pli
 */

#ifndef LOCALBUS_LB_LAYER2_CAN_H_
#define LOCALBUS_LB_LAYER2_CAN_H_





typedef enum
{
	MULTI_NO = 0x0,
	MULTI_ST = 0x01,
	MULTI_ING = 0x02,
	MULTI_END = 0x03,
}CAN_FRAME;



uint16_t LB_Layer2_Can_Init(FrameHandler callBack);
uint16_t LB_Layer2_Can_Tx(MsgFrame* data);
uint16_t LB_Layer2_Can_Rx(CanRxMsgTypeDef* rxMsg);

#endif /* LOCALBUS_LB_LAYER2_CAN_H_ */
