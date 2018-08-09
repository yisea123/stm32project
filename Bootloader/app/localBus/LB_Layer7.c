/*
 * LB_Layer7.c
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */


#include <string.h>
#include "main.h"
#include "LB_Layer_data.h"
#include "LB_Layer2.h"
#include "LB_Layer7.h"
#include "LB_App.h"





uint16_t LB_Layer7_Init(FrameHandler _ptrHandle)
{
#ifdef USE_CAN
	LB_CAN_Init(_ptrHandle);
#else
	LB_Layer_Data_Init();
	uint16_t ret = LB_Layer2_Init(_ptrHandle);

#endif
	return ret;
}


uint16_t LB_Layer7_Decode(MsgFrame* ptrMsgFrame)
{
	uint16_t ret = OK;
	assert(ptrMsgFrame);

	Layer2Frame* frame = &ptrMsgFrame->frame;

	assert(frame);

	assert(frame->St.function < FUNCTION_MSK);
	uint8_t func = frame->St.function & FUNCTION_MSK;
	switch(func)
	{
		case CMD_POLL:
			ret = LB_APP_Poll(ptrMsgFrame);
			break;
		case CMD_WRITE_OBJ:
			ret = LB_APP_WriteObj(ptrMsgFrame);
			break;
		case CMD_READ_OBJ:

		case CMD_READ_TYPE:
		case CMD_READ_LENGTH:
		case CMD_READ_ATR_NUM:
		case CMD_READ_NAME:
		case CMD_READ_RANGE:
			//todo
			ret = LB_APP_UnSupport(ptrMsgFrame);
			break;

		case CMD_READ_MEM:
			ret = LB_APP_ReadMem(ptrMsgFrame);
			break;
		case CMD_WRITE_MEM:
			ret = LB_APP_WriteMem(ptrMsgFrame);
			break;
		default:
			//not support yet;
			ret = LB_APP_UnSupport(ptrMsgFrame);
			break;
	}
	return ret;
}

