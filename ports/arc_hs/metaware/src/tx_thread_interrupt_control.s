;/***************************************************************************
; * Copyright (c) 2024 Microsoft Corporation 
; * 
; * This program and the accompanying materials are made available under the
; * terms of the MIT License which is available at
; * https://opensource.org/licenses/MIT.
; * 
; * SPDX-License-Identifier: MIT
; **************************************************************************/

;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */
;/** ThreadX Component                                                     */
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_interrupt_control                      ARC_HS/MetaWare   */
;/*                                                           6.2.1        */
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
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*  04-02-2021     Andres Mlinar            Modified comments,            */
;/*                                            resulting in version 6.1.6  */
;/*  03-08-2023     Cindy Deng               Modified comment(s), added    */
;/*                                            #include tx_user.h,         */
;/*                                            resulting in version 6.2.1  */
;/*                                                                        */
;/**************************************************************************/
;UINT   _tx_thread_interrupt_control(UINT new_posture)
;{
    .global _tx_thread_interrupt_control
    .type   _tx_thread_interrupt_control, @function
_tx_thread_interrupt_control:
;
;    /* Pickup current interrupt lockout posture.  */
;
    clri    r1                                          ; Get current interrupt state
;
;    /* Apply the new interrupt posture.  */
;
    seti    r0                                          ; Set desired interrupt state
    j_s.d   [blink]                                     ; Return to caller with delay slot
    mov     r0, r1                                      ; Return previous mask value. Return value is TX_INT_DISABLE or TX_INT_ENABLE.
;
;}
    .end
