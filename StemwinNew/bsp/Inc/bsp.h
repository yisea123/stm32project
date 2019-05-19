/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define WDI_TRIGGER		HAL_GPIO_TogglePin(WDI_GPIO_Port,WDI_Pin)

#pragma GCC diagnostic ignored "-Wpadded"

#define 	LEN_T		2 //sizeof(uint16_t);

#define TEST_PIN_H		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3 , GPIO_PIN_SET);
#define TEST_PIN_L		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3 , GPIO_PIN_RESET);
#define TEST_PIN_TOGGLE	 HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_3 );


#define TEST_PIN2_H		 HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2 , GPIO_PIN_SET);
#define TEST_PIN2_L		 HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2 , GPIO_PIN_RESET);
#define TEST_PIN2_TOGGLE	 HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_2 );

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	uint16_t	len;
	uint8_t		buff[2];
}PrintSt;


#pragma pack(pop)

typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
} PinInst;


#pragma GCC diagnostic pop

#define EVENT_MAX_DELAY		5


#define OFFSET_OF(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER )
#define IN_RANGE(x,min,max)		(((x)>=(min)) && ((x)<(max)))


void* MallocMem(uint16_t size);
void FreeMem(void*);
void bsp_init(void);
void SystemClock_Config(void);
void Error_Handler(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void InitMeasPins(void);
void InitNVICPins(void);

void assert_failed(uint8_t* file, uint32_t line);
int trace_printf(const char* format, ...);
int dbg_printf(const char* format, ...);
void delay_us(const uint32_t count);


uint32_t GetTickDeviation(const uint32_t oldTick, const uint32_t reqTick);
int32_t TickTimeDiff(const uint32_t oldTick, const uint32_t reqTick);


void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
uint16_t NewUartData(uint16_t type, uint8_t* ptrData);
void Usart2RXHandle(void);
void Usart3RXHandle(void);


enum
{
	MANUAL_STATE= RTC_BKP_DR6,
	INTERVAL_SET,
	TIME_EXEC,
	ADR_FLASHSTART,
	ADR_ENDFLASH,
};

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

