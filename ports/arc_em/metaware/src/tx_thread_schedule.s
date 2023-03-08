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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .equ    BTA, 0x412
    .equ    KSTACK_TOP,     0x264
    .equ    KSTACK_BASE,    0x265
    .equ    STATUS32_SC,    0x4000

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_schedule                             ARCv2_EM/MetaWare   */
;/*                                                           6.2.1        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function waits for a thread control block pointer to appear in */
;/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */
;/*    in the variable, the corresponding thread is resumed.               */
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
;/*    None                                                                */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    _tx_initialize_kernel_enter          ThreadX entry function         */
;/*    _tx_thread_system_return             Return to system from thread   */
;/*    _tx_thread_context_restore           Restore thread's context       */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*  04-02-2021     Andres Mlinar              Modified comment(s), and    */
;/*                                            fixed interrupt priority    */
;/*                                            overwritting bug, and       */
;/*                                            fixed hardware stack checker*/
;/*                                            disable and reenable logic, */
;/*                                            resulting in version 6.1.6  */
;/*  10-15-2021     Andres Mlinar            Modified comment(s), added    */
;/*                                            support for disabling the   */
;/*                                            loop control feature,       */
;/*                                            improved internal logic,    */
;/*                                            resulting in version 6.1.9  */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_schedule(VOID)
;{
    .global _tx_thread_schedule
    .type   _tx_thread_schedule, @function
_tx_thread_schedule:

    mov     sp, _estack

    .global _tx_thread_schedule_reenter
    .type   _tx_thread_schedule_reenter, @function
_tx_thread_schedule_reenter:

;
;    /* Enable interrupts.  */
;
    seti    0                                           ; Enable interrupts without changing threshold level

;
;    /* Wait for a thread to execute.  */
;    do
;    {
;
__tx_thread_schedule_loop:
;
    ld      r0, [gp, _tx_thread_execute_ptr@sda]        ; Pickup next thread to execute
    breq    r0, 0, __tx_thread_schedule_loop            ; If NULL, keep looking
;
;    }
;    while(_tx_thread_execute_ptr == TX_NULL);
;
;    /* Yes! We have a thread to execute.  Lockout interrupts and
;       transfer control to it.  */
;
    clri                                                ; Lockout interrupts
    nop                                                 ; Delay for interrupts to really be disabled
;
;    /* Setup the current thread pointer.  */
;    _tx_thread_current_ptr =  _tx_thread_execute_ptr;
;
    st      r0, [gp, _tx_thread_current_ptr@sda]        ; Setup current thread pointer
;
;    /* Increment the run count for this thread.  */
;    _tx_thread_current_ptr -> tx_thread_run_count++;
;
    ld      r3, [r0, 4]                                 ; Pickup run counter
    ld      r4, [r0, 24]                                ; Pickup time-slice for this thread
    add     r3, r3, 1                                   ; Increment run counter
    st      r3, [r0, 4]                                 ; Store the new run counter
;
;    /* Setup time-slice, if present.  */
;    _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;
;
    st      r4, [gp, _tx_timer_time_slice@sda]          ; Setup time-slice
;
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the thread entry function to indicate the thread is executing.  */
;
    bl.d    _tx_execution_thread_enter                  ; Call the thread execution enter function
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    .endif
;
;    /* Switch to the thread's stack.  */
;    sp =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;
;
    .ifdef  TX_ENABLE_HW_STACK_CHECKING
    lr      r2, [status32]                              ; Pickup current STATUS32
    and     r2, r2, ~STATUS32_SC                        ; Clear the hardware stack checking enable bit (SC)
    kflag   r2                                          ; Disable hardware stack checking
    .endif

    ld      sp, [r0, 8]                                 ; Switch to thread's stack

    .ifdef  TX_ENABLE_HW_STACK_CHECKING
    ld      r3, [r0, 12]                                ; Pickup the top of the thread's stack (lowest address)
    sr      r3, [KSTACK_TOP]                            ; Setup KSTACK_TOP
    ld      r3, [r0, 16]                                ; Pickup the base of the thread's stack (highest address)
    sr      r3, [KSTACK_BASE]                           ; Setup KSTACK_BASE
    or      r2, r2, STATUS32_SC                         ; Or in hardware stack checking enable bit (SC)
    kflag   r2                                          ; Enable hardware stack checking
    .endif

;    /* Determine if an interrupt frame or a synchronous task suspension frame
;       is present.  */
;
    ld      r1, [sp, 0]                                 ; Pickup the stack type
    brne    r1, 0, __tx_thread_schedule_int_ret         ; Compare to solicited stack type. If not, thread was interrupted
    ld      blink, [sp, 4]                              ; Recover blink
    ld      fp,  [sp, 8]                                ; Recover fp
    ld      gp,  [sp, 12]                               ; Recover gp
    ld      r25, [sp, 16]                               ; Recover r25
    ld      r24, [sp, 20]                               ; Recover r24
    ld      r23, [sp, 24]                               ; Recover r23
    ld      r22, [sp, 28]                               ; Recover r22
    ld      r21, [sp, 32]                               ; Recover r21
    ld      r20, [sp, 36]                               ; Recover r20
    ld      r19, [sp, 40]                               ; Recover r19
    ld      r18, [sp, 44]                               ; Recover r18
    ld      r17, [sp, 48]                               ; Recover r17
    ld      r16, [sp, 52]                               ; Recover r16
    ld      r15, [sp, 56]                               ; Recover r15
    ld      r14, [sp, 60]                               ; Recover r14
    ld      r13, [sp, 64]                               ; Recover r13
    ld      r1,  [sp, 68]                               ; Pickup STATUS32
    ld      r30, [sp, 72]                               ; Recover r30
    add     sp, sp, 76                                  ; Recover solicited stack frame
    j_s.d   [blink]                                     ; Return to thread and restore flags
    seti    r1                                          ; Recover STATUS32
;
__tx_thread_schedule_int_ret:
;
    mov     r0, 0x2                                     ; Pretend level 1 interrupt is returning
    sr      r0, [AUX_IRQ_ACT]                           ;

    .ifndef  TX_DISABLE_LP
    ld      r0, [sp, 4]                                 ; Recover LP_START
    sr      r0, [LP_START]                              ; Restore LP_START
    ld      r1, [sp, 8]                                 ; Recover LP_END
    sr      r1, [LP_END]                                ; Restore LP_END
    ld      r2, [sp, 12]                                ; Recover LP_COUNT
    mov     LP_COUNT, r2
    .endif

    ld      r0, [sp, 156]                               ; Pickup saved BTA
    sr      r0, [BTA]                                   ; Recover BTA
    ld      blink, [sp, 16]                             ; Recover blink
    ld      ilink, [sp, 20]                             ; Recover ilink
    ld      fp, [sp, 24]                                ; Recover fp
    ld      gp, [sp, 28]                                ; Recover gp
    ld      r25, [sp, 32]                               ; Recover r25
    ld      r24, [sp, 36]                               ; Recover r24
    ld      r23, [sp, 40]                               ; Recover r23
    ld      r22, [sp, 44]                               ; Recover r22
    ld      r21, [sp, 48]                               ; Recover r21
    ld      r20, [sp, 52]                               ; Recover r20
    ld      r19, [sp, 56]                               ; Recover r19
    ld      r18, [sp, 60]                               ; Recover r18
    ld      r17, [sp, 64]                               ; Recover r17
    ld      r16, [sp, 68]                               ; Recover r16
    ld      r15, [sp, 72]                               ; Recover r15
    ld      r14, [sp, 76]                               ; Recover r14
    ld      r13, [sp, 80]                               ; Recover r13
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
    ld      r0,  [sp, 132]                              ; Recover r0
    ld      r30, [sp, 136]                              ; Recover r30
    .ifdef   TX_ENABLE_ACC
    ld      r58, [sp, 140]                              ; Recover r58
    ld      r59, [sp, 144]                              ; Recover r59
    .endif
    add     sp, sp, 160                                 ; Recover interrupt stack frame
    rtie                                                ; Return to point of interrupt
;
;}
;
    .end

