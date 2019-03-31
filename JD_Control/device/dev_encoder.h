/*
 * dev_encoder.h
 *
 *  Created on: 2019年3月30日
 *      Author: pli
 */

#ifndef DEV_ENCODER_H_
#define DEV_ENCODER_H_

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define ENCODER_TIMx                        TIM3
#define ENCODER_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
#define ENCODER_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()

#define ENCODER_TIM_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define ENCODER_TIM_CH1_PIN                 GPIO_PIN_6
#define ENCODER_TIM_CH1_GPIO                GPIOC
#define ENCODER_TIM_CH2_PIN                 GPIO_PIN_7
#define ENCODER_TIM_CH2_GPIO                GPIOC

#define TIM_ENCODERMODE_TIx                 TIM_ENCODERMODE_TI12

#define ENCODER_TIM_IRQn                    TIM3_IRQn
#define ENCODER_TIM_IRQHANDLER              TIM3_IRQHandler


// 定义定时器预分频，定时器实际时钟频率为：84MHz/（ENCODER_TIMx_PRESCALER+1）
#define ENCODER_TIM_PRESCALER               0

// 定义定时器周期，当定时器开始计数到ENCODER_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define ENCODER_TIM_PERIOD                  0xFFFF

// 使用32bits 的计数器作为编码器计数,F4系列的TIM2,TIM5
//// 定义定时器周期，当定时器开始计数到ENCODER_TIMx_PERIOD值是更新定时器并生成对应事件和中断
//#define ENCODER_TIM_PERIOD                  0xFFFFFFFF
//#define CNT_MAX                             4294967295

#define USE_16CNT                       // 使用16bits 的计数器作为编码器计数,F4系列的TIM3,TIM4
#define ENCODER_TIM_PERIOD                0xFFFF
#define CNT_MAX                           ((int32_t)65536)

/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_Encoder;
extern int32_t OverflowCount ;//定时器溢出次数
/* 函数声明 ------------------------------------------------------------------*/
void ENCODER_TIMx_Init(void);



enum
{
	MOTOR_DIR_CW = 0,
	MOTOR_DIR_CCW = 1
};


#define MOTOR_HOME_DEVIATION 		50

#endif /* DEV_ENCODER_H_ */
