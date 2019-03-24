/*
 * LB_Layer2.c
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */



#include <string.h>
#include "main.h"
#include "LB_Layer_data.h"
#include "LB_Layer2_Uart.h"
#include "usart.h"

static FrameHandler	layer2Rx = NULL;

#define RX_DST_ADR_IDX		1
#define RX_SRC_ADR_IDX		0
#define RX_FRAME_ID_IDX		2
#define RX_FUNCTION_IDX		3
#define RX_DATA_IDX			4




uint16_t LB_Layer2_Uart_Init(FrameHandler _ptrHandle)
{
	uint16_t ret = OK;
	LB_Layer1_Uart_Init(LB_Layer2_Uart_Rx);
	layer2Rx = _ptrHandle;
	return ret;
}


uint16_t LB_Layer2_Uart_Rx(uint8_t* data, uint16_t len)
{
	uint16_t ret = OK;
	if(len > CRC_LEN)
	{
		MsgFrame* ptrMsgFrame = LB_Layer2_GetNewBuff();
		if((ptrMsgFrame) && (layer2Rx))
		{
			ptrMsgFrame->typeMsg = FROM_UART;
			memcpy((void*)&ptrMsgFrame->frame.data[0],(void*)&data[0], len);
			ret = layer2Rx(ptrMsgFrame);
		}

	}
	else
		ret = FATAL_ERROR;
	return ret;
}


uint16_t LB_Layer2_Uart_Tx(MsgFrame* msgFrame)
{
	uint16_t ret = OK;
	uint16_t len = GetFrameLen(&msgFrame->frame);
	ret = LB_Layer1_Uart_Tx(msgFrame->frame.data, len);
	return ret;
}


