/*
 * LB_App.h
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */

#ifndef LOCALBUS_LB_APP_H_
#define LOCALBUS_LB_APP_H_

#include "LB_Layer7.h"

enum
{
	CMD_POLL = 0x0,
	CMD_READ_OBJ = 0x01,
	CMD_WRITE_OBJ = 0x02,
	CMD_READ_TYPE = 0x03,
	CMD_READ_LENGTH = 0x04,
	CMD_READ_ATR_NUM = 0x05,
	CMD_READ_RANGE = 0x06,
	CMD_READ_NAME = 0x07,
	CMD_READ_MEM = 0x11,
	CMD_WRITE_MEM = 0x12,
	CMD_BURST_VALUE = 0x21,
};
/*


typedef struct
{
	uint32_t	memAdr;
	uint16_t	memLen;
	uint8_t*	memData;
}MemCmd;

typedef struct
{
	uint8_t		subId;
	uint8_t 	objId;
	uint8_t		atrId;
	uint16_t	ObjLen;
	uint8_t*	objData;
}ObjCmd;
*/


uint16_t LB_APP_Init(FrameHandler rxHandle, FrameHandler resp);

uint16_t LB_APP_ReadObj(MsgFrame* data);
uint16_t LB_APP_WriteObj(MsgFrame* data);
uint16_t LB_APP_ReadType(MsgFrame* frame);
uint16_t LB_APP_ReadLength(MsgFrame* frame);
uint16_t LB_APP_ReadAtrNum(MsgFrame* frame);
uint16_t LB_APP_ReadRange(MsgFrame* frame);
uint16_t LB_APP_WriteMem(MsgFrame* data);
uint16_t LB_APP_ReadMem(MsgFrame* data);
uint16_t LB_APP_Poll(MsgFrame* data);
uint16_t LB_APP_ReadName(MsgFrame* frame);
uint16_t LB_APP_UnSupport(MsgFrame* data);
uint16_t LB_APP_Burst(void);

#endif /* LOCALBUS_LB_APP_H_ */
