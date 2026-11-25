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
;
;
    SECTION `.text`:CODE:NOROOT(2)
    THUMB
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_interrupt_restore                      Cortex-M0/IAR     */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is responsible for disabling interrupts and returning */
;/*    the previous interrupt lockout posture.                             */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    old_posture                           Old interrupt lockout posture */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLS                                                                 */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    Application Code                                                    */
;/*                                                                        */
;/**************************************************************************/
;UINT   _tx_thread_interrupt_disable(UINT new_posture)
;{
    PUBLIC  _tx_thread_interrupt_disable
_tx_thread_interrupt_disable:
;
;    /* Return current interrupt lockout posture.  */
;
    MRS     r0, PRIMASK
    CPSID   i
    BX      lr
;
;}
    END
