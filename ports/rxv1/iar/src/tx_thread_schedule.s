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

    extern __tx_thread_execute_ptr
    extern __tx_thread_current_ptr
    extern __tx_timer_time_slice
#if (TX_LOW_POWER == 1)
    extern _tx_low_power_enter
    extern _tx_low_power_exit
    extern __tx_thread_preempt_disable
#endif

    section .text:CODE:ROOT

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_schedule                                  RXv1/IAR       */
;/*                                                           6.1.11       */
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
;/*  08-02-2021     William E. Lamie         Initial Version 6.1.8         */
;/*  10-15-2021     William E. Lamie         Modified comment(s), and      */
;/*                                            removed unnecessary stack   */
;/*                                            type checking,              */
;/*                                            resulting in version 6.1.9  */
;/*  01-31-2022     William E. Lamie         Modified comment(s),          */
;/*                                            resulting in version 6.1.10 */
;/*  04-25-2022     William E. Lamie         Modified comment(s), and      */ 
;/*                                            added low power support,    */ 
;/*                                            resulting in version 6.1.11 */ 
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_schedule(VOID)
;{
    public __tx_thread_schedule

__tx_thread_schedule:
;
;
;    /* Wait for a thread to execute.  */
;    do
;    {
    MOV.L    #__tx_thread_execute_ptr, R1        ; Address of thread to executer ptr
__tx_thread_schedule_loop:
    SETPSW I                                     ; Enable interrupts
    CLRPSW I                                     ; Disable interrupts
    MOV.L    [R1],R2                             ; Pickup next thread to execute
    CMP      #0,R2                               ; Is it NULL?
    BNE      __tx_thread_thread_ready            ; Not NULL, schedule the thread
                                                 ; Idle system - no thread is ready    
#if (TX_LOW_POWER == 1)
    MOV.L    #__tx_thread_preempt_disable, R1    ; Load prempt disable flag.
    MOV.L    [R1], R2
    ADD	     #1, R2                              ; Disable preemption while enter/exit
    MOV.L    R2, [R1]
    BSR      _tx_low_power_enter                 ; Possibly enter low power mode
#endif

#if (TX_ENABLE_WAIT == 1)
    WAIT                                         ; Wait for interrupt
#endif

#if (TX_LOW_POWER == 1)
    CLRPSW I                                     ; Disable interrupts (because WAIT enables interrupts)
    BSR      _tx_low_power_exit                  ; Possibly exit low power mode
    MOV.L    #__tx_thread_preempt_disable, R1    ; Load prempt disable flag.
    MOV.L    [R1], R2
    SUB	     #1, R2				 ; Enable preemption
    MOV.L    R2, [R1]
    MOV.L    #__tx_thread_execute_ptr, R1        ; Address of thread to executer ptr
#endif

    BRA      __tx_thread_schedule_loop           ; Idle system, keep checking

__tx_thread_thread_ready:
;
;    }
;    while(_tx_thread_execute_ptr == TX_NULL);
;    
;    /* Yes! We have a thread to execute. Note that interrupts are locked out at this point.  */
;
;    /* Setup the current thread pointer.  */
;    _tx_thread_current_ptr =  _tx_thread_execute_ptr;
;
    MOV.L    #__tx_thread_current_ptr, R3
    MOV.L    R2,[R3]                             ; Setup current thread pointer
;
;    /* Increment the run count for this thread.  */
;    _tx_thread_current_ptr -> tx_thread_run_count++;
;
    MOV.L    4[R2],R3                            ; Pickup run count
    ADD      #1,R3                               ; Increment run counter
    MOV.L    R3,4[R2]                            ; Store it back in control block
;
;    /* Setup time-slice, if present.  */
;    _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;
;
    MOV.L    24[R2],R3                           ; Pickup thread time-slice
    MOV.L    #__tx_timer_time_slice,R4           ; Pickup pointer to time-slice
    MOV.L    R3, [R4]                            ; Setup time-slice
;
;    /* Switch to the thread's stack.  */
;    SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;
    SETPSW U                                     ; User stack mode
    MOV.L   8[R2],R0                             ; Pickup stack pointer
;
;    /* Determine if an interrupt frame or a synchronous task suspension frame
;   is present.  */
;
    POPM    R1-R2                                ; Restore accumulator.
    MVTACLO R2
    MVTACHI R1

    POPM   R6-R13                                ; Recover interrupt stack frame
    POPM   R14-R15
    POPM   R3-R5
    POPM   R1-R2    
    RTE                                          ; Return to point of interrupt, this restores PC and PSW
 
;
;}

    extern __tx_thread_context_save
    extern __tx_thread_context_restore

; Software triggered interrupt used to perform context switches.
; The priority of this interrupt is set to the lowest priority within
; tx_initialize_low_level() and triggered by ThreadX when calling
; _tx_thread_system_return().
    public ___interrupt_27
___interrupt_27:

    PUSHM R1-R2

    BSR __tx_thread_context_save

    BRA __tx_thread_context_restore

    END
