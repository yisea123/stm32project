/*
 * LB_Layer1_Can.h
 *
 *  Created on: 2016��8��23��
 *      Author: pli
 */

#ifndef LB_LAYER1_CAN_H_
#define LB_LAYER1_CAN_H_


#define SLAVE

void NotifyLB(MsgFrame* frames, uint32_t state);

void HW_CanRXMsg(CAN_HandleTypeDef* hcan, uint16_t FIFONumber);
//sw interface
void StartCanTmrTask(void const * argument);
void StartCanTask(void const * argument);
void StartCanPollTask(void const * argument);
void StartCanMasterTask(void const * argument);

void Co_TPDO_Data_Update(uint16_t);
void RxMsgHandle(CanRxMsgTypeDef* CAN1_RxMsg);
CanTxMsgTypeDef* GetNewCanTxMsgID(uint8_t type);

#endif /* DEV_CAN_H_ */
