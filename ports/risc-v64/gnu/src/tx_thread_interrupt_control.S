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
/*                                                        6.5.1.202602a   */
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

#ifdef TX_RISCV_SMODE
    li      t1, 0x02                                    // Build SIE bit mask (bit 1)
    andi    a0, a0, 0x02                                // Mask incoming to only SIE bit
    bnez    a0, 1f                                      // If set, enable interrupts
    csrrc   t0, sstatus, t1                             // Atomically clear SIE, read old sstatus
    j       2f
1:
    csrrs   t0, sstatus, t1                             // Atomically set SIE, read old sstatus
2:
    andi    a0, t0, 0x02                                // Return original SIE bit
#else
    li      t1, 0x08                                    // Build MIE bit mask (bit 3)
    andi    a0, a0, 0x08                                // Mask incoming to only MIE bit
    bnez    a0, 1f                                      // If set, enable interrupts
    csrrc   t0, mstatus, t1                             // Atomically clear MIE, read old mstatus
    j       2f
1:
    csrrs   t0, mstatus, t1                             // Atomically set MIE, read old mstatus
2:
    andi    a0, t0, 0x08                                // Return original MIE bit
#endif
    ret
/* }  */
