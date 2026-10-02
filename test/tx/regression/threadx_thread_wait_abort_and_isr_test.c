/***************************************************************************/
/* Copyright (c) 2024 Microsoft Corporation                                */
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/

/* This test is designed to test for simultaneous thread suspension lifting AND thread wait abort calls.  */

#include   <stdio.h>
#include   <time.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;


static unsigned long   semaphore_put_counter =  0;
static unsigned long   condition_count  =  0;
extern UINT            _tx_timer_system_clock;

static TX_SEMAPHORE    semaphore_0;


#ifdef __linux__

/* The window this test waits for is a few instructions wide, and the interrupt
   that samples it is the same one that wakes the thread being sampled, so the
   two run in lockstep: the tick wakes thread 0, thread 0 does a fixed amount of
   work and suspends, and the next tick arrives a fixed interval later with
   thread 0 in the same place every time. That is the resonance the handler below
   describes, and perturbing the handler's duration only shifts the phase rather
   than decorrelating it.

   The simulator's timer thread waits on _tx_linux_timer_semaphore with a
   one-tick deadline and delivers an interrupt early if the semaphore is posted,
   which the port itself relies on elsewhere. A plain POSIX thread posting it
   therefore injects interrupts at moments unrelated to the tick grid, which
   samples thread 0 at arbitrary points in its cycle instead of the same one.

   It posts only when nothing is outstanding, so interrupts can never be queued
   faster than they are serviced and the injector cannot starve the system.  */

#include   <pthread.h>
#include   <semaphore.h>
#include   <unistd.h>

extern sem_t           _tx_linux_timer_semaphore;

static volatile int    injector_stop =  0;
static pthread_t       injector_thread;

#define INJECTOR_INTERVAL_USEC  100


static void  *interrupt_injector(void *p)
{

int     pending;


    (void) p;

    while (injector_stop == 0)
    {

        if ((sem_getvalue(&_tx_linux_timer_semaphore, &pending) == 0) && (pending == 0))
        {
            sem_post(&_tx_linux_timer_semaphore);
        }

        usleep(INJECTOR_INTERVAL_USEC);
    }

    return(NULL);
}


static void  interrupt_injector_start(void)
{

    injector_stop =  0;
    if (pthread_create(&injector_thread, NULL, interrupt_injector, NULL) != 0)
    {

        /* Without the injector the loop still runs, on its own budget.  */
        injector_stop =  1;
    }
}


static void  interrupt_injector_stop(void)
{

    if (injector_stop == 0)
    {

        injector_stop =  1;
        pthread_join(injector_thread, NULL);
    }
}

#else
#define interrupt_injector_start()
#define interrupt_injector_stop()
#endif


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    timer_0_entry(ULONG timer_input);

/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Define the external reference for the preempt disable flag.  */

extern volatile UINT   _tx_thread_preempt_disable;

/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void    isr_entry(void)
{

UINT    status;
static volatile UINT miss_count = 0;


    /* Attempt to sleep from a timer in order to test the error logic.  */
    status =  tx_thread_sleep(1);

    /* Check for proper error status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Blow up the test to force an error.  */
        condition_count =  10000000;
        semaphore_put_counter =  0xFFFF0000;
    }

    /* Determine if the interrupt occurred when the preempt disable flag was set.  */
    if (_tx_thread_preempt_disable)
    {

        /* Yes this is the condition we are looking for, increment the test condition counter.   */
        condition_count++;
    }

    /*
    It is possible for this test to get into a resonance condition in which
    the ISR never occurs while preemption is disabled (especially if the
    ISR is installed in the periodic timer interrupt handler, which is
    conveniently available). Detect this condition and break out of it by
    perturbing the duration of this ISR a pseudo-random amount of time.
    */
    else if (++miss_count > 100)
        for (miss_count = _tx_timer_system_clock % 100; miss_count != 0; --miss_count);

    /* Put the semaphore to wakeup thread 0.  */
    status =  tx_semaphore_put(&semaphore_0);

    /* Increment the semaphore counter.  */
    if (status == TX_SUCCESS)
        semaphore_put_counter++;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_wait_abort_and_isr_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,
            pointer, TEST_STACK_SIZE_PRINTF,
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,
            pointer, TEST_STACK_SIZE_PRINTF,
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create semaphore - consumer producer semaphore.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create a timer to ensure a context save is called for every interrupt.  */
    status =  tx_timer_create(&timer_0, "timer 0", timer_0_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #4\n");
        test_control_return(1);
    }

    /* Clear the condition count variable.  */
    condition_count  =  0;
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;

