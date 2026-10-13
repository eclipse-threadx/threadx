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


/* Define thread prototypes.  */

static void    control_thread_entry(ULONG thread_input);


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

    tx_mutex_get(&mutex[0], TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
}


/* Owner i takes mutex i and then waits on mutex i + 1.  The last owner parks.  */

static void    chain_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&mutex[thread_input], TX_WAIT_FOREVER);
    if (thread_input + 1 < OWNERS)
    {
        tx_mutex_get(&mutex[thread_input + 1], TX_WAIT_FOREVER);
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
   owner i only if the chain reaches i - 1 mutexes past owner 0.  */

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

    stop_threads(OWNERS);
}


/* Owner 1 at 20 holds mutex 1 and stays ready.  Owner 0 at 15 holds mutex 0 and waits
   on mutex 1, so owner 1 runs at 15 until the waiter blocks.  */

static void    chain_to_ready_owner(UINT error)
{

    owner_spin =  1;
    start_thread(&owner[1], spinning_owner_entry, 1, 1, 20);
    WAIT_UNTIL(mutex[1].tx_mutex_owner == &owner[1])
    start_thread(&owner[0], chain_owner_entry, 0, 0, 15);
    WAIT_UNTIL(owner[0].tx_thread_state == TX_MUTEX_SUSP)
    check(owner[1].tx_thread_state == TX_READY, error);
    check(owner[1].tx_thread_priority == 15, error + 1);

    start_waiter();
    check(owner[0].tx_thread_priority == 5, error + 2);
    check(owner[1].tx_thread_priority == (inherits(1) ? 5 : 15), error + 3);

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

    stop_threads(2);
}


static void    control_thread_entry(ULONG thread_input)
{

    /* Inform user.  */
    printf("Running Mutex Inherit Chain Test.................................... ");

    chain_of_suspended_owners(10);
    chain_to_ready_owner(20);
    chain_with_cycle(30);

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}
