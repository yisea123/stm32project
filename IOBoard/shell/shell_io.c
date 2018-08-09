
#include "cmsis_os.h"
#include "shell_io.h"
#include "main.h"
//#include "Driver_USART.h"
#include "stm32f0xx_hal.h"
#include "cmd.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
//extern ARM_DRIVER_USART Driver_USART0;
//static ARM_DRIVER_USART * USARTdrv;

extern UART_HandleTypeDef huart1;
extern SemaphoreHandle_t shellLock;

#define UNUSED_VARIABLE(N)  do { (void)(N); } while (0)


#define SHELL_MAX_ARGS (1+8) /*arg[0] is the cmd name, arg[1]...arg[N-1] are the actual cmd args*/

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

	MX_USART1_UART_Init();
	HAL_UART_Receive_IT(&huart1, (uint8_t *)revBuff, (uint16_t)MAX_BUFF_SIZE);
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
static int shell_func(shell_context_t *pShell, uint8_t* lines)
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
			pCmd->func(pShell->argc, pShell->argv,(char*)lines);
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

int shell_init(void)
{
	InitShell_HW();
#if 0
	shell_printf("\r\n\r\n");
	shell_printf(" #########              ###              ####\r\n");
	shell_printf(" ###########                             ####\r\n");
	shell_printf(" ###    ####  #######   ###  ##########  #########    #######   ###  ####  ####\r\n");
	shell_printf(" #########       #####  ###  ##### ####  ####  #### ####  ##### #### ##### ###\r\n");
	shell_printf(" ###  ###    #### ####  ###  ####  ####  ####  #### ####   ####  ############\r\n");
	shell_printf(" ###   ####  #### #### ####  ####  ####  #########   #########    ####  ####\r\n");
	shell_printf("\r\n");
	shell_printf("Welcome to dosing board CLI. Type \"i\" for help.\r\n");
#endif
	
	return 0;
}


extern osMessageQId SHELL_RX_ID;
#define UART_BUFF_SIZE 		128
static uint8_t shellData1[UART_BUFF_SIZE];
static uint8_t shellData1Len = 0;
uint16_t NewUartData(uint16_t type, uint8_t* ptrData)
{
	uint8_t data = *ptrData;

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
			MsgPush(SHELL_RX_ID, (uint32_t)&shellData1[0],0);
		}
		else if('\r' == data)
		{
			MsgPush(SHELL_RX_ID, 0,0);
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
#define _LOC_BUF_LEN    64
static uint8_t locBuf[_LOC_BUF_LEN+2];

static void LineCpy(uint8_t* __locBuf, const uint8_t* data)
{
	uint8_t idx = 0;
    for(;;)
    {

        if(*data >= 32 )//((*data++ != '\r') && (*data != '\n'))
            *(__locBuf++) = *(data++);
        else
        {

            *(__locBuf) = '\0';
            break;
        }
        if(++idx >= _LOC_BUF_LEN)
        {
            break;
        }
    }
}

void StartShellRXTask(void const * argument)
{
	(void)argument; // pc lint
	shell_context_t shell = {0};
	osEvent event;
	uint32_t tickOut = osWaitForever;
	shellLock = OS_CreateSemaphore();
	shell_init();
	while(1)
	{
		event = osMessageGet(SHELL_RX_ID, tickOut );
		if(event.status == osEventMessage )
		{
			uint8_t* data = (uint8_t*) event.value.p;
			if(event.value.v == 0)
				shell_Add("\n>>");
			else
			{
	            LineCpy(locBuf,data);
				shell.cmdLine = (char*)(data);
			//	shell_Add(shell.cmdLine);
				shell.argc = iParse_command_line(shell.cmdLine, shell.argv);
				shell_func(&shell, locBuf);
			}

		}
	}
}

static int shell_write(const char* buf, int cnt, void *extobj)
{
	UNUSED_VARIABLE(extobj);
	HAL_UART_Transmit(&huart1, (uint8_t*)buf, (uint16_t)cnt, (uint32_t)(cnt) );
	return cnt;
}

//

static char PrintBuff[MAX_PRINT_SIZE];
volatile int sizePrint = 0;
extern uint32_t fctTest;

void shell_printf(uint16_t chn, const char* str, ...)
{
	if((fctTest == 0)||(chn ==0))
	{
		va_list arp;
		int size;
		OS_Use(shellLock);
		va_start(arp, str);
		size = vsnprintf(PrintBuff, MAX_PRINT_SIZE, str,arp);
		va_end(arp);
		shell_write(PrintBuff,size,0);
		OS_Unuse(shellLock);
	}
}

