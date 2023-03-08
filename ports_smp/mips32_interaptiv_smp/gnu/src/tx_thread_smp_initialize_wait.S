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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#define UserLocal       $4,2
#define INITIAL_SR      0xFF00                  /* All IM bits set    */

    .text
    .set        noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_initialize_wait                MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the place where additional cores wait until        */
/*    initialization is complete before they enter the thread scheduling  */
/*    loop.                                                               */
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
/*    _tx_thread_schedule                   Thread scheduling loop        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Hardware                                                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
    .globl  _tx_thread_smp_initialize_wait
_tx_thread_smp_initialize_wait:

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    cfc1    $8, $31                             # Pickup current FPU control reg
    la      $9, _tx_thread_smp_initial_fpu_control_register
    sw      $8, ($9)                            # Save FPU control reg
#endif


    /* Pickup the release cores flag.  */


    la      $8, _tx_thread_smp_release_cores_flag  # Build address of release cores flag

wait_for_release:
    lw      $9, ($8)                            # Pickup release cores flag
    beq     $9, $0, wait_for_release            # Wait here until it is set
    nop

    /* Core 0 has released this core.  */

    /* Pickup the core ID.   */

    mfc0    $8, UserLocal                       # Pickup VPE ID
    sll     $8, $8, 2                           # Build index based on VPE number

    /* Clear this core's system state variable.  */

    la      $9, _tx_thread_system_state         # Build address of system state variable
    addu    $9, $9, $8                          #
    sw      $0, ($9)                            # Clear this VPE's system state entry

    /* Now wait for core 0 to finish it's initialization.  */

    di                                          # Disable interrupts
    ehb                                         #
    mfc0    $8, $12                             # Pickup current SR
    ori     $8, $8, INITIAL_SR                  # Build initial SR
    mtc0    $8, $12                             # Setup SR

core_0_wait_loop:
    la      $8, _tx_thread_system_state         # Build address of system state variable of logical VPE 0
    lw      $9, ($8)                            # Pickup system state

    bne     $9, $0, core_0_wait_loop            # If non-zero, keep waiting
    nop                                         #

    la      $8, _tx_thread_schedule             # Otherwise, initialization is done
    jr      $8                                  # Enter main scheduling loop
    nop                                         #
