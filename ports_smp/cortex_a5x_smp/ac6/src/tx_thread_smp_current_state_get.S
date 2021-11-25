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


    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_current_state_get                 Cortex-A5x-SMP/AC6 */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  10-15-2021     Andres Mlinar            Updated comments,             */
/*                                             added ARMv8.2-A support,   */
/*                                             resulting in version 6.1.9 */
/*                                                                        */
/**************************************************************************/
    .global  _tx_thread_smp_current_state_get
    .type    _tx_thread_smp_current_state_get, @function
_tx_thread_smp_current_state_get:

    MRS     x1, DAIF                            // Pickup current interrupt posture
    MSR     DAIFSet, 0x3                        // Lockout interrupts
    MRS     x2, MPIDR_EL1                       // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x3, x2, #16, #8                     // Isolate cluster ID
#endif
    UBFX    x2, x2, #8, #8                      // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x3, x2, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x2, x2, #0, #8                      // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x2, x2, x3, LSL #2                  // Calculate CPU ID
#endif
    LDR     x3, =_tx_thread_system_state        // Pickup the base of the current system state array
    LDR     w0, [x3, x2, LSL #2]                // Pickup the current system state for this core
    MSR     DAIF, x1                            // Restore interrupt posture
    RET
