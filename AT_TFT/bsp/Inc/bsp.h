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


#define 	LEN_T		2 //sizeof(uint16_t);




enum
{
	MAP_TRACE,
	MAP_SHELL,
	MAP_NONE,
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

