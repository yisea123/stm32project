STM32 hardware depend part
==========================

codrv_bxcan - CAN driver
codrv_canbittiming - additional bittimings
codrv_error - error handling

There are different examples for hardware initialization.
Compile and adapt only one of it!

cpu_stm32*          - hardware initialization
cpu_stm32.c         - old, dont use it for new projects
cpu_stm32f0_keil    - STM32F072, Keil uVision 5, STCube HAL, CAN1
cpu_stm32f1_keil    - STM32F103, Keil uVision 4, CAN1
cpu_stm32f1_atollic - STM32F105, Atollic, support of CAN1 and CAN2
cpu_stm32f3_atollic - STM32F303, Atollic, ST HAL, CAN
cpu_stm32f4_coocox  - STM32F407, CooCox, CAN1
cpu_stm32f4xx       - STM32F407, ST HAL, CAN1
cpu_stm32l4xx       - STM32L476, ST HAL, CAN1


Use of additional bittimings:
=============================
- set CODRV_BIT_TABLE_EXTERN  
- set CODRV_CANCLOCK_PRE_10BIT

depend of the Peripherie clock
- set CODRV_CANCLOCK_72MHZ
(please check the src included tables)

Debug functionality
===================
can_printf.c	- send printf information over CAN
printf.js   - script for CDE to see the printf information


HAL printf() - UART init example
================================
see cpu_stm32f4xx.c
- UART1 init
- _write() function

