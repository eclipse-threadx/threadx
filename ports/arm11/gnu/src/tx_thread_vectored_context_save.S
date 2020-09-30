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
@
@#define TX_SOURCE_CODE
@
@
@/* Include necessary system files.  */
@
@#include "tx_api.h"
@#include "tx_thread.h"
@
@
#ifdef TX_ENABLE_FIQ_SUPPORT
DISABLE_INTS    =       0xC0                    @ IRQ & FIQ interrupts disabled
#else
DISABLE_INTS    =       0x80                    @ IRQ interrupts disabled
#endif
@
@
    .global     _tx_thread_system_state
    .global     _tx_thread_current_ptr
    .global     _tx_execution_isr_enter
@
@
@
@/* No 16-bit Thumb mode veneer code is needed for _tx_thread_vectored_context_save
@   since it will never be called 16-bit mode.  */
@ 
    .arm
    .text
    .align 2
@/**************************************************************************/ 
@/*                                                                        */ 
@/*  FUNCTION                                               RELEASE        */ 
@/*                                                                        */ 
@/*    _tx_thread_vectored_context_save                    ARM11/GNU       */ 
@/*                                                            6.1         */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */ 
@/*    This function saves the context of an executing thread in the       */ 
@/*    beginning of interrupt processing.  The function also ensures that  */ 
@/*    the system stack is used upon return to the calling ISR.            */ 
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
@/*    ISRs                                                                */ 
@/*                                                                        */ 
@/*  RELEASE HISTORY                                                       */ 
@/*                                                                        */ 
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
@/*                                                                        */
@/**************************************************************************/
@VOID  _tx_thread_vectored_context_save(VOID)
@{
    .global  _tx_thread_vectored_context_save
    .type    _tx_thread_vectored_context_save,function
_tx_thread_vectored_context_save:
@
@    /* Upon entry to this routine, it is assumed that IRQ interrupts are locked
@       out, we are in IRQ mode, and all registers are intact.  */
@
@    /* Check for a nested interrupt condition.  */
@    if (_tx_thread_system_state++)
@    {
@
#ifdef TX_ENABLE_FIQ_SUPPORT
    MRS     r0, CPSR                            @ Pickup the CPSR
    ORR     r0, r0, #DISABLE_INTS               @ Build disable interrupt CPSR
    MSR     CPSR_cxsf, r0                       @ Disable interrupts
#endif
    LDR     r3, =_tx_thread_system_state        @ Pickup address of system state variable
    LDR     r2, [r3, #0]                        @ Pickup system state
    CMP     r2, #0                              @ Is this the first interrupt?
    BEQ     __tx_thread_not_nested_save         @ Yes, not a nested context save
@
@    /* Nested interrupt condition.  */
@
    ADD     r2, r2, #1                          @ Increment the interrupt counter
    STR     r2, [r3, #0]                        @ Store it back in the variable
@
@    /* Note: Minimal context of interrupted thread is already saved.  */
@
@    /* Return to the ISR.  */
@
    MOV     r10, #0                             @ Clear stack limit

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
@
@    /* Call the ISR enter function to indicate an ISR is executing.  */
@
    PUSH    {lr}                                @ Save ISR lr
    BL      _tx_execution_isr_enter             @ Call the ISR enter function
    POP     {lr}                                @ Recover ISR lr
#endif

    MOV     pc, lr                              @ Return to caller
@
__tx_thread_not_nested_save:
@    }
@
@    /* Otherwise, not nested, check to see if a thread was running.  */
@    else if (_tx_thread_current_ptr)
@    {
@
    ADD     r2, r2, #1                          @ Increment the interrupt counter
    STR     r2, [r3, #0]                        @ Store it back in the variable
    LDR     r1, =_tx_thread_current_ptr         @ Pickup address of current thread ptr
    LDR     r0, [r1, #0]                        @ Pickup current thread pointer
    CMP     r0, #0                              @ Is it NULL?
    BEQ     __tx_thread_idle_system_save        @ If so, interrupt occurred in 
                                                @   scheduling loop - nothing needs saving!
@
@    /* Note: Minimal context of interrupted thread is already saved.  */
@
@    /* Save the current stack pointer in the thread's control block.  */
@    _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
@
@    /* Switch to the system stack.  */
@    sp =  _tx_thread_system_stack_ptr;
@
    MOV     r10, #0                             @ Clear stack limit

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
@
@    /* Call the ISR enter function to indicate an ISR is executing.  */
@
    PUSH    {lr}                                @ Save ISR lr
    BL      _tx_execution_isr_enter             @ Call the ISR enter function
    POP     {lr}                                @ Recover ISR lr
#endif

    MOV     pc, lr                              @ Return to caller
@
@    }
@    else
@    {
@
__tx_thread_idle_system_save:
@
@    /* Interrupt occurred in the scheduling loop.  */
@
@    /* Not much to do here, just adjust the stack pointer, and return to IRQ 
@       processing.  */
@
    MOV     r10, #0                             @ Clear stack limit

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
@
@    /* Call the ISR enter function to indicate an ISR is executing.  */
@
    PUSH    {lr}                                @ Save ISR lr
    BL      _tx_execution_isr_enter             @ Call the ISR enter function
    POP     {lr}                                @ Recover ISR lr
#endif

    ADD     sp, sp, #32                         @ Recover saved registers
    MOV     pc, lr                              @ Return to caller
@
@    }
@}

