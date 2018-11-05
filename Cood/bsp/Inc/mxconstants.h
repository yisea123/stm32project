/**
  ******************************************************************************
  * File Name          : mxconstants.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#ifndef __MXCONSTANT_H
#define __MXCONSTANT_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define SPI4_SCK_Pin GPIO_PIN_2
#define SPI4_SCK_GPIO_Port GPIOE
#define SPI4_CS_Pin GPIO_PIN_3
#define SPI4_CS_GPIO_Port GPIOE
#define WDI_EN_Pin GPIO_PIN_4
#define WDI_EN_GPIO_Port GPIOE
#define SPI4_MISO_Pin GPIO_PIN_5
#define SPI4_MISO_GPIO_Port GPIOE
#define SPI4_MOSI_Pin GPIO_PIN_6
#define SPI4_MOSI_GPIO_Port GPIOE
#define NC_Pin GPIO_PIN_8
#define NC_GPIO_Port GPIOI
#define NCC13_Pin GPIO_PIN_13
#define NCC13_GPIO_Port GPIOC
#define NCI9_Pin GPIO_PIN_9
#define NCI9_GPIO_Port GPIOI
#define NCI10_Pin GPIO_PIN_10
#define NCI10_GPIO_Port GPIOI
#define NCI11_Pin GPIO_PIN_11
#define NCI11_GPIO_Port GPIOI
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SDA_GPIO_Port GPIOF
#define I2C2_SCK_Pin GPIO_PIN_1
#define I2C2_SCK_GPIO_Port GPIOF
#define NCF2_Pin GPIO_PIN_2
#define NCF2_GPIO_Port GPIOF
#define NCI12_Pin GPIO_PIN_12
#define NCI12_GPIO_Port GPIOI
#define NCI13_Pin GPIO_PIN_13
#define NCI13_GPIO_Port GPIOI
#define NCI14_Pin GPIO_PIN_14
#define NCI14_GPIO_Port GPIOI
#define V12_MONITOR_Pin GPIO_PIN_3
#define V12_MONITOR_GPIO_Port GPIOF
#define I_MON_Pin GPIO_PIN_4
#define I_MON_GPIO_Port GPIOF
#define SPI5_CS_Pin GPIO_PIN_5
#define SPI5_CS_GPIO_Port GPIOF
#define MOTOR1_STEP_Pin GPIO_PIN_6
#define MOTOR1_STEP_GPIO_Port GPIOF
#define SPI5_SCK_Pin GPIO_PIN_7
#define SPI5_SCK_GPIO_Port GPIOF
#define SPI5_MISO_Pin GPIO_PIN_8
#define SPI5_MISO_GPIO_Port GPIOF
#define SPI5_MOSI_Pin GPIO_PIN_9
#define SPI5_MOSI_GPIO_Port GPIOF
#define NCF10_Pin GPIO_PIN_10
#define NCF10_GPIO_Port GPIOF
#define NCC0_Pin GPIO_PIN_0
#define NCC0_GPIO_Port GPIOC
#define NCC1_Pin GPIO_PIN_1
#define NCC1_GPIO_Port GPIOC
#define NCC2_Pin GPIO_PIN_2
#define NCC2_GPIO_Port GPIOC
#define NCC3_Pin GPIO_PIN_3
#define NCC3_GPIO_Port GPIOC
#define VALVE1_Pin GPIO_PIN_0
#define VALVE1_GPIO_Port GPIOA
#define VALVE2_Pin GPIO_PIN_1
#define VALVE2_GPIO_Port GPIOA
#define VALVE3_Pin GPIO_PIN_2
#define VALVE3_GPIO_Port GPIOA
#define NCH2_Pin GPIO_PIN_2
#define NCH2_GPIO_Port GPIOH
#define NCH3_Pin GPIO_PIN_3
#define NCH3_GPIO_Port GPIOH
#define LED_WORK1_Pin GPIO_PIN_4
#define LED_WORK1_GPIO_Port GPIOH
#define LED_WORK2_Pin GPIO_PIN_5
#define LED_WORK2_GPIO_Port GPIOH
#define VALVE4_Pin GPIO_PIN_3
#define VALVE4_GPIO_Port GPIOA
#define NCA4_Pin GPIO_PIN_4
#define NCA4_GPIO_Port GPIOA
#define VALVE5_Pin GPIO_PIN_5
#define VALVE5_GPIO_Port GPIOA
#define MOTOR2_STEP_Pin GPIO_PIN_6
#define MOTOR2_STEP_GPIO_Port GPIOA
#define MOTOR3_STEP_Pin GPIO_PIN_7
#define MOTOR3_STEP_GPIO_Port GPIOA
#define NCC4_Pin GPIO_PIN_4
#define NCC4_GPIO_Port GPIOC
#define NCC5_Pin GPIO_PIN_5
#define NCC5_GPIO_Port GPIOC
#define VALVE6_Pin GPIO_PIN_0
#define VALVE6_GPIO_Port GPIOB
#define VALVE7_Pin GPIO_PIN_1
#define VALVE7_GPIO_Port GPIOB
#define NCI15_Pin GPIO_PIN_15
#define NCI15_GPIO_Port GPIOI
#define NCJ0_Pin GPIO_PIN_0
#define NCJ0_GPIO_Port GPIOJ
#define NCJ1_Pin GPIO_PIN_1
#define NCJ1_GPIO_Port GPIOJ
#define NCJ2_Pin GPIO_PIN_2
#define NCJ2_GPIO_Port GPIOJ
#define NCJ3_Pin GPIO_PIN_3
#define NCJ3_GPIO_Port GPIOJ
#define NCJ4_Pin GPIO_PIN_4
#define NCJ4_GPIO_Port GPIOJ
#define NCF11_Pin GPIO_PIN_11
#define NCF11_GPIO_Port GPIOF
#define NCF12_Pin GPIO_PIN_12
#define NCF12_GPIO_Port GPIOF
#define NCF13_Pin GPIO_PIN_13
#define NCF13_GPIO_Port GPIOF
#define NCF14_Pin GPIO_PIN_14
#define NCF14_GPIO_Port GPIOF
#define NCF15_Pin GPIO_PIN_15
#define NCF15_GPIO_Port GPIOF
#define NCG0_Pin GPIO_PIN_0
#define NCG0_GPIO_Port GPIOG
#define NCG1_Pin GPIO_PIN_1
#define NCG1_GPIO_Port GPIOG
#define LOW_VOL_DEC1_Pin GPIO_PIN_7
#define LOW_VOL_DEC1_GPIO_Port GPIOE
#define LOW_VOL_DEC1_EXTI_IRQn EXTI9_5_IRQn
#define LOW_VOL_DEC2_Pin GPIO_PIN_8
#define LOW_VOL_DEC2_GPIO_Port GPIOE
#define LOW_VOL_DEC2_EXTI_IRQn EXTI9_5_IRQn
#define NCE9_Pin GPIO_PIN_9
#define NCE9_GPIO_Port GPIOE
#define NCE10_Pin GPIO_PIN_10
#define NCE10_GPIO_Port GPIOE
#define MOTOR1_MODE0_Pin GPIO_PIN_11
#define MOTOR1_MODE0_GPIO_Port GPIOE
#define MOTOR1_EN_N_Pin GPIO_PIN_12
#define MOTOR1_EN_N_GPIO_Port GPIOE
#define MOTOR1_DIR_Pin GPIO_PIN_13
#define MOTOR1_DIR_GPIO_Port GPIOE
#define MOTOR1_FAULT_N_Pin GPIO_PIN_14
#define MOTOR1_FAULT_N_GPIO_Port GPIOE
#define MOTOR1_RST_N_Pin GPIO_PIN_15
#define MOTOR1_RST_N_GPIO_Port GPIOE
#define RS485_TXD_Pin GPIO_PIN_10
#define RS485_TXD_GPIO_Port GPIOB
#define RS485_RXD_Pin GPIO_PIN_11
#define RS485_RXD_GPIO_Port GPIOB
#define MOTOR1_CURR_Pin GPIO_PIN_5
#define MOTOR1_CURR_GPIO_Port GPIOJ
#define MOTOR4_STEP_Pin GPIO_PIN_6
#define MOTOR4_STEP_GPIO_Port GPIOH
#define NCH7_Pin GPIO_PIN_7
#define NCH7_GPIO_Port GPIOH
#define NCH8_Pin GPIO_PIN_8
#define NCH8_GPIO_Port GPIOH
#define NCH9_Pin GPIO_PIN_9
#define NCH9_GPIO_Port GPIOH
#define MOTOR2_MODE0_Pin GPIO_PIN_10
#define MOTOR2_MODE0_GPIO_Port GPIOH
#define VALVE8_Pin GPIO_PIN_11
#define VALVE8_GPIO_Port GPIOH
#define VALVE9_Pin GPIO_PIN_12
#define VALVE9_GPIO_Port GPIOH
#define MOTOR2_EN_N_Pin GPIO_PIN_12
#define MOTOR2_EN_N_GPIO_Port GPIOB
#define CAN2_TX_Pin GPIO_PIN_13
#define CAN2_TX_GPIO_Port GPIOB
#define RS485_DIR_Pin GPIO_PIN_14
#define RS485_DIR_GPIO_Port GPIOB
#define MOTOR2_FAULT_N_Pin GPIO_PIN_15
#define MOTOR2_FAULT_N_GPIO_Port GPIOB
#define MOTOR2_RST_N_Pin GPIO_PIN_12
#define MOTOR2_RST_N_GPIO_Port GPIOD
#define MOTOR2_CURR_Pin GPIO_PIN_9
#define MOTOR2_CURR_GPIO_Port GPIOD
#define MOTOR3_MODE0_Pin GPIO_PIN_10
#define MOTOR3_MODE0_GPIO_Port GPIOD
#define MOTOR3_EN_N_Pin GPIO_PIN_11
#define MOTOR3_EN_N_GPIO_Port GPIOD
#define MOTOR2_DIR_Pin GPIO_PIN_8
#define MOTOR2_DIR_GPIO_Port GPIOD
#define TEMP1_CTR_Pin GPIO_PIN_13
#define TEMP1_CTR_GPIO_Port GPIOD
#define NCD14_Pin GPIO_PIN_14
#define NCD14_GPIO_Port GPIOD
#define NCD15_Pin GPIO_PIN_15
#define NCD15_GPIO_Port GPIOD
#define MOTOR3_DIR_Pin GPIO_PIN_6
#define MOTOR3_DIR_GPIO_Port GPIOJ
#define MOTOR3_FAULT_N_Pin GPIO_PIN_7
#define MOTOR3_FAULT_N_GPIO_Port GPIOJ
#define MOTOR3_RST_N_Pin GPIO_PIN_8
#define MOTOR3_RST_N_GPIO_Port GPIOJ
#define MOTOR3_CURR_Pin GPIO_PIN_9
#define MOTOR3_CURR_GPIO_Port GPIOJ
#define MOTOR4_MODE0_Pin GPIO_PIN_10
#define MOTOR4_MODE0_GPIO_Port GPIOJ
#define MOTOR4_EN_N_Pin GPIO_PIN_11
#define MOTOR4_EN_N_GPIO_Port GPIOJ
#define MOTOR4_DIR_Pin GPIO_PIN_0
#define MOTOR4_DIR_GPIO_Port GPIOK
#define MOTOR4_FAULT_N_Pin GPIO_PIN_1
#define MOTOR4_FAULT_N_GPIO_Port GPIOK
#define MOTOR4_RST_N_Pin GPIO_PIN_2
#define MOTOR4_RST_N_GPIO_Port GPIOK
#define MOTOR4_CURR_Pin GPIO_PIN_2
#define MOTOR4_CURR_GPIO_Port GPIOG
#define NCG3_Pin GPIO_PIN_3
#define NCG3_GPIO_Port GPIOG
#define NCG4_Pin GPIO_PIN_4
#define NCG4_GPIO_Port GPIOG
#define NCG5_Pin GPIO_PIN_5
#define NCG5_GPIO_Port GPIOG
#define NCG6_Pin GPIO_PIN_6
#define NCG6_GPIO_Port GPIOG
#define LED_660_L_IC_Pin GPIO_PIN_7
#define LED_660_L_IC_GPIO_Port GPIOG
#define LED_880_L_IC_Pin GPIO_PIN_8
#define LED_880_L_IC_GPIO_Port GPIOG
#define VALVE10_Pin GPIO_PIN_6
#define VALVE10_GPIO_Port GPIOC
#define LED_660_S_IC_Pin GPIO_PIN_7
#define LED_660_S_IC_GPIO_Port GPIOC
#define LED_880_S_IC_Pin GPIO_PIN_8
#define LED_880_S_IC_GPIO_Port GPIOC
#define LED_880_S_Pin GPIO_PIN_9
#define LED_880_S_GPIO_Port GPIOC
#define LED_660_S_Pin GPIO_PIN_8
#define LED_660_S_GPIO_Port GPIOA
#define UART1_TX_Pin GPIO_PIN_9
#define UART1_TX_GPIO_Port GPIOA
#define UART1_RX_Pin GPIO_PIN_10
#define UART1_RX_GPIO_Port GPIOA
#define CAN1_RX_Pin GPIO_PIN_11
#define CAN1_RX_GPIO_Port GPIOA
#define CAN1_TX_Pin GPIO_PIN_12
#define CAN1_TX_GPIO_Port GPIOA
#define JTAG_TMS_Pin GPIO_PIN_13
#define JTAG_TMS_GPIO_Port GPIOA
#define LED_660_L_Pin GPIO_PIN_13
#define LED_660_L_GPIO_Port GPIOH
#define LED_880_L_Pin GPIO_PIN_14
#define LED_880_L_GPIO_Port GPIOH
#define SH_Pin GPIO_PIN_15
#define SH_GPIO_Port GPIOH
#define NCI0_Pin GPIO_PIN_0
#define NCI0_GPIO_Port GPIOI
#define SYNC_N_Pin GPIO_PIN_1
#define SYNC_N_GPIO_Port GPIOI
#define NCI2_Pin GPIO_PIN_2
#define NCI2_GPIO_Port GPIOI
#define SPI3_CS_Pin GPIO_PIN_3
#define SPI3_CS_GPIO_Port GPIOI
#define JTAG_TCK_Pin GPIO_PIN_14
#define JTAG_TCK_GPIO_Port GPIOA
#define JTAG_TDI_Pin GPIO_PIN_15
#define JTAG_TDI_GPIO_Port GPIOA
#define SPI3_SCLK_Pin GPIO_PIN_10
#define SPI3_SCLK_GPIO_Port GPIOC
#define SPI3_MISO_Pin GPIO_PIN_11
#define SPI3_MISO_GPIO_Port GPIOC
#define SPI3_MOSI_Pin GPIO_PIN_12
#define SPI3_MOSI_GPIO_Port GPIOC
#define SPI3_MISO_EXTI0_Pin GPIO_PIN_0
#define SPI3_MISO_EXTI0_GPIO_Port GPIOD
#define GAIN_S_REF3_Pin GPIO_PIN_1
#define GAIN_S_REF3_GPIO_Port GPIOD
#define GAIN_S_REF2_Pin GPIO_PIN_2
#define GAIN_S_REF2_GPIO_Port GPIOD
#define GAIN_S_REF1_Pin GPIO_PIN_3
#define GAIN_S_REF1_GPIO_Port GPIOD
#define NCD4_Pin GPIO_PIN_4
#define NCD4_GPIO_Port GPIOD
#define UART2_TX_Pin GPIO_PIN_5
#define UART2_TX_GPIO_Port GPIOD
#define UART2_RX_Pin GPIO_PIN_6
#define UART2_RX_GPIO_Port GPIOD
#define GAIN_L_REF3_Pin GPIO_PIN_7
#define GAIN_L_REF3_GPIO_Port GPIOD
#define GAIN_L_REF2_Pin GPIO_PIN_12
#define GAIN_L_REF2_GPIO_Port GPIOJ
#define GAIN_L_REF1_Pin GPIO_PIN_13
#define GAIN_L_REF1_GPIO_Port GPIOJ
#define NCJ14_Pin GPIO_PIN_14
#define NCJ14_GPIO_Port GPIOJ
#define GAIN_S_MEA3_Pin GPIO_PIN_15
#define GAIN_S_MEA3_GPIO_Port GPIOJ
#define GAIN_S_MEA2_Pin GPIO_PIN_9
#define GAIN_S_MEA2_GPIO_Port GPIOG
#define GAIN_S_MEA1_Pin GPIO_PIN_10
#define GAIN_S_MEA1_GPIO_Port GPIOG
#define SPI6_MISO_EXTI11_Pin GPIO_PIN_11
#define SPI6_MISO_EXTI11_GPIO_Port GPIOG
#define SPI6_MISO_Pin GPIO_PIN_12
#define SPI6_MISO_GPIO_Port GPIOG
#define SPI6_SCK_Pin GPIO_PIN_13
#define SPI6_SCK_GPIO_Port GPIOG
#define SPI6_MOSI_Pin GPIO_PIN_14
#define SPI6_MOSI_GPIO_Port GPIOG
#define SPI6_CS_Pin GPIO_PIN_3
#define SPI6_CS_GPIO_Port GPIOK
#define NCK4_Pin GPIO_PIN_4
#define NCK4_GPIO_Port GPIOK
#define GAIN_L_MEA3_Pin GPIO_PIN_5
#define GAIN_L_MEA3_GPIO_Port GPIOK
#define GAIN_L_MEA2_Pin GPIO_PIN_6
#define GAIN_L_MEA2_GPIO_Port GPIOK
#define GAIN_L_MEA1_Pin GPIO_PIN_7
#define GAIN_L_MEA1_GPIO_Port GPIOK
#define NCG15_Pin GPIO_PIN_15
#define NCG15_GPIO_Port GPIOG
#define JTAG_TDO_Pin GPIO_PIN_3
#define JTAG_TDO_GPIO_Port GPIOB
#define JTAG_RST_Pin GPIO_PIN_4
#define JTAG_RST_GPIO_Port GPIOB
#define CAN2_RX_Pin GPIO_PIN_5
#define CAN2_RX_GPIO_Port GPIOB
#define I2C1_SCK_Pin GPIO_PIN_6
#define I2C1_SCK_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define NCB8_Pin GPIO_PIN_8
#define NCB8_GPIO_Port GPIOB
#define NCB9_Pin GPIO_PIN_9
#define NCB9_GPIO_Port GPIOB
#define NCE0_Pin GPIO_PIN_0
#define NCE0_GPIO_Port GPIOE
#define NCE1_Pin GPIO_PIN_1
#define NCE1_GPIO_Port GPIOE
#define WDI_Pin GPIO_PIN_4
#define WDI_GPIO_Port GPIOI
#define SW_SNS1_Pin GPIO_PIN_5
#define SW_SNS1_GPIO_Port GPIOI
#define SW_SNS2_Pin GPIO_PIN_6
#define SW_SNS2_GPIO_Port GPIOI
#define FAN_CTR_Pin GPIO_PIN_7
#define FAN_CTR_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MXCONSTANT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
