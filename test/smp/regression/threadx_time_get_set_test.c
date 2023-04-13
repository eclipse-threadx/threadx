/* This test is designed to test simple tx_time_get and set services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_timer.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_time_get_set_application_define(void *first_unused_memory)
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

        printf("Running Time Simple Get/Set Test.................................... ERROR #1\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

ULONG   current_time;


    /* Inform user.  */
    printf("Running Time Simple Get/Set Test.................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Sleep for 1 tick to get a fresh timer.  */
    tx_thread_sleep(1);
    
    /* Set time to 0.  */
    tx_time_set(0);

    /* Sleep for a couple ticks.  */
    tx_thread_sleep(35);

    /* Pickup the current time.  */
    /* Call internal function to cover this function.  */
    current_time =  _tx_time_get();

    /* Check Current time.  It should be 35. */
    if (current_time != 35)
    {

        /* System time error.  */
        printf("ERROR #2\n");
        test_control_return(1);
    }

    /* Set the new time.  */
    tx_time_set(7);

    /* Check the new time.  */
    if (tx_time_get() != 7)
    {

        /* System time error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }
    else
    {

        /* Successful time test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

