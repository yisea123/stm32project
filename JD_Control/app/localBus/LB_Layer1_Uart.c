/*
 * LB_Layer1.c
 *
 *  Created on: 2016��12��12��
 *      Author: pli
 */


#include <string.h>
#include "main.h"
#include "usart.h"
#include "tim.h"
#include "LB_Layer_Data.h"
#include "LB_Layer1_Uart.h"
#include "shell_io.h"

#define  LB_UART_INIT		MX_USART1_UART_Init
#define  LB_UART			huart1

#define  UART_TIMER_INIT	MX_TIM7_Init
#define  UART_TIMER			htim7

static RxCallBack		layer2Rx = NULL;

uint16_t uartIdx = 0;
static uint8_t RxBuff[LB_FRAME_BUF_LEN];


uint16_t LB_Layer1_Uart_Init(RxCallBack callBack)
{
	uint16_t ret = OK;
		//do hw init
	LB_UART_INIT();
		//do timer init
	UART_TIMER_INIT();
	uartIdx = 0;

	static uint8_t revBuff[4];
	HAL_UART_Receive_IT(&LB_UART, (uint8_t *)revBuff, (uint16_t)4);

	layer2Rx = callBack;
	return ret;
}



uint16_t LB_Layer1_Uart_Rx(uint8_t data)
{
	uint16_t ret = OK;
	if(LB_FRAME_BUF_LEN <= uartIdx)
	{
		TraceDBG(TSK_ID_LOCAL_BUS, "LB RX length failed!\n");
		uartIdx = 0;
	}
	RxBuff[uartIdx++] = data;
	LB_Layer1_StartTimer();
	return ret;
}



#define UART3_DMA
uint16_t LB_Layer1_Uart_Tx(const uint8_t* data, uint16_t len)
{
	uint16_t ret = OK;

#ifdef UART3_DMA
	uint16_t count = 1000;
	while ( (--count > 0) && LB_UART.gState != HAL_UART_STATE_READY)
	{
		// Make sure the char was sent
		osDelay(5);
	}
	ret = HAL_UART_Transmit_DMA(&LB_UART, (void*)&data[0], len);
#else
//	static uint8_t txBuff[30];
//	memset(txBuff,0xE9,29 );
//	len = 30;
	ret = HAL_UART_Transmit(&LB_UART,&data[0], len, (uint32_t)(len/8+3) );


#endif
	if(ret != HAL_OK)
	{
		LB_Layer1_Uart_Init(layer2Rx);
		//reinit uart
		TraceDBG(TSK_ID_LOCAL_BUS,"Uart Tx Message:len: %d; %x %x %x %x, %x %x %x %x\n",\
						len, data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
	}
	TracePrint(TSK_ID_LOCAL_BUS,"Uart Tx Message:len: %d; %x %x %x %x, %x %x %x %x\n",\
				len, data[0], data[1],data[2],data[3],data[4],data[5],data[6],data[7]);

	return ret;
}

void LB_Layer1_StartTimer(void)
{
	//start timer
	UART_TIMER.Instance->DIER |= 0x01;
	UART_TIMER.Instance->CNT = 0;
	UART_TIMER.Instance->CR1 |= 0x01; //start

}

void LB_Layer1_TimerOut(void)
{
	UART_TIMER.Instance->DIER &= 0xFE;
	UART_TIMER.Instance->CR1 &= 0xFE;//stop
	UART_TIMER.Instance->CNT = 0;

	if(layer2Rx)
	{
		if(uartIdx)		layer2Rx(RxBuff, uartIdx);
	}
	uartIdx = 0;
}


