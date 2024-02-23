@/***************************************************************************
@ * Copyright (c) 2024 Microsoft Corporation 
@ * 
@ * This program and the accompanying materials are made available under the
@ * terms of the MIT License which is available at
@ * https://opensource.org/licenses/MIT.
@ * 
@ * SPDX-License-Identifier: MIT
@ **************************************************************************/
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
    .arm
    .text
    .align 2
@/**************************************************************************/ 
@/*                                                                        */ 
@/*  FUNCTION                                               RELEASE        */ 
@/*                                                                        */ 
@/*    _tx_thread_interrupt_restore                    SMP/Cortex-A9/GNU   */ 
@/*                                                           6.1          */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */ 
@/*    This function is responsible for restoring interrupts to the state  */
@/*    returned by a previous _tx_thread_interrupt_disable call.           */
@/*                                                                        */ 
@/*  INPUT                                                                 */ 
@/*                                                                        */ 
@/*    old_posture                           Old interrupt lockout posture */ 
@/*                                                                        */ 
@/*  OUTPUT                                                                */ 
@/*                                                                        */ 
@/*    None                                                                */ 
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
@UINT   _tx_thread_interrupt_restore(UINT old_posture)
@{
    .global _tx_thread_interrupt_restore
    .type _tx_thread_interrupt_restore,function
_tx_thread_interrupt_restore: 
@
@    /* Apply the new interrupt posture.  */
@
    MSR     CPSR_c, r0                          @ Setup new CPSR
#ifdef __THUMB_INTERWORK
    BX      lr                                  @ Return to caller
#else
    MOV     pc, lr                              @ Return to caller
#endif
@}
@

