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
    .global     _tx_thread_system_state
    .global     _tx_thread_current_ptr
    .global     _tx_thread_system_stack_ptr
    .global     _tx_thread_execute_ptr
    .global     _tx_timer_time_slice
    .global     _tx_thread_schedule
    .global     _tx_thread_preempt_disable
;
;
    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_context_restore                          C667x/TI        */ 
;/*                                                           6.1          */
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
_tx_thread_context_restore:
;
;    /* Lockout interrupts.  */
;
        MVC         CSR,B0                              ; Pickup CSR
        AND         -2,B0,B0                            ; Build interrupt lockout value
        MVC         B0,CSR                              ; Lockout interrupts
;
;    /* Determine if interrupts are nested.  */
;    if (--_tx_thread_system_state)
;    {
;
        MVKL        _tx_thread_system_state,A0          ; Build address of system state
        MVKH        _tx_thread_system_state,A0          ; 
        LDW         *A0,A1                              ; Pickup system state variable
        MVKL        _tx_thread_current_ptr,A2           ; Build address of current thread ptr
        NOP         3                                   ; Delay slots
        SUB         A1,1,A1                             ; Decrement system state
 [!A1]  B           _tx_thread_not_nested_restore       ; If 0, not a nested restore
        MVKH        _tx_thread_current_ptr,A2           ; 
        LDW         *A2,A3                              ; Pickup current thread pointer
        STW         A1,*A0                              ; Store system state
        NOP         2                                   ; Delay slots
;
;    /* Interrupts are nested.  */
;
;    /* Just recover the saved registers and return to the point of 
;       interrupt.  */
;
        LDW         *+SP(8),B0                          ; Recover saved CSR
        LDW         *+SP(12),B1                         ; Recover saved IRP
        LDW         *+SP(16),B2                         ; Recover saved AMR
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
        MVC         B0,CSR                              ; Setup CSR
        MVC         B1,IRP                              ; Setup IRP
        MVC         B2,AMR                              ; Setup AMR
        LDW         *+SP(268),B0                        ; Recover saved ILC
        LDW         *+SP(272),B1                        ; Recover saved RILC
        LDW         *+SP(276),B2                        ; Recover saved ITSR
        NOP         4
        MVC         B0,ILC                              ; Setup ILC
        MVC         B1,RILC                             ; Setup RILC
        MVC         B2,ITSR                             ; Setup ITSR
        LDW         *+SP(84),B0                         ; Recover B0
        LDW         *+SP(88),B1                         ; Recover B1
        LDW         *+SP(92),B2                         ; Recover B2
        LDW         *+SP(100),B4                        ; Recover B4
        LDW         *+SP(104),B5                        ; Recover B5
        LDW         *+SP(108),B6                        ; Recover B6
        LDW         *+SP(112),B7                        ; Recover B7
        LDW         *+SP(116),B8                        ; Recover B8
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
||      LDW         *+SP(120),B9                        ; Recover B9
        LDW         *+SP(96),B3                         ; Recover B3
        ADDK.S2     288,SP                              ; Recover stack space
        NOP         3                                   ; Delay slots
;
;    }
_tx_thread_not_nested_restore:
;
;    /* Determine if a thread was interrupted and no preemption is required.  */
;    else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr))
;               || (_tx_thread_preempt_disable))
;    {
;
        NOP                                             ; Delay
        MV          A3,A1                               ; Move thread pointer into A1
 [!A1]  B           _tx_thread_schedule                 ; If null, idle system restore
        MVKL        _tx_thread_preempt_disable,A0       ; Build preempt disable flag address
        MVKH        _tx_thread_preempt_disable,A0       ; 
        MVKL        _tx_thread_execute_ptr,A4           ; Build execute thread pointer
        MVKH        _tx_thread_execute_ptr,A4           ; 
        LDW         *A0,B1                              ; Pickup preempt disable flag

        LDW         *A4,A6                              ; Pickup next thread to execute 
        NOP         4                                   ; Delay slot
        CMPEQ       A6,A1,A7                            ; Determine if threads are the same?
        ADD         A7,B1,B1                            ; Add results together
 [B1]   B           _tx_thread_no_preempt_restore       ; If set, skip preeemption
        LDW         *+A1(8),A6                          ; Recover thread's stack pointer
        MVKL        _tx_timer_time_slice,A5             ; Build time slice address
        MVKH        _tx_timer_time_slice,A5             ; 
        LDW         *A5,B1                              ; Pickup current time-slice
        NOP                                             ; Delay slot
;
;
_tx_thread_preempt_restore:
;
;
        MVKL        1,A0                                ; Build the interrupt stack type
        STW         A0,*+A6(4)                          ; Save stack type
;
;   /* Store the remaining registers on the thread's stack.  */
;
        STW         A10,*+A6(60)                        ; Save A10
        STW         A11,*+A6(64)                        ; Save A11
        STW         A12,*+A6(68)                        ; Save A12
        STW         A13,*+A6(72)                        ; Save A13
        STW         A14,*+A6(76)                        ; Save A14
        STW         A15,*+A6(80)                        ; Save A15 (FP)
        STW         B10,*+A6(124)                       ; Save B10
        ADDK        128,A6                              ; Move stack pointer
        STW         B11,*+A6(0)                         ; Save B11
        STW         B12,*+A6(4)                         ; Save B12
        STW         B13,*+A6(8)                         ; Save B13
;
;    /* Save the remaining time-slice and disable it.  */
;    if (_tx_timer_time_slice)
;    {
;
;        _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
;        _tx_timer_time_slice =  0;
;
;    }
_tx_thread_dont_save_ts:
;
;
;    /* Clear the current task pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
;    /* Return to the scheduler.  */
;    _tx_thread_schedule();
;
        B           _tx_thread_schedule                 ; Return to scheduler
        STW         B1,*+A1(24)                         ; Store current time-slice
        ZERO        A3                                  ; Clear value
        STW         A3,*A2                              ; Set current thread pointer to NULL
        STW         A3,*A5                              ; Set time slice to 0
        NOP                                             ; Delay
;
;
_tx_thread_no_preempt_restore:
;
;    /* Restore interrupted thread.  */
;
;    /* Pickup the saved stack pointer.  */
;    SP =  _tx_thread_current_ptr -> tx_thread_stack_ptr;
;
;   /* Recover the saved context and return to the point of interrupt.  */
;
        MV          A6,SP                               ; Setup real stack pointer
        LDW         *+SP(8),B0                          ; Recover saved CSR
        LDW         *+SP(12),B1                         ; Recover saved IRP
        LDW         *+SP(16),B2                         ; Recover saved AMR
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
        MVC         B0,CSR                              ; Setup CSR
        MVC         B1,IRP                              ; Setup IRP
        MVC         B2,AMR                              ; Setup AMR
        LDW         *+SP(268),B0                        ; Recover saved ILC
        LDW         *+SP(272),B1                        ; Recover saved RILC
        LDW         *+SP(276),B2                        ; Recover saved ITSR
        NOP         4                                   ; Delay
        MVC         B0,ILC                              ; Setup ILC
        MVC         B1,RILC                             ; Setup RILC
        MVC         B2,ITSR                             ; Setup ITSR
        LDW         *+SP(84),B0                         ; Recover B0
        LDW         *+SP(88),B1                         ; Recover B1
        LDW         *+SP(92),B2                         ; Recover B2
        LDW         *+SP(100),B4                        ; Recover B4
        LDW         *+SP(104),B5                        ; Recover B5
        LDW         *+SP(108),B6                        ; Recover B6
        LDW         *+SP(112),B7                        ; Recover B7
        LDW         *+SP(116),B8                        ; Recover B8
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
||      LDW         *+SP(120),B9                        ; Recover B9
        LDW         *+SP(96),B3                         ; Recover B3
        ADDK.S2     288,SP                              ; Recover stack space
        NOP         3                                   ; Delay slots
;
;    }
;}

