/*
* printf.c
*
*  Created on: 2017Äê9ÔÂ26ÈÕ
*      Author: pli
*/



// Retarget stdio and stderr to shell IO
#include <stdarg.h>
#include <string.h>
#include "main.h"
#include "rtc.h"
#include "usart.h"
#include "shell_io.h"
#include "unit_flow_act.h"
extern UART_HandleTypeDef huart1;




#define PRINT_USART		(&huart1)



typedef enum
{
	PRINT_IDLE,
	PRINT_RUN,
	PRINT_RUN_DELAY,
	PRINT_FINISH,
}PRINT_STATE;

static uint8_t printBuff[MAX_PRINT_BUFF_SIZE];
#define PRINT_TRACE			trace_printf
static SemaphoreHandle_t lockBuff = NULL;
static SemaphoreHandle_t lockPrint = NULL;
static SemaphoreHandle_t lockUart1 = NULL;
PrintSt* GetAvailablePrintBuff(uint16_t len)
{
	static uint32_t  idxBuff = 0;
	len = (uint16_t)(len + LEN_T);

	OS_Use(lockBuff);
	idxBuff = (idxBuff+3)/4*4;
	if(idxBuff + len< MAX_PRINT_BUFF_SIZE)
	{
	}
	else
	{
		idxBuff = 0;
	}

	uint8_t* buff = &printBuff[idxBuff];
	idxBuff += len ;
	OS_Unuse(lockBuff);

	return (PrintSt*)buff;
}


extern uint16_t printCtrlCfg[MSG_TYPE_MAX];

static uint16_t printfEnable(uint16_t type)
{
    uint16_t ret = 0;
    if(type < MSG_TYPE_MAX)
        ret = printCtrlCfg[type];
    
    return ret;
}

static uint16_t CheckMsgPrintEnable(uint8_t* msgTable, uint16_t chn, uint16_t type)
{
	uint16_t ret = 0;
	ret = PRINT_MAP_NONE;
	if(chn >= MAX_TASK_ID )
	{
		ret = PRINT_MAP_SHELL;
	}
	else if(msgTable[chn] < PRINT_MAP_MAX)
	{
		if(printfEnable( type ))
		{
			ret = msgTable[chn];
		}
	}
	else
	{}
	return ret;
}




