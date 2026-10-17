/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* This test checks priority inheritance through a chain of mutexes: owner 0 holds
   mutex 0 and waits on mutex 1, held by owner 1, and so on.  When a thread at 5 then
   blocks on mutex 0, TX_MUTEX_INHERIT_CHAIN_MAX decides how far along the chain the
   priority is passed.  Without it, only owner 0 inherits.  The chain must also reach
   an owner that is ready, and must end on a lock cycle.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Wait a tick at a time, for at most 200 ticks, until a condition holds.  A thread the
   control thread starts can take more than a tick to get there on an SMP host.  */

#define WAIT_UNTIL(condition)   { UINT ticks = 0; while ((!(condition)) && (ticks < 200)) { tx_thread_sleep(1); ticks++; } }

#define OWNERS                  4


static TX_THREAD       control_thread;
static TX_THREAD       waiter_thread;
static TX_THREAD       owner[OWNERS];
static TX_MUTEX        mutex[OWNERS];
static CHAR            *thread_stack[OWNERS + 1];
static volatile UINT   owner_spin;
static UINT            owner_count = OWNERS;
static ULONG           waiter_wait = TX_WAIT_FOREVER;
static volatile UINT   waiter_status;
static TX_THREAD       extra_waiter;
static TX_MUTEX        extra_mutex;
static CHAR            *extra_stack;
static UINT            take_extra;
static UINT            release_first;
static volatile UINT   medium_ran;



/* Define thread prototypes.  */

static void    control_thread_entry(ULONG thread_input);
static void    medium_entry(ULONG input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_inherit_chain_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;
UINT    i;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    status =  tx_thread_create(&control_thread, "control thread", control_thread_entry, 0,
            pointer, TEST_STACK_SIZE_PRINTF,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* The remaining threads are created and deleted by each scenario.  */
    for (i = 0; i <= OWNERS; i++)
    {
        thread_stack[i] =  pointer;
        pointer =          pointer + TEST_STACK_SIZE_PRINTF;
    }

    extra_stack = pointer;
    status += tx_mutex_create(&extra_mutex, "extra", TX_INHERIT);

    for (i = 0; i < OWNERS; i++)
    {
        status +=  tx_mutex_create(&mutex[i], "chain mutex", TX_INHERIT);
    }

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Inherit Chain Test.................................... ERROR #1\n");
        test_control_return(1);
    }
}


/* The waiter blocks on mutex 0.  */

static void    waiter_entry(ULONG thread_input)
{

    waiter_status = tx_mutex_get(&mutex[0], waiter_wait);
    tx_thread_suspend(tx_thread_identify());
}


/* Owner i takes mutex i and then waits on mutex i + 1.  The last owner parks.  */

static void    chain_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&mutex[thread_input], TX_WAIT_FOREVER);
    if ((thread_input == 0) && take_extra)
    {
        tx_mutex_get(&extra_mutex, TX_WAIT_FOREVER);
    }
    if (thread_input + 1 < owner_count)
    {
        tx_mutex_get(&mutex[thread_input + 1], TX_WAIT_FOREVER);
    }
    tx_thread_suspend(tx_thread_identify());
    if ((thread_input == 0) && release_first)
    {
        tx_mutex_put(&mutex[0]);
    }
    tx_thread_suspend(tx_thread_identify());
}


/* Takes the mutex and stays ready until told to stop.  */

static void    spinning_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&mutex[thread_input], TX_WAIT_FOREVER);
    while (owner_spin)
    {
    }
    tx_thread_suspend(tx_thread_identify());
}


/* Owner i takes mutex i, parks, and once resumed waits on the other mutex.  */

static void    cycle_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&mutex[thread_input], TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
    tx_mutex_get(&mutex[1 - thread_input], TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
}


static void    check(UINT condition, UINT error)
{

    if (condition == TX_FALSE)
    {

        printf("ERROR #%u\n", error);
        test_control_return(1);
    }
}


