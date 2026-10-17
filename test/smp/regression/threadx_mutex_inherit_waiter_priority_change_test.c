/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* This test checks that changing the priority of a thread waiting on a
   priority-inheritance mutex is passed on to the mutex owner.  Raising a waiter above
   the owner must raise the owner, and lowering the waiter the owner inherited from
   must lower the owner to the highest priority still waiting on any priority
   inheritance mutex it holds, or to its own.  This is checked with the owner
   suspended and with the owner ready.  Changing the priority of a thread waiting on a
   mutex without priority inheritance must neither change the owner's priority nor
   the FIFO order in which the waiters obtain the mutex.  Two cases that scheduling on
   a single core cannot produce are set up by hand at the end.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_mutex.h"


/* Wait a tick at a time, for at most 200 ticks, until a condition holds.  A thread the
   control thread starts can take more than a tick to get there on an SMP host.  */

#define WAIT_UNTIL(condition)   { UINT ticks = 0; while ((!(condition)) && (ticks < 200)) { tx_thread_sleep(1); ticks++; } }

#define INHERIT_WAITERS         3
#define INHERIT_STEPS           6
#define FIFO_WAITERS            3


static TX_THREAD       control_thread;
static TX_THREAD       inherit_owner;
static TX_THREAD       inherit_waiter[INHERIT_WAITERS];
static TX_THREAD       fifo_owner;
static TX_THREAD       fifo_waiter[FIFO_WAITERS];
static TX_THREAD       ready_owner;
static TX_THREAD       ready_waiter;
static CHAR            *stack_pointer;

/* The owner of the first part holds two priority inheritance mutexes and one without
   priority inheritance, and the waiters of that part wait on the first two.  */
static TX_MUTEX        inherit_mutex[2];
static TX_MUTEX        plain_mutex;
static TX_MUTEX        fifo_mutex;
static TX_MUTEX        ready_mutex;

static volatile UINT   fifo_order[FIFO_WAITERS];
static volatile UINT   fifo_count;
static volatile UINT   ready_owner_release;
static volatile UINT   ready_waiter_obtained;


/* Define thread prototypes.  */

static void    control_thread_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_inherit_waiter_priority_change_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    status =  tx_thread_create(&control_thread, "control thread", control_thread_entry, 0,
            pointer, TEST_STACK_SIZE_PRINTF,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* The remaining threads are created by the control thread.  */
    stack_pointer =  pointer;

    status +=  tx_mutex_create(&inherit_mutex[0], "inherit mutex 0", TX_INHERIT);
    status +=  tx_mutex_create(&inherit_mutex[1], "inherit mutex 1", TX_INHERIT);
    status +=  tx_mutex_create(&plain_mutex, "plain mutex", TX_NO_INHERIT);
    status +=  tx_mutex_create(&fifo_mutex, "fifo mutex", TX_NO_INHERIT);
    status +=  tx_mutex_create(&ready_mutex, "ready mutex", TX_INHERIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Inherit Waiter Priority Change Test................... ERROR #1\n");
        test_control_return(1);
    }
}


/* The owner of the first part takes its three mutexes and parks.  */

static void    inherit_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&inherit_mutex[0], TX_WAIT_FOREVER);
    tx_mutex_get(&inherit_mutex[1], TX_WAIT_FOREVER);
    tx_mutex_get(&plain_mutex, TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
}


/* A waiter of the first part blocks on the mutex its input selects.  */

static void    inherit_waiter_entry(ULONG thread_input)
{

    tx_mutex_get(&inherit_mutex[thread_input], TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
}


/* The owner of the FIFO part takes the mutex, parks, and releases the mutex when it
   is resumed.  */

static void    fifo_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&fifo_mutex, TX_WAIT_FOREVER);
    tx_thread_suspend(tx_thread_identify());
    tx_mutex_put(&fifo_mutex);
    tx_thread_suspend(tx_thread_identify());
}


/* A waiter of the FIFO part records its input when it obtains the mutex, and passes
   the mutex on.  */

static void    fifo_waiter_entry(ULONG thread_input)
{

    tx_mutex_get(&fifo_mutex, TX_WAIT_FOREVER);
    fifo_order[fifo_count] =  (UINT) thread_input;
    fifo_count++;
    tx_mutex_put(&fifo_mutex);
    tx_thread_suspend(tx_thread_identify());
}


/* The owner of the ready part takes the mutex and stays ready until it is told to
   release the mutex.  */

static void    ready_owner_entry(ULONG thread_input)
{

    tx_mutex_get(&ready_mutex, TX_WAIT_FOREVER);
    while (ready_owner_release == TX_FALSE)
    {

        /* Spin, so the owner is preempted rather than suspended.  */
    }
    tx_mutex_put(&ready_mutex);
    tx_thread_suspend(tx_thread_identify());
}


/* The waiter of the ready part records that it obtained the mutex.  */

