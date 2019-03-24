/*
 * LB_Layer2_Proxy.c
 *
 *  Created on: 2017骞�5鏈�19鏃�
 *      Author: pli
 */

#include <string.h>
#include "main.h"
#include "LB_Layer_data.h"
#include "LB_Layer2_Uart.h"
//#include "LB_Layer2_Can.h"
#include "LB_Layer2.h"
#include "unit_rtc_cfg.h"

static FrameHandler	ptrLayer7RXHandle = NULL;



uint16_t LB_Layer2_Init(FrameHandler rxHandle)
{
	uint16_t ret = OK;
	LB_Layer2_Uart_Init(LB_Layer2_Rx);
//	LB_Layer2_Can_Init(LB_Layer2_Rx);

	ptrLayer7RXHandle = rxHandle;
	return ret;
}


uint16_t LB_Layer2_Tx(MsgFrame* ptrMsgFrame)
{
	uint16_t ret = OK;

	uint16_t len = (uint16_t)( GetFrameLen(&ptrMsgFrame->frame) - CRC_DATA_LEN );

	*(uint16_t*)&ptrMsgFrame->frame.data[len] = CalcCrc16Mem_COMMON(ptrMsgFrame->frame.data, InitCRC16_COMMON(), len);

	if(ptrMsgFrame->typeMsg == FROM_CAN)
	{
//		ret = LB_Layer2_Can_Tx(ptrMsgFrame);
	}
	else
	{
		ret = LB_Layer2_Uart_Tx(ptrMsgFrame);
	}
	FreeFrameBuff(ptrMsgFrame);
	return ret;
}


uint16_t LB_Layer2_Rx(MsgFrame* ptrMsgFrame)
{
	uint16_t ret = OK;
	uint16_t crc = 0;
	uint16_t len = GetFrameLen(&ptrMsgFrame->frame);
	if(len > CRC_LEN)
	{
		crc = CalcCrc16Mem_COMMON(ptrMsgFrame->frame.data, InitCRC16_COMMON(), (uint16_t)(len-CRC_LEN));
		if(*(uint16_t*)&ptrMsgFrame->frame.data[len-CRC_LEN] != crc)
		{
			TraceDBG(TSK_ID_LOCAL_BUS,"Error Frame, CRC check error\n");
			ret = RULE_VIOLATION;
		}
		else
		{
			ret = OK;
			if(ptrMsgFrame->frame.St.dst == SLAVE_ADR)
			{

				if(ptrLayer7RXHandle)
				{
					ret = ptrLayer7RXHandle(ptrMsgFrame);
				}
			}
			else
			{
				TraceDBG(TSK_ID_LOCAL_BUS,"Error Frame, addr error\n");
			}
		}
	}
	else
	{
		TraceDBG(TSK_ID_LOCAL_BUS,"Error len, \n");
		ret = FATAL_ERROR;
	}


	return ret;
}
