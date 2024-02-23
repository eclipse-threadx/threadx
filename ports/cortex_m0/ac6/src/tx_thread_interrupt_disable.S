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
@/*    _tx_thread_interrupt_disable                      Cortex-M0/AC6     */
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
@/*  RELEASE HISTORY                                                       */
@/*                                                                        */
@/*    DATE              NAME                      DESCRIPTION             */
@/*                                                                        */
@/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
@/*  03-08-2023      Scott Larson            Include tx_user.h,            */
@/*                                            resulting in version 6.2.1  */
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
