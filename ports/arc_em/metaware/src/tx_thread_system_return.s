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

;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */
;/** ThreadX Component                                                     */
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/

    .equ    KSTACK_TOP,     0x264
    .equ    KSTACK_BASE,    0x265
    .equ    STATUS32_SC,    0x4000

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_system_return                        ARCv2_EM/MetaWare   */
;/*                                                           6.1.6        */
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
;/*  04-02-2021     Andres Mlinar            Modified comments,            */
;/*                                            resulting in version 6.1.6  */
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
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the thread exit function to indicate the thread is no longer executing.  */
;
    bl.d    _tx_execution_thread_exit                   ; Call the thread exit function
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    mov     r3, 0                                       ; Build clear value
    .endif
;
;    /* Save current stack and switch to system stack.  */
;    _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
;    sp = _tx_thread_system_stack_ptr;
;
    ld      r0, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread ptr
    st      sp, [r0, 8]                                 ; Save thread's stack pointer
    ld      r5, [gp, _tx_timer_time_slice@sda]          ; Pickup current time-slice

    .ifdef  TX_ENABLE_HW_STACK_CHECKING
    lr      r2, [status32]                              ; Pickup current STATUS32
    and     r2, r2, ~STATUS32_SC                        ; Clear the hardware stack checking enable bit (SC)
    kflag   r2                                          ; Disable hardware stack checking
    mov     r1, _tx_system_stack_top_address            ; Pickup top of system stack (lowest memory address)
    sr		r1, [KSTACK_TOP]                            ; Setup KSTACK_TOP
    mov     r1, _tx_system_stack_base_address           ; Pickup base of system stack (highest memory address)
    sr      r1, [KSTACK_BASE]                           ; Setup KSTACK_BASE
    .endif

    ld      sp, [gp, _tx_thread_system_stack_ptr@sda]   ; Switch to system stack

    .ifdef  TX_ENABLE_HW_STACK_CHECKING
    or      r2, r2, STATUS32_SC                         ; Or in hardware stack checking enable bit (SC)
    kflag   r2                                          ; Enable hardware stack checking
    .endif
;
;    /* Determine if the time-slice is active.  */
;    if (_tx_timer_time_slice)
;    {
;
    breq    r5, 0, __tx_thread_dont_save_ts             ; If not, skip save processing
;
;       /* Save time-slice for the thread and clear the current time-slice.  */
;       _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
;       _tx_timer_time_slice =  0;
;
    st      r3, [gp, _tx_timer_time_slice@sda]          ; Clear time-slice variable
    st      r5, [r0, 24]                                ; Save current time-slice
;
;    }
__tx_thread_dont_save_ts:
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
    b.d     _tx_thread_schedule                         ; Return to scheduler..
    st      r3, [gp, _tx_thread_current_ptr@sda]        ; ..clearing current thread pointer
;
;}
    .end


