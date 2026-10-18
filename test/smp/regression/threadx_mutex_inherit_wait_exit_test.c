/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* This test checks that a priority-inheritance mutex owner gives up an inherited
   priority when the thread it inherited it from stops waiting without obtaining the
   mutex: through a wait timeout, tx_thread_wait_abort or tx_thread_terminate.  The
   owner must drop to the highest priority still waiting on any mutex it owns, or to
   its own priority if none is.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Wait a tick at a time, for at most 200 ticks, until a condition holds.  A thread the
   control thread starts can take more than a tick to get there on an SMP host.  */

#define WAIT_UNTIL(condition)   { UINT ticks = 0; while ((!(condition)) && (ticks < 200)) { tx_thread_sleep(1); ticks++; } }

#define WAITERS                 4


static TX_THREAD       control_thread;
static TX_THREAD       owner_thread;
static TX_THREAD       waiter[WAITERS];
static CHAR            *thread_stack[WAITERS + 1];
static TX_MUTEX        mutex_0;
static TX_MUTEX        mutex_1;
static ULONG           waiter_wait_option;
static volatile UINT   waiter_status;
static volatile UINT   run_sequence;


/* Define thread prototypes.  */

static void    control_thread_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_inherit_wait_exit_application_define(void *first_unused_memory)
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
    for (i = 0; i <= WAITERS; i++)
    {
        thread_stack[i] =  pointer;
        pointer =          pointer + TEST_STACK_SIZE_PRINTF;
    }

    status +=  tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT);
    status +=  tx_mutex_create(&mutex_1, "mutex 1", TX_INHERIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Inherit Wait Exit Test................................ ERROR #1\n");
        test_control_return(1);
    }
}


/* The owner takes both mutexes and parks.  */

static void    owner_thread_entry(ULONG thread_input)
{

    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
    tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
    tx_thread_suspend(&owner_thread);
}


/* A waiter blocks on mutex 0, or on mutex 1 if its input is 1.  */

