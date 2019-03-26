/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "Common_crc.h"
#include "def.h"

#define 	LEN_T		2 //sizeof(uint16_t);




enum
{
	MAP_TRACE,
	MAP_SHELL,
	MAP_XPRINTFF,
	MAP_NONE,
	MAP_PRINT_MAX,
};

enum
{
    MSG_TYPE_PRINT,
    MSG_TYPE_MSG,
    MSG_TYPE_DBG,
    MSG_TYPE_USER,
    MSG_TYPE_MAX,
};
enum
{
	PRINT_MAP_NONE,
 	PRINT_MAP_SHELL,
	PRINT_MAP_PRINT,

    PRINT_MAP_MAX,
};

#pragma GCC diagnostic ignored "-Wpadded"



typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
} PinInst __attribute__ ((aligned (2)));


#pragma GCC diagnostic pop

#define EVENT_MAX_DELAY		5


#define OFFSET_OF(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER )
#define IN_RANGE(x,min,max)		(((x)>=(min)) && ((x)<(max)))



//shell
#ifdef TEST_L_BUS
extern UART_HandleTypeDef huart3;

#define shellUart		(&huart3)

#else
extern UART_HandleTypeDef huart1;
#define shellUart		(&huart1)
#define ShellUartInit	MX_USART1_UART_Init
#endif





void* MallocMem(uint16_t size);
void FreeMem(void*);
void bsp_init(void);
void SystemClock_Config(void);
void Error_Handler(void);
void InitMeasPins(void);
void InitNVICPins(void);

void assert_failed(uint8_t* file, uint32_t line);
int trace_printf(const char* format, ...);
int dbg_printf(const char* format, ...);
void delay_us(const uint32_t count);
void delayms(uint32_t timeout);

void ShellRXHandle(void);


#ifdef __cplusplus
}
#endif
#endif /*__ __BSP_H */

/**
 * @}
 */

/**
 * @}
 */

