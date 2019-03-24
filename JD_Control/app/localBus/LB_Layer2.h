/*
 * LB_Layer2.h
 *
 *  Created on: 2017年5月19日
 *      Author: pli
 */

#ifndef LOCALBUS_LB_LAYER2_H_
#define LOCALBUS_LB_LAYER2_H_

uint16_t LB_Layer2_Tx(MsgFrame* frame);
uint16_t LB_Layer2_Init(FrameHandler rxHandle);
uint16_t LB_Layer2_Rx(MsgFrame* frame);


#endif /* LOCALBUS_LB_LAYER2_H_ */
