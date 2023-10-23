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
/*    _tx_thread_smp_initialize_wait                     ARMv8-A-SMP      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-31-2022     Andres Mlinar            Updated comments,             */
/*                                             added ARMv8.2-A support,   */
/*                                            resulting in version 6.1.10 */
/*  10-31-2023     Tiejun Zhou              Modified comment(s), added    */
/*                                            #include tx_user.h,         */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
    .global  _tx_thread_smp_initialize_wait
    .type    _tx_thread_smp_initialize_wait, @function
_tx_thread_smp_initialize_wait:

    /* Lockout interrupts.  */

    MSR     DAIFSet, 0x3                        // Lockout interrupts

    /* Pickup the Core ID.   */

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

    /* Make sure the system state for this core is TX_INITIALIZE_IN_PROGRESS before we check the release
       flag.  */

    LDR     w1, =0xF0F0F0F0                     // Build TX_INITIALIZE_IN_PROGRESS flag
    LDR     x3, =_tx_thread_system_state        // Pickup the base of the current system state array
wait_for_initialize:
    LDR     w0, [x3, x2, LSL #2]                // Pickup the current system state for this core
    CMP     w0, w1                              // Make sure the TX_INITIALIZE_IN_PROGRESS flag is set
    BNE     wait_for_initialize                 // Not equal, just spin here

    /* Save the system stack pointer for this core.  */

    LDR     x0, =_tx_thread_system_stack_ptr    // Pickup address of system stack ptr
    MOV     x1, sp                              // Pickup SP
    SUB     x1, x1, #15                         //
    BIC     x1, x1, #0xF                        // Get 16-bit alignment
    STR     x1, [x0, x2, LSL #3]                // Store system stack pointer


    /* Pickup the release cores flag.  */

    LDR     x4, =_tx_thread_smp_release_cores_flag  // Build address of release cores flag
wait_for_release:
    LDR     w0, [x4, #0]                        // Pickup the flag
    CMP     w0, #0                              // Is it set?
    BEQ     wait_for_release                    // Wait for the flag to be set

    /* Core 0 has released this core.  */

    /* Clear this core's system state variable.  */

    MOV     x0, #0                              // Build clear value
    STR     w0, [x3, x2, LSL #2]                // Set the current system state for this core to zero

    /* Now wait for core 0 to finish it's initialization.  */

core_0_wait_loop:
    LDR     w0, [x3, #0]                        // Pickup the current system state for core 0
    CMP     w0, #0                              // Is it 0?
    BNE     core_0_wait_loop                    // No, keep waiting for core 0 to finish its initialization

    /* Initialization is complete, enter the scheduling loop!  */

    B       _tx_thread_schedule                 // Enter the scheduling loop for this core

    RET
