/*
* shell_hw.c
*
*  Created on: 2018Äê2ÔÂ5ÈÕ
*      Author: pli
*/
#include "includes.h"
#include "ManufTest.h"
#include "cod.h"
#include "cod_hw.h"


                          
#define UART_BUFF_SIZE 		128
#define UART_IDX_MSK		(0x00007FU)
static uint8_t shellData1[UART_BUFF_SIZE];
static uint8_t shellData1Len = 0;
void Usart3RXHandle(uint8_t dataRx)
{
	uint8_t data = dataRx;
    
	static uint8_t uartBuff[UART_BUFF_SIZE];
	static uint8_t uartShellIdx = 0;
	if ('\r' == data ||'\n' == data)
	{
		if(uartShellIdx != 0)
		{
			uartBuff[uartShellIdx++] = '\n';
			memset((void*)shellData1,0,sizeof(shellData1));
			memcpy((void*)shellData1,(void*)uartBuff,uartShellIdx);
			shellData1Len = uartShellIdx;
			SendTskMsgINT(&MB_UARTRx, TSK_INIT, (uint32_t)&shellData1[0], NULL,NULL);
		}
		else if('\r' == data)
		{
			SendTskMsgINT(&MB_UARTRx, TSK_INIT, (uint32_t)0, NULL,NULL);
		}
		uartShellIdx = 0;
	}
	else
	{
		if(data != 127)//backspace
			uartBuff[uartShellIdx++] = data;
		else if(uartShellIdx)
			uartShellIdx--;
		if(uartShellIdx >= UART_BUFF_SIZE)
			uartShellIdx = 0;
	}
    
}