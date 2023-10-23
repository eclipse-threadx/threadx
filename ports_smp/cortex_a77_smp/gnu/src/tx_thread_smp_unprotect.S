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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_unprotect                           ARMv8-A-SMP      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  09-30-2020      William E. Lamie        Initial Version 6.1           */
/*  01-31-2022      Andres Mlinar           Updated comments,             */
/*                                            added ARMv8.2-A support,    */
/*                                            resulting in version 6.1.10 */
/*  04-25-2022      William E. Lamie        Modified comments, removed    */
/*                                            FIFO queueing,              */
/*                                            resulting in version 6.1.11 */
/*  10-31-2023     Tiejun Zhou              Modified comment(s), added    */
/*                                            #include tx_user.h,         */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
    .global  _tx_thread_smp_unprotect
    .type    _tx_thread_smp_unprotect, @function
_tx_thread_smp_unprotect:
    MSR     DAIFSet, 0x3                        // Lockout interrupts

    MRS     x1, MPIDR_EL1                       // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x2, x1, #16, #8                     // Isolate cluster ID
#endif
    UBFX    x1, x1, #8, #8                      // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x2, x1, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x1, x1, #0, #8                      // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x1, x1, x2, LSL #2                  // Calculate CPU ID
#endif

    LDR     x2,=_tx_thread_smp_protection       // Build address of protection structure
    LDR     w3, [x2, #4]                        // Pickup the owning core
    CMP     w1, w3                              // Is it this core?
    BNE     _still_protected                    // If this is not the owning core, protection is in force elsewhere

    LDR     w3, [x2, #8]                        // Pickup the protection count
    CMP     w3, #0                              // Check to see if the protection is still active
    BEQ     _still_protected                    // If the protection count is zero, protection has already been cleared

    SUB     w3, w3, #1                          // Decrement the protection count
    STR     w3, [x2, #8]                        // Store the new count back
    CMP     w3, #0                              // Check to see if the protection is still active
    BNE     _still_protected                    // If the protection count is non-zero, protection is still in force
    LDR     x2,=_tx_thread_preempt_disable      // Build address of preempt disable flag
    LDR     w3, [x2]                            // Pickup preempt disable flag
    CMP     w3, #0                              // Is the preempt disable flag set?
    BNE     _still_protected                    // Yes, skip the protection release

    LDR     x2,=_tx_thread_smp_protection       // Build address of protection structure
    MOV     w3, #0xFFFFFFFF                     // Build invalid value
    STR     w3, [x2, #4]                        // Mark the protected core as invalid
    DMB     ISH                                 // Ensure that accesses to shared resource have completed
    MOV     w3, #0                              // Build release protection value
    STR     w3, [x2, #0]                        // Release the protection
    DSB     ISH                                 // To ensure update of the protection occurs before other CPUs awake

_still_protected:
#ifdef TX_ENABLE_WFE
    SEV                                         // Send event to other CPUs, wakes anyone waiting on the protection (using WFE)
#endif
    MSR     DAIF, x0                            // Restore interrupt posture
    RET
