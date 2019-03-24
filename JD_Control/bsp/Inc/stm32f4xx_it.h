/**
  ******************************************************************************
  * @file    stm32f4xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

enum
{
	ISR_PRIORITY_TICK		= 15,
	ISR_PRIORITY_ETH		= 7,
	ISR_PRIORITY_USB		= 14,
	ISR_PRIORITY_CAN		= 9,
	ISR_PRIORITY_TIM6		= 6,
	ISR_PRIORITY_ADC1 		= 5,
	ISR_PRIORITY_UART       = 12,
	ISR_PRIORITY_USART_DMA  = 12,
	ISR_PRIORITY_SDIO 		= 4,
	ISR_PRIORITY_SDIO_DMA   = 5,
	ISR_PRIORITY_TIMER		= 8,
};

/*  */
enum
{
	ISR_SUB_PRIORITY_TICK		= 0,
	ISR_SUB_PRIORITY_I2C2DMA	= 0,
	ISR_SUB_PRIORITY_I2C2_ER	= 8,
	ISR_SUB_PRIORITY_I2C2_EV 	= 9,


	ISR_SUB_PRIORITY_SPI2  		= 0,
	ISR_SUB_PRIORITY_SPI5  		= 8,

	ISR_SUB_PRIORITY_ADC1 		= 0,
	ISR_SUB_PRIORITY_ADC3	 	= 1,
	ISR_SUB_PRIORITY_CAN1_TX	= 0,
	ISR_SUB_PRIORITY_CAN1_RX	= 1,
	ISR_SUB_PRIORITY_CAN1_SCE	= 2,
	ISR_SUB_PRIORITY_CAN2_TX	= 3,
	ISR_SUB_PRIORITY_CAN2_RX	= 4,
	ISR_SUB_PRIORITY_CAN2_SCE	= 5,
	ISR_SUB_PRIORITY_EXIT		= 0,


	ISR_SUB_PRIORITY_UART_DMA	= 0,
	ISR_SUB_PRIORITY_UART       = 8,
	ISR_SUB_PRIORITY_TIMER      = 0,
	ISR_SUB_PRIORITY_AD_TIMER   = 0,
};

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void SysTick_Handler(void);
void ADC_IRQHandler(void);
void CAN1_RX0_IRQHandler(void);
void CAN1_RX1_IRQHandler(void);
void CAN1_SCE_IRQHandler(void);
void SDIO_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void);
void OTG_FS_IRQHandler(void);
void DMA2_Stream6_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);
void USART6_IRQHandler(void);
void TIM6_DAC_IRQHandler(void);
void RCC_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void);
void DMA1_Stream7_IRQHandler(void);
void ETH_IRQHandler(void);
void OTG_FS_IRQHandler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
