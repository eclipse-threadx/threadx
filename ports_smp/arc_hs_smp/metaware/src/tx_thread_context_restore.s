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
;
;#define TX_SOURCE_CODE
;
    .equ    BTA, 0x412
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
;/*    _tx_thread_context_restore                      SMP/ARC_HS/MetaWare */
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function restores the interrupt context if it is processing a  */ 
;/*    nested interrupt.  If not, it returns to the interrupt thread if no */ 
;/*    preemption is necessary.  Otherwise, if preemption is necessary or  */ 
;/*    if no thread was running, the function returns to the scheduler.    */ 
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
;/*    _tx_thread_schedule                   Thread scheduling routine     */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    ISRs                                  Interrupt Service Routines    */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_restore(VOID)
;{
    .global _tx_thread_context_restore
    .type   _tx_thread_context_restore, @function 
_tx_thread_context_restore:
;
;    /* Note: it is assumed that the stack pointer is in the same position now as
;       it was after the last context save call.  */
;
;    /* Lockout interrupts.  */
;
    clri                                                ; Disable interrupts
    nop                                                 ; Delay for interrupts to really be disabled

    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR exit function to indicate an ISR is complete.  */
;
    bl.d    _tx_execution_isr_exit                      ; Call the ISR exit function
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    .endif
;
;    /* Determine if interrupts are nested.  */
;    if (--_tx_thread_system_state)
;    {
;
    lr      r3, [IDENTITY]                              ; Pickup core ID
    xbfu    r3, r3, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r3, r3, 1                                   ; Subtract 1 to make 0-based
    .endif
    asl     r3, r3, 2                                   ; Build index into core arrays
    mov     r0, _tx_thread_system_state                 ; Build address of system state
    add     r1, r0, r3                                  ; 
    ld      r0, [r1]                                    ; Pickup system state
    sub     r0, r0, 1                                   ; Decrement the system state
    st      r0, [r1]                                    ; Store the new system state
    breq    r0, 0, __tx_thread_not_nested_restore       ; If zero, not a nested interrupt
;
;    /* Interrupts are nested.  */
;
;    /* Just recover the saved registers and return to the point of 
;       interrupt.  */
;

__tx_thread_nested_restore:

    .ifndef  TX_DISABLE_LP
    ld      r0, [sp, 4]                                 ; Recover LP_START
    sr      r0, [LP_START]                              ; Restore LP_START
    ld      r4, [sp, 8]                                 ; Recover LP_END
    sr      r4, [LP_END]                                ; Restore LP_END
    ld      r2, [sp, 12]                                ; Recover LP_COUNT 
    mov     LP_COUNT, r2
    .endif
    
    ld      r2, [sp, 156]                               ; Pickup BTA
    sr      r2, [BTA]                                   ; Recover BTA
    .ifdef  TX_ENABLE_ACC
    ld      r58, [sp, 140]                              ; Recover r58
    ld      r59, [sp, 144]                              ; Recover r59
    .endif
    ld      blink, [sp, 16]                             ; Recover blink
    ld      r12, [sp, 84]                               ; Recover r12
    ld      r11, [sp, 88]                               ; Recover r11
    ld      r10, [sp, 92]                               ; Recover r10
    ld      r9,  [sp, 96]                               ; Recover r9
    ld      r8,  [sp, 100]                              ; Recover r8
    ld      r7,  [sp, 104]                              ; Recover r7
    ld      r6,  [sp, 108]                              ; Recover r6
    ld      r5,  [sp, 112]                              ; Recover r5
    ld      r4,  [sp, 116]                              ; Recover r4
    ld      r3,  [sp, 120]                              ; Recover r3
    ld      r2,  [sp, 124]                              ; Recover r2
    ld      r1,  [sp, 128]                              ; Recover r1
    ld      r0, [sp, 132]                               ; Recover r0
    add     sp, sp, 160                                 ; Recover interrupt stack frame 
    rtie                                                ; Return from interrupt
;
;
;    }
__tx_thread_not_nested_restore:
;
;    /* Determine if a thread was interrupted and no preemption is required.  */
;    else if (((_tx_thread_current_ptr[core]) && (_tx_thread_current_ptr[core] == _tx_thread_execute_ptr[core]) 
;               || (_tx_thread_preempt_disable))
;    {
;
    mov     r1, _tx_thread_current_ptr                  ; Build current thread pointer address
    add     r1, r1, r3                                  ; 
    ld      r0, [r1]                                    ; Pickup current thread pointer
    mov     r2, _tx_thread_smp_protection               ; Build protection address
    sub.f   0, r0, 0                                    ; Set condition codes
    beq.d   __tx_thread_idle_system_restore             ; If NULL, idle system was interrupted
    lr      r4, [AUX_IRQ_ACT]                           ; Pickup the interrupt active register
    neg     r5, r4                                      ; Negate
    and     r5, r4, r5                                  ; See if there are any other interrupts present
    brne    r4, r5, __tx_thread_no_preempt_restore      ; If more interrupts, just return to the point of interrupt
    mov     r4, _tx_thread_execute_ptr                  ; Build address of next thread to execute for this core
    add     r4, r4, r3                                  ; 
    ld      r4, [r4]                                    ; Pickup next thread to execute
    ld      r2, [r2, 8]                                 ; Pickup core that has protection
    breq    r0, r4, __tx_thread_no_preempt_restore      ; If equal, simply restore executing thread
    ld      r5, [gp, _tx_thread_preempt_disable@sda]    ; Pickup preempt disable flag
    asr     r4, r3, 2                                   ; Shift core index back down for core ID    
    brne    r2, r4, __tx_thread_preempt_restore         ; If the core doesn't have protection, preempt thread without looking at preempt disable flag
    breq    r5, 0, __tx_thread_preempt_restore          ; If preempt disable not set, preempt executing thread
;
;
__tx_thread_no_preempt_restore:
;
;    /* Restore interrupted thread or ISR.  */
;
;    /* Pickup the saved stack pointer.  */
;    sp =  _tx_thread_current_ptr[core] -> tx_thread_stack_ptr;
;

;   /* Recover the saved context and return to the point of interrupt.  */
;
    ld      sp, [r0, 8]                                 ; Switch back to thread's stack

    .ifndef  TX_DISABLE_LP
    ld      r0, [sp, 4]                                 ; Recover LP_START
    sr      r0, [LP_START]                              ; Restore LP_START
    ld      r4, [sp, 8]                                 ; Recover LP_END
    sr      r4, [LP_END]                                ; Restore LP_END
    ld      r2, [sp, 12]                                ; Recover LP_COUNT 
    mov     LP_COUNT, r2
    .endif

    ld      r2, [sp, 156]                               ; Pickup BTA
    sr      r2, [BTA]                                   ; Recover BTA
    .ifdef  TX_ENABLE_ACC
    ld      r58, [sp, 140]                              ; Recover r58
    ld      r59, [sp, 144]                              ; Recover r59
    .endif
    ld      blink, [sp, 16]                             ; Recover blink
    ld      r12, [sp, 84]                               ; Recover r12
    ld      r11, [sp, 88]                               ; Recover r11
    ld      r10, [sp, 92]                               ; Recover r10
    ld      r9,  [sp, 96]                               ; Recover r9
    ld      r8,  [sp, 100]                              ; Recover r8
    ld      r7,  [sp, 104]                              ; Recover r7
    ld      r6,  [sp, 108]                              ; Recover r6
    ld      r5,  [sp, 112]                              ; Recover r5
    ld      r4,  [sp, 116]                              ; Recover r4
    ld      r3,  [sp, 120]                              ; Recover r3
    ld      r2,  [sp, 124]                              ; Recover r2
    ld      r1,  [sp, 128]                              ; Recover r1
    ld      r0, [sp, 132]                               ; Recover r0
    add     sp, sp, 160                                 ; Recover interrupt stack frame
    rtie                                                ; Return from interrupt
;
;    }
;    else
;    {
__tx_thread_preempt_restore:
;
    ld      r7, [r0, 8]                                 ; Pickup stack pointer
    mov     r6, 1                                       ; Build interrupt stack type
    st      r6,  [r7, 0]                                ; Setup interrupt stack type
    st      fp,  [r7, 24]                               ; Save fp
    st      gp,  [r7, 28]                               ; Save gp
    st      r25, [r7, 32]                               ; Save r25
    st      r24, [r7, 36]                               ; Save r24
    st      r23, [r7, 40]                               ; Save r23
    st      r22, [r7, 44]                               ; Save r22
    st      r21, [r7, 48]                               ; Save r21
    st      r20, [r7, 52]                               ; Save r20
    st      r19, [r7, 56]                               ; Save r19
    st      r18, [r7, 60]                               ; Save r18
    st      r17, [r7, 64]                               ; Save r17
    st      r16, [r7, 68]                               ; Save r16
    st      r15, [r7, 72]                               ; Save r15
    st      r14, [r7, 76]                               ; Save r14
    st      r13, [r7, 80]                               ; Save r13
    st      r30, [r7, 136]                              ; Save r30
__tx_preempt_save_done:
;
;    /* Save the remaining time-slice and disable it.  */
;    if (_tx_timer_time_slice)
;    {
;
    mov     r5, _tx_timer_time_slice                    ; Build time-slice address
    add     r5, r5, r3                                  ; 
    ld      r2, [r5]                                    ; Pickup time-slice contents
    mov     r7, 0                                       ; Build clear/NULL value
    breq    r2, 0, __tx_thread_dont_save_ts             ; No time-slice, don't need to save it
;
;        _tx_thread_current_ptr[core] -> tx_thread_time_slice =  _tx_timer_time_slice[core];
;        _tx_timer_time_slice =  0;
;
    st      r2, [r0, 24]                                ; If set, save remaining time-slice
    st      r7, [r5]                                    ; If set, clear time slice
;
;    }
__tx_thread_dont_save_ts:
;
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr[core] =  TX_NULL;
;
    st      r7, [r1]                                    ; Set current thread ptr to NULL
;
;    /* Set bit indicating this thread is ready for execution.  */
;
    dmb     3                                           ; Data memory barrier
    st      1, [r0, 164]                                ; Set ready bit

    sub     sp, sp, 8                                   ; Allocate a small stack frame on the system stack
    lr      r0, [STATUS32]                              ; Pickup STATUS32
    st      r0, [sp, 4]                                 ; Place on stack
    mov     r0, _tx_thread_schedule                     ; Build address of scheduler
    st      r0, [sp, 0]                                 ; Write over the point of interrupt
    rtie                                                ; Return from interrupt to scheduler
;
;    }
;
;    /* Return to the scheduler.  */
;    _tx_thread_schedule();
;
__tx_thread_idle_system_restore:

    lr      r4, [AUX_IRQ_ACT]                           ; Pickup the interrupt active register
    neg     r5, r4                                      ; Negate
    and     r5, r4, r5                                  ; See if there are any other interrupts present
    sub.f   0, r4, r5                                   ; Set condition codes
    bne     __tx_thread_nested_restore                  ; If more interrupts, just return to the point of interrupt

    lr      r0, [STATUS32]                              ; Pickup STATUS32
    st      r0, [sp, 4]                                 ; Place on stack
    mov     r0, _tx_thread_schedule                     ; Build address of scheduler
    st      r0, [sp, 0]                                 ; Write over the point of interrupt
    rtie                                                ; Return from interrupt to scheduler
;
;}
    .end

