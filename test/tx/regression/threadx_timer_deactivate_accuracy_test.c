/* This test is designed to test application timer activation/deactivation services
   from threads...  Make sure the remaining ticks are being saved/restored properly.  */

#include   <stdio.h>
#include   "tx_api.h"

//static unsigned long   expected_time;
static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    timer_0_expiration(ULONG timer_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_timer_deactivate_accuracy_application_define(void *first_unused_memory)
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

        printf("Running Timer Activation/Deactivation Accuracy Test................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        29, 31, TX_NO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Activation/Deactivation Accuracy Test................. ERROR #2\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* Inform user.  */
    printf("Running Timer Activation/Deactivation Accuracy Test................. ");

    /* Sleep so we have to handle the wrap condition.  */
    tx_thread_sleep(10);

    /* Activate the timer.  */
    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Sleep */
    tx_thread_sleep(2);

    /* Deactivate and activate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Sleep */
    tx_thread_sleep(3);

    /* Deactivate and activate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
    
    /* Sleep */
    tx_thread_sleep(4);

    /* Deactivate and activate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Sleep */
    tx_thread_sleep(5);

    /* Deactivate and activate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Sleep */
    tx_thread_sleep(6);

    /* Deactivate and activate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Sleep */
    tx_thread_sleep(7);

    /* Deactivate and activate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if ((status != TX_SUCCESS) || (timer_0_counter))
    {

        /* Application timer error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Sleep */
    tx_thread_sleep(2);

    /* At this point the timer should have ran!  */
    status =  tx_timer_deactivate(&timer_0);

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Check for status.  */
    if ((status != TX_SUCCESS) || (timer_0_counter != 1))
    {

        /* Application timer error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
    else
    {

        /* Successful Re-activate test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    timer_0_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_0_counter++;
}

