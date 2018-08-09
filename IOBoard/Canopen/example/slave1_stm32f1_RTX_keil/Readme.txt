CANopen Slave Example used with a RTOS

Compiler: Keil RealArm µVision5
CPU: STM32F103
CAN: bxCAN1
Board: Olimex
OS: RTX

The OS specific setting are done in canopen_rtx.h.
The timerevent is shown in the driver related file 
cpu_stm32f1_RTX_keil.c!


Attention:
The printf() communication was copied from our µVision 4 project,
but currently not used and checked with µVision5.

