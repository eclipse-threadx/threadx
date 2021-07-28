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
;/*    _tx_timer_interrupt                               ARC_HS/MetaWare   */
;/*                                                           6.1.3        */
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
;/*  12-31-2020     Scott Larson             Modified comment(s), remove   */
;/*                                            unneeded load of            */
;/*                                            _tx_thread_preempt_disable, */
;/*                                            resulting in version 6.1.3  */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_timer_interrupt(VOID)
;{
    .global _tx_timer_interrupt
    .type   _tx_timer_interrupt, @function 
_tx_timer_interrupt:
;
;    /* Upon entry to this routine, it is assumed the interrupt stack frame has
;       already been allocated and registers r0, r1, and r2 have already been saved
;       at offsets 0, 4, and 8 respectively.  */
;
;    /* Increment the system clock.  */
;    _tx_timer_system_clock++;
;
    clri                                                ; Lockout interrupts
    ld      r0, [gp,_tx_timer_system_clock@sda]         ; Pickup current system clock
    ld      r2, [gp, _tx_timer_time_slice@sda]          ; Pickup current time-slice
    add     r0, r0, 1                                   ; Increment the system clock
    st      r0, [gp,_tx_timer_system_clock@sda]         ; Store system clock back in memory

;    /* Test for time-slice expiration.  */
;    if (_tx_timer_time_slice)
;    {
;
    mov     r1, 0                                       ; Clear expiration flag
    breq    r2, 0, __tx_timer_no_time_slice             ; If zero, no time-slice is active
;
;       /* Decrement the time_slice.  */
;       _tx_timer_time_slice--;
;
    sub     r2, r2, 1                                   ; Decrement time-slice
    st      r2, [gp, _tx_timer_time_slice@sda]          ; Store new time-slice value
;
;       /* Check for expiration.  */
;       if (__tx_timer_time_slice == 0)
;
    brne    r2, 0, __tx_timer_no_time_slice             ; If non-zero, skip over expiration
;
;       /* Set the time-slice expired flag.  */
;       _tx_timer_expired_time_slice =  TX_TRUE;
;
    mov     r1, 1                                      ; Set register flag
    st      r1, [gp, _tx_timer_expired_time_slice@sda] ; Set the time-slice expired flag

;
;    }
;
__tx_timer_no_time_slice:
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
;    if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
;    {
;
    breq    r1, 0, __tx_timer_nothing_expired           ; If 0, nothing has expired
;
__tx_something_expired:
;
    ld      r0, [sp, 0]                                 ; Recover r0
    ld      r1, [sp, 4]                                 ; Recover r1
    ld      r2, [sp, 8]                                 ; Recover r2
    st      blink, [sp, 16]                             ; Save blink
    bl      _tx_thread_context_save                     ; Save interrupted context
;
;    /* Did a timer expire?  */
;    if (_tx_timer_expired)
;    {
;
    ld      r2, [gp, _tx_timer_expired@sda]             ; Pickup timer expired flag
    breq    r2, 0, __tx_timer_dont_activate             ; If not set, skip expiration processing
;
;        /* Process the timer expiration.  */
;        /* _tx_timer_expiration_process();  */
    bl.d    _tx_timer_expiration_process                ; Call the timer expiration handling routine
    sub     sp, sp, 16                                  ; ..allocating some space on the stack
    add     sp, sp, 16                                  ; Recover the stack space
;
;    }
__tx_timer_dont_activate:
;
;    /* Did time slice expire?  */
;    if (_tx_timer_expired_time_slice)
;    {
;
    ld      r2, [gp, _tx_timer_expired_time_slice@sda]  ; Pickup expired time-slice flag
    breq    r2, 0, __tx_timer_not_ts_expiration         ; If not set, skip time-slice
;
;        /* Time slice interrupted thread.  */
;        /* _tx_thread_time_slice();  */

    bl.d    _tx_thread_time_slice                       ; Call time-slice processing
    sub     sp, sp, 16                                  ; ..allocating some stack space
    add     sp, sp, 16                                  ; Recover stack space
;
;    }
;
__tx_timer_not_ts_expiration:
;
    st      0, [gp, _tx_timer_expired_time_slice@sda]  
    b       _tx_thread_context_restore                  ; Go restore interrupt context..
                                                        ; ..clearing time-slice expired flag
                                                        ; Note that we don't return from
                                                        ;   this function.
;
;    }
;
__tx_timer_nothing_expired:
;
    ld      r0, [sp, 0]                                 ; Recover r0
    ld      r1, [sp, 4]                                 ; Recover r1
    ld      r2, [sp, 8]                                 ; Recover r2
    add     sp, sp, 160                                 ; Recover interrupt stack frame 
    rtie                                                ; Return to point of interrupt
;
;}
    .end
    
