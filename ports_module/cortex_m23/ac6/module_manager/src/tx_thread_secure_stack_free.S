/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
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


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_secure_stack_free                      Cortex-M23/AC6    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function enters the SVC handler to free a secure stack.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Thread control block pointer  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Actual completion status      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    SVC 2                                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
// UINT   _tx_thread_secure_stack_free(TX_THREAD *thread_ptr)
// {
    .section .text
    .balign 4
    .syntax unified
    .eabi_attribute Tag_ABI_align_preserved, 1
    .global _tx_thread_secure_stack_free
    .global _tx_free_return
    .thumb_func
.type _tx_thread_secure_stack_free, function
_tx_thread_secure_stack_free:
#if !defined(TX_SINGLE_MODE_SECURE) && !defined(TX_SINGLE_MODE_NON_SECURE)
    MRS     r3, PRIMASK     // Save interrupt mask
    CPSIE   i               // Enable interrupts for SVC call
    SVC     2
_tx_free_return:
    CMP     r3, #0          // If interrupts enabled, just return
    BEQ     _free_return_interrupt_enabled
    CPSID   i               // Otherwise, disable interrupts
#else
    MOVS    r0, #0xFF       // Feature not enabled
#endif
_free_return_interrupt_enabled:
    BX      lr
    .end
