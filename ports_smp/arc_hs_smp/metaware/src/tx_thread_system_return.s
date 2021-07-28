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
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_system_return                        SMP/ARC_HS/MetaWare */
;/*                                                            6.1         */
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
    .global _tx_thread_system_return
    .type   _tx_thread_system_return, @function
_tx_thread_system_return:
;
;    /* Save minimal context on the stack.  */
;
;   /* Lockout interrupts.  */
;
    clri    r2                                          ; Disable interrupts
    lr      r1, [IDENTITY]                              ; Pickup core ID
    xbfu    r1, r1, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r1, r1, 1                                   ; Subtract 1 to make 0-based
    .endif
    asl     r4, r1, 2                                   ; Build index into core arrays
    mov     r7, _tx_thread_current_ptr                  ; Pickup base of the current thread pointer
    add     r7, r7, r4                                  ; Build address of current thread pointer 
    ld      r0, [r7]                                    ; Pickup current thread ptr
    sub     sp, sp, 76                                  ; Allocate a solicited stack frame
    mov     r3, 0                                       ; Build a solicited stack type
    st      r3, [sp, 0]                                 ; Store stack type on the top
    st      blink, [sp, 4]                              ; Save return address and flags
    st      fp, [sp, 8]                                 ; Save fp
    st      r26, [sp, 12]                               ; Save r26
    st      r25, [sp, 16]                               ; Save r25
    st      r24, [sp, 20]                               ; Save r24
    st      r23, [sp, 24]                               ; Save r23
    st      r22, [sp, 28]                               ; Save r22
    st      r21, [sp, 32]                               ; Save r21
    st      r20, [sp, 36]                               ; Save r20
    st      r19, [sp, 40]                               ; Save r19
    st      r18, [sp, 44]                               ; Save r18
    st      r17, [sp, 48]                               ; Save r17
    st      r16, [sp, 52]                               ; Save r16
    st      r15, [sp, 56]                               ; Save r15
    st      r14, [sp, 60]                               ; Save r14
    st      r13, [sp, 64]                               ; Save r13
    st      r2,  [sp, 68]                               ; Save status32
    st      r30, [sp, 72]                               ; Save r30
;
;    /* Save current stack and switch to system stack.  */
;    _tx_thread_current_ptr[core] -> tx_thread_stack_ptr =  sp;
;    sp = _tx_thread_system_stack_ptr[core];
;
;   
    st      sp, [r0, 8]                                 ; Save thread's stack pointer
    mov     r6, _tx_thread_system_stack_ptr             ; Pickup address of system stack pointer
    add     r6, r6, r4                                  ; Build address of this core's entry
    ld      sp, [r6]                                    ; Switch to system stack
;
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the thread exit function to indicate the thread is no longer executing.  */
;
    mov     r13, r0                                     ; Save thread control block pointer
    mov     r14, r1                                     ; Save core executing
    mov     r15, r7                                     ; Save current thread pointer for core
    mov     r16, blink                                  ; Save blink
    bl.d    _tx_execution_thread_exit                   ; Call the thread exit function
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    ld      r0, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread ptr
    mov     r3, 0                                       ; Build clear value
    mov     r0, r13                                     ; Recover thread control block
    mov     r1, r14                                     ; Recover current core executing
    mov     r7, r15                                     ; Recover current thread pointer for core
    mov     blink, r16                                  ; Recover blink
    asl     r4, r1, 2                                   ; Build index into core arrays 
   
    .endif
;
;    /* Determine if the time-slice is active.  */
;    if (_tx_timer_time_slice)
;    {
;
    mov     r6, _tx_timer_time_slice                    ; Build address of current time-slice
    add     r6, r6, r4                                  ; 
    ld      r5, [r6]                                    ; Pickup current time-slice
    breq    r5, 0, __tx_thread_dont_save_ts             ; If not, skip save processing
;
;       /* Save time-slice for the thread and clear the current time-slice.  */
;       _tx_thread_current_ptr[core] -> tx_thread_time_slice =  _tx_timer_time_slice[core];
;       _tx_timer_time_slice =  0;
;
    st      r3, [r6]                                    ; Clear time-slice variable
    st      r5, [r0, 24]                                ; Save current time-slice
;
;    }
__tx_thread_dont_save_ts:
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr[core] =  TX_NULL;
;
    st      r3, [r7]                                    ; Clear current thread pointer
;
;    /* Set ready bit in thread control block.  */
;
    dmb     3                                           ; Data memory barrier
    st      1, [r0, 164]                                ; Set ready bit for this thread
;
;    /* Now clear protection. It is assumed that protection is in force whenever this routine is called.  */
;
    mov     r6, _tx_thread_smp_protection               ; Pickup address of protection structure

    .ifdef TX_SMP_DEBUG_ENABLE
    st      blink, [r6, 24]                             ; Save last caller
    ld      r5, [r6, 4]                                 ; Pickup last core owner
__error_loop:
    brne    r1, r5, __error_loop                        ; Check for an error - if found stay here!
    .endif

    st      r3, [gp, _tx_thread_preempt_disable@sda]    ; Clear the preempt disable flag
    st      r3, [r6, 12]                                ; Clear the protection count
    mov     r5, 0xFFFFFFFF                              ; Build invalid core value
    st      r5, [r6, 8]                                 ; Set core to an invalid value
    dmb     3                                           ; Data memory barrier
    st      r3, [r6]                                    ; Clear protection
    dmb     3                                           ; Data memory barrier

    b       _tx_thread_schedule                         ; Return to scheduler..
;
;}
    .end


