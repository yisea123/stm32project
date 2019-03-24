/*
 * LB_Cfg.h
 *
 *  Created on: 2019Äê3ÔÂ24ÈÕ
 *      Author: pli
 */

#ifndef LOCALBUS_LB_CFG_H_
#define LOCALBUS_LB_CFG_H_


#define tskCan					taskThreadID[TSK_ID_CAN1_TSK]
#define RX_CAN_BUF2			0x02
#define TX_CAN_BUF2			0x01
#define ERR_CAN_2			0x08
#define CAN_SIG_INFO		(ERR_CAN_2 | RX_CAN_BUF2 | TX_CAN_BUF2)
#define CAN_TMR_EV			0x01
extern CAN_HandleTypeDef 	hcan1;
#define ptrCanDevice		(&hcan1)

#endif /* LOCALBUS_LB_CFG_H_ */
