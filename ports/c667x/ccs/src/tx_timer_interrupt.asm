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
FP          .set    A15
DP          .set    B14
SP          .set    B15
;
;Define Assembly language external references...
;
    .global     _tx_timer_time_slice
    .global     _tx_timer_system_clock
    .global     _tx_timer_current_ptr
    .global     _tx_timer_list_start
    .global     _tx_timer_list_end
    .global     _tx_timer_expired_time_slice
    .global     _tx_timer_expired
    .global     _tx_timer_expiration_process
    .global     _tx_thread_time_slice
    .global     _tx_thread_context_save
    .global     _tx_thread_context_restore
;
;
    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_timer_interrupt                                 C667x/TI        */ 
;/*                                                           6.1          */
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
;/*    _tx_thread_context_save               Context save                  */ 
;/*    _tx_thread_context_restore            Context restore               */ 
;/*    _tx_thread_time_slice                 Time slice interrupted thread */ 
;/*    _tx_timer_expiration_process          Timer expiration processing   */ 
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
_tx_timer_interrupt:
;
;    /* Upon entry to this routine, it is assumed that registers B3, A0-A4 have
;       already been saved and the space for saving additional registers has
;       already been reserved.  In addition, interrupts are locked out and must
;       remain so until context save returns.  */
;
;    /* Increment the system clock.  */
;    _tx_timer_system_clock++;
;
        MVKL        _tx_timer_system_clock,A0           ; Build address of system clock
        MVKH        _tx_timer_system_clock,A0           ; 
        LDW         *A0,A2                              ; Pickup system clock
        MVKL        _tx_timer_time_slice,A3             ; Build address of time slice
        MVKH        _tx_timer_time_slice,A3             ; 
        LDW         *A3,A1                              ; Pickup time slice
        NOP         2                                   ; Delay
        ADD         1,A2,A2                             ; Increment the system clock
        STW         A2,*A0                              ; Store it back in memory
