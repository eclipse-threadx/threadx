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


/* Include macros for modifying the wait list.  */
#include "tx_thread_smp_protection_wait_list_macros.h"


    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_protect                           Cortex-A35-SMP/AC6 */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets protection for running inside the ThreadX        */
/*    source. This is acomplished by a combination of a test-and-set      */
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
/*  09-30-2020      William E. Lamie        Initial Version 6.1           */
/*  10-15-2021      Andres Mlinar           Updated comments,             */
/*                                            added ARMv8.2-A support,    */
/*                                            improved SMP code,          */
/*                                            resulting in version 6.1.9  */
/*  04-25-2022      William E. Lamie        Modified comments, removed    */
/*                                            FIFO queueing,              */
/*                                            resulting in version 6.1.11 */
/*                                                                        */
/**************************************************************************/
    .global  _tx_thread_smp_protect
    .type    _tx_thread_smp_protect, @function
_tx_thread_smp_protect:

    /* Disable interrupts so we don't get preempted.  */

    MRS     x0, DAIF                            // Pickup current interrupt posture
    MSR     DAIFSet, 0x3                        // Lockout interrupts

    /* Pickup the CPU ID.   */

    MRS     x2, MPIDR_EL1                       // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x7, x2, #16, #8                     // Isolate cluster ID
#endif
    UBFX    x2, x2, #8, #8                      // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x7, x2, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x2, x2, #0, #8                      // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x2, x2, x7, LSL #2                  // Calculate CPU ID
#endif

    LDR     x1, =_tx_thread_smp_protection      // Build address to protection structure
    LDR     w3, [x1, #4]                        // Pickup the owning core
    CMP     w3, w2                              // Is it this core?
    BEQ     _owned                              // Yes, the protection is already owned

    LDAXR   w4, [x1, #0]                        // Pickup the protection flag
    CBZ     w4, _get_protection                 // Yes, get the protection
    MSR     DAIF, x0                            // Restore interrupts
    ISB                                         //
#ifdef TX_ENABLE_WFE
    WFE                                         // Go into standby
#endif
    B       _tx_thread_smp_protect              // On waking, restart the protection attempt

_get_protection:
    MOV     x4, #1                              // Build lock value
    STXR    w5, w4, [x1]                        // Attempt to get the protection
    CBZ     w5, _got_protection                 // Did it succeed?  w5 = 0 means success!
    MSR     DAIF, x0                            // Restore interrupts
    B       _tx_thread_smp_protect              // Restart the protection attempt
    
_got_protection:
    DMB     ISH                                 // 
    STR     w2, [x1, #4]                        // Save owning core
_owned:
    LDR     w5, [x1, #8]                        // Pickup ownership count
    ADD     w5, w5, #1                          // Increment ownership count
    STR     w5, [x1, #8]                        // Store ownership count
    DMB     ISH                                 //
    RET
