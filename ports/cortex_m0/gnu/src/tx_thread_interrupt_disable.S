@/***************************************************************************
@ * Copyright (c) 2024 Microsoft Corporation
@ *
@ * This program and the accompanying materials are made available under the
@ * terms of the MIT License which is available at
@ * https://opensource.org/licenses/MIT.
@ *
@ * SPDX-License-Identifier: MIT
@ **************************************************************************/


@/**************************************************************************/
@/**************************************************************************/
@/**                                                                       */
@/** ThreadX Component                                                     */
@/**                                                                       */
@/**   Thread                                                              */
@/**                                                                       */
@/**************************************************************************/
@/**************************************************************************/

#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .text 32
    .align 4
    .syntax unified

@/**************************************************************************/
@/*                                                                        */
@/*  FUNCTION                                               RELEASE        */
@/*                                                                        */
@/*    _tx_thread_interrupt_disable                      Cortex-M0/GNU     */
@/*                                                           6.2.1        */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */
@/*    This function is responsible for disabling interrupts.              */
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
@/**************************************************************************/
@/* UINT   _tx_thread_interrupt_disable(VOID)
{  */
    .global  _tx_thread_interrupt_disable
    .thumb_func
_tx_thread_interrupt_disable:

@/* Pickup current interrupt lockout posture.  */

    MRS     r0, PRIMASK
    CPSID   i
    BX      lr

@/* } */
