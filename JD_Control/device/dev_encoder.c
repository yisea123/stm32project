/*
 * dev_encoder.c
 *
 *  Created on: 2019��3��30��
 *      Author: pli
 */



#include "dev_encoder.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
int32_t OverflowCount = 0;//��ʱ���������
/* Timer handler declaration */
TIM_HandleTypeDef    htimx_Encoder;

/* Timer Encoder Configuration Structure declaration */
TIM_Encoder_InitTypeDef sEncoderConfig;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ͨ�ö�ʱ����ʼ��������ͨ��PWM���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void ENCODER_TIMx_Init(void)
{
  ENCODER_TIM_RCC_CLK_ENABLE();
  htimx_Encoder.Instance = ENCODER_TIMx;
  htimx_Encoder.Init.Prescaler = ENCODER_TIM_PRESCALER;
  htimx_Encoder.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx_Encoder.Init.Period = ENCODER_TIM_PERIOD;
  htimx_Encoder.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;

  sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TIx;
  sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING;
  sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;
  sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1;
  sEncoderConfig.IC1Filter          = 0;

  sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_RISING;
  sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;
  sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1;
  sEncoderConfig.IC2Filter          = 0;
  __HAL_TIM_SET_COUNTER(&htimx_Encoder,0);

  HAL_TIM_Encoder_Init(&htimx_Encoder, &sEncoderConfig);

  __HAL_TIM_CLEAR_IT(&htimx_Encoder, TIM_IT_UPDATE);  // ��������жϱ�־λ
  __HAL_TIM_URS_ENABLE(&htimx_Encoder);               // ���������������Ų��������ж�
  __HAL_TIM_ENABLE_IT(&htimx_Encoder,TIM_IT_UPDATE);  // ʹ�ܸ����ж�

  HAL_NVIC_SetPriority(ENCODER_TIM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ENCODER_TIM_IRQn);

	HAL_TIM_Encoder_Start(&htimx_Encoder, TIM_CHANNEL_ALL);
}

/**
  * ��������: ������ʱ��Ӳ����ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==ENCODER_TIMx)
  {
    /* ������ʱ������ʱ��ʹ�� */
    ENCODER_TIM_GPIO_CLK_ENABLE();

    /* ��ʱ��ͨ��1��������IO��ʼ�� */
    GPIO_InitStruct.Pin = ENCODER_TIM_CH1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(ENCODER_TIM_CH1_GPIO, &GPIO_InitStruct);

    /* ��ʱ��ͨ��2��������IO��ʼ�� */
    GPIO_InitStruct.Pin = ENCODER_TIM_CH2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(ENCODER_TIM_CH2_GPIO, &GPIO_InitStruct);
  }
}

/**
  * ��������: ������ʱ��Ӳ������ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==ENCODER_TIMx)
  {
    /* ������ʱ������ʱ�ӽ��� */
    ENCODER_TIM_RCC_CLK_DISABLE();

    HAL_GPIO_DeInit(ENCODER_TIM_CH1_GPIO, ENCODER_TIM_CH1_PIN);
    HAL_GPIO_DeInit(ENCODER_TIM_CH2_GPIO, ENCODER_TIM_CH2_PIN);
  }
}
/**
  * ��������: ��ʱ�������ж�
  * �������: *htim,��ʱ�����
  * �� �� ֵ: ��
  * ˵    ��: �����������������
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==ENCODER_TIMx)
	{
	  if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder))
		OverflowCount--;       //���¼������
	  else
		OverflowCount++;       //���ϼ������
	}
}
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

