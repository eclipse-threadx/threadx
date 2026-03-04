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


#define UserLocal       $4,2
#define C0_Status       $12

    .text
    .set        noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_current_state_get              MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is gets the current state of the calling core.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX Components                                                  */
/*                                                                        */
/**************************************************************************/
    .globl  _tx_thread_smp_current_state_get
_tx_thread_smp_current_state_get:

    di      $8                                  # Disable interrupts
    ehb                                         #
    la      $12, _tx_thread_system_state        # Pickup start of the current state array
    mfc0    $25, UserLocal                      # Pickup VPE ID
    sll     $25, $25, 2                         # Build index based on VPE number
    addu    $12, $12, $25                       # Build address of current state for this VPE
    lw      $2, ($12)                           # Pickup current state
    mtc0    $8, C0_Status                       # Restore interrupts
    jr.hb   $31                                 # Return to caller
    nop                                         #
