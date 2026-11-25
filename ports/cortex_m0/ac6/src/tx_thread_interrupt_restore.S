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
@/*    _tx_thread_interrupt_restore                      Cortex-M0/AC6     */
@/*                                                           6.2.1        */
@/*  AUTHOR                                                                */
@/*                                                                        */
@/*    William E. Lamie, Microsoft Corporation                             */
@/*                                                                        */
@/*  DESCRIPTION                                                           */
@/*                                                                        */
@/*    This function is responsible for restoring the interrupt lockout    */
@/*    posture of the system.                                              */
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
@/**************************************************************************/
@/* VOID   _tx_thread_interrupt_restore(UINT old_posture)
{  */
    .global  _tx_thread_interrupt_restore
    .thumb_func
_tx_thread_interrupt_restore:

    MSR     PRIMASK, r0
    BX      lr

@/* } */