static void    start_thread(TX_THREAD *thread_ptr, VOID (*entry)(ULONG), ULONG input, UINT stack, UINT priority)
{

    check(tx_thread_create(thread_ptr, "test thread", entry, input, thread_stack[stack], TEST_STACK_SIZE_PRINTF,
            priority, priority, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS, 2);
}


/* Start the waiter on mutex 0, which owner 0 holds.  */

static void    start_waiter(void)
{

    waiter_status = 0xFFFF;
    start_thread(&waiter_thread, waiter_entry, 0, OWNERS, 5);
    WAIT_UNTIL(waiter_thread.tx_thread_state == TX_MUTEX_SUSP)
}


/* Delete the waiter and the first count owners.  */

static void    stop_threads(UINT count)
{

UINT    i;


    tx_thread_terminate(&waiter_thread);
    tx_thread_delete(&waiter_thread);
    for (i = count; i > 0; i--)
    {
        tx_thread_terminate(&owner[i - 1]);
        tx_thread_delete(&owner[i - 1]);
    }
}


/* Determine if owner i inherits the waiter's priority, which owner 0 does directly and
   owner i only if the chain reaches i mutex edges past owner 0.  */

static UINT    inherits(UINT i)
{

#ifdef TX_MUTEX_INHERIT_CHAIN_MAX
    return(i <= (UINT) TX_MUTEX_INHERIT_CHAIN_MAX);
#else
    return(i == 0);
#endif
}


/* Owners at 20, 19, 18 and 17, each but the last waiting on the next one's mutex.
   Each waits on a higher-priority owner, so nothing is inherited until the waiter
   blocks.  */

static void    chain_of_suspended_owners(UINT error)
{

UINT    i;


    /* Start from the end of the chain, so each mutex is owned before it is waited on.  */
    for (i = OWNERS; i > 0; i--)
    {
        start_thread(&owner[i - 1], chain_owner_entry, i - 1, i - 1, 20 - (i - 1));
        WAIT_UNTIL((mutex[i - 1].tx_mutex_owner == &owner[i - 1]) &&
                   (owner[i - 1].tx_thread_state == ((i < OWNERS) ? TX_MUTEX_SUSP : TX_SUSPENDED)))
    }
    check(owner[OWNERS - 1].tx_thread_priority == 20 - (OWNERS - 1), error);

    start_waiter();
    for (i = 0; i < OWNERS; i++)
    {
        check(owner[i].tx_thread_priority == (inherits(i) ? 5 : 20 - i), error + 1 + i);
    }

    check(tx_thread_wait_abort(&waiter_thread) == TX_SUCCESS, error + 5);
    for (i = 0; i < OWNERS; i++)
    {
        /* Beyond the bound neither the boost nor its withdrawal is applied. */
        check(owner[i].tx_thread_priority == 20 - i, error + 6 + i);
    }
    stop_threads(OWNERS);
}


/* Owner 1 at 20 holds mutex 1 and stays ready.  Owner 0 at 15 holds mutex 0 and waits
   on mutex 1, so owner 1 runs at 15 until the waiter blocks.  */

static void    chain_to_ready_owner(UINT error)
{

    owner_spin =  1;
    start_thread(&owner[1], spinning_owner_entry, 1, 1, 30);
#if defined(TX_THREAD_SMP_MAX_CORES) && TX_THREAD_SMP_MAX_CORES > 1
    /* Exercise inheritance and restoration of an owner executing on another core. */
    check(tx_thread_smp_core_exclude(&owner[1], 1) == TX_SUCCESS, error + 14);
#endif
    WAIT_UNTIL(mutex[1].tx_mutex_owner == &owner[1])
    start_thread(&owner[0], chain_owner_entry, 0, 0, 20);
    WAIT_UNTIL(owner[0].tx_thread_state == TX_MUTEX_SUSP)
    check(owner[1].tx_thread_state == TX_READY, error);
    check(owner[1].tx_thread_priority == 20, error + 1);

    start_waiter();
    check(owner[0].tx_thread_priority == 5, error + 2);
    check(owner[1].tx_thread_priority == (inherits(1) ? 5 : 20), error + 3);

    check(tx_thread_wait_abort(&waiter_thread) == TX_SUCCESS, error + 4);
    printf("\nAfter H abort: L1=%u L2=%u (expected 20,20)\n",
           owner[0].tx_thread_priority, owner[1].tx_thread_priority);
    check(owner[0].tx_thread_priority == 20, error + 5);
    check(owner[1].tx_thread_priority == 20, error + 6);

    /* Crossing an inherited priority from a stronger user priority must
       also restore a ready (and, on SMP, remotely executing) owner. */
    {
        UINT old;
        check(tx_thread_priority_change(&owner[1], 15, &old) == TX_SUCCESS, error + 15);
        check(owner[1].tx_thread_priority == 15, error + 16);
        check(tx_thread_priority_change(&owner[1], 30, &old) == TX_SUCCESS, error + 17);
        check(owner[1].tx_thread_priority == 20, error + 18);
    }

    /* A runnable thread at 10 must now outrank the ready owner at 20. */
    {
        UINT old;
        medium_ran = 0;
        check(tx_thread_create(&extra_waiter, "medium", medium_entry, 0,
              extra_stack, TEST_STACK_SIZE_PRINTF, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS, error + 10);
        check(tx_thread_priority_change(&control_thread, 12, &old) == TX_SUCCESS, error + 11);
#ifdef TX_THREAD_SMP_MAX_CORES
        WAIT_UNTIL(medium_ran)
#endif
        check(medium_ran == 1, error + 12);
        check(tx_thread_priority_change(&control_thread, 1, &old) == TX_SUCCESS, error + 13);
        tx_thread_terminate(&extra_waiter);
        tx_thread_delete(&extra_waiter);
    }

    /* Removing L1's wait must also remove its remaining contribution. */
    check(tx_thread_wait_abort(&owner[0]) == TX_SUCCESS, error + 7);
    check(owner[1].tx_thread_priority == 30, error + 8);
    check(mutex[1].tx_mutex_suspended_count == 0, error + 9);
    owner_spin =  0;
    WAIT_UNTIL(owner[1].tx_thread_state == TX_SUSPENDED)
    stop_threads(2);
}


/* Owners at 15 and 20 each hold one mutex and wait on the other's, a deadlock.  The
   walk started by the waiter must end.  */

static void    chain_with_cycle(UINT error)
{

    start_thread(&owner[0], cycle_owner_entry, 0, 0, 15);
    start_thread(&owner[1], cycle_owner_entry, 1, 1, 20);
    WAIT_UNTIL((mutex[0].tx_mutex_owner == &owner[0]) && (owner[0].tx_thread_state == TX_SUSPENDED) &&
               (mutex[1].tx_mutex_owner == &owner[1]) && (owner[1].tx_thread_state == TX_SUSPENDED))
    tx_thread_resume(&owner[0]);
    tx_thread_resume(&owner[1]);
    WAIT_UNTIL((owner[0].tx_thread_state == TX_MUTEX_SUSP) && (owner[1].tx_thread_state == TX_MUTEX_SUSP))

    start_waiter();
    check(owner[0].tx_thread_priority == 5, error);
    check(owner[1].tx_thread_priority == (inherits(1) ? 5 : 15), error + 1);

#if defined(TX_MUTEX_INHERIT_CHAIN_MAX) && TX_MUTEX_INHERIT_CHAIN_MAX >= 1
    check(tx_thread_wait_abort(&waiter_thread) == TX_SUCCESS, error + 2);
    check(owner[0].tx_thread_priority == 15, error + 3);
    check(owner[1].tx_thread_priority == 15, error + 4);
    check(owner[0].tx_thread_state == TX_MUTEX_SUSP, error + 5);
    check(owner[1].tx_thread_state == TX_MUTEX_SUSP, error + 6);
    check(mutex[0].tx_mutex_suspended_count == 1, error + 7);
    check(mutex[1].tx_mutex_suspended_count == 1, error + 8);
    check(owner[0].tx_thread_suspended_next == &owner[0], error + 13);
    check(owner[1].tx_thread_suspended_next == &owner[1], error + 14);
    /* A base-priority decrease inside the cycle must discard its own old boost. */
    {
        UINT old;
        check(tx_thread_priority_change(&owner[0], 25, &old) == TX_SUCCESS, error + 9);
        check(old == 15, error + 10);
        check(owner[0].tx_thread_priority == 20, error + 11);
        check(owner[1].tx_thread_priority == 20, error + 12);
    }
#endif
    stop_threads(2);
}



static void medium_entry(ULONG input)
{
    medium_ran = 1;
    tx_thread_suspend(tx_thread_identify());
}

/* A second waiter can depend on mutex A or on another mutex owned by L1. */
static void extra_waiter_entry(ULONG input)
{
    tx_mutex_get(input ? &extra_mutex : &mutex[0], TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
}

static void start_owners(UINT count)
{
UINT i;

    owner_count = count;
    for (i = count; i > 0; i--)
    {
        start_thread(&owner[i - 1], chain_owner_entry, i - 1, i - 1, 20 + 3 * (i - 1));
        WAIT_UNTIL(owner[i - 1].tx_thread_state == ((i == count) ? TX_SUSPENDED : TX_MUTEX_SUSP))
        check(mutex[i - 1].tx_mutex_owner == &owner[i - 1], 100);
        check(owner[i - 1].tx_thread_state == ((i == count) ? TX_SUSPENDED : TX_MUTEX_SUSP), 101);
    }
}

/* T1-T4: timeout, abort and termination, with two and three owners. */
static void restore_chain(UINT count, UINT how, UINT error)
{
UINT i;

    start_owners(count);
    waiter_wait = (how == 0) ? 50 : TX_WAIT_FOREVER;
    start_waiter();
    check(waiter_thread.tx_thread_state == TX_MUTEX_SUSP, error);
    for (i = 0; i < count; i++)
    {
        check(owner[i].tx_thread_priority == (inherits(i) ? 5 : 20), error + 1 + i);
    }
    if (how == 1)
    {
        check(tx_thread_wait_abort(&waiter_thread) == TX_SUCCESS, error + 5);
    }
    else if (how == 2)
    {
        check(tx_thread_terminate(&waiter_thread) == TX_SUCCESS, error + 6);
        check(waiter_thread.tx_thread_state == TX_TERMINATED, error + 7);
    }
    if (how != 2)
    {
        WAIT_UNTIL(waiter_status != 0xFFFF)
        check(waiter_status == ((how == 0) ? TX_NOT_AVAILABLE : TX_WAIT_ABORTED), error + 8);
    }
    check(mutex[0].tx_mutex_suspended_count == 0, error + 9);
    check(mutex[0].tx_mutex_suspension_list == TX_NULL, error + 10);
    check(mutex[1].tx_mutex_suspension_list == &owner[0], error + 14);
    check(owner[0].tx_thread_suspended_next == &owner[0], error + 15);
    check(owner[0].tx_thread_suspended_previous == &owner[0], error + 16);
    check(owner[0].tx_thread_owned_mutex_count == 1, error + 17);
    for (i = 0; i < count; i++)
    {
        check(owner[i].tx_thread_priority == 20, error + 11 + i);
    }
    stop_threads(count);
    owner_count = OWNERS;
    waiter_wait = TX_WAIT_FOREVER;
}

/* T5-T7: retain other waiters, including another owned mutex, and handle
   both directions of API priority change while the dependency is active. */
static void remaining_contributions(UINT another_mutex, UINT error)
{
UINT old;

    take_extra = another_mutex;
    start_owners(2);
    check(tx_thread_create(&extra_waiter, "extra waiter", extra_waiter_entry, another_mutex,
          extra_stack, TEST_STACK_SIZE_PRINTF, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS, error);
    WAIT_UNTIL(extra_waiter.tx_thread_state == TX_MUTEX_SUSP)
    check(extra_waiter.tx_thread_state == TX_MUTEX_SUSP, error + 1);
    start_waiter();
    check(owner[0].tx_thread_priority == 5, error + 2);
    check(owner[1].tx_thread_priority == (inherits(1) ? 5 : 20), error + 3);
    check(tx_thread_priority_change(&waiter_thread, 12, &old) == TX_SUCCESS, error + 4);
    check(old == 5, error + 5);
    check(owner[0].tx_thread_priority == 10, error + 6);
    check(owner[1].tx_thread_priority == (inherits(1) ? 10 : 20), error + 7);
    check(tx_thread_priority_change(&waiter_thread, 4, &old) == TX_SUCCESS, error + 8);
    check(old == 12, error + 9);
    check(owner[0].tx_thread_priority == 4, error + 10);
    check(owner[1].tx_thread_priority == (inherits(1) ? 4 : 20), error + 11);
    check(tx_thread_wait_abort(&waiter_thread) == TX_SUCCESS, error + 12);
    check(owner[0].tx_thread_priority == 10, error + 13);
    check(owner[1].tx_thread_priority == (inherits(1) ? 10 : 20), error + 14);
    check(tx_thread_wait_abort(&extra_waiter) == TX_SUCCESS, error + 15);
    check(owner[0].tx_thread_priority == 20, error + 16);
    check(owner[1].tx_thread_priority == 20, error + 17);
    tx_thread_terminate(&extra_waiter);
    tx_thread_delete(&extra_waiter);
    stop_threads(2);
    take_extra = 0;
    owner_count = OWNERS;
}

/* T8: release A after removing L1's wait on B. H becomes A's owner;
   both the former owner and B's owner must retain only valid donations. */
static void ownership_transfer(UINT error)
{
    start_owners(2);
    start_waiter();
    check(tx_thread_create(&extra_waiter, "transfer waiter", extra_waiter_entry, 0,
          extra_stack, TEST_STACK_SIZE_PRINTF, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS, error + 7);
    start_thread(&owner[3], extra_waiter_entry, 0, 3, 15);
    WAIT_UNTIL(mutex[0].tx_mutex_suspended_count == 3)
    check(mutex[0].tx_mutex_suspended_count == 3, error + 8);
    release_first = 1;
    check(tx_thread_wait_abort(&owner[0]) == TX_SUCCESS, error);
    check(owner[1].tx_thread_priority == 23, error + 1);
    WAIT_UNTIL(owner[0].tx_thread_state == TX_SUSPENDED)
    check(owner[0].tx_thread_state == TX_SUSPENDED, error + 2);
    check(tx_thread_resume(&owner[0]) == TX_SUCCESS, error + 3);
    WAIT_UNTIL(mutex[0].tx_mutex_owner == &waiter_thread)
    check(mutex[0].tx_mutex_owner == &waiter_thread, error + 4);
    check(owner[0].tx_thread_priority == 20, error + 5);
    check(owner[0].tx_thread_inherit_priority == TX_MAX_PRIORITIES, error + 6);
    /* The transferred mutex must contribute its remaining waiters to H's
       inheritance, including when H's own priority subsequently drops. */
    {
        UINT old;
        check(waiter_thread.tx_thread_inherit_priority == 10, error + 9);
        check(tx_thread_priority_change(&waiter_thread, 18, &old) == TX_SUCCESS, error + 10);
        check(waiter_thread.tx_thread_priority == 10, error + 11);
        check(tx_thread_terminate(&extra_waiter) == TX_SUCCESS, error + 12);
        check(waiter_thread.tx_thread_priority == 15, error + 13);
        check(tx_thread_terminate(&owner[3]) == TX_SUCCESS, error + 14);
        check(waiter_thread.tx_thread_priority == 18, error + 15);
    }
    tx_thread_delete(&extra_waiter);
    tx_thread_delete(&owner[3]);
    stop_threads(2);
    release_first = 0;
    owner_count = OWNERS;
}

/* Kernel deletion releases an owner's mutex while that owner still waits on B. */
static void delete_while_waiting(UINT error)
{
    start_owners(2);
    start_waiter();
    check(tx_mutex_delete(&mutex[0]) == TX_SUCCESS, error);
    check(owner[0].tx_thread_state == TX_MUTEX_SUSP, error + 1);
    check(owner[0].tx_thread_priority == 20, error + 2);
    check(owner[1].tx_thread_priority == 20, error + 3);
    WAIT_UNTIL(waiter_status != 0xFFFF)
    check(waiter_status == TX_DELETED, error + 4);
    check(mutex[1].tx_mutex_suspended_count == 1, error + 5);
    check(mutex[1].tx_mutex_suspension_list == &owner[0], error + 6);
    stop_threads(2);
    check(tx_mutex_create(&mutex[0], "inherit", TX_INHERIT) == TX_SUCCESS, error + 7);
    owner_count = OWNERS;
}

/* A non-inheriting edge ends the path and preserves its FIFO list. */
static void non_inheriting_edge(UINT error)
{
    check(tx_mutex_delete(&mutex[1]) == TX_SUCCESS, error);
    check(tx_mutex_create(&mutex[1], "no inherit", TX_NO_INHERIT) == TX_SUCCESS, error + 1);
    start_owners(2);
    start_waiter();
    check(owner[0].tx_thread_priority == 5, error + 2);
    check(owner[1].tx_thread_priority == 23, error + 3);
    check(tx_thread_wait_abort(&waiter_thread) == TX_SUCCESS, error + 4);
    check(owner[0].tx_thread_priority == 20, error + 5);
    check(owner[1].tx_thread_priority == 23, error + 6);
    stop_threads(2);
    check(tx_mutex_delete(&mutex[1]) == TX_SUCCESS, error + 7);
    check(tx_mutex_create(&mutex[1], "inherit", TX_INHERIT) == TX_SUCCESS, error + 8);
    owner_count = OWNERS;
}

static void    control_thread_entry(ULONG thread_input)
{

    /* Inform user.  */
    printf("Running Mutex Inherit Chain Test.................................... ");

    chain_to_ready_owner(20);
    chain_of_suspended_owners(10);
    chain_with_cycle(50);
    restore_chain(2, 0, 110);
    restore_chain(2, 1, 130);
    restore_chain(2, 2, 150);
#if defined(TX_MUTEX_INHERIT_CHAIN_MAX) && TX_MUTEX_INHERIT_CHAIN_MAX >= 2
    restore_chain(3, 0, 170);
    restore_chain(3, 1, 190);
    restore_chain(3, 2, 210);
#endif
    remaining_contributions(0, 230);
    remaining_contributions(1, 250);
    ownership_transfer(270);
    non_inheriting_edge(310);
    delete_while_waiting(330);

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}
