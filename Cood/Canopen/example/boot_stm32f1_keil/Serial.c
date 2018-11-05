/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2007 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define xDEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define xTRCENA          0x01000000

//#include <stm32f10x_lib.h> /* STM32F10x Library Definitions      */
#include <stm32f10x.h>      /* STM32F10x Library Definitions      */

#ifdef NO_PRINTF
#else /* NO_PRINTF */

void SetupUART (void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

/* Enable GPIOA clock                                                       */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	

/* Configure USART2 RTS (PA1) and USART2 Tx (PA2) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

/* Configure USART2 CTS (PA0) and USART2 Rx (PA3) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

/* USART2 configured as follow:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
- One Stop Bit
- No parity
- Hardware flow control disabled
- Receive and transmit enabled
*/
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

/* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);
}


/* Implementation of putchar (also used by printf function to output data)    */
int SendChar (int ch)                             /* Write character to Serial Port     */
{

#ifdef DBG_ITM
    if (xDEMCR & xTRCENA)
    {
        while (ITM_Port32(0) == 0) {}
        ITM_Port8(0) = ch;
    }
#else
    USART_SendData(USART2, (unsigned char) ch);
    while (!(USART2->SR & USART_FLAG_TXE)) {}
#endif
    return (ch);
}


int GetKey (void)                                 /* Read character from Serial Port    */
{

    while (!(USART2->SR & USART_FLAG_RXNE)) {}
    return (USART_ReceiveData(USART2));
}

#endif

