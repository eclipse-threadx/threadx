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
;    /* Define the lock for the ARC simulator workaround.  The ARC HS SMP
;       simulator does not execute cores in a lock-step fashion, i.e., a 
;       core can stall for many cycles while the other core executes. This
;       does not happen on actual hardware and thus the need for a schedule
;       lock is not required since:  1) ThreadX SMP will not load the same 
;       thread into two places on the _tx_thread_execute_list, and moving 
;       a thread from one entry on the _tx_thread_execute_list to another 
;       is more than the 5 instructions executed between examination of the
;       the _tx_thread_execute_list thread and clearing its ready bit in 
;       preparation for scheduling.  */
;
    .ifdef TX_ARC_SIMULATOR_WORKAROUND
    .globl  _tx_thread_schedule_lock
    .size   _tx_thread_schedule_lock, 4
    .type   _tx_thread_schedule_lock,@object
_tx_thread_schedule_lock:
    .word    0
    
    .endif
;
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_schedule                             SMP/ARC_HS/MetaWare */
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function waits for a thread control block pointer to appear in */ 
;/*    the _tx_thread_execute_ptr array. Once a thread pointer appears     */ 
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
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_schedule(VOID)
;{
    .global _tx_thread_schedule_restart
_tx_thread_schedule_restart:
    .ifdef TX_ARC_SIMULATOR_WORKAROUND
    mov     r7, _tx_thread_schedule_lock
    dmb     3
    mov     r3, 0
    st      r3, [r7]
    dmb     3
    .endif
    
    .global _tx_thread_schedule
    .type   _tx_thread_schedule, @function
_tx_thread_schedule:
;

;
;    /* Enable interrupts.  */
;
    mov     r0, 0x1F                                    ; Build enable interrupt value
    seti    r0                                          ; Enable interrupts
;
;    /* Wait for a thread to execute.  */
;    do
;    {
;
    lr      r1, [IDENTITY]                              ; Pickup core ID
    xbfu    r1, r1, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r1, r1, 1                                   ; Subtract 1 to make 0-based
    .endif
    asl     r4, r1, 2                                   ; Build index into core arrays
    mov     r7, _tx_thread_execute_ptr                  ; Pickup base of the execute thread pointer
    add     r7, r7, r4                                  ; Build address of current thread pointer 

__tx_thread_schedule_loop:
;
    clri                                                ; Lockout interrupts
    nop                                                 ; Delay for interrupts to really be disabled
    .ifdef TX_ARC_SIMULATOR_WORKAROUND
    mov     r0, _tx_thread_schedule_lock
    llock   r5, [r0]
    breq    r5, 0, _continue
    b       _tx_thread_schedule
_continue:
    add     r5, r1, 1
    scond   r5, [r0]
    beq_s   _got_lock
    b       _tx_thread_schedule
_got_lock:
    .endif
   
    ld      r0, [r7]                                    ; Pickup next thread to execute
    breq    r0, 0, _tx_thread_schedule_restart          ; If NULL, keep looking
;
;    }
;    while(_tx_thread_execute_ptr[core] == TX_NULL);
;    
;    
;    /* Now make sure the thread's ready bit is set.  */
;
    ld      r5, [r0, 164]                               ; Pickup the ready bit for this thread to see if it can be executed
    breq    r5, 0, _tx_thread_schedule_restart          ; If not set, start over
    st      0, [r0, 164]                                ; Clear the ready bit now that this thread is being executed
    dmb     3                                           ; Data memory barrier

    .ifdef TX_ARC_SIMULATOR_WORKAROUND
    mov     r7, _tx_thread_schedule_lock
    dmb     3
    mov     r3, 0
    st      r3, [r7]
    dmb     3
    .endif
;
;    /* Yes! We have a thread to execute - transfer control to it.  */
;
;    /* Setup the current thread pointer.  */
;    _tx_thread_current_ptr[core] =  _tx_thread_execute_ptr[core];
;
    mov     r7, _tx_thread_current_ptr                  ; Build address of current thread pointer
    add     r7, r7, r4                                  ; 
    st      r0, [r7]                                    ; Setup current thread pointer
;
;    /* Increment the run count for this thread.  */
;    _tx_thread_current_ptr[core] -> tx_thread_run_count++;
;
    ld      r3, [r0, 4]                                 ; Pickup run counter
    ld      r5, [r0, 24]                                ; Pickup time-slice for this thread
    add     r3, r3, 1                                   ; Increment run counter
    st      r3, [r0, 4]                                 ; Store the new run counter
;
;    /* Setup time-slice, if present.  */
;    _tx_timer_time_slice[core] =  _tx_thread_current_ptr -> tx_thread_time_slice;
;
    mov     r6, _tx_timer_time_slice                    ; Build address of time-slice for this core
    add     r6, r6, r4                                  ; 
    st      r5, [r6]                                    ; Setup time-slice
;
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the thread entry function to indicate the thread is executing.  */
;
    mov     r13, r0                                     ; Save thread control block pointer
    bl.d    _tx_execution_thread_enter                  ; Call the thread execution enter function
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    mov     r0, r13                                     ; Recover thread control block pointer
    .endif
;
;    /* Switch to the thread's stack.  */
;    sp =  _tx_thread_execute_ptr[core] -> tx_thread_stack_ptr;
;
    ld      sp, [r0, 8]                                 ; Switch to thread's stack
;
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
    ld      r1,  [sp, 68]                               ; Pickup status32
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

