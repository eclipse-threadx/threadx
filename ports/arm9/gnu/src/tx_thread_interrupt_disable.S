@/**************************************************************************/
@/*                                                                        */
@/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
@/*                                                                        */
@/*       This software is licensed under the Microsoft Software License   */
@/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
@/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
@/*       and in the root directory of this software.                      */
@/*                                                                        */
@/**************************************************************************/
@
@
@/**************************************************************************/
@/**************************************************************************/
@/**                                                                       */ 
@/** ThreadX Component                                                     */ 
@/**                                                                       */
@/**   Thread                                                              */
@/**                                                                       */
@/**************************************************************************/
@/**************************************************************************/
@
@#define TX_SOURCE_CODE
@
@
@/* Include necessary system files.  */
@
@#include "tx_api.h"
@#include "tx_thread.h"
@
@
#ifdef TX_ENABLE_FIQ_SUPPORT
DISABLE_INTS    =           0xC0                @ IRQ & FIQ interrupts disabled
#else
DISABLE_INTS    =           0x80                @ IRQ interrupts disabled
#endif
@
@
@/* Define the 16-bit Thumb mode veneer for _tx_thread_interrupt_disable for
@   applications calling this function from to 16-bit Thumb mode.  */
@
    .text
    .align 2
    .global $_tx_thread_interrupt_disable
$_tx_thread_interrupt_disable:
        .thumb
     BX        pc                               @ Switch to 32-bit mode
     NOP                                        @ 
    .arm
     STMFD     sp!, {lr}                        @ Save return address
     BL        _tx_thread_interrupt_disable     @ Call _tx_thread_interrupt_disable function
     LDMFD     sp!, {lr}                        @ Recover saved return address
     BX        lr                               @ Return to 16-bit caller
@
@
    .text
    .align 2
@/**************************************************************************/ 
@/*                                                                        */ 
@/*  FUNCTION                                               RELEASE        */ 
@/*                                                                        */ 
@/*    _tx_thread_interrupt_disable                         ARM9/GNU       */ 
@/*                                                           6.1          */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */ 
@/*    This function is responsible for disabling interrupts               */ 
@/*                                                                        */ 
@/*  INPUT                                                                 */ 
@/*                                                                        */ 
@/*    None                                                                */ 
@/*                                                                        */ 
@/*  OUTPUT                                                                */ 
@/*                                                                        */ 
@/*    old_posture                           Old interrupt lockout posture */ 
@/*                                                                        */ 
@/*  CALLS                                                                 */ 
@/*                                                                        */ 
@/*    None                                                                */ 
@/*                                                                        */ 
@/*  CALLED BY                                                             */ 
@/*                                                                        */ 
@/*    Application Code                                                    */ 
@/*                                                                        */ 
@/*  RELEASE HISTORY                                                       */ 
@/*                                                                        */ 
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
@/*                                                                        */
@/**************************************************************************/
@UINT   _tx_thread_interrupt_disable(void)
@{
    .global _tx_thread_interrupt_disable
    .type   _tx_thread_interrupt_disable,function
_tx_thread_interrupt_disable:
@
@    /* Pickup current interrupt lockout posture.  */
@
    MRS     r0, CPSR                            @ Pickup current CPSR
@
@    /* Mask interrupts.  */
@
    ORR     r1, r0, #DISABLE_INTS               @ Mask interrupts
    MSR     CPSR_cxsf, r1                       @ Setup new CPSR
#ifdef __THUMB_INTERWORK
    BX      lr                                  @ Return to caller
#else
    MOV     pc, lr                              @ Return to caller
#endif
@}


