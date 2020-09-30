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
;/*    _tx_thread_smp_current_state_get                SMP/ARC_HS/MetaWare */ 
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function is gets the current state of the calling core.        */ 
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
;/*    ThreadX Components                                                  */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
    .global _tx_thread_smp_current_state_get
    .type   _tx_thread_smp_current_state_get, @function
_tx_thread_smp_current_state_get:

    clri    r2                                          ; Disable interrupts
    lr      r0, [IDENTITY]                              ; Pickup core ID
    xbfu    r0, r0, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r0, r0, 1                                   ; Subtract 1 to make 0-based
    .endif
    asl     r0, r0, 2                                   ; Build index into _tx_thread_system_state
    mov     r1, _tx_thread_system_state                 ; Build address of _tx_thread_system_state
    add     r1, r1, r0                                  ; 
    ld      r0, [r1]                                    ; Pickup current system state for this core
    j_s.d   [blink]                                     ; Return to caller with delay slot
    seti    r2                                          ; Restore previous interrupt state
    
    .end

