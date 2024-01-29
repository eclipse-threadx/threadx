/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    .text
    .align 4
    .syntax unified
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    .global  _tx_execution_isr_enter
#endif
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                          Cortex-M0+/AC6     */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is only needed for legacy applications and it should  */
/*    not be called in any new development on a Cortex-M.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [_tx_execution_isr_enter]             Execution profiling ISR enter */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                                                */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  01-31-2022      Scott Larson            Initial Version 6.1.10        */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_context_save(VOID)
// {
    .global  _tx_thread_context_save
    .thumb_func
_tx_thread_context_save:

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the ISR enter function to indicate an ISR is starting.  */
    PUSH    {r0, lr}                                // Save return address
    BL      _tx_execution_isr_enter                 // Call the ISR enter function
    POP     {r0, r1}                                // Recover return address
    MOV     lr, r1
#endif

    /* Context is already saved - just return.  */

    BX      lr
// }
