/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "shell_io.h"
#include "cmsis_os.h"
#include "t_data_obj.h"
#include "t_unit.h"
#include "Common_crc.h"


#pragma GCC diagnostic ignored "-Wpadded"



#define TEST_PIN_H		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3 , GPIO_PIN_SET);
#define TEST_PIN_L		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3 , GPIO_PIN_RESET);
#define TEST_PIN_TOGGLE	 HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_3 );


#define TEST_PIN2_H		 HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2 , GPIO_PIN_SET);
#define TEST_PIN2_L		 HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2 , GPIO_PIN_RESET);
#define TEST_PIN2_TOGGLE	 HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_2 );

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
void SetLedOutMask(uint16_t id, uint16_t val);
void assert_failed(uint8_t* file, uint32_t line);
int trace_printf(const char* format, ...);
int dbg_printf(const char* format, ...);
void delay_us(const uint32_t count);


uint32_t GetTickDeviation(const uint32_t oldTick, const uint32_t reqTick);
int32_t TickTimeDiff(const uint32_t oldTick, const uint32_t reqTick);
void SetLedCurrentGain(void);

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

