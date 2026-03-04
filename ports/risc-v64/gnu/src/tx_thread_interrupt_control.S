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
/**************************************************************************/
/* UINT   _tx_thread_interrupt_control(UINT new_posture)
{  */
    .global  _tx_thread_interrupt_control
_tx_thread_interrupt_control:
    /* Pickup current interrupt lockout posture.  */
    /* old_mstatus = mstatus;  */

    csrr    t0, mstatus
    mv      t1, t0                                      // Save original mstatus for return

    /* Apply the new interrupt posture while preserving unrelated mstatus bits. */
    /* Only modify the MIE bit (bit 3)  */
    /* mstatus = (mstatus & ~MIE) | (new_posture & MIE);  */

    li      t2, ~0x08                                   // Build mask to clear MIE
    and     t0, t0, t2                                  // Clear MIE bit
    and     a0, a0, 0x08                                // Mask incoming to only MIE bit
    or      t0, t0, a0                                  // Set requested MIE state
    csrw    mstatus, t0
    andi    a0, t1, 0x08                                // Return original MIE bit
    ret
/* }  */
