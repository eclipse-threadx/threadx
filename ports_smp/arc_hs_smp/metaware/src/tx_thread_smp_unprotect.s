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
;/*    _tx_thread_smp_unprotect                        SMP/ARC_HS/MetaWare */ 
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function releases previously obtained protection. The supplied */ 
;/*    previous SR is restored. If the value of _tx_thread_system_state    */ 
;/*    and _tx_thread_preempt_disable are both zero, then multithreading   */ 
;/*    is enabled as well.                                                 */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    Previous Status Register                                            */ 
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
;/*    ThreadX Source                                                      */
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
    .global _tx_thread_smp_unprotect
    .type   _tx_thread_smp_unprotect, @function
_tx_thread_smp_unprotect:
    clri                                                ; Disable interrupts
    lr      r1, [IDENTITY]                              ; Pickup core ID
    xbfu    r1, r1, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r1, r1, 1                                   ; Subtract 1 to make 0-based
    .endif
    mov     r4, _tx_thread_smp_protection               ; Build address of the protection structure
    ld      r5, [r4, 8]                                 ; Pickup the owning core
    brne    r1, r5, _still_protected                    ; If not the owning core, protection is in force elsewhere
    ld      r6, [r4, 12]                                ; Pickup ownership count
    breq    r6, 0, _still_protected                     ; If zero, protection is still active
    sub     r6, r6, 1                                   ; Decrement the ownership count
    st      r6, [r4, 12]                                ; Store the ownership count      
    brne    r6, 0, _still_protected                     ; If non-zero, protection is still active
    ld      r6, [gp, _tx_thread_preempt_disable@sda]    ; Pickup preempt disable flag
    brne    r6, 0, _still_protected                     ; If non-zero, don't release the protection
    mov     r2, 0xFFFFFFFF                              ; Build invalid value
    st      r2, [r4, 8]                                 ; Set the owning core to an invalid value
    .ifdef TX_SMP_DEBUG_ENABLE
    st      blink, [r4, 24]                             ; Save caller of unprotect
    .endif    
    mov     r2, 0                                       ; Build clear value
    dmb     3                                           ; Data memory barrier
    st      r2, [r4]                                    ; Release the protection
    dmb     3                                           ; Data memory barrier
_still_protected:
    j_s.d   [blink]                                     ; Return to caller with delay slot
    seti    r0                                          ; Set desired interrupt state       

    .end
    
