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
/**   Thread - Low Level SMP Support                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_core_preempt                      Cortex-A35-SMP/AC6 */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function preempts the specified core in situations where the   */
/*    thread corresponding to this core is no longer ready or when the    */
/*    core must be used for a higher-priority thread. If the specified is */
/*    the current core, this processing is skipped since the will give up */
/*    control subsequently on its own.                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    core                                  The core to preempt           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX Source                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Andres Mlinar            Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/
    .global  _tx_thread_smp_core_preempt
    .type    _tx_thread_smp_core_preempt, @function
_tx_thread_smp_core_preempt:
    DSB ISH
    MOV x2, #0x1                                //
    LSL x2, x2, x0                              // Shift by the core ID
    MSR ICC_SGI1R_EL1, x2                       // Issue inter-core interrupt
    RET

