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

    /* Inform user.  */
    printf("Running Timer Multiple Timer Test................................... ");

    /* Sleep for a couple ticks.  */
    tx_thread_sleep(301);

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

    /* Sleep for 200.  */
    tx_thread_sleep(200);

    /* Insure that each timer haven't run again.  */
    if ((timer_0_counter != 103) || (timer_1_counter != 53) ||
        (timer_2_counter != 36))
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

