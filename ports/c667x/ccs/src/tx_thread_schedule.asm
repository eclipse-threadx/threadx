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
FP          .set    A15
DP          .set    B14
SP          .set    B15
;
;
    .global    _tx_thread_execute_ptr
    .global    _tx_thread_current_ptr
    .global    _tx_timer_time_slice
;
;
    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_schedule                                 C667x/TI        */ 
;/*                                                           6.1          */
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
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_schedule(VOID)
;{
    .global _tx_thread_schedule
_tx_thread_schedule
;
;    /* Enable interrupts.  */
;
        MVC         CSR,B0                              ; Pickup current CSR
        OR          1,B0,B0                             ; Build interrupt enable value
        MVC         B0,CSR                              ; Enable interrupts
        MVKL        _tx_thread_execute_ptr,A0           ; Build address of execute pointer
        MVKH        _tx_thread_execute_ptr,A0           ; 
;
;    /* Wait for a thread to execute.  */
;    do
;    {
;
_tx_thread_schedule_loop:
;
        LDW         *A0,A1                              ; Pickup next thread to execute
        NOP         4                                   ; Delay slots
 [!A1]  B           _tx_thread_schedule_loop            ; If Null, just wait here for thread
                                                        ;   to become ready
        MV          A1,A4                               ; Move thread pointer to A4
        MVKL        _tx_thread_current_ptr,A1           ; Build address of current thread ptr
        MVKH        _tx_thread_current_ptr,A1           ; 
        MVKL        _tx_timer_time_slice,A2             ; Build address of time-slice
        MVKH        _tx_timer_time_slice,A2             ; 
;
;    }
;    while(_tx_thread_execute_ptr == TX_NULL);
;    
;    /* Yes! We have a thread to execute.  Lockout interrupts and
;       transfer control to it.  */
;
        AND         -2,B0,B0                            ; Build interrupt lockout value
        MVC         B0,CSR                              ; Lockout interrupts
;
;    /* Setup the current thread pointer.  */
;    _tx_thread_current_ptr =  _tx_thread_execute_ptr;
;
        LDW         *+A4(4),A3                          ; Pickup run-count
        LDW         *+A4(8),SP                          ; Switch to thread's stack
        LDW         *+A4(24),B1                         ; Pickup time-slice
        STW         A4,*A1                              ; Setup current pointer
        NOP                                             ; Delay
;
;    /* Increment the run count for this thread.  */
;    _tx_thread_current_ptr -> tx_thread_run_count++;
;
        ADD         1,A3,A3                             ; Increment run-counter
        LDW         *+SP(4),B0                          ; Pickup stack-type
        STW         A3,*+A4(4)                          ; Store run-counter
;
;    /* Setup time-slice, if present.  */
;    _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;
;
        STW         B1,*A2                              ; Setup time-slice
;
;    /* Switch to the thread's stack.  */
;    SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;
;
;    /* Determine if an interrupt frame or a synchronous task suspension frame
;   is present.  */
;
        NOP         2                                   ; Delay slots
 [B0]   B           _tx_thread_interrupt_stack          ; Look for interrupt stack frame
        LDW         *+SP(8),B0                          ; Pickup saved CSR
        LDW         *+SP(12),B3                         ; Pickup saved B3/IPR
        LDW         *+SP(16),B1                         ; Pickup saved AMR
        LDW         *+SP(20),A10                        ; Restore A10
        LDW         *+SP(24),A11                        ; Restore A11
        LDW         *+SP(28),A12                        ; Restore A12
        LDW         *+SP(32),A13                        ; Restore A13
        MVC         B0,CSR                              ; Restore CSR
        MVC         B1,AMR                              ; Restore AMR
        LDW         *+SP(36),A14                        ; Restore A14
        LDW         *+SP(40),A15                        ; Restore A15
        LDW         *+SP(44),B10                        ; Restore B10
        LDW         *+SP(48),B11                        ; Restore B11
        LDW         *+SP(52),B12                        ; Restore B12
        LDW         *+SP(56),B13                        ; Restore B13
        LDW         *+SP(60),B0                         ; Restore ILC
        LDW         *+SP(64),B1                         ; Restore RILC
        NOP         4                                   ; Delay slots
        MVC         B0,ILC                              ; Restore ILC
        MVC         B1,RILC                             ; Restore RILC
        B           B3                                  ; Return to caller
        ADDK        64,SP                               ; Recover stack space
        NOP         4                                   ; Delay slots
;
_tx_thread_interrupt_stack:
        MVC         B0,CSR                              ; Restore CSR
        MVC         B3,IRP                              ; Restore IPR
        MVC         B1,AMR                              ; Restore AMR
        LDW         *+SP(268),B0                        ; Recover saved ILC
        LDW         *+SP(272),B1                        ; Recover saved RILC
        LDW         *+SP(276),B2                        ; Recover saved ITSR
        NOP         4                                   ; Delay
        MVC         B0,ILC                              ; Setup ILC
        MVC         B1,RILC                             ; Setup RILC
        MVC         B2,ITSR                             ; Setup ITSR
        LDW         *+SP(20),A0                         ; Recover A0
        LDW         *+SP(24),A1                         ; Recover A1
        LDW         *+SP(28),A2                         ; Recover A2
        LDW         *+SP(32),A3                         ; Recover A3
        LDW         *+SP(36),A4                         ; Recover A4
        LDW         *+SP(40),A5                         ; Recover A5
        LDW         *+SP(44),A6                         ; Recover A6
        LDW         *+SP(48),A7                         ; Recover A7
        LDW         *+SP(52),A8                         ; Recover A8
        LDW         *+SP(56),A9                         ; Recover A9
        LDW         *+SP(60),A10                        ; Recover A10
        LDW         *+SP(64),A11                        ; Recover A11
        LDW         *+SP(68),A12                        ; Recover A12
        LDW         *+SP(72),A13                        ; Recover A13
        LDW         *+SP(76),A14                        ; Recover A14
        LDW         *+SP(80),A15                        ; Recover A15 (FP)
        LDW         *+SP(84),B0                         ; Recover B0
        LDW         *+SP(88),B1                         ; Recover B1
        LDW         *+SP(92),B2                         ; Recover B2
        LDW         *+SP(96),B3                         ; Recover B3
        LDW         *+SP(100),B4                        ; Recover B4
        LDW         *+SP(104),B5                        ; Recover B5
        LDW         *+SP(108),B6                        ; Recover B6
        LDW         *+SP(112),B7                        ; Recover B7
        LDW         *+SP(116),B8                        ; Recover B8
        LDW         *+SP(120),B9                        ; Recover B9
        LDW         *+SP(124),B10                       ; Recover B10
        LDW         *+SP(128),B11                       ; Recover B11
        LDW         *+SP(140),A16                       ; Recover A16
        LDW         *+SP(144),A17                       ; Recover A17
        LDW         *+SP(148),A18                       ; Recover A18
        LDW         *+SP(152),A19                       ; Recover A19
        LDW         *+SP(156),A20                       ; Recover A20
        LDW         *+SP(160),A21                       ; Recover A21
        LDW         *+SP(164),A22                       ; Recover A22
        LDW         *+SP(168),A23                       ; Recover A23
        LDW         *+SP(172),A24                       ; Recover A24
        LDW         *+SP(176),A25                       ; Recover A25
        LDW         *+SP(180),A26                       ; Recover A26
        LDW         *+SP(184),A27                       ; Recover A27
        LDW         *+SP(188),A28                       ; Recover A28
        LDW         *+SP(192),A29                       ; Recover A29
        LDW         *+SP(196),A30                       ; Recover A30
        LDW         *+SP(200),A31                       ; Recover A31
        LDW         *+SP(204),B16                       ; Recover B16
        LDW         *+SP(208),B17                       ; Recover B17
        LDW         *+SP(212),B18                       ; Recover B18
        LDW         *+SP(216),B19                       ; Recover B19
        LDW         *+SP(220),B20                       ; Recover B20
        LDW         *+SP(224),B21                       ; Recover B21
        LDW         *+SP(228),B22                       ; Recover B22
        LDW         *+SP(232),B23                       ; Recover B23
        LDW         *+SP(236),B24                       ; Recover B24
        LDW         *+SP(240),B25                       ; Recover B25
        LDW         *+SP(244),B26                       ; Recover B26
        LDW         *+SP(248),B27                       ; Recover B27
        LDW         *+SP(252),B28                       ; Recover B28
        LDW         *+SP(256),B29                       ; Recover B29
        LDW         *+SP(260),B30                       ; Recover B30
        LDW         *+SP(264),B31                       ; Recover B31
        B           IRP                                 ; Return to point of interrupt
||      LDW         *+SP(132),B12                       ; Recover B12
        LDW         *+SP(136),B13                       ; Recover B13
        ADDK.S2     288,SP                              ; Recover stack space
        NOP         3                                   ; Delay slots
;
;}

