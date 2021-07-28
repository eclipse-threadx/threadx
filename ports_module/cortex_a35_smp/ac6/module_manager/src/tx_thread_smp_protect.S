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
/*                                                           6.1          */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
    .global  _tx_thread_smp_protect
    .type    _tx_thread_smp_protect, @function
_tx_thread_smp_protect:

    /* Disable interrupts so we don't get preempted.  */

    MRS     x0, DAIF                            // Pickup current interrupt posture
    MSR     DAIFSet, 0x3                        // Lockout interrupts

    /* Pickup the CPU ID.   */

    MRS     x1, MPIDR_EL1                       // Pickup the core ID
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x7, x1, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x1, x1, #0, #8                      // Isolate core ID
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x1, x1, x7, LSL #2                  // Calculate CPU ID
#endif

    /* Do we already have protection?  */
    // if (this_core == _tx_thread_smp_protection.tx_thread_smp_protect_core)
    // {

    LDR     x2, =_tx_thread_smp_protection      // Build address to protection structure
    LDR     w3, [x2, #4]                        // Pickup the owning core
    CMP     w1, w3                              // Is it not this core?
    BNE     _protection_not_owned               // No, the protection is not already owned

    /* We already have protection. */

    /* Increment the protection count. */
    // _tx_thread_smp_protection.tx_thread_smp_protect_count++;

    LDR     w3, [x2, #8]                        // Pickup ownership count
    ADD     w3, w3, #1                          // Increment ownership count
    STR     w3, [x2, #8]                        // Store ownership count
    DMB     ISH

    B       _return

_protection_not_owned:

    /* Is the lock available?  */
    // if (_tx_thread_smp_protection.tx_thread_smp_protect_in_force == 0)
    // {

    LDAXR   w3, [x2, #0]                        // Pickup the protection flag
    CMP     w3, #0
    BNE     _start_waiting                      // No, protection not available

    /* Is the list empty?  */
    // if (_tx_thread_smp_protect_wait_list_head == _tx_thread_smp_protect_wait_list_tail)
    // {

    LDR     x3, =_tx_thread_smp_protect_wait_list_head
    LDR     w3, [x3]
    LDR     x4, =_tx_thread_smp_protect_wait_list_tail
    LDR     w4, [x4]
    CMP     w3, w4
    BNE     _list_not_empty

    /* Try to get the lock.  */
    // if (write_exclusive(&_tx_thread_smp_protection.tx_thread_smp_protect_in_force, 1) == SUCCESS)
    // {

    MOV     w3, #1                              // Build lock value
    STXR    w4, w3, [x2, #0]                    // Attempt to get the protection
    CMP     w4, #0
    BNE     _start_waiting                      // Did it fail?

    /* We got the lock! */
    // _tx_thread_smp_protect_lock_got();

    DMB     ISH                                 // Ensure write to protection finishes
    _tx_thread_smp_protect_lock_got             // Call the lock got function

    B       _return

_list_not_empty:

    /* Are we at the front of the list?  */
    // if (this_core == _tx_thread_smp_protect_wait_list[_tx_thread_smp_protect_wait_list_head])
    // {

    LDR     x3, =_tx_thread_smp_protect_wait_list_head // Get the address of the head
    LDR     w3, [x3]                            // Get the value of the head
    LDR     x4, =_tx_thread_smp_protect_wait_list // Get the address of the list
    LDR     w4, [x4, x3, LSL #2]                // Get the value at the head index

    CMP     w1, w4
    BNE     _start_waiting

    /* Is the lock still available?  */
    // if (_tx_thread_smp_protection.tx_thread_smp_protect_in_force == 0)
    // {

    LDR     w3, [x2, #0]                        // Pickup the protection flag
    CMP     w3, #0
    BNE     _start_waiting                      // No, protection not available

    /* Get the lock.  */
    // _tx_thread_smp_protection.tx_thread_smp_protect_in_force = 1;

    MOV     w3, #1                              // Build lock value
    STR     w3, [x2, #0]                        // Store lock value
    DMB     ISH                                 //

    /* Got the lock.  */
    // _tx_thread_smp_protect_lock_got();

    _tx_thread_smp_protect_lock_got

    /* Remove this core from the wait list.  */
    // _tx_thread_smp_protect_remove_from_front_of_list();

    _tx_thread_smp_protect_remove_from_front_of_list

    B       _return

_start_waiting:

    /* For one reason or another, we didn't get the lock.  */

    /* Increment wait count. */
    // _tx_thread_smp_protect_wait_counts[this_core]++;

    LDR     x3, =_tx_thread_smp_protect_wait_counts // Load wait list counts
    LDR     w4, [x3, x1, LSL #2]                // Load waiting value for this core
    ADD     w4, w4, #1                          // Increment wait value
    STR     w4, [x3, x1, LSL #2]                // Store new wait value

    /* Have we not added ourselves to the list yet? */
    // if (_tx_thread_smp_protect_wait_counts[this_core] == 1)
    // {

    CMP     w4, #1
    BNE     _already_in_list0                   // Is this core already waiting?

    /* Add ourselves to the list.  */
    // _tx_thread_smp_protect_wait_list_add(this_core);

    _tx_thread_smp_protect_wait_list_add        // Call macro to add ourselves to the list

    // }

_already_in_list0:

    /* Restore interrupts. */

    MSR     DAIF, x0                            // Restore interrupts
    ISB                                         // 
#ifdef TX_ENABLE_WFE
    WFE                                         // Go into standby
#endif

    /* We do this until we have the lock. */
    // while (1)
    // {

_try_to_get_lock:

    /* Disable interrupts so we don't get preempted.  */

    MRS     x0, DAIF                            // Pickup current interrupt posture
    MSR     DAIFSet, 0x3                        // Lockout interrupts

    /* Pickup the CPU ID.   */

    MRS     x1, MPIDR_EL1                       // Pickup the core ID
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x7, x1, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x1, x1, #0, #8                      // Isolate core ID
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x1, x1, x7, LSL #2                  // Calculate CPU ID
#endif

    /* Do we already have protection?  */
    // if (this_core == _tx_thread_smp_protection.tx_thread_smp_protect_core)
    // {

    LDR     w3, [x2, #4]                        // Pickup the owning core
    CMP     w3, w1                              // Is it this core?
    BEQ     _got_lock_after_waiting             // Yes, the protection is already owned. This means
                                                // an ISR preempted us and got protection

    // }

    /* Are we at the front of the list?  */
    // if (this_core == _tx_thread_smp_protect_wait_list[_tx_thread_smp_protect_wait_list_head])
    // {/

    LDR     x3, =_tx_thread_smp_protect_wait_list_head // Get the address of the head
    LDR     w3, [x3]                            // Get the value of the head
    LDR     x4, =_tx_thread_smp_protect_wait_list // Get the address of the list
    LDR     w4, [x4, x3, LSL #2]                // Get the value at the head index

    CMP     w1, w4
    BNE     _did_not_get_lock

    /* Is the lock still available? */
    // if (_tx_thread_smp_protection.tx_thread_smp_protect_in_force == 0)
    // {

    LDR     w3, [x2, #0]                        // Pickup the protection flag
    CMP     w3, #0
    BNE     _did_not_get_lock                   // No, protection not available

    /* Get the lock.  */
    // _tx_thread_smp_protection.tx_thread_smp_protect_in_force = 1;

    MOV     w3, #1                              // Build lock value
    STR     w3, [x2, #0]                        // Store lock value
    DMB     ISH                                 //

    /* Got the lock.  */
    // _tx_thread_smp_protect_lock_got();

    _tx_thread_smp_protect_lock_got

    /* Remove this core from the wait list.  */
    // _tx_thread_smp_protect_remove_from_front_of_list();

    _tx_thread_smp_protect_remove_from_front_of_list

    B       _got_lock_after_waiting

_did_not_get_lock:

    /* For one reason or another, we didn't get the lock.  */

    /* Were we removed from the list? This can happen if we're a thread
       and we got preempted. */
    // if (_tx_thread_smp_protect_wait_counts[this_core] == 0)
    // {

    LDR     x3, =_tx_thread_smp_protect_wait_counts // Load wait list counts
    LDR     w4, [x3, x1, LSL #2]                // Load waiting value for this core
    CMP     w4, #0
    BNE     _already_in_list1                   // Is this core already in the list?

    /* Add ourselves to the list.  */
    // _tx_thread_smp_protect_wait_list_add(this_core);

    _tx_thread_smp_protect_wait_list_add        // Call macro to add ourselves to the list

    /* Our waiting count was also reset when we were preempted. Increment it again. */
    // _tx_thread_smp_protect_wait_counts[this_core]++;

    LDR     x3, =_tx_thread_smp_protect_wait_counts // Load wait list counts
    LDR     w4, [x3, x1, LSL #2]                // Load waiting value for this core
    ADD     w4, w4, #1                          // Increment wait value
    STR     w4, [x3, x1, LSL #2]                // Store new wait value value

    // }

_already_in_list1:

    /* Restore interrupts and try again.  */

    MSR     DAIF, x0                            // Restore interrupts
    ISB                                         // 
#ifdef TX_ENABLE_WFE
    WFE                                         // Go into standby
#endif
    B       _try_to_get_lock                    // On waking, restart the protection attempt

_got_lock_after_waiting:

    /* We're no longer waiting.  */
    // _tx_thread_smp_protect_wait_counts[this_core]--;

    LDR     x3, =_tx_thread_smp_protect_wait_counts // Load waiting list
    LDR     w4, [x3, x1, LSL #2]                // Load current wait value
    SUB     w4, w4, #1                          // Decrement wait value
    STR     w4, [x3, x1, LSL #2]                // Store new wait value value

    /* Restore registers and return.  */

_return:

    RET