void Init_PrintSem()
{
	if(lockBuff == NULL)	lockBuff = OS_CreateSemaphore();
	if(lockPrint == NULL)	lockPrint = OS_CreateSemaphore();
	if(lockUart1 == NULL)	lockUart1 = OS_CreateSemaphore();

}
void PrintChn(uint8_t* msgTable,uint8_t enabled, uint16_t chn, const char* str, ...)
{
	static uint8_t lcBuff[MAX_PRINT_SIZE+8];
	uint16_t chnPrint = CheckMsgPrintEnable(msgTable,chn,enabled);
	PrintSt* buffStRet = NULL;

	va_list arp;

	int len = 0;

	if(chnPrint != MAP_NONE )
	{
		OS_Use(lockPrint);
		va_start(arp, str);


		PrintSt* buffSt = (PrintSt*)&lcBuff[0];

		len = sprintf_rtc(&buffSt->buff[0], MAX_PRINT_SIZE);

		if(enabled == 2)
		{
			len  += snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[31m");
		}
		else if(enabled == 1)
		{
			len  +=  snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[33m");
		}
		else if(enabled == 3)
		{
			len  +=  snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[35m");
		}
		else
		{}

		if(chn < MAX_TASK_ID)
		{
			len  += snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"%s:\t",TskName[chn]);
		}
		//lint -e586
		va_start(arp, str);
		//lint -e586
		len += vsnprintf((char*)&buffSt->buff[len], (size_t)(MAX_PRINT_SIZE-(size_t)len) , str,arp);
		//lint -e586
		va_end(arp);

		if(enabled)
			len  += snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[0m");
		buffSt->len = (uint16_t)len;

		buffStRet = GetAvailablePrintBuff( (uint16_t)(len+1));

		memcpy((void*)buffStRet, (void*)buffSt, (uint16_t)(len + 3));
		va_end(arp);
		OS_Unuse(lockPrint);
		assert(buffStRet->len < MAX_PRINT_SIZE);
	}


	switch(chnPrint)
	{
	case MAP_TRACE:
		trace_write((void*)&buffStRet->buff[0], buffStRet->len, 0);
		break;
	case MAP_SHELL:
		MsgPush ( SHELL_TX_ID, (uint32_t)buffStRet, 0);
		break;
	case MAP_XPRINTFF:
		MsgPush ( PRINT_ID, (uint32_t)buffStRet, 0);
		break;

	default:
		break;

	}




}
void PrintChn1(uint8_t* msgTable,uint8_t enabled, uint16_t chn, const char* str, ...)
{
	static uint8_t lcBuff[MAX_PRINT_SIZE+8];
	uint16_t chnPrint = CheckMsgPrintEnable(msgTable,chn,enabled);
	PrintSt* buffStRet = NULL;

	va_list arp;

	int len = 0;

	if(chnPrint != MAP_NONE )
	{
		OS_Use(lockPrint);
		va_start(arp, str);


		PrintSt* buffSt = (PrintSt*)&lcBuff[0];

		len = sprintf_rtc(&buffSt->buff[0], MAX_PRINT_SIZE);

		if(enabled == 2)
		{
			len  += snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[31m");
		}
		else if(enabled == 1)
		{
			len  +=  snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[33m");
		}
		else if(enabled == 3)
		{
			len  +=  snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[35m");
		}
		else
		{}

		if(chn < MAX_TASK_ID)
		{
			len  += snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"%s:\t",TskName[chn]);
		}
		//lint -e586
		va_start(arp, str);
		//lint -e586
		len += vsnprintf((char*)&buffSt->buff[len], (size_t)(MAX_PRINT_SIZE-(size_t)len) , str,arp);
		//lint -e586
		va_end(arp);

		if(enabled)
			len  += snprintf((char*)&buffSt->buff[len],(size_t)(MAX_PRINT_SIZE-(size_t)len) ,"\033[0m");
		buffSt->len = (uint16_t)len;

		buffStRet = GetAvailablePrintBuff( (uint16_t)(len+1));

		memcpy((void*)buffStRet, (void*)buffSt, (uint16_t)(len + 3));
		va_end(arp);
		OS_Unuse(lockPrint);
		assert(buffStRet->len < MAX_PRINT_SIZE);
	}


	switch(chnPrint)
	{
	case MAP_TRACE:
		trace_write((void*)&buffStRet->buff[0], buffStRet->len, 0);
		break;
	case MAP_SHELL:
		MsgPush ( SHELL_TX_ID, (uint32_t)buffStRet, 0);
		break;
	case MAP_XPRINTFF:
		MsgPush ( PRINT_ID, (uint32_t)buffStRet, 0);
		break;

	default:
		break;

	}




}
void Print_SendRaw(const uint8_t* buff, uint16_t len)
{
	OS_Use(lockUart1);
	//todo, add semaphore
	HAL_UART_Transmit_DMA(PRINT_USART, (void*)&buff[0], len);
	osDelay( (uint32_t)(len/8+10));
	OS_Unuse(lockUart1);
}


void StartPrintTask(void const * argument);

void StartPrintTask(void const * argument)
{
	(void)argument; // pc lint
#define PRINT_DATA_SIZE         150
#define REFRESH_TIME_CYC        1000
	static uint8_t printData[PRINT_DATA_SIZE];
	osEvent event;
	uint32_t count = 0;
	uint32_t tickOut = 0;
	uint32_t tickLast = 0;
	uint32_t calcTick = 0;
	uint32_t osTick;
	const uint16_t taskID = TSK_ID_PRINT;
	MX_USART1_UART_Init();
	TracePrint(TSK_ID_PRINT,"task started  \n");
	//HAL_UART_Receive_IT(PRINT_USART, (uint8_t *)revBuff, (uint16_t)MAX_BUFF_SIZE);
	while(TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		event = osMessageGet(PRINT_ID, tickOut );
		if(FVT_Test)
		{

		}
		else
		{
			osTick = HAL_GetTick();
			if (event.status == osEventMessage)
			{
				assert(!(((uint32_t)event.value.v > 0x10000000) && ((uint32_t)event.value.v < 0x20000000)));//cc_arm
				PrintSt* buffSt = (PrintSt*)(event.value.p);

				Print_SendRaw( &buffSt->buff[0], buffSt->len);
				count = 0;
				if ( (PRINT_USART)->gState != HAL_UART_STATE_READY && count++ <100)
				{
					// Make sure the char was sent
					osDelay(5);
				}

				if( (PRINT_USART)->gState != HAL_UART_STATE_READY)
				{
					MX_USART1_UART_Init();
					TraceDBG(TSK_ID_PRINT, "UART 1 force reset\nUART 1 force reset\nUART 1 force reset\nUART 1 force reset\n");
				}

			}
			else
			{
				tickLast = osTick;
				int len = sprintf_rtc(&printData[0], PRINT_DATA_SIZE);
				//measure ad,refer ad,led0,led1,t0,t1,t_pcb,mainstep,substep,microstep
/*
				len += snprintf((void*)&printData[len],PRINT_DATA_SIZE-len,"C, %.4f, %.4f,\tAD: %5d, %5d,\tLed, %4d, %4d,\tTemp, %4d, %5d, %4d,\tStep, %3d, %3d, %3d, %3d, %3d,\n",\
						realTimeAbs, \
						realTimeConcentration,\
						advalFinal[ID_VAL_MEAS],\
						advalFinal[ID_VAL_REF],\
						 advalFinal[ID_VAL_LED_LV0],\
						 advalFinal[ID_VAL_LED_LV1],\
						 advalFinal[ID_TEMPERATURE0],\
						 advalFinal[ID_TEMPERATURE1],\
						 advalFinal[ID_PCB_TEMP],\
						 gFlowStep.mainStep,\
						 gFlowStep.preMain,\
						 gFlowStep.subStep,\
						 gFlowStep.preSub,\
						 gFlowStep.microStep
						);

			   printSend( printData, len);
			   */
			}
			osTick = HAL_GetTick();
			if(osTick > tickLast)
				calcTick = (osTick - tickLast);
			else
				calcTick = (osTick + (0xFFFFFFFF-tickLast));

			if(calcTick >= REFRESH_TIME_CYC)
				tickOut = 0;
			else
				tickOut = REFRESH_TIME_CYC - calcTick;
		}
	}
}


void xprintfRTC(const char* str, ...)
{
	va_list arp;
	PrintSt* buffSt = GetAvailablePrintBuff(MAX_PRINT_SIZE);
	int len = sprintf_rtc(&buffSt->buff[0], MAX_PRINT_SIZE);
	//lint -e586
	va_start(arp, str);
	//lint -e586
	len += (int)vsnprintf((char*)&buffSt->buff[len], (size_t)(MAX_PRINT_SIZE-(size_t)len), str,arp);
	//lint -e586
	va_end(arp);
	buffSt->len = (uint16_t)len;
	assert(buffSt->len<0xff);
	MsgPush ( PRINT_ID, (uint32_t)buffSt, 0);


	return ;

}

void xprintf(const char* str, ...)
{

	va_list arp;

	PrintSt* buffSt = GetAvailablePrintBuff(MAX_PRINT_SIZE);
	//lint -e586
	va_start(arp, str);
	//lint -e586
	buffSt->len = (uint8_t)vsnprintf((char*)&buffSt->buff[0], MAX_PRINT_SIZE, str,arp);
	//lint -e586
	va_end(arp);
	assert(buffSt->len<0xff);
	MsgPush ( PRINT_ID, (uint32_t)buffSt, 0);
	return ;
}



