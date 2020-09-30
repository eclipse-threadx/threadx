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
@
    .global     _tx_thread_system_state
    .global     _tx_thread_current_ptr
    .global     _tx_thread_system_stack_ptr
    .global     _tx_thread_execute_ptr
    .global     _tx_timer_time_slice
    .global     _tx_thread_schedule
    .global     _tx_thread_preempt_disable
@
@
    .text
    .align 4
    .syntax unified
@/**************************************************************************/
@/*                                                                        */
@/*  FUNCTION                                               RELEASE        */
@/*                                                                        */
@/*    _tx_thread_context_restore                        Cortex-M4/AC6     */
@/*                                                           6.1          */
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
@/*  RELEASE HISTORY                                                       */
@/*                                                                        */
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
@/*                                                                        */
@/**************************************************************************/
@VOID   _tx_thread_context_restore(VOID)
@{
    .global  _tx_thread_context_restore
    .thumb_func
_tx_thread_context_restore:
@
@ /* Not needed for this port - just return!  */
    BX      lr
@}
