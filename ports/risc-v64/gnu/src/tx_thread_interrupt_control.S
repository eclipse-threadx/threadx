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

    RETURN_MASK = 0x000000000000000F
    SET_SR_MASK = 0xFFFFFFFFFFFFFFF0

    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_control                       RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for changing the interrupt lockout     */
/*    posture of the system.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_posture                           New interrupt lockout posture */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    old_posture                           Old interrupt lockout posture */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* UINT   _tx_thread_interrupt_control(UINT new_posture)
{  */
    .global  _tx_thread_interrupt_control
_tx_thread_interrupt_control:
    /* Pickup current interrupt lockout posture.  */

    csrr    t0, mstatus
    mv      t1, t0                                      // Save original mstatus for return

    /* Apply the new interrupt posture.  */

    li      t2, SET_SR_MASK                             // Build set SR mask
    and     t0, t0, t2                                  // Isolate interrupt lockout bits
    or      t0, t0, a0                                  // Put new lockout bits in
    csrw    mstatus, t0
    andi    a0, t1, RETURN_MASK                         // Return original mstatus.
    ret
/* }  */
