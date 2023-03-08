/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread - Low Level SMP Support                                      */
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
/*    _tx_thread_smp_unprotect                      MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function releases previously obtained protection. The supplied */
/*    previous SR is restored. If the value of _tx_thread_system_state    */
/*    and _tx_thread_preempt_disable are both zero, then multithreading   */
/*    is enabled as well.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    Previous Status Register                                            */
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
/*    ThreadX Source                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
    .globl  _tx_thread_smp_unprotect
_tx_thread_smp_unprotect:

    di                                          # Disable interrupts
    ehb                                         #
    sync
    mfc0    $25, UserLocal                      # Pickup VPE ID
    la      $10, _tx_thread_smp_protection      # Build address of protection structure
    lw      $9, 8($10)                          # Pickup owning VPE
    bne     $9, $25, _still_protected           # Not the same VPE, protection is in force somewhere else
    lw      $12, 12($10)                        # Pickup protection count
    beq     $12, $0, _still_protected           # If zero, protection is not in force anymore
    subu    $12, $12, 1                         # Decrement
    sw      $12, 12($10)                        # Store back the protection count
    bne     $12, $0, _still_protected           # If non-zero, nested protection condition
    nop                                         #
    la      $11, _tx_thread_preempt_disable     # Build address of preempt disable flag
    lw      $12, ($11)                          # Pickup preempt disable flag
    bne     $12, $0, _still_protected           # Don't release protection if preempt disable flag is set
    li      $8, 0xFFFFFFFF                      # Setup invalid value
    sw      $8, 8($10)                          # Mark VPE as invalid
#ifdef TX_THREAD_SMP_DEBUG_ENABLE
    sw      $31, 24($10)                        # Remember the caller of the unprotect
#endif

_release_protect_loop:
    sync
    sw      $0, ($10)                           # Clear protection
    sync

_still_protected:
    mtc0    $4, C0_Status                       # Restore interrupt posture
    jr.hb   $31                                 # Return to caller
    nop                                         #
