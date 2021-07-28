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
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_context_fast_save                      ARC_HS/MetaWare   */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function saves the context of an executing thread in the       */ 
;/*    beginning of fast interrupt processing. The function assumes that   */ 
;/*    fast interrupts are enabled (priority 0) and multiple register      */ 
;/*    banks are available. In this case, register bank 1 is reserved by   */ 
;/*    hardware for fast interrupts. Additional assumptions include that   */ 
;/*    there will be no nested fast interrupts and the LP_START, LP_END,   */ 
;/*    and LP_COUNT registers are not used in the application's fast       */ 
;/*    interrupt ISR.                                                      */ 
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
;VOID   _tx_thread_context_fast_save(VOID)
;{
    .global _tx_thread_context_fast_save
    .type   _tx_thread_context_fast_save, @function 
_tx_thread_context_fast_save:
;
;    /* Increment nested interrupt count.  */
;    _tx_thread_system_state++;
;
    ld      r0, [gp, _tx_thread_system_state@sda]       ; Pickup system state
    add     r0, r0, 1                                   ; Increment the nested interrupt count
    st      r0, [gp, _tx_thread_system_state@sda]       ; Update system state
;
;
    .ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR enter function to indicate an ISR is executing.  */
;
    sub     sp, sp, 32                                  ; Allocating some space on the stack
    st      blink, [sp, 16]                             ; Save blink
    bl.d    _tx_execution_isr_enter                     ; Call the ISR enter function
    nop                                                 ; Delay slot
    ld      blink, [sp, 16]                             ; Recover blink
    add     sp, sp, 32                                  ; Recover the stack space
    .endif
;

    j       [blink]                                     ; Return to the ISR
;
;}
    .end
