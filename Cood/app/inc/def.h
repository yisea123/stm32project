/*
 * def.h
 *
 *  Created on: 2018Äê9ÔÂ7ÈÕ
 *      Author: pli
 */

#ifndef INC_DEF_H_
#define INC_DEF_H_



#define VALID_PASSWORD_ID			6000


#define TIME_UNIT						100u


 #define IS_ROM_ADR(x)	(((x)>=0x08000000)&&((x)<0x08200000) )
 #define IS_RAM_ADR(x)	((x)<0x001FFFFF)

extern uint16_t FVT_Test;
extern uint16_t  	masterState;
 #pragma GCC diagnostic ignored "-Wpadded"
#define TASK_LOOP_ST			true









#define TO_STR(x)	(char*)#x

 #pragma GCC diagnostic pop
 #define FORCE_STOP		0xFFFFFF

#define MAX_IO_BOARD_NUM		16



#define TICK_HEAT_DELAY (600*1000)
#define MOTOR_EXEC_TIME 120000//120s

extern uint16_t dummyRam;

extern int16_t hadc1080_Val[2];
extern volatile uint16_t measDataFlag;

#define NULL_T_DATA_OBJ		CONSTRUCT_SIMPLE_U16(&dummyRam, RAM)






#define NMT_TRIGGER			0x01
#define NEW_MEAS_DATA		0x01
#define SHELL_RX_DATA		0x01
#define RX_CAN_BUF2			0x02
#define TX_CAN_BUF2			0x01
#define ERR_CAN_2			0x08
#define CAN_SIG_INFO		(ERR_CAN_2 | RX_CAN_BUF2 | TX_CAN_BUF2)
#define CAN_TMR_EV			0x01



#endif /* INC_DEF_H_ */
