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
    .global     _tx_thread_current_ptr
    .global     _tx_timer_time_slice
    .global     _tx_thread_schedule
    .global     _tx_thread_system_stack_ptr
;
;
    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_system_return                            C667x/TI        */ 
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function is target processor specific.  It is used to transfer */ 
;/*    control from a thread back to the ThreadX system.  Only a           */ 
;/*    minimal context is saved since the compiler assumes temp registers  */ 
;/*    are going to get slicked by a function call anyway.                 */ 
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
;/*    _tx_thread_schedule                   Thread scheduling loop        */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    ThreadX components                                                  */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_system_return(VOID)
;{
    .global _tx_thread_system_return
_tx_thread_system_return:
;
;    /* Save minimal context on the stack.  */
;
        MVC         CSR,B0                              ; Pickup current CSR
        MVC         AMR,B1                              ; Pickup current AMR
        ADDK        -64,SP                              ; Allocate stack space
        ZERO        B2                                  ; Build solicited stack type
        STW         B2,*+SP(4)                          ; Save stack type
        STW         B0,*+SP(8)                          ; Save CSR
        STW         B3,*+SP(12)                         ; Save B3 (return address)
        STW         B1,*+SP(16)                         ; Save AMR
        STW         A10,*+SP(20)                        ; Save A10
        STW         A11,*+SP(24)                        ; Save A11
        STW         A12,*+SP(28)                        ; Save A12
        STW         A13,*+SP(32)                        ; Save A13
        STW         A14,*+SP(36)                        ; Save A14
        STW         A15,*+SP(40)                        ; Save A15
        STW         B10,*+SP(44)                        ; Save B10
        STW         B11,*+SP(48)                        ; Save B11
        STW         B12,*+SP(52)                        ; Save B12
        STW         B13,*+SP(56)                        ; Save B13
        MVC         ILC,B0                              ; Pickup ILC
        MVC         RILC,B1                             ; Pickup RILC
        STW         B0,*+SP(60)                         ; Save ILC
        STW         B1,*+SP(64)                         ; Save RILC
;   
;   /* Lockout interrupts.  */
;
        AND         -2,B0,B0                            ; Build interrupt disable value
        MVC         B0,CSR                              ; Lockout interrupts
;
;    /* Save current stack and switch to system stack.  */
;    _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
;    SP = _tx_thread_system_stack_ptr;
;
        MVKL        _tx_timer_time_slice,A2             ; Pickup address of time slice
        MVKH        _tx_timer_time_slice,A2             ; 
        LDW         *A2,B0                              ; Pickup time slice
        MVKL        _tx_thread_current_ptr,A1           ; Pickup address of current thread
        MVKH        _tx_thread_current_ptr,A1           ; 
        LDW         *A1,A4                              ; Pickup current thread pointer
        MVKL        _tx_thread_system_stack_ptr,A3      ; Pickup address of system stack
        MVKH        _tx_thread_system_stack_ptr,A3      ; 
;
;    /* Determine if the time-slice is active.  */
;    if (_tx_timer_time_slice)
;    {
;
 [!B0]  B           _tx_thread_dont_save_ts             ; If no-time slice, skip save
        NOP                                             ; Delay slot
        STW         SP,*+A4(8)                          ; Save thread's stack pointer 
        LDW         *A3,SP                              ; Switch to system stack pointer
        NOP                                             ; Delay slot
;
;       /* Save time-slice for the thread and clear the current time-slice.  */
;       _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
;       _tx_timer_time_slice =  0;
        NOP                                             ; 
        STW         B2,*A2                              ; Clear time-slice
        NOP         2                                   ; Delay slots
        STW         B0,*+A4(24)                         ; Save time-slice
;
;
;    }
_tx_thread_dont_save_ts
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
        B           _tx_thread_schedule                 ; Return to scheduling loop
        STW         B2,*A1                              ; Set current thread to NULL
        NOP         4                                   ; Delay slots
;
;}

