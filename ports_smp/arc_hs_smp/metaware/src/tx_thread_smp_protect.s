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
;/**   Thread - Low Level SMP Support                                      */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/
;
;
;#define TX_SOURCE_CODE
;#define TX_THREAD_SMP_SOURCE_CODE
;
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_thread.h"
;#include "tx_timer.h"  */
;
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_smp_protect                          SMP/ARC_HS/MetaWare */ 
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function gets protection for running inside the ThreadX        */ 
;/*    source. This is acomplished by a combination of a test-and-set      */ 
;/*    flag and periodically disabling interrupts.                         */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */
;/*                                                                        */ 
;/*  OUTPUT                                                                */ 
;/*                                                                        */ 
;/*    Previous Status Register                                            */ 
;/*                                                                        */ 
;/*  CALLS                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    ThreadX Source                                                      */
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
    .global _tx_thread_smp_protect
    .type   _tx_thread_smp_protect, @function
_tx_thread_smp_protect:

    clri    r0                                          ; Disable interrupts
    lr      r1, [IDENTITY]                              ; Pickup core ID
    xbfu    r1, r1, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r1, r1, 1                                   ; Subtract 1 to make 0-based
    .endif
    mov     r2, 1                                       ; Build protection flag
    mov     r3, 0                                       ; Build clear value
    mov     r4, _tx_thread_smp_protection               ; Build address of the protection structure
    ld      r5, [r4, 8]                                 ; Pickup the owning core
    breq    r1, r5, _owned                              ; Check if the core already owns the protection
    llock   r6, [r4]                                    ; Attempt to get the inter-core lock
    breq    r3, r6, _continue                           ; If the lock is available, continue
    b.d     _tx_thread_smp_protect                      ; Attempt to get the lock again
    seti    r0                                          ; Restore original interrupt posture
_continue:
    scond   r2, [r4]                                    ; Attempt to set the lock
    beq_s   _got_lock                                   ; If STATUS32[Z] flag is set, we got the lock!
    b.d     _tx_thread_smp_protect                      ; Attempt to get the lock again
    seti    r0                                          ; Restore original interrupt posture
_got_lock:
    dmb     3                                           ; Data memory barrier
    st      r1, [r4, 8]                                 ; Store the owning core
    .ifdef TX_SMP_DEBUG_ENABLE
    asl     r0, r1, 2                                   ; Build index into _tx_thread_system_state
    mov     r7, _tx_thread_current_ptr                  ; Pickup base of the current thread pointer
    add     r7, r7, r0                                  ; Build address of current thread pointer
    ld      r8, [r7]                                    ; Pickup the current thread pointer
    st      r8, [r4, 4]                                 ; Save the current thread pointer
    st      blink, [r4, 16]                             ; Save last caller's return address
    st      r0, [r4, 20]                                ; Save STATUS32
    .endif
_owned:
    ld      r6, [r4, 12]                                ; Pickup ownership count
    add     r6, r6, 1                                   ; Increment ownership count
    j_s.d   [blink]                                     ; Return to caller with delay slot
    st      r6, [r4, 12]                                ; Store new ownership count

    .end


