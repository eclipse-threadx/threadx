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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .equ    BTA, 0x412
    
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_context_fast_restore                   ARC_HS/MetaWare   */
;/*                                                           6.2.1        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function restores the fast interrupt context, which can be a   */ 
;/*    nesting condition on a non-fast ISR, an idle system restore, a      */ 
;/*    restore of an interrupted thread, and a preemption of an interrupted*/ 
;/*    thread.                                                             */ 
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
;/*  03-08-2023     Cindy Deng               Modified comment(s), added    */
;/*                                            #include tx_user.h,         */
;/*                                            resulting in version 6.2.1  */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_fast_restore(VOID)
;{

    .global _tx_thread_context_fast_restore
    .type   _tx_thread_context_fast_restore, @function 
_tx_thread_context_fast_restore:
;
;    /* Note: it is assumed that the stack pointer is in the same position now as
;       it was after the last context fast save call.  */
;

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
    ld      r0, [gp, _tx_thread_system_state@sda]       ; Pickup system state contents
    sub     r0, r0, 1                                   ; Decrement the system state
    st      r0, [gp, _tx_thread_system_state@sda]       ; Store the new system state
    breq    r0, 0, __tx_thread_not_nested_restore       ; If zero, not a nested interrupt
;
;    /* Interrupts are nested.  */
;
;    /* Just recover the saved registers and return to the point of 
;       interrupt.  */
;

__tx_thread_nested_restore:
    rtie                                                ; Return from interrupt
;
;
;    }
__tx_thread_not_nested_restore:
;
;    /* Determine if a thread was interrupted and no preemption is required.  */
;    else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr))
;               || (_tx_thread_preempt_disable))
;    {
;
    ld      r0, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread pointer
    ld      r2, [gp, _tx_thread_preempt_disable@sda]    ; Pickup preempt disable flag
    sub.f   0, r0, 0                                    ; Set condition codes
    beq.d   __tx_thread_idle_system_restore             ; If NULL, idle system was interrupted
    lr      r4, [AUX_IRQ_ACT]                           ; Pickup the interrupt active register
    neg     r5, r4                                      ; Negate
    and     r5, r4, r5                                  ; See if there are any other interrupts present
    brne.d  r4, r5, __tx_thread_no_preempt_restore      ; If more interrupts, just return to the point of interrupt
    ld      r4, [gp, _tx_thread_execute_ptr@sda]        ; Pickup next thread to execute
    brne    r2, 0, __tx_thread_no_preempt_restore       ; If set, don't preempt executing thread
    brne    r0, r4, __tx_thread_preempt_restore         ; Not equal, preempt executing thread
;
;
__tx_thread_no_preempt_restore:
;
;    /* Restore interrupted thread or ISR.  */
;
;    /* Pickup the saved stack pointer.  */
;    sp =  _tx_thread_current_ptr -> tx_thread_stack_ptr;
;
;   /* Return to the point of interrupt.  */
;
    rtie                                                ; Return from interrupt
;
;    }
;    else
;    {
__tx_thread_preempt_restore:
;
    lr      r3, [status32_p0]                           ; Pickup the interrupted status32.p0 register
    lsr     r4, r3, 16                                  ; Move the register bank bits down
    and     r4, r4, 7                                   ; Isolate the register bank
    breq    r4, 0, __tx_software_interrupt_context      ; If register bank 0, software interrupt context is present
    st      ilink, [sp, 0]                              ; Save ilink (point of interrupt)
    st      r3, [sp, 4]                                 ; Save actual status32.p0
    bclr    r4, r3, 16                                  ; Build register bank 0 value
    bclr    r4, r4, 17                                  ;
    bclr    r4, r4, 18                                  ;
    sr      r4, [status32_p0]                           ; Setup status32.p0 to return to bank 0 when fast ISR is finishe
    mov     ilink, sp                                   ; Pass the information back to the other register bank via ilink
    bclr    r3, r3, 31                                  ; Make sure interrupts are not enabled
    kflag   r3                                          ; Switch back to the interrupted thread's hardware register bank
    sub     sp, sp, 168                                 ; Allocate an hardware interrupt stack frame
    st      r0, [sp, 132]                               ; Temporarily save r0
    mov     r0, 3                                       ; Build hardware interrupt stack type
    st      r0,  [sp, 0]                                ; Setup interrupt stack type
    
    .ifndef  TX_DISABLE_LP
    lr      r0, [LP_START]                              ; Pickup LP_START
    st      r0, [sp, 4]                                 ; Save LP_START
    lr      r0, [LP_END]                                ; Pickup LP_END
    st      r0, [sp, 8]                                 ; Save LP_END
    st      LP_COUNT, [sp, 12]                          ; Save LP_COUNT
    .endif

    lr      r0, [BTA]                                   ; Pickup BTA
    st      r0, [sp, 156]                               ; Save BTA
    ld      r0, [ilink, 0]                              ; Pickup the point of interrupt
    st      r0, [sp, 160]                               ; Setup the point of interrupt
    st      r0, [sp, 20]                                ; Save ilink
    ld      r0, [ilink, 4]                              ; Pickup the status32
    st      r0, [sp, 164]                               ; Setup the status32
    ld      r0, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread pointer
    st      sp, [r0, 8]                                 ; Save stack pointer in thread control block
    ld      r0, [sp, 132]                               ; Restore r0
    lr      ilink, [status32]                           ; Pickup status32 register
    bset    ilink, ilink, 16                            ; Build register bank 1 value
    bclr    ilink, ilink, 17                            ;
    bclr    ilink, ilink, 18                            ;
    kflag   ilink                                       ; Move back to register bank 0
    b       __tx_preempt_save_done                      ; Done, finished with preemption save

__tx_software_interrupt_context:    
    st      ilink, [sp, 0]                              ; Save ilink (point of interrupt)
    st      r3, [sp, 4]                                 ; Save status32
    mov     ilink, sp                                   ; Pass the information back to the other register bank via ilink
    bclr    r3, r3, 31                                  ; Make sure interrupts are not enabled
    kflag   r3                                          ; Switch back to the interrupted thread's hardware register bank
    sub     sp, sp, 168                                 ; Allocate an hardware interrupt stack frame
    st      blink, [sp, 16]                             ; Save blink
    st      fp,  [sp, 24]                               ; Save fp
    st      gp,  [sp, 28]                               ; Save gp
    st      r25, [sp, 32]                               ; Save r25
    st      r24, [sp, 36]                               ; Save r24
    st      r23, [sp, 40]                               ; Save r23
    st      r22, [sp, 44]                               ; Save r22
    st      r21, [sp, 48]                               ; Save r21
    st      r20, [sp, 52]                               ; Save r20
    st      r19, [sp, 56]                               ; Save r19
    st      r18, [sp, 60]                               ; Save r18
    st      r17, [sp, 64]                               ; Save r17
    st      r16, [sp, 68]                               ; Save r16
    st      r15, [sp, 72]                               ; Save r15
    st      r14, [sp, 76]                               ; Save r14
    st      r13, [sp, 80]                               ; Save r13
    st      r12, [sp, 84]                               ; Save r12
    st      r11, [sp, 88]                               ; Save r11
    st      r10, [sp, 92]                               ; Save r10
    st      r9,  [sp, 96]                               ; Save r9
    st      r8,  [sp, 100]                              ; Save r8
    st      r7,  [sp, 104]                              ; Save r7
    st      r6,  [sp, 108]                              ; Save r6
    st      r5,  [sp, 112]                              ; Save r5
    st      r4,  [sp, 116]                              ; Save r6
    st      r3,  [sp, 120]                              ; Save r3
    st      r2,  [sp, 124]                              ; Save r2
    st      r1,  [sp, 128]                              ; Save r1
    st      r0,  [sp, 132]                              ; Save r0
    st      r30, [sp, 136]                              ; Save r30
    
    .ifndef  TX_DISABLE_LP
    lr      r10, [LP_START]                             ; Pickup LP_START
    lr      r9,  [LP_END]                               ; Pickup LP_END
    st      LP_COUNT, [sp, 12]                          ; Save LP_COUNT
    st      r10, [sp, 4]                                ; Save LP_START
    st      r9,  [sp, 8]                                ; Save LP_END
    .endif

    lr      r0,  [BTA]                                  ; Pickup BTA
    st      r0,  [sp, 156]                              ; Save BTA
    mov     r6, 1                                       ; Build interrupt stack type
    st      r6,  [sp, 0]                                ; Setup interrupt stack type
    ld      r0, [ilink, 0]                              ; Pickup the point of interrupt
    st      r0, [sp, 160]                               ; Setup the point of interrupt
    st      r0, [sp, 20]                                ; Save ilink
    ld      r0, [ilink, 4]                              ; Pickup the status32
    st      r0, [sp, 164]                               ; Setup the status32
    ld      r0, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread pointer
    st      sp, [r0, 8]                                 ; Save stack pointer in thread control block
    lr      ilink, [status32]                           ; Pickup status32 register
    bset    ilink, ilink, 16                            ; Build register bank 1 value
    bclr    ilink, ilink, 17                            ;
    bclr    ilink, ilink, 18                            ;
    kflag   ilink                                       ; Move back to register bank 1
__tx_preempt_save_done:
;
    ld      r0, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread ptr
;
;    /* Save the remaining time-slice and disable it.  */
;    if (_tx_timer_time_slice)
;    {
;
    ld      r2, [gp, _tx_timer_time_slice@sda]          ; Pickup time-slice contents
    mov     r7, 0                                       ; Build clear/NULL value
    breq    r2, 0, __tx_thread_dont_save_ts             ; No time-slice, don't need to save it
;
;        _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
;        _tx_timer_time_slice =  0;
;
    st      r2, [r0, 24]                                ; If set, save remaining time-slice
    st      r7, [gp, _tx_timer_time_slice@sda]          ; If set, clear time slice
;
;    }
__tx_thread_dont_save_ts:
;
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
    st      r7, [gp, _tx_thread_current_ptr@sda]        ; Set current thread ptr to NULL
    mov     ilink, _tx_thread_schedule                  ; Build address of scheduler
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

    mov     ilink, _tx_thread_schedule                  ; Build address of scheduler
    rtie                                                ; Return from interrupt to scheduler
;
;}
    .end

