/******************************************************************************/
/* SETUP.C: Setup Functions                                                   */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2007 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

/*
 * adapted from emtas to create a working example
 */


#include <stm32f10x.h>              /* STM32F10x Library Definitions */


#ifdef HW_OLIMEX

/* PC12 */
void SetupLED (void) 
{
GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure PC12 as outputs push-pull, max speed 50 MHz */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* PA0 */
void SetupKBD (void) 
{
GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /* Configure PA.00 as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

#endif /* OLIMEX */

