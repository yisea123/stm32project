/*
* cpu_stm32.h
*
* Copyright (c) 2012-2016 emtas GmbH
*-------------------------------------------------------------------
* SVN  $Id: cpu_stm32.h 15811 2016-10-28 09:38:22Z boe $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief CPU for Cortex-M3 from ST (e.g. STM32F103)
*
* The file is used for the Cortex-M4 STM32F4 too.
*
*/

#ifndef CPU_STM32_H
#define CPU_STM32_H 1

/*lint -save -e961 function like macros */
# if defined(HW_OLIMEX)
/* Olimex board */
#define LED_Off(led)     do { GPIOC->ODR |=  (led << 12); } while(0)
#define LED_On(led)    	 do { GPIOC->ODR &= ~(led << 12); } while(0)
#define LED_toggle(led)  do { GPIOC->ODR ^= (led << 12); } while(0)


#elif defined (HW_STM32F4DISCOVERY)
#define LED_Off(led)
#define LED_On(led)
#define LED_toggle(led)

#elif (0)

#else
#define LED_Off(led)
#define LED_On(led)
#define LED_toggle(led)

#endif
/*lint -restore */


/* setup.c */
extern void SetupClock (void);
extern void SetupLED (void);
extern void SetupKBD (void);
extern void SetupUART (void);

/* general hardware initialization */
void codrvHardwareInit(void);
/* for compatibility */
#define initHardware()	codrvHardwareInit()

/* init CAN related hardware part */
void codrvHardwareCanInit(void);
/* for compatibility */
/* #define initCanHW()	codrvHardwareCanInit() */

void NVIC_Configuration(void);

#endif /* CPU_STM32_H */
