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
;
;
    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_context_save                             C667x/TI        */ 
;/*                                                           6.1          */
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
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_save(VOID)
;{
    .global _tx_thread_context_save
_tx_thread_context_save:
;
;    /* Upon entry to this routine, it is assumed that all interrupts are locked
;       out, an initial stack frame of 288 bytes has been allocated and registers
;       A0, A1, A2, A3, A4, and B3 have been saved in the frame. Additionally,
;       it is assumed that register B3 contains the ISR's return address.  */
;
;    /* Check for a nested interrupt condition.  */
;    if (_tx_thread_system_state++)
;    {
;
        MVKL        _tx_thread_system_state,A0          ; Build address of system state
        MVKH        _tx_thread_system_state,A0          ; 
        LDW         *A0,A1                              ; Pickup current system state
        STW         A5,*+SP(40)                         ; Save A5
        STW         A6,*+SP(44)                         ; Save A6
        STW         A7,*+SP(48)                         ; Save A7
        STW         A8,*+SP(52)                         ; Save A8
 [!A1]  B           _tx_thread_not_nested_save          ; If 0, not a nested save condition
        MVKL        _tx_thread_current_ptr,A3           ; Build address of current thread ptr
        MVKH        _tx_thread_current_ptr,A3           ; 
        LDW         *A3,A2                              ; Pickup current thread pointer
        ADD         1,A1,A1                             ; Increment the system state (nested) counter
        STW         A1,*A0                              ; Store system state
;
;    /* Nested interrupt condition. Save remaining scratch registers, and control registers
;       and return to calling ISR.  */
;
        STW         A9,*+SP(56)                         ; Save A9
        STW         B0,*+SP(84)                         ; Save B0
        STW         B1,*+SP(88)                         ; Save B1
        STW         B2,*+SP(92)                         ; Save B2
                                                        ; B3 is already saved!
        STW         B4,*+SP(100)                        ; Save B4
        STW         B5,*+SP(104)                        ; Save B5
        STW         B6,*+SP(108)                        ; Save B6
        STW         B7,*+SP(112)                        ; Save B7
        STW         B8,*+SP(116)                        ; Save B8
        STW         B9,*+SP(120)                        ; Save B9
        MVC         CSR,B0                              ; Pickup CSR
        B           B3                                  ; Return to calling ISR
        MVC         IRP,B1                              ; Pickup IRP
        MVC         AMR,B2                              ; Pickup AMR
        STW         B0,*+SP(8)                          ; Save CSR
        STW         B1,*+SP(12)                         ; Save IRP
        STW         B2,*+SP(16)                         ; Save AMR
        ZERO        B0                                  ; Clear B0
        MVC         B0,AMR                              ; Clear AMR for linear addressing in ISR
        STW         A16,*+SP(140)                       ; Save A16
        STW         A17,*+SP(144)                       ; Save A17
        STW         A18,*+SP(148)                       ; Save A18
        STW         A19,*+SP(152)                       ; Save A19
        STW         A20,*+SP(156)                       ; Save A20
        STW         A21,*+SP(160)                       ; Save A21
        STW         A22,*+SP(164)                       ; Save A22
        STW         A23,*+SP(168)                       ; Save A23
        STW         A24,*+SP(172)                       ; Save A24
        STW         A25,*+SP(176)                       ; Save A25
        STW         A26,*+SP(180)                       ; Save A26
        STW         A27,*+SP(184)                       ; Save A27
        STW         A28,*+SP(188)                       ; Save A28
        STW         A29,*+SP(192)                       ; Save A29
        STW         A30,*+SP(196)                       ; Save A30
        STW         A31,*+SP(200)                       ; Save A31
        STW         B16,*+SP(204)                       ; Save B16
        STW         B17,*+SP(208)                       ; Save B17
        STW         B18,*+SP(212)                       ; Save B18
        STW         B19,*+SP(216)                       ; Save B19
        STW         B20,*+SP(220)                       ; Save B20
        STW         B21,*+SP(224)                       ; Save B21
        STW         B22,*+SP(228)                       ; Save B22
        STW         B23,*+SP(232)                       ; Save B23
        STW         B24,*+SP(236)                       ; Save B24
        STW         B25,*+SP(240)                       ; Save B25
        STW         B26,*+SP(244)                       ; Save B26
        STW         B27,*+SP(248)                       ; Save B27
        STW         B28,*+SP(252)                       ; Save B28
        STW         B29,*+SP(256)                       ; Save B29
        STW         B30,*+SP(260)                       ; Save B30
        STW         B31,*+SP(264)                       ; Save B31
        MVC         ILC,B0                              ; Pickup ILC
        MVC         RILC,B1                             ; Pickup RILC
        MVC         ITSR,B2                             ; Pickup ITSR
        STW         B0,*+SP(268)                        ; Save ILC
        STW         B1,*+SP(272)                        ; Save RILC
        STW         B2,*+SP(276)                        ; Save ITSR
;
_tx_thread_not_nested_save:
;    }
;
;    /* Otherwise, not nested, check to see if a thread was running.  */
;    else if (_tx_thread_current_ptr)
;    {
;
        MVKL        _tx_thread_system_stack_ptr,A0      ; Build address of system
        MVKH        _tx_thread_system_stack_ptr,A0      ;   stack pointer
        MV          A2,A1                               ; Transfer to A1 for B compare
 [!A1]  B           _tx_thread_idle_system_save         ; If Null, idle system save
        STW         A9,*+SP(56)                         ; Save A9
        STW         B0,*+SP(84)                         ; Save B0
        STW         B1,*+SP(88)                         ; Save B1
        STW         B2,*+SP(92)                         ; Save B2
        NOP                                             ; Delay slot
;
;    /* At this point, a thread was interrupted and the remainder of its scratch and
;       control registers must be saved.  */
;
                                                        ; B3 was already saved!
        STW         B4,*+SP(100)                        ; Save B4
        STW         B5,*+SP(104)                        ; Save B5
        STW         B6,*+SP(108)                        ; Save B6
        STW         B7,*+SP(112)                        ; Save B7
        STW         B8,*+SP(116)                        ; Save B8
        STW         B9,*+SP(120)                        ; Save B9
        MVC         CSR,B0                              ; Pickup CSR
        MVC         IRP,B1                              ; Pickup IRP
        MVC         AMR,B2                              ; Pickup AMR
        STW         B0,*+SP(8)                          ; Save CSR
        STW         B1,*+SP(12)                         ; Save IRP
        STW         B2,*+SP(16)                         ; Save AMR
        ZERO        B0                                  ; Clear B0
        MVC         B0,AMR                              ; Clear AMR for linear addressing in ISR
        STW         A16,*+SP(140)                       ; Save A16
        STW         A17,*+SP(144)                       ; Save A17
        STW         A18,*+SP(148)                       ; Save A18
        STW         A19,*+SP(152)                       ; Save A19
        STW         A20,*+SP(156)                       ; Save A20
        STW         A21,*+SP(160)                       ; Save A21
        STW         A22,*+SP(164)                       ; Save A22
        STW         A23,*+SP(168)                       ; Save A23
        STW         A24,*+SP(172)                       ; Save A24
        STW         A25,*+SP(176)                       ; Save A25
        STW         A26,*+SP(180)                       ; Save A26
        STW         A27,*+SP(184)                       ; Save A27
        STW         A28,*+SP(188)                       ; Save A28
        STW         A29,*+SP(192)                       ; Save A29
        STW         A30,*+SP(196)                       ; Save A30
        STW         A31,*+SP(200)                       ; Save A31
        STW         B16,*+SP(204)                       ; Save B16
        STW         B17,*+SP(208)                       ; Save B17
        STW         B18,*+SP(212)                       ; Save B18
        STW         B19,*+SP(216)                       ; Save B19
        STW         B20,*+SP(220)                       ; Save B20
        STW         B21,*+SP(224)                       ; Save B21
        STW         B22,*+SP(228)                       ; Save B22
        STW         B23,*+SP(232)                       ; Save B23
        STW         B24,*+SP(236)                       ; Save B24
        STW         B25,*+SP(240)                       ; Save B25
        STW         B26,*+SP(244)                       ; Save B26
        STW         B27,*+SP(248)                       ; Save B27
        STW         B28,*+SP(252)                       ; Save B28
        STW         B29,*+SP(256)                       ; Save B29
        STW         B30,*+SP(260)                       ; Save B30
        STW         B31,*+SP(264)                       ; Save B31
        MVC         ILC,B0                              ; Pickup ILC
        MVC         RILC,B1                             ; Pickup RILC
        MVC         ITSR,B2                             ; Pickup ITSR
        STW         B0,*+SP(268)                        ; Save ILC
        STW         B1,*+SP(272)                        ; Save RILC
        STW         B2,*+SP(276)                        ; Save ITSR
;
;    /* Save the current stack pointer in the thread's control block.  */
;    _tx_thread_current_ptr -> tx_thread_stack_ptr =  SP;
;
;    /* Switch to the system stack.  */
;    SP =  _tx_thread_system_stack_ptr;
;
        STW         SP,*+A1(8)                          ; Save stack pointer
        B           B3                                  ; Return to calling ISR
        LDW         *A0,SP                              ; Switch to system stack
        NOP         4                                   ; Stack pointer is valid upon return!
;
;    }
;    else
;    {
;
_tx_thread_idle_system_save:
;
;    /* Interrupt occurred in the scheduling loop.  */
;
;    /* Not much to do here, just adjust the stack pointer, and return to ISR 
;       processing.  */
;
        B           B3                                  ; Return to ISR
        ADDK.S2     288,SP                              ; Recover stack space
        NOP         4                                   ; Delay slot
;
;    }
;}

