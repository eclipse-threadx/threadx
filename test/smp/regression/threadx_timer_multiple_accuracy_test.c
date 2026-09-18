/* This test is designed to test the accuracy of three free running timers.  */

#include   <stdio.h>
#include   "tx_api.h"

//static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;
static unsigned long   timer_1_counter =  0;
static TX_TIMER        timer_1;
static unsigned long   timer_2_counter =  0;
static TX_TIMER        timer_2;


/* Define prototypes.  */
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
void    threadx_timer_multiple_accuracy_application_define(void *first_unused_memory)
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

        printf("Running Timer Multiple Timer Accuracy Test.......................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        1, 1, TX_NO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Accuracy Test.......................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_1, "timer 1", timer_1_expiration, 0x1234,
                        2, 2, TX_NO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Accuracy Test.......................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_2, "timer 2", timer_2_expiration, 0x1234,
                        3, 3, TX_NO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Multiple Timer Accuracy Test.......................... ERROR #4\n");
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
    printf("Running Timer Multiple Timer Accuracy Test.......................... ");

    /* Sleep to get a fresh timer. */
    tx_thread_sleep(1);

    /* Note when the timers are about to be activated, so the counters below can
       be checked against the window that actually elapsed rather than the one
       that was asked for.  */
    start_time =  tx_time_get();

    /* Activate all the timers.  */
    status =  tx_timer_activate(&timer_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_1);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_2);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Sleep for a some ticks.  */
    tx_thread_sleep(300);

    /* Insure that each timer ran as often as its period allows.

       The three timers have periods of one, two and three ticks and the sleep
       above asks for 300, so the nominal counts are 300, 150 and 100 and the
       period-one timer changes its count on every single tick. An exact equality
       therefore has no margin at all: this thread is woken by a host timer thread
       and given one of four emulated cores, so a wake one tick late moves every
       counter, and the expirations themselves are processed by the system timer
       thread rather than in the interrupt, so under load the counters lag the
       clock and catch up afterwards. It was seen failing on exactly that.

       What is exact, and is the property being checked, is that a timer cannot
       expire more often than its period allows over the window that elapsed. That
       is the ceiling. The floors allow five ticks of lag and are still far tighter
       than any period error: a period of two ticks on the first timer would read
       about 150 against a floor of 295.  */
    elapsed =  tx_time_get() - start_time;
    if ((elapsed < 300) ||
        (timer_0_counter > elapsed) || (timer_0_counter < 295) ||
        (timer_1_counter > (elapsed / 2)) || (timer_1_counter < 145) ||
        (timer_2_counter > (elapsed / 3)) || (timer_2_counter < 95))
    {

        /* Application timer error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
    else
    {

        /* Successful timer test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
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