static void    ready_waiter_entry(ULONG thread_input)
{

    tx_mutex_get(&ready_mutex, TX_WAIT_FOREVER);
    ready_waiter_obtained =  TX_TRUE;
    tx_mutex_put(&ready_mutex);
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


static void    start_thread(TX_THREAD *thread_ptr, VOID (*entry)(ULONG), ULONG input, UINT priority)
{

    check(tx_thread_create(thread_ptr, "test thread", entry, input, stack_pointer, TEST_STACK_SIZE_PRINTF,
            priority, priority, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS, 2);
    stack_pointer =  stack_pointer + TEST_STACK_SIZE_PRINTF;
}


/* The owner at 15 holds both priority inheritance mutexes, with waiters 0 at 20 and 1
   at 8 on the first and waiter 2 at 13 on the second, so the owner runs at 8.  It also
   holds a mutex without priority inheritance, which must not count.  Each step changes
   the priority of one waiter, and the owner must follow the highest priority left
   waiting on either mutex.  */

static void    inherit_suspended_owner(void)
{

static const UINT   step_waiter[INHERIT_STEPS] =          {0, 0, 1, 0, 0, 2};
static const UINT   step_priority[INHERIT_STEPS] =        {5, 12, 20, 18, 16, 20};
static const UINT   step_highest_waiting[INHERIT_STEPS] = {5, 8, 12, 18, 16, 20};
static const UINT   step_owner_priority[INHERIT_STEPS] =  {5, 8, 12, 13, 13, 15};
static const UINT   waiter_mutex[INHERIT_WAITERS] =       {0, 0, 1};
UINT                old_priority;
UINT                i;


    start_thread(&inherit_owner, inherit_owner_entry, 0, 15);
    WAIT_UNTIL((plain_mutex.tx_mutex_owner == &inherit_owner) && (inherit_owner.tx_thread_state == TX_SUSPENDED))
    start_thread(&inherit_waiter[0], inherit_waiter_entry, 0, 20);
    start_thread(&inherit_waiter[1], inherit_waiter_entry, 0, 8);
    start_thread(&inherit_waiter[2], inherit_waiter_entry, 1, 13);
    WAIT_UNTIL((inherit_mutex[0].tx_mutex_suspended_count == 2) && (inherit_mutex[1].tx_mutex_suspended_count == 1))
    check(inherit_owner.tx_thread_priority == 8, 10);

    for (i = 0; i < INHERIT_STEPS; i++)
    {
        tx_thread_priority_change(&inherit_waiter[step_waiter[i]], step_priority[i], &old_priority);
        check(inherit_waiter[step_waiter[i]].tx_thread_state == TX_MUTEX_SUSP, 20 + i);
        check(inherit_mutex[waiter_mutex[step_waiter[i]]].tx_mutex_highest_priority_waiting == step_highest_waiting[i], 30 + i);
        check(inherit_owner.tx_thread_priority == step_owner_priority[i], 40 + i);
        check(inherit_owner.tx_thread_state == TX_SUSPENDED, 50 + i);
    }
}


/* The owner at 15 holds a mutex without priority inheritance, with waiters 0 at 20,
   1 at 18 and 2 at 16 blocked in that order.  Raising waiter 2 above the owner and
   the other waiters must leave the owner's priority and the order of the waiters as
   they are, so the waiters obtain the mutex in the order they blocked.  */

static void    no_inherit_fifo_order(void)
{

UINT                old_priority;
UINT                i;


    start_thread(&fifo_owner, fifo_owner_entry, 0, 15);
    WAIT_UNTIL((fifo_mutex.tx_mutex_owner == &fifo_owner) && (fifo_owner.tx_thread_state == TX_SUSPENDED))
    for (i = 0; i < FIFO_WAITERS; i++)
    {
        start_thread(&fifo_waiter[i], fifo_waiter_entry, i, 20 - (2 * i));
        WAIT_UNTIL(fifo_mutex.tx_mutex_suspended_count == (i + 1))
    }
    check(fifo_mutex.tx_mutex_suspended_count == FIFO_WAITERS, 60);

    tx_thread_priority_change(&fifo_waiter[2], 5, &old_priority);
    check(fifo_waiter[2].tx_thread_state == TX_MUTEX_SUSP, 61);
    check(fifo_mutex.tx_mutex_suspension_list == &fifo_waiter[0], 62);
    check(fifo_owner.tx_thread_priority == 15, 63);

    /* Let the owner release the mutex, and the waiters pass it on.  */
    tx_thread_resume(&fifo_owner);
    WAIT_UNTIL(fifo_count == FIFO_WAITERS)
    check(fifo_count == FIFO_WAITERS, 64);
    for (i = 0; i < FIFO_WAITERS; i++)
    {
        check(fifo_order[i] == i, 65 + i);
    }
}


/* The owner at 15 holds a priority inheritance mutex and stays ready, with the waiter
   at 10 blocked on it, so the owner is ready at 10.  Lowering and raising the waiter
   must move the ready owner to 15, to 8 and back to 15.  Raising the waiter while
   the caller runs below the new priority of the owner must let the owner run before
   the call returns.  */

static void    inherit_ready_owner(void)
{

UINT                old_priority;


    start_thread(&ready_owner, ready_owner_entry, 0, 15);
    WAIT_UNTIL(ready_mutex.tx_mutex_owner == &ready_owner)
    start_thread(&ready_waiter, ready_waiter_entry, 0, 10);
    WAIT_UNTIL(ready_mutex.tx_mutex_suspended_count == 1)
    check(ready_owner.tx_thread_state == TX_READY, 70);
    check(ready_owner.tx_thread_priority == 10, 71);

    /* Lower the waiter below the owner.  */
    tx_thread_priority_change(&ready_waiter, 20, &old_priority);
    check(ready_waiter.tx_thread_state == TX_MUTEX_SUSP, 72);
    check(ready_mutex.tx_mutex_highest_priority_waiting == 20, 73);
    check(ready_owner.tx_thread_state == TX_READY, 74);
    check(ready_owner.tx_thread_priority == 15, 75);

    /* Raise the waiter above the owner.  */
    tx_thread_priority_change(&ready_waiter, 8, &old_priority);
    check(ready_mutex.tx_mutex_highest_priority_waiting == 8, 76);
    check(ready_owner.tx_thread_state == TX_READY, 77);
    check(ready_owner.tx_thread_priority == 8, 78);

    /* Lower the waiter below the owner again.  */
    tx_thread_priority_change(&ready_waiter, 20, &old_priority);
    check(ready_owner.tx_thread_state == TX_READY, 79);
    check(ready_owner.tx_thread_priority == 15, 80);

    /* Run below the owner's next priority but above its own, so only inheritance
       lets the owner run, and raise the waiter again.  The owner releases the mutex
       as soon as it runs, and the waiter obtains it.  */
    tx_thread_priority_change(&control_thread, 12, &old_priority);
    ready_owner_release =  TX_TRUE;
    tx_thread_priority_change(&ready_waiter, 5, &old_priority);
    check(ready_waiter_obtained == TX_TRUE, 81);
    check(ready_owner.tx_thread_priority == 15, 82);
    tx_thread_priority_change(&control_thread, 1, &old_priority);
}


/* A thread in TX_MUTEX_SUSP without a cleanup routine has already been taken off the
   suspension list by tx_mutex_put or the mutex cleanup, which another core can be
   doing while the priority changes, and changing its priority must leave the mutex
   alone.  And the waiter an update is made for can leave the suspension list before
   the update runs, for example through tx_thread_wait_abort from an ISR, so the
   update must handle a mutex that nobody waits on any more.  */

static void    hand_set_up_cases(void)
{

UINT                old_priority;
UINT                saved_state;
VOID                *saved_control_block;


    /* Make waiter 0 of the FIFO part, parked without a cleanup routine, look like a
       thread being taken off the suspension list of the first mutex of the first
       part, and change its priority.  */
    saved_state =                                     fifo_waiter[0].tx_thread_state;
    saved_control_block =                             fifo_waiter[0].tx_thread_suspend_control_block;
    fifo_waiter[0].tx_thread_state =                  TX_MUTEX_SUSP;
    fifo_waiter[0].tx_thread_suspend_cleanup =        TX_NULL;
    fifo_waiter[0].tx_thread_suspend_control_block =  (VOID *) &inherit_mutex[0];
    tx_thread_priority_change(&fifo_waiter[0], 2, &old_priority);
    fifo_waiter[0].tx_thread_state =                  saved_state;
    fifo_waiter[0].tx_thread_suspend_control_block =  saved_control_block;
    check(fifo_waiter[0].tx_thread_priority == 2, 90);
    check(inherit_mutex[0].tx_mutex_highest_priority_waiting == 16, 91);
    check(inherit_owner.tx_thread_priority == 15, 92);

    /* Raise waiter 2 of the first part above the owner, so the owner inherits its
       priority, then take the waiter off the suspension list of the second mutex and
       update that mutex.  The owner must return to its own priority, since the
       highest priority still waiting on its mutexes is 16.  */
    tx_thread_priority_change(&inherit_waiter[2], 10, &old_priority);
    check(inherit_owner.tx_thread_priority == 10, 93);
    tx_thread_wait_abort(&inherit_waiter[2]);
    _tx_mutex_inherit_priority_update(&inherit_mutex[1]);
    check(inherit_mutex[1].tx_mutex_suspension_list == TX_NULL, 94);
    check(inherit_mutex[1].tx_mutex_highest_priority_waiting == ((UINT) TX_MAX_PRIORITIES), 95);
    check(inherit_owner.tx_thread_priority == 15, 96);
}


static void    control_thread_entry(ULONG thread_input)
{


    /* Inform user.  */
    printf("Running Mutex Inherit Waiter Priority Change Test................... ");

    inherit_suspended_owner();
    no_inherit_fifo_order();
    inherit_ready_owner();
    hand_set_up_cases();

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}
