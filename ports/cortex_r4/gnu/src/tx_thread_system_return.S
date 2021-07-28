@/**************************************************************************/
@/*                                                                        */
@/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
@/*                                                                        */
@/*       This software is licensed under the Microsoft Software License   */
@/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
@/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
@/*       and in the root directory of this software.                      */
@/*                                                                        */
@/**************************************************************************/
@
@
@/**************************************************************************/
@/**************************************************************************/
@/**                                                                       */ 
@/** ThreadX Component                                                     */ 
@/**                                                                       */
@/**   Thread                                                              */
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
@#include "tx_thread.h"
@#include "tx_timer.h"
@
    .arm
@
@
    .global     _tx_thread_current_ptr
    .global     _tx_timer_time_slice
    .global     _tx_thread_schedule
    .global     _tx_execution_thread_exit
@
@
@
@/* Define the 16-bit Thumb mode veneer for _tx_thread_system_return for
@   applications calling this function from to 16-bit Thumb mode.  */
@
    .text
    .align  2
    .global $_tx_thread_system_return
    .type   $_tx_thread_system_return,function
$_tx_thread_system_return:
    .thumb
     BX        pc                               @ Switch to 32-bit mode
     NOP                                        @ 
    .arm
     STMFD     sp!, {lr}                        @ Save return address
     BL        _tx_thread_system_return         @ Call _tx_thread_system_return function
     LDMFD     sp!, {lr}                        @ Recover saved return address
     BX        lr                               @ Return to 16-bit caller
@
@
    .text
    .align  2
@/**************************************************************************/ 
@/*                                                                        */ 
@/*  FUNCTION                                               RELEASE        */ 
@/*                                                                        */ 
@/*    _tx_thread_system_return                          Cortex-R4/GNU     */ 
@/*                                                           6.1          */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */ 
@/*    This function is target processor specific.  It is used to transfer */ 
@/*    control from a thread back to the ThreadX system.  Only a           */ 
@/*    minimal context is saved since the compiler assumes temp registers  */ 
@/*    are going to get slicked by a function call anyway.                 */ 
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
@/*    _tx_thread_schedule                   Thread scheduling loop        */ 
@/*                                                                        */ 
@/*  CALLED BY                                                             */ 
@/*                                                                        */ 
@/*    ThreadX components                                                  */ 
@/*                                                                        */ 
@/*  RELEASE HISTORY                                                       */ 
@/*                                                                        */ 
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
@/*                                                                        */
@/**************************************************************************/
@VOID   _tx_thread_system_return(VOID)
@{
    .global _tx_thread_system_return
    .type   _tx_thread_system_return,function
_tx_thread_system_return:
@
@   /* Lockout interrupts.  */
@
    MRS     r1, CPSR                            @ Pickup the CPSR
#ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  @ Enable IRQ and FIQ interrupts
#else
    CPSID   i                                   @ Enable IRQ interrupts
#endif
@    /* Save minimal context on the stack.  */
@
    STMDB   sp!, {r4-r11, lr}                   @ Save minimal context
    LDR     r5, =_tx_thread_current_ptr         @ Pickup address of current ptr
    LDR     r6, [r5]                            @ Pickup current thread pointer
@
#ifdef TX_ENABLE_VFP_SUPPORT
    LDR     r0, [r6, #144]                      @ Pickup the VFP enabled flag
    CMP     r0, #0                              @ Is the VFP enabled?
    BEQ     _tx_skip_solicited_vfp_save         @ No, skip VFP solicited save
    VMRS    r4, FPSCR                           @ Pickup the FPSCR
    STR     r4, [sp, #-4]!                      @ Save FPSCR
    VSTMDB  sp!, {D8-D15}                       @ Save D8-D15
_tx_skip_solicited_vfp_save:
#endif
@
    MOV     r0, #0                              @ Build a solicited stack type
    STMDB   sp!, {r0-r1}                        @ Save type and CPSR
@   
@
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
@
@    /* Call the thread exit function to indicate the thread is no longer executing.  */
@
    BL      _tx_execution_thread_exit           @ Call the thread exit function
#endif
@
    LDR     r2, =_tx_timer_time_slice           @ Pickup address of time slice
    LDR     r1, [r2]                            @ Pickup current time slice
@
@    /* Save current stack and switch to system stack.  */
@    _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
@    sp = _tx_thread_system_stack_ptr;
@
    STR     sp, [r6, #8]                        @ Save thread stack pointer
@
@    /* Determine if the time-slice is active.  */
@    if (_tx_timer_time_slice)
@    {
@
    MOV     r4, #0                              @ Build clear value
    CMP     r1, #0                              @ Is a time-slice active?
    BEQ     __tx_thread_dont_save_ts            @ No, don't save the time-slice
@
@       /* Save time-slice for the thread and clear the current time-slice.  */
@       _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
@       _tx_timer_time_slice =  0;
@
    STR     r4, [r2]                            @ Clear time-slice
    STR     r1, [r6, #24]                       @ Save current time-slice
@
@    }
__tx_thread_dont_save_ts:
@
@    /* Clear the current thread pointer.  */
@    _tx_thread_current_ptr =  TX_NULL;
@
    STR     r4, [r5]                            @ Clear current thread pointer
    B       _tx_thread_schedule                 @ Jump to scheduler!
@
@}

