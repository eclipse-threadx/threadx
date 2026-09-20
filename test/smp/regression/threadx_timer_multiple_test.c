/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * Copyright (c) 2026 Eclipse ThreadX contributors
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/
// Portions of this file were generated with AI assistance.

/* This test is designed to test a simple application timer services, 
   including create, activate, deactivate, change, and delete with multiple timers.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;
static unsigned long   timer_1_counter =  0;
static TX_TIMER        timer_1;
static unsigned long   timer_2_counter =  0;
static TX_TIMER        timer_2;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    timer_0_expiration(ULONG timer_input);
static void    timer_1_expiration(ULONG timer_input);
static void    timer_2_expiration(ULONG timer_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_timer_multiple_application_define(void *first_unused_memory)
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
            16, 16, 3, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Test................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        100, 200, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Test................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_1, "timer 1", timer_1_expiration, 0x1234,
                        100, 200, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Test................................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_2, "timer 2", timer_2_expiration, 0x1234,
                        100, 200, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Test................................... ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   start_time;
ULONG   elapsed;

    /* Inform user.  */
    printf("Running Timer Multiple Timer Test................................... ");

    /* Sleep past the second expiration of each timer, with margin.

       The timers carry an initial 100 ticks and a 200 tick reschedule, so they
       expire at 300 and not again until 500, and the count of two below holds
       anywhere in that window. The margin that matters is the one at the bottom
       of it: expirations are processed by the system timer thread rather than in
       the interrupt, so a count read immediately after the expiring tick can
       still be short. Waiting ten ticks past it leaves 189 at the other end.  */
    tx_thread_sleep(310);

    /* Insure that each timer ran twice.  */
    if ((timer_0_counter != 2) || (timer_1_counter != 2) ||
        (timer_2_counter != 2))
    {

        /* Application timer error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Deactivate the timers.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_timer_deactivate(&timer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    status =  tx_timer_deactivate(&timer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Sleep again.  */
    tx_thread_sleep(100);

    /* Insure that each timer haven't run again.  */
    if ((timer_0_counter != 2) || (timer_1_counter != 2) ||
        (timer_2_counter != 2))
    {

        /* Application timer error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Modify the timer.  */
    status =  tx_timer_change(&timer_0, 100, 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    status =  tx_timer_change(&timer_1, 100, 2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    status =  tx_timer_change(&timer_2, 100, 3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Activate the timer.  */
    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Note when the timers are about to run again, so the counters below can be
       checked against the window that actually elapsed rather than the one that
       was asked for.  */
    start_time =  tx_time_get();

    /* Sleep for 200.  */
    tx_thread_sleep(200);

    /* Insure that each timer ran as often as its period allows.

       The three timers now carry an initial 100 ticks and reschedule periods of
       one, two and three, and each already stands at two, so the nominal counts
       over a 200 tick window are 103, 53 and 36 and the first of them changes on
       every single tick. An exact equality therefore has no margin at all, unlike
       the one above: this thread is woken by a host timer thread and given one of
       four emulated cores, so a wake one tick late moves every counter, and the
       expirations are processed by the system timer thread rather than in the
       interrupt, so under load the counters lag the clock and catch up
       afterwards. It was seen failing on exactly that.

       What is exact, and is the property being checked, is that a timer cannot
       expire more often than its period allows over the window that elapsed: at
       most one expiration at the initial 100 ticks and one per period after it,
       on top of the two already counted. That is the ceiling. The floors allow
       five expirations of lag and are still far tighter than any period error,
       since a reschedule of two ticks on the first timer would read about 53
       against a floor of 98.  */
    elapsed =  tx_time_get() - start_time;
    if ((elapsed < 200) ||
        (timer_0_counter > (((elapsed - 100) / 1) + 3)) || (timer_0_counter < 98) ||
        (timer_1_counter > (((elapsed - 100) / 2) + 3)) || (timer_1_counter < 48) ||
        (timer_2_counter > (((elapsed - 100) / 3) + 3)) || (timer_2_counter < 31))
    {

        /* Application timer error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Deactivate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    status =  tx_timer_deactivate(&timer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    status =  tx_timer_deactivate(&timer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Delete the timer.  */
    status =  tx_timer_delete(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    status =  tx_timer_delete(&timer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    status =  tx_timer_delete(&timer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Successful Multiple timer test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void    timer_0_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_0_counter++;
}

static void    timer_1_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_1_counter++;
}

static void    timer_2_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_2_counter++;
}

