/*
 * LB_Layer7.h
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */

#ifndef LOCALBUS_LB_LAYER7_H_
#define LOCALBUS_LB_LAYER7_H_

#include "LB_Layer2_Uart.h"

uint16_t LB_Layer7_Init(FrameHandler _ptrHandle);

uint16_t LB_Layer7_Decode(MsgFrame* data);


#endif /* LOCALBUS_LB_LAYER7_H_ */
