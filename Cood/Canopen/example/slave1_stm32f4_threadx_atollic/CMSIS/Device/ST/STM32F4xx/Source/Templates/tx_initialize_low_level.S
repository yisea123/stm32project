@/**************************************************************************/ 
@/*                                                                        */ 
@/*            Copyright (c) 1996-2014 by Express Logic Inc.               */
@/*                                                                        */ 
@/*  This software is copyrighted by and is the sole property of Express   */ 
@/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
@/*  in the software remain the property of Express Logic, Inc.  This      */ 
@/*  software may only be used in accordance with the corresponding        */ 
@/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
@/*  distribution, or disclosure of this software is expressly forbidden.  */ 
@/*                                                                        */
@/*  This Copyright notice may not be removed or modified without prior    */ 
@/*  written consent of Express Logic, Inc.                                */ 
@/*                                                                        */ 
@/*  Express Logic, Inc. reserves the right to modify this software        */ 
@/*  without notice.                                                       */ 
@/*                                                                        */ 
@/*  Express Logic, Inc.                     info@expresslogic.com         */
@/*  11423 West Bernardo Court               http://www.expresslogic.com   */
@/*  San Diego, CA  92127                                                  */
@/*                                                                        */
@/**************************************************************************/
@
@
@/**************************************************************************/
@/**************************************************************************/
@/**                                                                       */ 
@/** ThreadX Component                                                     */ 
@/**                                                                       */
@/**   Initialize                                                          */
@/**                                                                       */
@/**************************************************************************/
@/**************************************************************************/
@
@#define TX_SOURCE_CODE
@
@
@/* Include necessary system files.  */
@
@#include "tx_api.h"
@#include "tx_initialize.h"
@#include "tx_thread.h"
@#include "tx_timer.h"
@
@
        .global  _tx_thread_system_stack_ptr
        .global  _tx_initialize_unused_memory
        .global __RAM_segment_used_end__
        .global  _tx_thread_context_save
        .global  _tx_thread_context_restore
        .global  _tx_timer_interrupt
        .global  __main
        .global  __tx_SVCallHandler
        .global  __tx_PendSVHandler
        .global   _vectors
        .global __tx_NMIHandler                     @ NMI
        .global __tx_BadHandler                     @ HardFault
        .global __tx_SVCallHandler                  @ SVCall
        .global __tx_DBGHandler                     @ Monitor
        .global __tx_PendSVHandler                  @ PendSV
        .global __tx_SysTickHandler                 @ SysTick
        .global __tx_IntHandler                     @ Int 0
@
@
SYSTEM_CLOCK      =   168000000
SYSTICK_CYCLES    =   ((SYSTEM_CLOCK / 100) -1)

        .text 32
        .align 4
        .syntax unified

