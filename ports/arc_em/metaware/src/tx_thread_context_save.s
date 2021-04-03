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

    .equ    BTA, 0x412
    .equ    KSTACK_TOP,     0x264
    .equ    KSTACK_BASE,    0x265
    .equ    STATUS32_SC,    0x4000

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_context_save                         ARCv2_EM/MetaWare   */
;/*                                                           6.1.6        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function saves the context of an executing thread in the       */
;/*    beginning of interrupt processing.  The function also ensures that  */
;/*    the system stack is used upon return to the calling ISR.            */
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
;/*    ISRs                                                                */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*  04-02-2021     Andres Mlinar            Modified comment(s), and      */
;/*                                            r25/r30 are caller saved,   */
;/*                                            resulting in version 6.1.6  */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_save(VOID)
;{
    .global _tx_thread_context_save
    .type   _tx_thread_context_save, @function
_tx_thread_context_save:
;
;    /* Upon entry to this routine, it is assumed that an interrupt stack frame
;       has already been allocated, and the interrupted blink register is already saved.  */
;
    clri                                                ; Disable interrupts
    st      r1, [sp, 128]                               ; Save r1
    st      r0, [sp, 132]                               ; Save r0
;
;    /* Check for a nested interrupt condition.  */
;    if (_tx_thread_system_state++)
;    {
;
    ld      r0, [gp, _tx_thread_system_state@sda]       ; Pickup system state
    st      r3, [sp, 120]                               ; Save r3
    st      r2, [sp, 124]                               ; Save r2
    breq    r0, 0, __tx_thread_not_nested_save          ; If 0, we are not in a nested
                                                        ;   condition
;
;    /* Nested interrupt condition.  */
;
    add     r0, r0, 1                                   ; Increment the nested interrupt count
    st      r0, [gp, _tx_thread_system_state@sda]       ; Update system state
;
;   /* Save the rest of the scratch registers on the stack and return to the
;      calling ISR.  */
;
__tx_thread_nested_save:                                ; Label is for special nested interrupt case from idle system save below
    st      r30, [sp, 136]                              ; Save r30
    st      r25, [sp, 32]                               ; Save r25
    st      r12, [sp, 84]                               ; Save r12
    st      r11, [sp, 88]                               ; Save r11
    st      r10, [sp, 92]                               ; Save r10
    st      r9,  [sp, 96]                               ; Save r9
    st      r8,  [sp, 100]                              ; Save r8
    st      r7,  [sp, 104]                              ; Save r7
    st      r6,  [sp, 108]                              ; Save r6
    st      r5,  [sp, 112]                              ; Save r5
    st      r4,  [sp, 116]                              ; Save r4
    lr      r10, [LP_START]                             ; Pickup LP_START
    lr      r9,  [LP_END]                               ; Pickup LP_END
    st      LP_COUNT, [sp, 12]                          ; Save LP_COUNT
    st      r10, [sp, 4]                                ; Save LP_START
    st      r9,  [sp, 8]                                ; Save LP_END
    .ifdef   TX_ENABLE_ACC
    st      r58, [sp, 140]                              ; Save r58
    st      r59, [sp, 144]                              ; Save r59
    .endif
    lr      r0,  [BTA]                                  ; Pickup BTA
    st      r0,  [sp, 156]                              ; Save BTA

;
;    /* Return to the ISR.  */
;
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR enter function to indicate an ISR is executing.  */
;
    sub     sp, sp, 32                                  ; Allocating some space on the stack
    st      blink, [sp, 16]                             ; Save blink
    bl.d    _tx_execution_isr_enter                     ; Call the ISR enter function
    nop                                                 ; Delay slot
    ld      blink, [sp, 16]                             ; Recover blink
    add     sp, sp, 32                                  ; Recover the stack space
    .endif
;

    j.d     [blink]                                     ; Return to Level 1 ISR
    st      ilink, [sp, 20]                             ; Save ilink
;
__tx_thread_not_nested_save:
;    }
;
;    /* Otherwise, not nested, check to see if a thread was running.  */
;    else if (_tx_thread_current_ptr)
;    {
;
    add     r0, r0, 1                                   ; Increment the nested interrupt count
    st      r0, [gp, _tx_thread_system_state@sda]       ; Update system state
    ld      r1, [gp, _tx_thread_current_ptr@sda]        ; Pickup current thread pointer
    st      r30, [sp, 136]                              ; Save r30
    st      r25, [sp, 32]                               ; Save r25
    st      r12, [sp, 84]                               ; Save r12
    st      r11, [sp, 88]                               ; Save r11
    breq    r1, 0, __tx_thread_idle_system_save         ; If no thread is running, idle system was
                                                        ;   interrupted.
;
;    /* Save minimal context of interrupted thread.  */
;
    st      r10, [sp, 92]                               ; Save r10
    st      r9,  [sp, 96]                               ; Save r9
    st      r8,  [sp, 100]                              ; Save r8
    st      r7,  [sp, 104]                              ; Save r7
    st      r6,  [sp, 108]                              ; Save r6
    st      r5,  [sp, 112]                              ; Save r5
    st      r4,  [sp, 116]                              ; Save r4
    lr      r10, [LP_START]                             ; Pickup LP_START
    lr      r9,  [LP_END]                               ; Pickup LP_END
    st      LP_COUNT, [sp, 12]                          ; Save LP_COUNT
    st      r10, [sp, 4]                                ; Save LP_START
    st      r9,  [sp, 8]                                ; Save LP_END
    st      ilink, [sp, 20]                             ; Save ilink
    .ifdef   TX_ENABLE_ACC
    st      r58, [sp, 140]                              ; Save r58
    st      r59, [sp, 144]                              ; Save r59
    .endif
    lr      r0,  [BTA]                                  ; Pickup BTA
    st      r0,  [sp, 156]                              ; Save BTA
;
;    /* Save the current stack pointer in the thread's control block.  */
;    _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
;
    st      sp, [r1, 8]                                 ; Save thread's stack pointer

    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR enter function to indicate an ISR is executing.  */
;
    sub     sp, sp, 32                                  ; Allocating some space on the stack
    st      blink, [sp, 16]                             ; Save blink
    bl.d    _tx_execution_isr_enter                     ; Call the ISR enter function
    nop                                                 ; Delay slot
    ld      blink, [sp, 16]                             ; Recover blink
    add     sp, sp, 32                                  ; Recover the stack space
    .endif

    .ifdef  TX_ENABLE_HW_STACK_CHECKING
    lr      r2, [status32]                              ; Pickup current STATUS32
    and     r2, r2, ~STATUS32_SC                        ; Clear the hardware stack checking enable bit (SC)
    kflag   r2                                          ; Disable hardware stack checking
    mov     r1, _tx_system_stack_top_address            ; Pickup top of system stack (lowest memory address)
    sr		r1, [KSTACK_TOP]                            ; Setup KSTACK_TOP
    mov     r1, _tx_system_stack_base_address           ; Pickup base of system stack (highest memory address)
    sr      r1, [KSTACK_BASE]                           ; Setup KSTACK_BASE
    ld      sp, [gp, _tx_thread_system_stack_ptr@sda]   ; Switch to system stack
    or      r2, r2, STATUS32_SC                         ; Or in hardware stack checking enable bit (SC)
    j_s.d   [blink]                                     ; Return to calling ISR
    kflag   r2                                          ; Enable hardware stack checking
    .else
;
;    /* Switch to the system stack.  */
;    sp =  _tx_thread_system_stack_ptr;
;
    j_s.d   [blink]                                     ; Return to calling ISR
    ld      sp, [gp, _tx_thread_system_stack_ptr@sda]   ; Switch to system stack
    .endif
;
;    }
;    else
;    {
;
__tx_thread_idle_system_save:
;
;    /* Interrupt occurred in the scheduling loop.  */
;
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR enter function to indicate an ISR is executing.  */
;
    sub     sp, sp, 32                                  ; Allocating some space on the stack
    st      blink, [sp, 16]                             ; Save blink
    bl.d    _tx_execution_isr_enter                     ; Call the ISR enter function
    nop                                                 ; Delay slot
    ld      blink, [sp, 16]                             ; Recover blink
    add     sp, sp, 32                                  ; Recover the stack space
    .endif
;
;     /* See if we have a special nesting condition.  This happens when the higher priority
;        interrupt occurs before the nested interrupt logic is valid.  */
;
    lr      r0, [AUX_IRQ_ACT]                           ; Pickup the interrupt active register
    neg     r1, r0                                      ; Negate
    and     r1, r0, r1                                  ; See if there are any other interrupts present
    breq    r0, r1, __tx_thread_not_nested
    j __tx_thread_nested_save                           ; If more interrupts, go into the nested interrupt save logic
__tx_thread_not_nested:
;
;    /* Not much to do here, just adjust the stack pointer, and return to
;       ISR processing.  */
;
    j_s.d   [blink]                                     ; Return to ISR
    add     sp, sp, 160                                 ; Recover stack space
;
;    }
;}
    .end
