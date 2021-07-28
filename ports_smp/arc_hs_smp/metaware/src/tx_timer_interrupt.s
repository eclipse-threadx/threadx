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
;/**   Timer                                                               */
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
;#include "tx_timer.h"
;#include "tx_thread.h"
;
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_timer_interrupt                             SMP/ARC_HS/MetaWare */
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function processes the hardware timer interrupt.  This         */ 
;/*    processing includes incrementing the system clock and checking for  */ 
;/*    time slice and/or timer expiration.  If either is found, the        */ 
;/*    interrupt context save/restore functions are called along with the  */ 
;/*    expiration functions.                                               */ 
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
;/*    _tx_timer_expiration_process          Process timer expiration      */ 
;/*    _tx_thread_time_slice                 Time slice interrupted thread */ 
;/*    _tx_thread_context_save               Save interrupt context        */ 
;/*    _tx_thread_context_restore            Restore interrupt context     */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    interrupt vector                                                    */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_timer_interrupt(VOID)
;{
    .global _tx_timer_interrupt
    .type   _tx_timer_interrupt, @function 
_tx_timer_interrupt:
;
;    /* Upon entry to this routine, it is assumed that context save has already
;       been called, and therefore the compiler scratch registers are available
;       for use.  */
;
    lr      r1, [IDENTITY]                              ; Pickup core ID
    xbfu    r1, r1, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r1, r1, 1                                   ; Subtract 1 to make 0-based
    .endif
    breq    r1, 0, __tx_process_timer                   ; By default if core 0, process timer   
    j_s.d   [blink]                                     ; Return to caller with delay
    nop                                                 ; 
__tx_process_timer:

    sub     sp, sp, 16                                  ; Allocate some stack space
    st      blink, [sp]                                 ; Save return address
    bl.d    _tx_thread_smp_protect                      ; Get SMP protecton
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    st      r0, [sp, 4]                                 ; Save returned interrupt posture on stack   
    ld      r0, [gp,_tx_timer_interrupt_active@sda]     ; Pickup current timer active count
    add     r0, r0, 1                                   ; Increment the active count
    st      r0, [gp,_tx_timer_interrupt_active@sda]     ; Store the new timer active count
    dmb     3                                           ; Data memory barrier
;
;    /* Increment the system clock.  */
;    _tx_timer_system_clock++;
;
    ld      r0, [gp,_tx_timer_system_clock@sda]         ; Pickup current system clock
    add     r0, r0, 1                                   ; Increment the system clock
    st      r0, [gp,_tx_timer_system_clock@sda]         ; Store system clock back in memory
;
;    /* Test for timer expiration.  */
;    if (*_tx_timer_current_ptr)
;    {
;
    ld      r0, [gp, _tx_timer_current_ptr@sda]         ; Pickup current timer pointer
    ld      r2, [r0, 0]                                 ; Pickup examine actual list entry
    breq    r2, 0, __tx_timer_no_timer                         ; 
                                                        ; If NULL, no timer has expired, just move to the next entry
;
;        /* Set expiration flag.  */
;        _tx_timer_expired =  TX_TRUE;
;
    mov     r1, 1                                       ; Build expiration value
    b.d     __tx_timer_done                             ; Skip moving the timer pointer
    st      r1, [gp, _tx_timer_expired@sda]             ; Set the expired value
;
;    }
;    else
;    {
__tx_timer_no_timer:
;
;        /* No timer expired, increment the timer pointer.  */
;        _tx_timer_current_ptr++;
;
    ld      r2, [gp, _tx_timer_list_end@sda]            ; Pickup end of list
    add     r0, r0, 4                                   ; Move to next timer entry
;
;        /* Check for wrap-around.  */
;        if (_tx_timer_current_ptr == _tx_timer_list_end)
;
    st      r0, [gp, _tx_timer_current_ptr@sda]         ; Store the current timer
    brne    r0, r2, __tx_timer_skip_wrap                ; If not equal, don't wrap the list
;
;            /* Wrap to beginning of list.  */
;            _tx_timer_current_ptr =  _tx_timer_list_start;
;
    ld      r2, [gp, _tx_timer_list_start@sda]          ; Pickup start of timer list
    st      r2, [gp, _tx_timer_current_ptr@sda]         ; Set current timer to the start
;
__tx_timer_skip_wrap:
;
;    }
;
__tx_timer_done:
;
;
;    /* See if anything has expired.  */
;    if (_tx_timer_expired)
;    {
;
    breq    r1, 0, __tx_timer_nothing_expired           ; If 0, nothing has expired
;
__tx_something_expired:
;
;
;        /* Process the timer expiration.  */
;        /* _tx_timer_expiration_process();  */
    bl.d    _tx_timer_expiration_process                ; Call the timer expiration handling routine
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
;
;    }

__tx_timer_nothing_expired:
;
;        /* Call time-slice processing.  */
;        /* _tx_thread_time_slice();  */

    bl.d    _tx_thread_time_slice                       ; Call time-slice processing
    sub     sp, sp, 16                                  ; ..allocating some stack space
    add     sp, sp, 16                                  ; Recover stack space
;
;    }
;
    ld      r0, [gp,_tx_timer_interrupt_active@sda]     ; Pickup current timer active count
    sub     r0, r0, 1                                   ; Decrement the active count
    st      r0, [gp,_tx_timer_interrupt_active@sda]     ; Store the new timer active count
    dmb     3                                           ; Data memory barrier

    ld      r0, [sp, 4]                                 ; Recover previous interrupt posture
    bl.d    _tx_thread_smp_unprotect                    ; Get SMP protecton
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
    ld      blink, [sp]                                 ; Recover original blink
;
;
    j_s.d   [blink]                                     ; Return to caller with delay slot
    add     sp, sp, 16                                  ; Recover temporary stack space
;
;}
    .end
    
