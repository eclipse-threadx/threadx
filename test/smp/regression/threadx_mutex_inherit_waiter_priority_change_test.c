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
   must lower the owner to the highest priority still waiting, or to its own.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Wait a tick at a time, for at most 200 ticks, until a condition holds.  A thread the
   control thread starts can take more than a tick to get there on an SMP host.  */

#define WAIT_UNTIL(condition)   { UINT ticks = 0; while ((!(condition)) && (ticks < 200)) { tx_thread_sleep(1); ticks++; } }

#define WAITERS                 2
#define STEPS                   4


static TX_THREAD       control_thread;
static TX_THREAD       owner_thread;
static TX_THREAD       waiter[WAITERS];
static CHAR            *thread_stack[WAITERS + 1];
static TX_MUTEX        mutex_0;


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
UINT    i;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    status =  tx_thread_create(&control_thread, "control thread", control_thread_entry, 0,
            pointer, TEST_STACK_SIZE_PRINTF,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* The remaining threads are created by the control thread.  */
    for (i = 0; i <= WAITERS; i++)
    {
        thread_stack[i] =  pointer;
        pointer =          pointer + TEST_STACK_SIZE_PRINTF;
    }

    status +=  tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Inherit Waiter Priority Change Test................... ERROR #1\n");
        test_control_return(1);
    }
}


/* The owner takes the mutex and parks, and the waiters block on it.  */

static void    mutex_thread_entry(ULONG thread_input)
{

    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
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


static void    start_thread(TX_THREAD *thread_ptr, UINT stack, UINT priority)
{

    check(tx_thread_create(thread_ptr, "test thread", mutex_thread_entry, 0, thread_stack[stack], TEST_STACK_SIZE_PRINTF,
            priority, priority, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS, 2);
}


/* The owner at 15 holds the mutex, with waiter 0 at 20 and waiter 1 at 8, so the owner
   runs at 8.  Each step changes the priority of one waiter, and the owner must follow
   the highest priority left waiting.  */

static void    control_thread_entry(ULONG thread_input)
{

static const UINT   step_waiter[STEPS] =          {0, 0, 1, 0};
static const UINT   step_priority[STEPS] =        {5, 12, 20, 20};
static const UINT   step_highest_waiting[STEPS] = {5, 8, 12, 20};
static const UINT   step_owner_priority[STEPS] =  {5, 8, 12, 15};
UINT                old_priority;
UINT                i;


    /* Inform user.  */
    printf("Running Mutex Inherit Waiter Priority Change Test................... ");

    start_thread(&owner_thread, WAITERS, 15);
    WAIT_UNTIL((mutex_0.tx_mutex_owner == &owner_thread) && (owner_thread.tx_thread_state == TX_SUSPENDED))
    start_thread(&waiter[0], 0, 20);
    start_thread(&waiter[1], 1, 8);
    WAIT_UNTIL(mutex_0.tx_mutex_suspended_count == WAITERS)
    check(owner_thread.tx_thread_priority == 8, 10);

    for (i = 0; i < STEPS; i++)
    {
        tx_thread_priority_change(&waiter[step_waiter[i]], step_priority[i], &old_priority);
        check(waiter[step_waiter[i]].tx_thread_state == TX_MUTEX_SUSP, 20 + i);
        check(mutex_0.tx_mutex_highest_priority_waiting == step_highest_waiting[i], 30 + i);
        check(owner_thread.tx_thread_priority == step_owner_priority[i], 40 + i);
    }

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}
