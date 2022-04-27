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

/*
#define TX_SOURCE_CODE
#define TX_THREAD_SMP_SOURCE_CODE
*/

/* Include necessary system files.  */

/*
#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
*/

    EXTERN     _tx_thread_current_ptr
    EXTERN     _tx_thread_smp_protection
    EXTERN     _tx_thread_smp_protect_wait_list_head
    EXTERN     _tx_thread_smp_protect_wait_list_tail
    EXTERN     _tx_thread_smp_protect_wait_list
    EXTERN     _tx_thread_smp_protect_wait_counts
    EXTERN     _tx_thread_smp_protect_wait_list_size
    EXTERN     _tx_thread_smp_protect_wait_list_lock_protect_in_force

    SECTION `.text`:CODE:NOROOT(3)
    CODE


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_protect                           Cortex-A5x-SMP/IAR */
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
/*  10-15-2021      William E. Lamie        Initial Version 6.1.9         */
/*  04-25-2022      William E. Lamie        Modified comments, removed    */
/*                                            FIFO queueing,              */
/*                                            resulting in version 6.1.11 */
/*                                                                        */
/**************************************************************************/
    PUBLIC   _tx_thread_smp_protect
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
    B.EQ    _owned                              // Yes, the protection is already owned

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

    END
