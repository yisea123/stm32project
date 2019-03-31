/*
 * dev_encoder.h
 *
 *  Created on: 2019��3��30��
 *      Author: pli
 */

#ifndef DEV_ENCODER_H_
#define DEV_ENCODER_H_

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
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


// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��ENCODER_TIMx_PRESCALER+1��
#define ENCODER_TIM_PRESCALER               0

// ���嶨ʱ�����ڣ�����ʱ����ʼ������ENCODER_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define ENCODER_TIM_PERIOD                  0xFFFF

// ʹ��32bits �ļ�������Ϊ����������,F4ϵ�е�TIM2,TIM5
//// ���嶨ʱ�����ڣ�����ʱ����ʼ������ENCODER_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
//#define ENCODER_TIM_PERIOD                  0xFFFFFFFF
//#define CNT_MAX                             4294967295

#define USE_16CNT                       // ʹ��16bits �ļ�������Ϊ����������,F4ϵ�е�TIM3,TIM4
#define ENCODER_TIM_PERIOD                0xFFFF
#define CNT_MAX                           ((int32_t)65536)

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_Encoder;
extern int32_t OverflowCount ;//��ʱ���������
/* �������� ------------------------------------------------------------------*/
void ENCODER_TIMx_Init(void);



enum
{
	MOTOR_DIR_CW = 0,
	MOTOR_DIR_CCW = 1
};


#define MOTOR_HOME_DEVIATION 		50

#endif /* DEV_ENCODER_H_ */