/* The window this test waits for is probabilistic, and the handler above says
   as much: it can settle into a resonance in which the condition is never met.
   Waiting for it without a bound means such a run never ends, and this test has
   been the most expensive thing in the suite: run one at a time in CI it took
   between 148 and 726 seconds per configuration, 88 percent of the whole
   ThreadX suite, against 273 seconds for the other ninety five tests together.

   The budget is in wall clock seconds rather than ticks on purpose. The
   simulated tick clock is not a proxy for elapsed time here: a tick arrives only
   when the port's timer thread gets to run, so under load, or under coverage
   instrumentation, ticks fall behind and never catch up. A budget of 20000
   ticks, nominally 200 seconds, failed to stop a run that took 726 seconds,
   because fewer than 20000 ticks had passed. time() does not drift that way.

   How many windows to ask for is set by what a run can actually reach. Before the
   injector above, four CI runs of the same tree measured this loop in two modes:
   one where a window arrives in milliseconds, and one where a single window
   costs around forty seconds. Seven of twenty configuration-runs landed in the
   slow mode and ran out of budget, reaching 0, 3, 3, 3, 4, 7 and 7 of the ten
   then asked for, every one of them still reporting a pass, and the count had to
   come down to three to stay reachable. With the interrupt source decorrelated
   from the thread it samples the slow mode does not occur: all six
   configurations of a run reach ten of ten in under a second. The count is
   therefore back to ten, on the same reasoning that lowered it.

   The SMP copy deliberately does not get the injector. It has never been in the
   slow mode, reaching its twenty windows in about a second, and injecting
   interrupts there measurably hurts: 8.9 seconds against 0 to 1 for the same
   twenty, which is the extra interrupt traffic contending across the simulated
   cores. Its budget and ceiling still match this one; only the injector differs,
   because only this copy has the problem it solves.

   Reaching the window no times at all is different in kind, which is what the
   ceiling below is for. The check after the loop compares semaphore bookkeeping
   that a window has to have touched to mean anything, so a pass with a count of
   zero claims coverage the run did not have. One of those twenty runs did
   exactly that, and said so only in an artifact nobody reads. A run that has not
   reached the window once keeps trying up to the ceiling, and fails if it still
   has not.

   The budget and the ceiling stay as they were, as a net rather than a working
   limit. They were sized for the slow mode: 180 seconds covers three windows at
   the worst rate measured, 17 to 40 seconds each. Nothing should approach them
   now, and if something does, the behaviour is what it was before.  */
#define WAIT_ABORT_WINDOWS_WANTED   ((ULONG) 10)
#define WAIT_ABORT_SECOND_BUDGET    ((ULONG) 180)
#define WAIT_ABORT_ZERO_WINDOW_CEILING  ((ULONG) 300)

time_t  start_wall;


    /* Setup ISR for this test.  */
    test_isr_dispatch =  isr_entry;

    /* Inform user.  */
    printf("Running Thread Wait Abort and ISR Resume Test....................... ");

    /* Loop to exploit the probability window inside tx_thread_wait_abort.  */
    start_wall =  time(TX_NULL);
    interrupt_injector_start();
    while (condition_count < WAIT_ABORT_WINDOWS_WANTED)
    {

        /* Suspend on the semaphore that is going to be set via the ISR.  */
        status =  tx_semaphore_get(&semaphore_0, (thread_0_counter % 5) + 1);

        /* Determine if we have an unexpected result.  */
        if ((status != TX_SUCCESS) && (status != TX_WAIT_ABORTED))
        {

            /* Test error!  */
            printf("ERROR #5\n");
            test_control_return(1);
        }

        /* Check for the preempt disable flag being set.  */
        if (_tx_thread_preempt_disable)
        {

            /* Test error!  */
            printf("ERROR #6\n");
            test_control_return(2);
        }

        /* Determine if we really got the semaphore.  */
        if (status == TX_SUCCESS)
        {

            /* Increment the thread count.  */
            thread_0_counter++;

#ifdef TX_NOT_INTERRUPTABLE

            /* Determine if we have a non-interruptable build of ThreadX. If so, just
               get out of this loop after 100 passes.  */

            if (thread_0_counter >= 100)
                break;
#endif

        }

        /* Out of budget?  A run that has not reached the window even once has
           verified nothing yet, so it gets the higher ceiling before giving up.  */
#ifdef TX_NOT_INTERRUPTABLE
        if (((ULONG) (time(TX_NULL) - start_wall)) > WAIT_ABORT_SECOND_BUDGET)
            break;
#else
        if (condition_count == 0)
        {
            if (((ULONG) (time(TX_NULL) - start_wall)) > WAIT_ABORT_ZERO_WINDOW_CEILING)
                break;
        }
        else if (((ULONG) (time(TX_NULL) - start_wall)) > WAIT_ABORT_SECOND_BUDGET)
            break;
#endif
    }

    interrupt_injector_stop();

    /* Clear ISR dispatch.  */
    test_isr_dispatch =  TX_NULL;

    /* Say what this run reached, on every run and not only a short one. A count
       printed only on shortfall cannot be told apart from a count nobody
       recorded, and this line is what the CI artifacts carry. Falling short is a
       gap in what was exercised rather than a fault in the code under test, so
       the check below still runs and still means what it did.  */
    printf("(reached %lu of %lu windows in %lu seconds) ",
           (ULONG) condition_count, WAIT_ABORT_WINDOWS_WANTED,
           (ULONG) (time(TX_NULL) - start_wall));

#ifndef TX_NOT_INTERRUPTABLE

    /* Reached it no times?  Then the check below compares bookkeeping no window
       ever touched, and a pass would report coverage this run did not have.  */
    if (condition_count == 0)
    {

        /* Test error!  */
        printf("ERROR #8\n");
        test_control_return(4);
    }
#endif

#ifdef TX_NOT_INTERRUPTABLE
    /* At this point, check to see if we got all the semaphores!  */
    if ((thread_0_counter != (semaphore_put_counter - semaphore_0.tx_semaphore_count)) ||
        (condition_count != 0))
#else
    /* At this point, check to see if we got all the semaphores!  */
    if (thread_0_counter != (semaphore_put_counter - semaphore_0.tx_semaphore_count))
#endif
    {

        /* Test error!  */
        printf("ERROR #7\n");
        test_control_return(3);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    thread_1_entry(ULONG thread_input)
{

    /* Loop forever!  */
    while(1)
    {


        /* Abort the suspension on the semaphore in thread 0.  */
        tx_thread_wait_abort(&thread_0);

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Let thread 0 run again!  */
        tx_thread_relinquish();
    }
}


static void    timer_0_entry(ULONG input)
{
    timer_0_counter++;
}

