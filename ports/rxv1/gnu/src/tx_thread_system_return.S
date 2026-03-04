;/***************************************************************************
; * Copyright (c) 2024 Microsoft Corporation
; *
; * This program and the accompanying materials are made available under the
; * terms of the MIT License which is available at
; * https://opensource.org/licenses/MIT.
; *
; * SPDX-License-Identifier: MIT
; **************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */
;/** ThreadX Component                                                     */
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/

    .global     __tx_thread_current_ptr
    .global     __tx_timer_time_slice
    .global     __tx_thread_schedule

    .text
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_system_return                             RXv1/GNURX     */
;/*                                                           6.1.11       */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is target processor specific.  It is used to transfer */
;/*    control from a thread back to the system.  Only a minimal context   */
;/*    is saved since the compiler assumes temp registers are going to get */
;/*    slicked by a function call anyway.                                  */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLS                                                                 */
;/*                                                                        */
;/*    _tx_thread_schedule                   Thread scheduling loop        */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    ThreadX components                                                  */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_system_return(VOID)
;{
    .GLB __tx_thread_system_return
__tx_thread_system_return:

    BRA  __tx_thread_system_return
;}
    .end

