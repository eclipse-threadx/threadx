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
;/*    _tx_thread_interrupt_control                      Cortex-M0/IAR     */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is responsible for changing the interrupt lockout     */
;/*    posture of the system.                                              */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    new_posture                           New interrupt lockout posture */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    old_posture                           Old interrupt lockout posture */
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
;UINT   _tx_thread_interrupt_control(UINT new_posture)
;{
    PUBLIC  _tx_thread_interrupt_control
_tx_thread_interrupt_control:
;
;    /* Pickup current interrupt lockout posture.  */
;
    MRS     r1, PRIMASK
    MSR     PRIMASK, r0
    MOV     r0, r1
    BX      lr
;
;}
    END
