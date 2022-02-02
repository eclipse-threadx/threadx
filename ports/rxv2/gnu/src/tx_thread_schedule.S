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
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_thread.h"
;#include "tx_timer.h"
;
;
    .global     __tx_thread_execute_ptr
    .global     __tx_thread_current_ptr
    .global     __tx_timer_time_slice
;
    .text

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_schedule                                  RXv2/GNURX     */
;/*                                                           6.1.10       */
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
;/*  12-30-2020     William E. Lamie         Initial Version 6.1.3         */
;/*  10-15-2021     William E. Lamie         Modified comment(s), and      */
;/*                                            removed unnecessary stack   */
;/*                                            type checking,              */
;/*                                            resulting in version 6.1.9  */
;/*  01-31-2022     William E. Lamie         Modified comment(s),          */
;/*                                            resulting in version 6.1.10 */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_schedule(VOID)
;{
    .global __tx_thread_schedule
__tx_thread_schedule:
;
;    /* Enable interrupts.  */
;
    SETPSW I
;
;    /* Wait for a thread to execute.  */
;    do
;    {
    MOV.L    #__tx_thread_execute_ptr, R1        ; Address of thread to executer ptr
__tx_thread_schedule_loop:
    MOV.L    [R1],R2                             ; Pickup next thread to execute
    CMP      #0,R2                               ; Is it NULL?
    BEQ      __tx_thread_schedule_loop           ; Yes, idle system, keep checking
;
;    }
;    while(_tx_thread_execute_ptr == TX_NULL);
;
;    /* Yes! We have a thread to execute.  Lockout interrupts and
;       transfer control to it.  */
;
    CLRPSW I                                     ; Disable interrupts
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

    POPM    R1-R3                                ; Restore accumulators.
    MVTACLO R3, A0
    MVTACHI R2, A0
    MVTACGU R1, A0
    POPM    R1-R3
    MVTACLO R3, A1
    MVTACHI R2, A1
    MVTACGU R1, A1

    POPM   R6-R13                                ; Recover interrupt stack frame
    POPC   FPSW
    POPM   R14-R15
    POPM   R3-R5
    POPM   R1-R2
    RTE                                          ; Return to point of interrupt, this restores PC and PSW

;}


.global __tx_thread_context_save
.global __tx_thread_context_restore


; Software triggered interrupt used to perform context switches.
; The priority of this interrupt is set to the lowest priority within
; tx_initialize_low_level() and triggered by ThreadX when calling
; _tx_thread_system_return().
.global $tableentry$27$.rvectors
$tableentry$27$.rvectors:

    PUSHM R1-R2

    BSR __tx_thread_context_save

    BRA __tx_thread_context_restore

    .end