@/**************************************************************************/ 
@/*                                                                        */ 
@/*  FUNCTION                                               RELEASE        */ 
@/*                                                                        */ 
@/*    _tx_initialize_low_level                          Cortex-M4/Atollic */ 
@/*                                                           5.1          */
@/*  AUTHOR                                                                */ 
@/*                                                                        */ 
@/*    William E. Lamie, Express Logic, Inc.                               */ 
@/*                                                                        */ 
@/*  DESCRIPTION                                                           */ 
@/*                                                                        */ 
@/*    This function is responsible for any low-level processor            */ 
@/*    initialization, including setting up interrupt vectors, setting     */ 
@/*    up a periodic timer interrupt source, saving the system stack       */ 
@/*    pointer for use in ISR processing later, and finding the first      */ 
@/*    available RAM memory address for tx_application_define.             */ 
@/*                                                                        */ 
@/*  INPUT                                                                 */ 
@/*                                                                        */ 
@/*    None                                                                */ 
@/*                                                                        */ 
@/*  OUTPUT                                                                */ 
@/*                                                                        */ 
@/*    None                                                                */ 
@/*                                                                        */ 
@/*  CALLS                                                                 */ 
@/*                                                                        */ 
@/*    None                                                                */ 
@/*                                                                        */ 
@/*  CALLED BY                                                             */ 
@/*                                                                        */ 
@/*    _tx_initialize_kernel_enter           ThreadX entry function        */ 
@/*                                                                        */ 
@/*  RELEASE HISTORY                                                       */ 
@/*                                                                        */ 
@/*    DATE              NAME                      DESCRIPTION             */ 
@/*                                                                        */ 
@/*  08-09-2012     William E. Lamie         Initial Version 5.0           */ 
@/*  01-01-2014     William E. Lamie         Modified comment(s),          */
@/*                                            resulting in version 5.1    */
@/*                                                                        */ 
@/**************************************************************************/
@VOID   _tx_initialize_low_level(VOID)
@{
        .global  _tx_initialize_low_level
        .thumb_func
_tx_initialize_low_level:
@
@    /* Ensure that interrupts are disabled.  */
@

        CPSID   i                               @ Disable interrupts

@
@    /* Set base of available memory to end of non-initialised RAM area.  */
@     
        LDR     r0, =_tx_initialize_unused_memory @ Build address of unused memory pointer
        LDR     r1, =__RAM_segment_used_end__   @ Build first free address
        ADD     r1, r1, #4                      @
        STR     r1, [r0]                        @ Setup first unused memory pointer
@
@    /* Setup Vector Table Offset Register.  */
@    
        MOV     r0, #0xE000E000                 @ Build address of NVIC registers
        LDR     r1, =_vectors                   @ Pickup address of vector table
        STR     r1, [r0, #0xD08]                @ Set vector table address
@
@    /* Set system stack pointer from vector value.  */
@
        LDR     r0, =_tx_thread_system_stack_ptr @ Build address of system stack pointer
        LDR     r1, =_vectors                   @ Pickup address of vector table
        LDR     r1, [r1]                        @ Pickup reset stack pointer
        STR     r1, [r0]                        @ Save system stack pointer
@
@    /* Enable the cycle count register.  */
@
        LDR     r0, =0xE0001000                 @ Build address of DWT register
        LDR     r1, [r0]                        @ Pickup the current value
        ORR     r1, r1, #1                      @ Set the CYCCNTENA bit
        STR     r1, [r0]                        @ Enable the cycle count register
@
@    /* Configure SysTick for 100Hz clock, or 16384 cycles if no reference.  */
@
        MOV     r0, #0xE000E000                 @ Build address of NVIC registers
        LDR     r1, =SYSTICK_CYCLES
        STR     r1, [r0, #0x14]                 @ Setup SysTick Reload Value
        MOV     r1, #0x7                        @ Build SysTick Control Enable Value
        STR     r1, [r0, #0x10]                 @ Setup SysTick Control
@
@    /* Configure handler priorities.  */
@
        LDR     r1, =0x00000000                 @ Rsrv, UsgF, BusF, MemM
        STR     r1, [r0, #0xD18]                @ Setup System Handlers 4-7 Priority Registers

        LDR     r1, =0xFF000000                 @ SVCl, Rsrv, Rsrv, Rsrv
        STR     r1, [r0, #0xD1C]                @ Setup System Handlers 8-11 Priority Registers
                                                @ Note: SVC must be lowest priority, which is 0xFF

        LDR     r1, =0x40FF0000                 @ SysT, PnSV, Rsrv, DbgM
        STR     r1, [r0, #0xD20]                @ Setup System Handlers 12-15 Priority Registers
                                                @ Note: PnSV must be lowest priority, which is 0xFF
#ifdef TX_ENABLE_FPU_SUPPORT
        LDR     r0, =0xE000EF34                 @ Pickup FPCCR
        LDR     r1, [r0]                        @
        LDR     r2, =0x3FFFFFFF                 @ Build mask to clear ASPEN and LSPEN
        AND     r1, r1, r2                      @ Clear the ASPEN and LSPEN bits
        STR     r1, [r0]                        @ Update FPCCR
#endif
        
@
@    /* Return to caller.  */
@    
        BX      lr 
@}
@

@/* Define shells for each of the unused vectors.  */
@
        .global  __tx_BadHandler
        .thumb_func
__tx_BadHandler:
        B       __tx_BadHandler

@ /* added to catch the hardfault */

        .global  __tx_HardfaultHandler
        .thumb_func
__tx_HardfaultHandler:
        B       __tx_HardfaultHandler


@ /* Generic interrupt handler template */
        .global  __tx_IntHandler
        .thumb_func
__tx_IntHandler:
@ VOID InterruptHandler (VOID)
@ {
        PUSH    {lr}
        BL      _tx_thread_context_save
        
@    /* Do interrupt handler work here */
@    /* BL <your C Function>.... */

        B       _tx_thread_context_restore
@ }

@ /* SSytem Tick timer interrupt handler */
        .global  __tx_SysTickHandler
        .thumb_func
__tx_SysTickHandler:
@ VOID TimerInterruptHandler (VOID)
@ {
@
        PUSH    {lr}
        BL      _tx_thread_context_save
        BL      _tx_timer_interrupt
        B       _tx_thread_context_restore

@ }


@        .global  ETH_IRQHandler
@        .thumb_func
@        .global  __nx_driver_stm32f4xx_ethernet_isr 
@ETH_IRQHandler:
@__nx_driver_stm32f4xx_ethernet_isr:
@        PUSH    {lr}
@        BL      _tx_thread_context_save
@        BL      nx_driver_stm32f4xx_ethernet_isr
@        B       _tx_thread_context_restore



@ /* NMI, DBG handlers */
        .global  __tx_NMIHandler
        .thumb_func
__tx_NMIHandler:
        B       __tx_NMIHandler

        .global  __tx_DBGHandler
          .thumb_func
__tx_DBGHandler:
        B       __tx_DBGHandler



