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
;/*    _tx_thread_smp_initialize_wait                  SMP/ARC_HS/MetaWare */ 
;/*                                                            6.1         */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function is the place where additional cores wait until        */ 
;/*    initialization is complete before they enter the thread scheduling  */ 
;/*    loop.                                                               */ 
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
;/*    Hardware                                                            */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
    .global _tx_thread_smp_initialize_wait
    .type   _tx_thread_smp_initialize_wait, @function
_tx_thread_smp_initialize_wait:
;
;    /* Lockout interrupts.  */
;
    clri    r2                                          ; Disable interrupts
;
;    /* Pickup the CPU ID.   */
;
    lr      r0, [IDENTITY]                              ; Pickup core ID
    xbfu    r0, r0, 8, 8                                ; Shift down and isolate core ID
    .ifndef TX_ZERO_BASED_CORE_ID
    sub     r0, r0, 1                                   ; Subtract 1 to make 0-based
    .endif
    asl     r0, r0, 2                                   ; Build index into _tx_thread_system_state
    mov     r1, _tx_thread_system_stack_ptr             ; Build system stack pointer address
    add     r1, r1, r0                                  ; 
    st      sp, [r1]                                    ; Save this core's system address
    
    mov     r1, _tx_thread_system_state                 ; Build system state pointer address
    add     r1, r1, r0                                  ;     
    mov     r3, 0xF0F0F0F0                              ; Build TX_INITIALIZE_IN_PROGRESS flag 
wait_for_initialize:    
    ld      r0, [r1]                                    ; Pickup current system state for this core
                                                        ; Has the TX_INITIALIZE_IN_PROGRESS flag been set yet? 
    brne    r3, r0, wait_for_initialize                 ; No, simply wait here until this value is set
;
;    /* Pickup the release cores flag.  */
;
    mov     r3, 0                                       ; Build clear value
wait_for_release:
    ld      r0, [gp, _tx_thread_smp_release_cores_flag@sda] ; Pickup the release cores flag
    breq    r3, r0, wait_for_release                    ; No, simply wait here until this flag is set
;
;    /* Core 0 has released this core.  */
;    
;    /* Clear this core's system state variable.  */
;
    st      r3, [r1]                                    ; Clear this core's state value
;
;    /* Now wait for core 0 to finish it's initialization.  */
;
core_0_wait_loop:
    ld      r0, [gp, _tx_thread_system_state@sda]       ; Pickup core 0's state value
    brne    r3, r0, core_0_wait_loop                    ; If set, continue to wait here
;
;    /* Initialize is complete, enter the scheduling loop!  */
;
    b       _tx_thread_schedule                         ; Return to scheduler..

    .end


