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

    extern __tx_thread_system_state
    extern __tx_thread_current_ptr

    section .text:CODE:ROOT
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_context_save                              RXv2/IAR       */
;/*                                                           6.1.3        */
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
;/*  12-31-2020     William E. Lamie         Initial Version 6.1.3         */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_save(VOID)
;{
    public __tx_thread_context_save

__tx_thread_context_save:
;
;    /* Upon entry to this routine, it is assumed that interrupts are locked
;       out and the (interrupt) stack frame looks like the following:
;
;           (lower address) SP   ->     [return address of this call]
;                           SP+4 ->     Saved R1
;                           SP+8 ->     Saved R2
;                           SP+12->     Interrupted PC
;                           SP+16->     Interrupted PSW
;
;    /* Check for a nested interrupt condition.  */
;    if (_tx_thread_system_state++)
;    {
;

    MOV.L   #__tx_thread_system_state, R1     ; pick up address of system state
    MOV.L   [R1], R2                           ; pick up system state
    CMP     #0, R2                             ; 0 -> no nesting
    BEQ     __tx_thread_not_nested_save
;
;    /* Nested interrupt condition.  */
;   
    ADD   #1, r2                             ; _tx_thread_system_state++
    MOV.L   r2, [r1]

;
;   /* Save the rest of the scratch registers on the interrupt stack and return to the
;       calling ISR.  */
    POP R1                                      ; recuperate return address from stack
    PUSHM   R3-R5
    PUSHM   R14-R15
    PUSHC   FPSW                                ; (top) FPSW, R14, R15, R3, R4, R5, R1, R2, PC, PSW (bottom)
    JMP     R1                                  ; return address was preserved in R1

;
__tx_thread_not_nested_save:
;    }
;
;    /* Otherwise, not nested, check to see if a thread was running.  */
;    else if (_tx_thread_current_ptr)
;    {
;
    ADD     #1, R2                                ; _tx_thread_system_state++
    MOV.L   R2, [R1]

    MOV.L   #__tx_thread_current_ptr, R2         ; Pickup current thread pointer
    MOV.L   [R2], R2
    CMP     #0,R2                                ; Is it NULL?  
    BEQ      __tx_thread_idle_system_save        ; Yes, idle system is running - idle restore
;
;    /* Move stack frame over to the current threads stack.  */
;    /* complete stack frame with registers not saved yet (R3-R5, R14-R15, FPSW)   */
;
    MVFC    USP, R1                               ; pick up user stack pointer
    MOV.L   16[R0], R2
    MOV.L   R2, [-R1]                             ; save PSW on thread stack
    MOV.L   12[R0], R2
    MOV.L   R2, [-R1]                             ; save PC on thread stack
    MOV.L   8[R0], R2
    MOV.L   R2, [-R1]                             ; save R2 on thread stack 
    MOV.L   4[R0], R2
    MOV.L   R2, [-R1]                             ; save R1 on thread stack 
    MOV.L   R5, [-R1]                             ; save R5 on thread stack
    MOV.L   R4, [-R1]                             ; save R4 on thread stack
    MOV.L   R3, [-R1]                             ; save R3 on thread stack
    MOV.L   R15, [-R1]                            ; save R15 on thread stack
    MOV.L   R14, [-R1]                            ; save R14 on thread stack
    MVFC    FPSW, R3
    MOV.L   R3, [-R1]                             ; save FPSW on thread stack
	
    POP     R2                                    ; pick up return address from interrupt stack
    ADD     #16, R0, R0                           ; correct interrupt stack pointer back to the bottom   
    MVTC    R1, USP                               ; set user/thread stack pointer
    JMP     R2                                    ; return to ISR

;    }
;    else
;    {
;
__tx_thread_idle_system_save:
;
;        /* Interrupt occurred in the scheduling loop.  */
;
    POP     R1                                   ; pick up return address
    ADD     #16, R0, R0                          ; correct interrupt stack pointer back to the bottom (PC), don't care about saved registers
    JMP     R1                                   ; return to caller
;
;    }
;}    
    END
