/*
 * shell_io.c
 *
 *  Created on: 2017Äê9ÔÂ26ÈÕ
 *      Author: pli
 */


#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "cmd.h"
#include "usart.h"
#include "main.h"
#include "rtc.h"
#include "shell_io.h"


//extern ARM_DRIVER_USART Driver_USART0;
//static ARM_DRIVER_USART * USARTdrv;
#ifdef TEST_L_BUS
extern UART_HandleTypeDef huart3;

#define shellUart		(&huart3)

#else
extern UART_HandleTypeDef huart6;
#define shellUart		(&huart6)
#endif


#define SHELL_MAX_ARGS (1+8) /*arg[0] is the cmd name, arg[1]...arg[N-1] are the actual cmd args*/


uint16_t shellStatus = 0;

typedef struct shell_context_tag {
    char *argv[SHELL_MAX_ARGS];
    int argc;
    char* cmdLine;
    unsigned int cmdNumber;
    bool exitShell;

}shell_context_t;


static void InitShell_HW(void)
{

#define MAX_BUFF_SIZE	4
	static uint8_t revBuff[MAX_BUFF_SIZE];
//	__USART2_FORCE_RESET();
//	__USART2_RELEASE_RESET();
	if(shellUart->Instance)
		HAL_UART_DeInit(shellUart);
	MX_USART6_UART_Init();
	HAL_UART_Receive_IT(shellUart, (uint8_t *)revBuff, (uint16_t)MAX_BUFF_SIZE);
}


inline static bool iIsEffectiveChar(char c)
{
    return ('\x20' != c && '\t' != c && '\n' != c && '\r' != c);
}

static int iParse_command_line( char* command_line_ptr, char * argv[ ] )
{
    char *sptr;
    int i;
    int argc;

    sptr = command_line_ptr;
    argc = 0;
    for (i=0;i<SHELL_MAX_ARGS;i++)  {
        while (*sptr && !iIsEffectiveChar(*sptr)) { ++sptr; }
        if (!*sptr) {
            argv[i] = 0;
        } else {
            ++argc;
            argv[i] = sptr;
            while (*sptr && iIsEffectiveChar(*sptr)) { ++sptr; }
            if (*sptr) {
                *sptr++ = '\0';
            }
        }
    }
    return argc;
}


extern const command_table_t cmdlist[];
static int shell_func(shell_context_t *pShell)
{
	uint16_t execnt = 0;
	const command_table_t *pCmd = cmdlist;
	if (0 == pShell || 0 == cmdlist)
	{
		return 0;
	}
	if (pShell->argc < 0 || 0 == pShell->argv[0])
	{
		return 0;
	}
	const char * cmdName = pShell->argv[0];

	 while(pCmd->command != 0)
	{
		if (strcmp(cmdName, pCmd->command) == 0)
		{
			pCmd->func(pShell->argc, pShell->argv);
			execnt++;
			break;
		}
		++pCmd;
	}

	if (execnt == 0)
	{
		cmd_i();
	}

	return 0;
}



static uint32_t testDbg = 10;

static int shell_init(void)
{


	shell_Add("\n\n\n\n");
	shell_Add(" #########              ###              ####\n");
	shell_Add(" ###########                             ####\n");
	shell_Add(" ###    ####  #######   ###  ##########  #########    #######   ###  ####  ####\n");
	shell_Add(" #########       #####  ###  ##### ####  ####  #### ####  ##### #### ##### ###\n");
	shell_Add(" ###  ###    #### ####  ###  ####  ####  ####  #### ####   ####  ############\n");
	shell_Add(" ###   ####  #### #### ####  ####  ####  #########   #########    ####  ####\n");
	shell_Add("\n");
	shell_Add("Welcome to Measure board CLI. Type \"i\" for help.\n\n\n\n");
	testDbg = 100;

	return 0;
}


#if 0
static void shell_printf(const char* str, ...)
{
	va_list arp;

	PrintSt* buffSt = GetAvailablePrintBuff(MAX_PRINT_SIZE);
	int len = 0;
	//lint -e586
	va_start(arp, str);
	//lint -e586
	len += vsnprintf((char*)&buffSt->buff[len], MAX_PRINT_SIZE-len, str,arp);
	//lint -e586
	va_end(arp);
	assert(len<0xff);
	buffSt->len = (uint16_t)len;
	shellSend((const char*)&buffSt->buff[0],buffSt->len, DMA_USART);

}

#endif
static const uint8_t dbgStr[] = "\n\n\n\n\n System Failed Dump Msg!\n\n\n\n";
void SendBugMsg(void)
{
	InitShell_HW();
	if(validPrintMsg == VALID_DBG_MSG)
	{


		uint16_t size = sizeof(dbgStr);

		shellSend((const char*)&dbgStr[0], size, DMA_USART);

		if(validPrintLen != 0)
		{

			if(validPrintLen >= DUMP_SIZE)
			{
				int printLen = (int)(validPrintLen%DUMP_SIZE);
				shellSend((const char*)&printfbuffDump[0],printLen, DMA_USART);

				shellSend((const char*)&printfbuffDump[printLen],DUMP_SIZE - printLen, DMA_USART);
				validPrintLen = validPrintLen % (2*DUMP_SIZE);
			}
			else
			{
				shellSend((const char*)&printfbuffDump[0],(int)(validPrintLen), DMA_USART);
			}

		}
		else
		{
			shellSend((const char*)&dbgStr[0],size, DMA_USART);
			osDelay((uint32_t)(size/8+4));
		}

	}
}



