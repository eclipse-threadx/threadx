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
/*    _tx_thread_smp_protect                        MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets protection for running inside the ThreadX        */
/*    source. This is ccomplished by a combination of a test-and-set      */
/*    flag and periodically disabling interrupts.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Previous Status Register                                            */
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
    .globl  _tx_thread_smp_protect
_tx_thread_smp_protect:

    di      $2                                  # Disable interrupts
    ehb                                         #
    sync
    mfc0    $25, UserLocal                      # Pickup VPE ID
    la      $10, _tx_thread_smp_protection      # Build address to protection structure
    lw      $9, ($10)                           # Pickup the protection
    bne     $9, $0, _already_owned              # If non-zero, then the protection is already in force
    nop                                         #
    ll      $11, ($10)                          # Pickup the protection in force flag
    bne     $11, $0, _get_retry                 # Is the protection still available?
    li      $11, 1                              # Build protection in force flag
    sc      $11, ($10)                          # Attempt to get the semaphore
    beq     $11, $0, _get_retry                 # If successful, we got the protection!
    sync
    sw      $25, 8($10)                         # Save VPE
    sw      $11, 12($10)                        # Setup the initial count

#ifdef TX_THREAD_SMP_DEBUG_ENABLE
    sll     $13, $25, 2                         # Build index based on VPE number
    la      $12, _tx_thread_current_ptr         # Pickup the current thread pointer
    addu    $12, $12, $13                       # Build address of current thread pointer for this VPE
    lw      $9, ($12)                           # Pickup current thread pointer
    sw      $31, 16($10)                        # Save caller info
    sw      $2, 20($10)                         # Save SR
    sw      $9, 4($10)                          # Save the current thread pointer
#endif

    j       $31                                 # Return to caller
    nop

_get_retry:
    mtc0    $2, C0_Status                       # Restore interrupt posture
    ehb                                         #
    b       _tx_thread_smp_protect              # Try to get the protection again
    nop                                         #

_already_owned:
    lw      $9, 8($10)                          # Pickup the owned VPE
    beq     $9, $25, _have_the_protection       # If equal, we already have the protection
    lw      $12, 12($10)                        # Pickup protection count
    mtc0    $2, C0_Status                       # Restore interrupt posture
    ehb                                         #
    b       _tx_thread_smp_protect              # Try to get the protection again
    nop                                         #

_have_the_protection:
    addu    $12, $12, 1                         # Increment
    j       $31                                 # Return to caller
    sw      $12, 12($10)                        # Store back the protection count
