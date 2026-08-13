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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif
@
    .global  _tx_thread_system_state
    .global  _tx_thread_current_ptr
    .global  _tx_thread_system_stack_ptr
    .global  _tx_thread_execute_ptr
    .global  _tx_timer_time_slice
    .global  _tx_thread_schedule
    .global  _tx_thread_preempt_disable
    .global _tx_execution_isr_exit
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    .global  _tx_execution_isr_exit
#endif
@
@
    .text 32
    .align 4
    .syntax unified
@/**************************************************************************/
@/*                                                                        */
@/*  FUNCTION                                               RELEASE        */
@/*                                                                        */
@/*    _tx_thread_context_restore                        Cortex-M0/GNU     */
@/*                                                           6.2.1        */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */
@/*    This function is only needed for legacy applications and it should  */
@/*    not be called in any new development on a Cortex-M.                 */
@/*    This function restores the interrupt context if it is processing a  */
@/*    nested interrupt.  If not, it returns to the interrupt thread if no */
@/*    preemption is necessary.  Otherwise, if preemption is necessary or  */
@/*    if no thread was running, the function returns to the scheduler.    */
@/*                                                                        */
@/*  INPUT                                                                 */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  OUTPUT                                                                */
@/*                                                                        */
@/*    None                                                                */
@/*                                                                        */
@/*  CALLS                                                                 */
@/*                                                                        */
@/*    _tx_thread_schedule                   Thread scheduling routine     */
@/*                                                                        */
@/*  CALLED BY                                                             */
@/*                                                                        */
@/*    ISRs                                  Interrupt Service Routines    */
@/*                                                                        */
@/**************************************************************************/
@VOID   _tx_thread_context_restore(VOID)
@{
    .global  _tx_thread_context_restore
    .thumb_func
_tx_thread_context_restore:
@
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the ISR exit function to indicate an ISR is complete.  */
    PUSH    {r0, lr}                                // Save return address
    BL      _tx_execution_isr_exit                  // Call the ISR exit function
    POP     {r0, r1}                                // Recover r0 and the return address
    MOV     lr, r1                                  // This core cannot POP into LR: the
                                                    //   16-bit Thumb POP takes r0-r7 and
                                                    //   pc only.  MOV to a high register
                                                    //   is permitted, so restore LR from
                                                    //   a scratch register.  r1 is free --
                                                    //   the BL above may clobber r0-r3,
                                                    //   which is why r0 is saved at all.
#endif
    BX      lr
@}