;
;    /* Test for time-slice expiration.  */
;    if (_tx_timer_time_slice)
;    {
;
 [!A1]  B           _tx_timer_no_time_slice             ; If 0, skip time slice processing
        SUB         A1,1,A1                             ; Decrement time-slice value 
        NOP         4                                   ; Delay slots
;
;       /* Decrement the time_slice.  */
;       _tx_timer_time_slice--;
;
;       /* Check for expiration.  */
;       if (_tx_timer_time_slice == 0)
;
 [A1]   B           _tx_timer_no_time_slice             ; If non-zero, not expired yet
        STW         A1,*A3                              ; Store new time-slice    
        MVKL        _tx_timer_expired_time_slice,A0     ; Build address of expired flag
        MVKH        _tx_timer_expired_time_slice,A0     ; 
        MVKL        1,A4                                ; Expired flag
        NOP                                             ; Delay
;
;       /* Set the time-slice expired flag.  */
;       _tx_timer_expired_time_slice =  TX_TRUE;
;
        STW         A4,*A0                              ; Set expired flag
;    }
;
_tx_timer_no_time_slice:
;
;    /* Test for timer expiration.  */
;    if (*_tx_timer_current_ptr)
;    {
;
        MVKL        _tx_timer_current_ptr,A2            ; Build address of current timer pointer
        MVKH        _tx_timer_current_ptr,A2            ; 
        LDW         *A2,A0                              ; Pickup timer list address
        MVKL        _tx_timer_expired,A3                ; Build address of expired flag
        MVKH        _tx_timer_expired,A3                ; 
        NOP         2                                   ; Delay slots
        LDW         *A0,A1                              ; Pickup current timer entry
        ADD         4,A0,A0                             ; Increment the current pointer
        NOP         3                                   ; Delay slots
 [A1]   B           _tx_timer_done                      ; If non-NULL, something has expired
;
;
;        /* Set expiration flag.  */
;        _tx_timer_expired =  TX_TRUE;
;
        MVKL        1,A4                                ; Build expired flag
 [A1]   STW         A4,*A3                              ; Set expired flag
        NOP         3                                   ; Delay slots
;
;    }
;    else
;    {
_tx_timer_no_timer:
;
;        /* No timer expired, increment the timer pointer.  */
;        _tx_timer_current_ptr++;
;
;        /* Check for wrap-around.  */
;        if (_tx_timer_current_ptr == _tx_timer_list_end)
;
        MVKL        _tx_timer_list_end,A3               ; Build timer list end address
        MVKH        _tx_timer_list_end,A3               ; 
        LDW         *A3,A4                              ; Pickup list end address
        MVKL        _tx_timer_list_start,A3             ; Build timer list start address
        MVKH        _tx_timer_list_start,A3             ; 
        NOP         2                                   ; Delay slots
        CMPEQ       A4,A0,A1                            ; Compare current pointer with end
 [A1]   LDW         *A3,A0                              ; If at the end, pickup timer list start
        NOP         4                                   ; Delay slots
;
;            /* Wrap to beginning of list.  */
;            _tx_timer_current_ptr =  _tx_timer_list_start;
;
_tx_timer_skip_wrap:
;
;
        STW         A0,*A2                              ; Store current timer pointer
;    }
;
_tx_timer_done:
;
;
;    /* See if anything has expired.  */
;    if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
;    {
;
        MVKL        _tx_timer_expired_time_slice,A3     ; Build time-slice expired flag
        MVKH        _tx_timer_expired_time_slice,A3     ; 
        LDW         *A3,A4                              ; Pickup time-slice expired flag
        MVKL        _tx_timer_expired,A0                ; Build timer expired flag
        MVKH        _tx_timer_expired,A0                ; 
        LDW         *A0,A2                              ; Pickup timer expired flag
        NOP         4                                   ; Delay slots
        OR          A2,A4,A1                            ; Combine expired flags
 [!A1]  B           _tx_timer_nothing_expired
        NOP         5                                   ; Delay slots
;
_tx_something_expired:
;
;
;    /* Something expired, call context save.  */
;    _tx_thread_context_save();
;
        B           _tx_thread_context_save             ; Call context save routine
        MVKL        _tx_timer_ISR_return,B3             ; Build return address
        MVKH        _tx_timer_ISR_return,B3             ; 
        NOP         3                                   ; Delay slots  
_tx_timer_ISR_return:
;
;    /* Did a timer expire?  */
;    if (_tx_timer_expired)
;    {
;
        MVKL        _tx_timer_expired,A0                ; Build timer expired address
        MVKH        _tx_timer_expired,A0                ; 
        LDW         *A0,A1                              ; Pickup expired flag
        NOP         4                                   ; Delay slots
 [!A1]  B           _tx_timer_dont_activate             ; If not set, skip timer activation
        NOP         5                                   ; Delay slots
;
;        /* Process timer expiration.  */
;        _tx_timer_expiration_process();
;
        B           _tx_timer_expiration_process        ; Process timer expiration
        MVKL        _tx_timer_ISR_return_1,B3           ; Build return address
        MVKH        _tx_timer_ISR_return_1,B3           ; 
        NOP         3                                   ; Delay slots
_tx_timer_ISR_return_1:
;
;    }
_tx_timer_dont_activate:
;
;    /* Did time slice expire?  */
;    if (_tx_timer_expired_time_slice)
;    {
;
        MVKL        _tx_timer_expired_time_slice,A0     ; Build address of expired flag
        MVKH        _tx_timer_expired_time_slice,A0     ; 
        LDW         *A0,A1                              ; Pickup expired flag
        NOP         4                                   ; Delay slots
 [!A1]  B           _tx_timer_not_ts_expiration         ; If not set, skip time-slice processing
        NOP         5                                   ; Delay slots
;
;        /* Time slice interrupted thread.  */
;        _tx_thread_time_slice();
;
        B           _tx_thread_time_slice               ; Call time-slice processing
        MVKL        _tx_timer_ISR_return_2,B3           ; Build return address
        MVKH        _tx_timer_ISR_return_2,B3           ; 
        NOP         3                                   ; Delay slots
_tx_timer_ISR_return_2:
;
;    }
;
_tx_timer_not_ts_expiration:
;
;
;    /* Call context restore.  */
;    _tx_thread_context_restore();
;
        B           _tx_thread_context_restore          ; Jump to context restore - no return!
        NOP         5                                   ; Delay slots
;
;    }
;
_tx_timer_nothing_expired:
;
        LDW         *+SP(20),A0                         ; Recover A0
        LDW         *+SP(24),A1                         ; Recover A1
        LDW         *+SP(28),A2                         ; Recover A2
        LDW         *+SP(32),A3                         ; Recover A3
        B           IRP                                 ; Return to point of interrupt
||      LDW         *+SP(36),A4                         ; Recover A4
        LDW         *+SP(96),B3                         ; Recover B3
        ADDK.S2     288,SP                              ; Recover stack space
        NOP         3                                   ; Delay slots
;
;}