static void    waiter_entry(ULONG thread_input)
{

    waiter_status =  tx_mutex_get((thread_input == 0) ? &mutex_0 : &mutex_1, waiter_wait_option);
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


static void    start_owner(void)
{

    start_thread(&owner_thread, owner_thread_entry, 0, WAITERS, 15);
    WAIT_UNTIL((mutex_1.tx_mutex_owner == &owner_thread) && (owner_thread.tx_thread_state == TX_SUSPENDED))
}


/* The owner at 15 holds mutex 0, and waiter 0 at 5 blocks on it and then leaves
   without it.  how: 0 is a wait timeout, 1 is tx_thread_wait_abort, 2 is
   tx_thread_terminate.  */

static void    single_waiter_leaves(UINT how, UINT error)
{

    start_owner();
    waiter_wait_option =  (how == 0) ? 50 : TX_WAIT_FOREVER;
    waiter_status =       0xFFFF;
    start_thread(&waiter[0], waiter_entry, 0, 0, 5);
    WAIT_UNTIL(waiter[0].tx_thread_state == TX_MUTEX_SUSP)
    check(owner_thread.tx_thread_priority == 5, error);

    if (how == 1)
    {
        tx_thread_wait_abort(&waiter[0]);
    }
    else if (how == 2)
    {
        tx_thread_terminate(&waiter[0]);
    }

    if (how != 2)
    {
        WAIT_UNTIL(waiter_status != 0xFFFF)
        check(waiter_status == ((how == 0) ? TX_NOT_AVAILABLE : TX_WAIT_ABORTED), error + 1);
    }

    /* Nothing waits on the mutex any more, so the owner is back at its own priority.  */
    check(mutex_0.tx_mutex_highest_priority_waiting == TX_MAX_PRIORITIES, error + 2);
    check(owner_thread.tx_thread_priority == 15, error + 3);

    tx_thread_terminate(&waiter[0]);
    tx_thread_delete(&waiter[0]);
    tx_thread_terminate(&owner_thread);
    tx_thread_delete(&owner_thread);
}


/* The owner at 15 holds both mutexes.  Waiters at 5, 8 and 20 block on mutex 0 and
   one at 12 on mutex 1.  As they are aborted in turn, the owner keeps the highest
   priority still waiting on either mutex.  */

static void    waiters_leave_in_turn(UINT error)
{

static const UINT   priority[WAITERS] =       {5, 8, 20, 12};
static const UINT   abort_order[WAITERS] =    {2, 0, 1, 3};
static const UINT   owner_priority[WAITERS] = {5, 8, 12, 15};
UINT                i;


    start_owner();
    waiter_wait_option =  TX_WAIT_FOREVER;
    for (i = 0; i < WAITERS; i++)
    {
        start_thread(&waiter[i], waiter_entry, (i == 3) ? 1 : 0, i, priority[i]);
    }
    WAIT_UNTIL((mutex_0.tx_mutex_suspended_count == 3) && (mutex_1.tx_mutex_suspended_count == 1))
    check(owner_thread.tx_thread_priority == 5, error);

    for (i = 0; i < WAITERS; i++)
    {
        tx_thread_wait_abort(&waiter[abort_order[i]]);
        check(owner_thread.tx_thread_priority == owner_priority[i], error + 1 + i);
    }

    for (i = 0; i < WAITERS; i++)
    {
        tx_thread_terminate(&waiter[i]);
        tx_thread_delete(&waiter[i]);
    }
    tx_thread_terminate(&owner_thread);
    tx_thread_delete(&owner_thread);
}


/* These threads share core 0 in the SMP regression configuration.  The waiter
   must run first after its wait is aborted, followed by the medium-priority
   thread, while the owner remains ready and still holds the mutex.  */

static void    ready_waiter_entry(ULONG thread_input)
{

    check(tx_mutex_get(&mutex_0, TX_WAIT_FOREVER) == TX_WAIT_ABORTED, 55);
    check(owner_thread.tx_thread_priority == 15, 56);
    check(owner_thread.tx_thread_state == TX_READY, 57);
    check(run_sequence == 0, 58);
    run_sequence =  1;
    tx_thread_suspend(tx_thread_identify());
}


static void    medium_thread_entry(ULONG thread_input)
{

    check(owner_thread.tx_thread_priority == 15, 59);
    check(owner_thread.tx_thread_state == TX_READY, 60);
    check(mutex_0.tx_mutex_owner == &owner_thread, 61);
    check(run_sequence == 1, 62);
    run_sequence =  2;
    tx_thread_suspend(tx_thread_identify());
}


static void    ready_owner_entry(ULONG thread_input)
{

    check(tx_mutex_get(&mutex_0, TX_WAIT_FOREVER) == TX_SUCCESS, 50);

    /* The waiter preempts the owner, blocks, and raises the owner from 15 to 5.
       A thread at 10 then becomes ready but cannot preempt the owner yet.  */
    start_thread(&waiter[0], ready_waiter_entry, 0, 0, 5);
    check(waiter[0].tx_thread_state == TX_MUTEX_SUSP, 51);
    check(owner_thread.tx_thread_priority == 5, 52);
    start_thread(&waiter[1], medium_thread_entry, 0, 1, 10);
    check(waiter[1].tx_thread_state == TX_READY, 53);
    check(run_sequence == 0, 54);

    /* Restoring the owner's priority must let both threads run before this
       call returns, without releasing the mutex or suspending the owner.  */
    check(tx_thread_wait_abort(&waiter[0]) == TX_SUCCESS, 63);
    check(run_sequence == 2, 64);
    check(owner_thread.tx_thread_priority == 15, 65);
    check(mutex_0.tx_mutex_highest_priority_waiting == TX_MAX_PRIORITIES, 66);
    run_sequence =  3;
    tx_thread_resume(&control_thread);
}


static void    ready_owner_waiter_aborted(void)
{

    run_sequence =  0;
    start_thread(&owner_thread, ready_owner_entry, 0, WAITERS, 15);
    tx_thread_suspend(&control_thread);
    check(run_sequence == 3, 67);

    tx_thread_terminate(&waiter[0]);
    tx_thread_delete(&waiter[0]);
    tx_thread_terminate(&waiter[1]);
    tx_thread_delete(&waiter[1]);
    tx_thread_terminate(&owner_thread);
    tx_thread_delete(&owner_thread);
}


static void    control_thread_entry(ULONG thread_input)
{

    /* Inform user.  */
    printf("Running Mutex Inherit Wait Exit Test................................ ");

    single_waiter_leaves(0, 10);
    single_waiter_leaves(1, 20);
    single_waiter_leaves(2, 30);
    waiters_leave_in_turn(40);
    ready_owner_waiter_aborted();

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}
