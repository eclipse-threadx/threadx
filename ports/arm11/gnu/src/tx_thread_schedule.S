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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

#ifdef TX_ENABLE_FIQ_SUPPORT
ENABLE_INTS     =       0xC0                    @ IRQ & FIQ Interrupts enabled mask
#else
ENABLE_INTS     =       0x80                    @ IRQ Interrupts enabled mask
#endif
@
@
    .global     _tx_thread_execute_ptr
    .global     _tx_thread_current_ptr
    .global     _tx_timer_time_slice
    .global     _tx_execution_thread_enter
@
@
@/* Define the 16-bit Thumb mode veneer for _tx_thread_schedule for
@   applications calling this function from to 16-bit Thumb mode.  */
@
    .text
    .align  2
    .global $_tx_thread_schedule
    .type   $_tx_thread_schedule,function
$_tx_thread_schedule:
         .thumb
     BX        pc                               @ Switch to 32-bit mode
     NOP                                        @ 
    .arm
     STMFD     sp!, {lr}                        @ Save return address
     BL        _tx_thread_schedule              @ Call _tx_thread_schedule function
     LDMFD     sp!, {lr}                        @ Recover saved return address
     BX        lr                               @ Return to 16-bit caller
@
@
    .text
    .align 2
@/**************************************************************************/ 
@/*                                                                        */ 
@/*  FUNCTION                                               RELEASE        */ 
@/*                                                                        */ 
@/*    _tx_thread_schedule                                 ARM11/GNU       */ 
@/*                                                            6.2.1       */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */ 
@/*    This function waits for a thread control block pointer to appear in */ 
@/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */ 
@/*    in the variable, the corresponding thread is resumed.               */ 
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
@/*    _tx_initialize_kernel_enter          ThreadX entry function         */ 
@/*    _tx_thread_system_return             Return to system from thread   */ 
@/*    _tx_thread_context_restore           Restore thread's context       */ 
@/*                                                                        */ 
@/*  RELEASE HISTORY                                                       */ 
@/*                                                                        */ 
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
@/*  03-08-2023     Cindy Deng               Modified comment(s), added    */
@/*                                            #include tx_user.h,         */
@/*                                            resulting in version 6.2.1  */
@/*                                                                        */
@/**************************************************************************/
@VOID   _tx_thread_schedule(VOID)
@{
    .global _tx_thread_schedule
        .type  _tx_thread_schedule,function
_tx_thread_schedule:
@
@    /* Enable interrupts.  */
@
    MRS     r2, CPSR                        @ Pickup CPSR
    BIC     r0, r2, #ENABLE_INTS            @ Clear the disable bit(s)
    MSR     CPSR_cxsf, r0                   @ Enable interrupts
@
@    /* Wait for a thread to execute.  */
@    do
@    {
    LDR     r1, =_tx_thread_execute_ptr     @ Address of thread execute ptr
@
__tx_thread_schedule_loop:
@
    LDR     r0, [r1]                        @ Pickup next thread to execute
    CMP     r0, #0                          @ Is it NULL?
    BEQ     __tx_thread_schedule_loop       @ If so, keep looking for a thread
@
@    }
@    while(_tx_thread_execute_ptr == TX_NULL);
@    
@    /* Yes! We have a thread to execute.  Lockout interrupts and
@       transfer control to it.  */
@
    MSR     CPSR_cxsf, r2                   @ Disable interrupts
@
@    /* Setup the current thread pointer.  */
@    _tx_thread_current_ptr =  _tx_thread_execute_ptr;
@
    LDR     r1, =_tx_thread_current_ptr     @ Pickup address of current thread 
    STR     r0, [r1]                        @ Setup current thread pointer
@
@    /* Increment the run count for this thread.  */
@    _tx_thread_current_ptr -> tx_thread_run_count++;
@
    LDR     r2, [r0, #4]                    @ Pickup run counter
    LDR     r3, [r0, #24]                   @ Pickup time-slice for this thread
    ADD     r2, r2, #1                      @ Increment thread run-counter
    STR     r2, [r0, #4]                    @ Store the new run counter
@
@    /* Setup time-slice, if present.  */
@    _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;
@
    LDR     r2, =_tx_timer_time_slice       @ Pickup address of time-slice 
                                            @   variable
    LDR     sp, [r0, #8]                    @ Switch stack pointers
    STR     r3, [r2]                        @ Setup time-slice
@
@    /* Switch to the thread's stack.  */
@    sp =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;
@
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
@
@    /* Call the thread entry function to indicate the thread is executing.  */
@
    BL      _tx_execution_thread_enter      @ Call the thread execution enter function
#endif
@
@    /* Determine if an interrupt frame or a synchronous task suspension frame
@   is present.  */
@
    LDMIA   sp!, {r0, r1}                   @ Pickup the stack type and saved CPSR
    CMP     r0, #0                          @ Check for synchronous context switch
    MSRNE   SPSR_cxsf, r1                   @   Setup SPSR for return
    LDMNEIA sp!, {r0-r12, lr, pc}^          @ Return to point of thread interrupt
    LDMIA   sp!, {r4-r11, lr}               @ Return to thread synchronously
    MSR     CPSR_cxsf, r1                   @   Recover CPSR
#ifdef __THUMB_INTERWORK
    BX      lr                              @ Return to caller
#else
    MOV     pc, lr                          @ Return to caller
#endif
@
@}
@

