;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/*       This software is licensed under the Microsoft Software License   */
;/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
;/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
;/*       and in the root directory of this software.                      */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */ 
;/** ThreadX Component                                                     */ 
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/
;
;#define TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_thread.h"
;#include "tx_timer.h"
;
;
    IF :DEF:TX_ENABLE_FIQ_SUPPORT
DISABLE_INTS    EQU     0xC0                ; IRQ & FIQ interrupts disabled
    ELSE
DISABLE_INTS    EQU     0x80                ; IRQ interrupts disabled
    ENDIF
;
;
    IMPORT      _tx_thread_current_ptr
    IMPORT      _tx_timer_time_slice
    IMPORT      _tx_thread_schedule
    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    IMPORT      _tx_execution_thread_exit        
    ENDIF
;
;
        AREA ||.text||, CODE, READONLY
        PRESERVE8
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_system_return                             ARM9/AC5       */ 
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function is target processor specific.  It is used to transfer */ 
;/*    control from a thread back to the ThreadX system.  Only a           */ 
;/*    minimal context is saved since the compiler assumes temp registers  */ 
;/*    are going to get slicked by a function call anyway.                 */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  OUTPUT                                                                */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  CALLS                                                                 */ 
;/*                                                                        */ 
;/*    _tx_thread_schedule                   Thread scheduling loop        */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    ThreadX components                                                  */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_system_return(VOID)
;{
    EXPORT  _tx_thread_system_return
_tx_thread_system_return
;
;    /* Save minimal context on the stack.  */
;
    MOV     r0, #0                              ; Build a solicited stack type
    MRS     r1, CPSR                            ; Pickup the CPSR
    STMDB   sp!, {r0-r1, r4-r11, lr}            ; Save minimal context
;   
;   /* Lockout interrupts.  */
;
    ORR     r2, r1, #DISABLE_INTS               ; Build disable interrupt CPSR
    MSR     CPSR_cxsf, r2                       ; Disable interrupts

    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the thread exit function to indicate the thread is no longer executing.  */
;
    BL      _tx_execution_thread_exit           ; Call the thread exit function
    ENDIF

    LDR     r3, =_tx_thread_current_ptr         ; Pickup address of current ptr
    LDR     r0, [r3, #0]                        ; Pickup current thread pointer
    LDR     r2, =_tx_timer_time_slice           ; Pickup address of time slice
    LDR     r1, [r2, #0]                        ; Pickup current time slice
;
;    /* Save current stack and switch to system stack.  */
;    _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
;    sp = _tx_thread_system_stack_ptr;
;
    STR     sp, [r0, #8]                        ; Save thread stack pointer
;
;    /* Determine if the time-slice is active.  */
;    if (_tx_timer_time_slice)
;    {
;
    MOV     r4, #0                              ; Build clear value
    CMP     r1, #0                              ; Is a time-slice active?
    BEQ     __tx_thread_dont_save_ts            ; No, don't save the time-slice
;
;       /* Save time-slice for the thread and clear the current time-slice.  */
;       _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
;       _tx_timer_time_slice =  0;
;
    STR     r4, [r2, #0]                        ; Clear time-slice
    STR     r1, [r0, #24]                       ; Save current time-slice
;
;    }
__tx_thread_dont_save_ts
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
    STR     r4, [r3, #0]                        ; Clear current thread pointer
    B       _tx_thread_schedule                 ; Jump to scheduler!
;
;}
    END