void StartShellRXTask(void const * argument)
{
	(void)argument; // pc lint
	shell_context_t shell = {0};
	osEvent event;
	uint32_t tickOut = osWaitForever;

	const uint8_t taskID = TSK_ID_SHELL_RX;
	while(1)
	{
		event = osMessageGet(SHELL_RX_ID, tickOut );

		if(event.status == osEventMessage )
		{
			if(event.value.v == 0)
				shell_Add_Rawdata("\n>>",sizeof("\n>>"));
			else
			{

				shell.cmdLine = (char*)(event.value.p);
				shell_Add(shell.cmdLine);
				shell.argc = iParse_command_line(shell.cmdLine, shell.argv);
				shell_func(&shell);
			}

		}
	}
}

static const uint8_t resetStr[] = "\n\n\t\t\t\tDevice Reset is called!\n\n";
void StartShellTXTask(void const * argument)
{
	(void)argument; // pc lint
	osEvent event;
	uint16_t whileCount = 0;
	SendBugMsg();
	shell_init();
	while (TASK_LOOP_ST)
	{
		event = osMessageGet(SHELL_TX_ID, osWaitForever);
		if (event.status == osEventMessage)
		{

		//	xprintf("test output \n");
			PrintSt* buffSt = (PrintSt*) event.value.p;
			if(buffSt)
			{
				whileCount = 0;
				while((shellUart->gState != HAL_UART_STATE_READY) && (whileCount++ < 300))
				{
					osDelay(2);
				}
				if(shellUart->gState != HAL_UART_STATE_READY)
				{
					InitShell_HW();
					TraceDBG(0xFFFF, "UART 2 force reset\nUART 2 force reset\nUART 2 force reset\nUART 2 force reset\n");

				}
			//	else
				{
					if((buffSt->len<0x00ff) && (buffSt->len > 0))
					{
					//	assert(buffSt->len<0x00ff);
						if(buffSt->len<0x00ff)
						{
							shellSend((const char*)&buffSt->buff[0],buffSt->len, DMA_USART);
						}
					}
				}
			}
			else
			{

				shellSend((const char*)&resetStr[0],sizeof(resetStr), DMA_USART);
				osDelay((uint32_t)(sizeof(resetStr)/8+4));
				ResetDevice(0);
			}
		}
	}

}

int shellSend(const char* buf, int cnt, uint8_t dma)
{
	/*
	assert(!(((uint32_t)buf > 0x10000000) && ((uint32_t)buf < 0x20000000)));//cc_arm
	HAL_StatusTypeDef ret;
	if(dma)
		ret = HAL_UART_Transmit_DMA(shellUart, (uint8_t*)buf, (uint16_t)cnt);
	else
		ret = HAL_UART_Transmit(shellUart, (uint8_t*)buf, (uint16_t)cnt, cnt/7+4);
	if(ret != HAL_OK)
	{
		InitShell_HW();
		TraceDBG(TSK_ID_SHELL_RX,"Uart Tx Error: %d",ret);
	}
	else
	{
		if(dma)
			osDelay((uint32_t)(cnt/7+4));
	}
	*/
	//input IO
	if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
	{
		shellStatus = 1;
	}
	//else if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3))
	//{
	//	shellStatus = 1;
	//}
	else
	{
		shellStatus = 0;
	}
	if((dma == OTHER_USART) || (shellStatus == 1))
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_UART_Transmit(shellUart, (uint8_t*)buf, (uint16_t)cnt, 10000);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	}

	return cnt;
}




//
void shell_Add(const char* str, ...)
{
	int len = 0;
	va_list arp;
	PrintSt* buffSt = GetAvailablePrintBuff(MAX_PRINT_SIZE);

	//lint -e586
	va_start(arp, str);
	//lint -e586
	len = (uint16_t)vsnprintf((char*)&buffSt->buff[0], MAX_PRINT_SIZE, str,arp);
	//lint -e586
	va_end(arp);
	buffSt->len = (uint16_t)len;
	assert(buffSt->len<0xff);
	MsgPush (SHELL_TX_ID, (uint32_t)buffSt, 0);
}


void shell_Add_Rawdata(const char* str, uint16_t cnt)
{
	PrintSt* buffSt = GetAvailablePrintBuff( (uint16_t)(cnt));
	buffSt->len = cnt;
	memcpy((void*)&buffSt->buff[0], (const void*)str, cnt);
	assert(buffSt->len<0xff);
	MsgPush (SHELL_TX_ID, (uint32_t)buffSt, 0);
}
void shell_AddRTC(const char* str, ...)
{
	va_list arp;
	PrintSt* buffSt = GetAvailablePrintBuff(MAX_PRINT_SIZE);
	int len = sprintf_rtc(&buffSt->buff[0], MAX_PRINT_SIZE);
	//lint -e586
	va_start(arp, str);
	//lint -e586
	len += vsnprintf((char*)&buffSt->buff[len], MAX_PRINT_SIZE-len, str,arp);
	//lint -e586
	va_end(arp);
	assert(len<0xff);
	buffSt->len = (uint16_t)len;
	MsgPush ( SHELL_TX_ID, (uint32_t)buffSt, 0);
	return ;
}



