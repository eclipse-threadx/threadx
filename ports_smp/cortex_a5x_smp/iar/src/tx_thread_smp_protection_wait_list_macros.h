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

_tx_thread_smp_protect_lock_got  macro

    /* Set the currently owned core.  */
    /* _tx_thread_smp_protection.tx_thread_smp_protect_core = this_core; */

    STR     w1, [x2, #4]                        // Store this core

    /* Increment the protection count. */
    /* _tx_thread_smp_protection.tx_thread_smp_protect_count++; */

    LDR     w3, [x2, #8]                        // Pickup ownership count
    ADD     w3, w3, #1                          // Increment ownership count
    STR     w3, [x2, #8]                        // Store ownership count
    DMB     ISH

    endm

_tx_thread_smp_protect_remove_from_front_of_list   macro
    LOCAL _store_new_head

    /* Remove ourselves from the list.  */
    /* _tx_thread_smp_protect_wait_list[_tx_thread_smp_protect_wait_list_head++] =  0xFFFFFFFF; */

    MOV     w3, #0xFFFFFFFF                     // Build the invalid core value
    LDR     x4, =_tx_thread_smp_protect_wait_list_head // Get the address of the head
    LDR     w5, [x4]                            // Get the value of the head
    LDR     x6, =_tx_thread_smp_protect_wait_list // Get the address of the list
    STR     w3, [x6, x5, LSL #2]                // Store the invalid core value
    ADD     w5, w5, #1                          // Increment the head

    /* Did we wrap?  */
    /* if (_tx_thread_smp_protect_wait_list_head == TX_THREAD_SMP_MAX_CORES + 1)
    { */

    LDR     x3, =_tx_thread_smp_protect_wait_list_size // Load address of core list size
    LDR     w3, [x3]                            // Load the max cores value
    CMP     w5, w3                              // Compare the head to it
    B.NE     _store_new_head                    // Are we at the max?

    /* _tx_thread_smp_protect_wait_list_head = 0; */

    EOR     w5, w5, w5                          // We're at the max. Set it to zero

    /* } */

_store_new_head:

    STR     w5, [x4]                            // Store the new head

    /* We have the lock!  */
    DMB     ISH                                 // Ensure write to protection finishes

    /* return; */

    endm


_tx_thread_smp_protect_wait_list_lock_get    macro
    LOCAL _tx_thread_smp_protect_wait_list_lock_get__try_to_get_lock

/* VOID  _tx_thread_smp_protect_wait_list_lock_get()
{ */
    /* We do this until we have the lock.  */
    /* while (1)
    { */

_tx_thread_smp_protect_wait_list_lock_get__try_to_get_lock:

    /* Is the list lock available?  */
    /* _tx_thread_smp_protect_wait_list_lock_protect_in_force = load_exclusive(&_tx_thread_smp_protect_wait_list_lock_protect_in_force); */

    LDR     x1, =_tx_thread_smp_protect_wait_list_lock_protect_in_force
    LDAXR   w2, [x1]                            // Pickup the protection flag

    /* if (protect_in_force == 0)
    { */

    CMP     w2, #0
    B.NE     _tx_thread_smp_protect_wait_list_lock_get__try_to_get_lock // No, protection not available

    /* Try to get the list.  */
    /* int status = store_exclusive(&_tx_thread_smp_protect_wait_list_lock_protect_in_force, 1); */

    MOV     w2, #1                              // Build lock value
    STXR    w3, w2, [x1]                        // Attempt to get the protection

    /* if (status == SUCCESS) */

    CMP     w3, #0
    B.NE     _tx_thread_smp_protect_wait_list_lock_get__try_to_get_lock // Did it fail? If so, try again.

    /* We have the lock!  */
    /* return; */

    endm


_tx_thread_smp_protect_wait_list_add    macro
    LOCAL _tx_thread_smp_protect_wait_list_add__no_wrap

/* VOID  _tx_thread_smp_protect_wait_list_add(UINT new_core)
{ */

    /* We're about to modify the list, so get the list lock.  */
    /* _tx_thread_smp_protect_wait_list_lock_get(); */

    STP     x1, x2, [sp, #-16]!                 // Save registers we'll be using

    _tx_thread_smp_protect_wait_list_lock_get

    LDP     x1, x2, [sp], #16

    /* Add this core.  */
    /* _tx_thread_smp_protect_wait_list[_tx_thread_smp_protect_wait_list_tail++] = new_core; */

    LDR     x3, =_tx_thread_smp_protect_wait_list_tail // Get the address of the tail
    LDR     w4, [x3]                            // Get the value of tail
    LDR     x5, =_tx_thread_smp_protect_wait_list // Get the address of the list
    STR     w1, [x5, x4, LSL #2]                // Store the new core value
    ADD     w4, w4, #1                          // Increment the tail

    /* Did we wrap?  */
    /* if (_tx_thread_smp_protect_wait_list_tail == _tx_thread_smp_protect_wait_list_size)
    { */

    LDR     x5, =_tx_thread_smp_protect_wait_list_size // Load max cores address
    LDR     w5, [x5]                            // Load max cores value
    CMP     w4, w5                              // Compare max cores to tail
    B.NE     _tx_thread_smp_protect_wait_list_add__no_wrap // Did we wrap?

    /* _tx_thread_smp_protect_wait_list_tail = 0; */

    MOV     w4, #0

    /* } */

_tx_thread_smp_protect_wait_list_add__no_wrap:

    STR     w4, [x3]                            // Store the new tail value.
    DMB     ISH                                 // Ensure that accesses to shared resource have completed

    /* Release the list lock.  */
    /* _tx_thread_smp_protect_wait_list_lock_protect_in_force = 0; */

    MOV     w3, #0                              // Build lock value
    LDR     x4, =_tx_thread_smp_protect_wait_list_lock_protect_in_force
    STR     w3, [x4]                            // Store the new value
    DMB     ISH                                 // Ensure write to protection finishes

    endm


_tx_thread_smp_protect_wait_list_remove    macro
    LOCAL _tx_thread_smp_protect_wait_list_remove__check_cur_core
    LOCAL _tx_thread_smp_protect_wait_list_remove__found_core
    LOCAL _tx_thread_smp_protect_wait_list_remove__next_index_no_wrap
    LOCAL _tx_thread_smp_protect_wait_list_remove__tail_not_zero
    LOCAL _tx_thread_smp_protect_wait_list_remove__removed
    LOCAL _tx_thread_smp_protect_wait_list_remove__compare_index_to_tail

/* VOID _tx_thread_smp_protect_wait_list_remove(UINT core)
{ */

    /* Get the core index.  */
    /* UINT core_index;
    for (core_index = 0;; core_index++) */

    EOR     w4, w4, w4                          // Clear for 'core_index'
    LDR     x2, =_tx_thread_smp_protect_wait_list // Get the address of the list

    /* { */

_tx_thread_smp_protect_wait_list_remove__check_cur_core:

    /* Is this the core?  */
    /* if (_tx_thread_smp_protect_wait_list[core_index] == core)
    {
        break; */

    LDR     w3, [x2, x4, LSL #2]                // Get the value at the current index
    CMP     w3, w8                              // Did we find the core?
    B.EQ    _tx_thread_smp_protect_wait_list_remove__found_core

    /* } */

    ADD     w4, w4, #1                          // Increment cur index
    B       _tx_thread_smp_protect_wait_list_remove__check_cur_core // Restart the loop

    /* } */

_tx_thread_smp_protect_wait_list_remove__found_core:

    /* We're about to modify the list. Get the lock. We need the lock because another
       core could be simultaneously adding (a core is simultaneously trying to get
       the inter-core lock) or removing (a core is simultaneously being preempted,
       like what is currently happening).  */
    /* _tx_thread_smp_protect_wait_list_lock_get(); */

    MOV     x6, x1
    _tx_thread_smp_protect_wait_list_lock_get
    MOV     x1, x6

    /* We remove by shifting.  */
    /* while (core_index != _tx_thread_smp_protect_wait_list_tail)
    { */

_tx_thread_smp_protect_wait_list_remove__compare_index_to_tail:

    LDR     x2, =_tx_thread_smp_protect_wait_list_tail // Load tail address
    LDR     w2, [x2]                            // Load tail value
    CMP     w4, w2                              // Compare cur index and tail
    B.EQ    _tx_thread_smp_protect_wait_list_remove__removed

    /* UINT next_index = core_index + 1; */

    MOV     w2, w4                              // Move current index to next index register
    ADD     w2, w2, #1                          // Add 1

    /* if (next_index == _tx_thread_smp_protect_wait_list_size)
    { */

    LDR     x3, =_tx_thread_smp_protect_wait_list_size
    LDR     w3, [x3]
    CMP     w2, w3
    B.NE     _tx_thread_smp_protect_wait_list_remove__next_index_no_wrap

    /* next_index = 0; */

    MOV     w2, #0

    /* } */

_tx_thread_smp_protect_wait_list_remove__next_index_no_wrap:

    /* list_cores[core_index] = list_cores[next_index]; */

    LDR     x5, =_tx_thread_smp_protect_wait_list // Get the address of the list
    LDR     w3, [x5, x2, LSL #2]                // Get the value at the next index
    STR     w3, [x5, x4, LSL #2]                // Store the value at the current index

    /* core_index = next_index; */

    MOV     w4, w2

    B       _tx_thread_smp_protect_wait_list_remove__compare_index_to_tail

    /* } */

_tx_thread_smp_protect_wait_list_remove__removed:

    /* Now update the tail.  */
    /* if (_tx_thread_smp_protect_wait_list_tail == 0)
    { */

    LDR     x5, =_tx_thread_smp_protect_wait_list_tail // Load tail address
    LDR     w4, [x5]                            // Load tail value
    CMP     w4, #0
    B.NE     _tx_thread_smp_protect_wait_list_remove__tail_not_zero

    /* _tx_thread_smp_protect_wait_list_tail = _tx_thread_smp_protect_wait_list_size; */

    LDR     x2, =_tx_thread_smp_protect_wait_list_size
    LDR     w4, [x2]

    /* } */

_tx_thread_smp_protect_wait_list_remove__tail_not_zero:

    /* _tx_thread_smp_protect_wait_list_tail--; */

    SUB     w4, w4, #1
    STR     w4, [x5]                            // Store new tail value
    DMB     ISH                                 // Ensure that accesses to shared resource have completed

    /* Release the list lock.  */
    /* _tx_thread_smp_protect_wait_list_lock_protect_in_force = 0; */

    MOV     w2, #0                              // Build lock value
    LDR     x4, =_tx_thread_smp_protect_wait_list_lock_protect_in_force // Load lock address
    STR     w2, [x4]                            // Store the new value
    DMB     ISH                                 // Ensure write to protection finishes

    /* We're no longer waiting. Note that this should be zero since, again,
       this function is only called when a thread preemption is occurring.  */
    /* _tx_thread_smp_protect_wait_counts[core]--; */
    LDR     x4, =_tx_thread_smp_protect_wait_counts // Load wait list counts
    LDR     w2, [x4, x8, LSL #2]                // Load waiting value
    SUB     w2, w2, #1                          // Subtract 1
    STR     w2, [x4, x8, LSL #2]                // Store new waiting value

    endm
