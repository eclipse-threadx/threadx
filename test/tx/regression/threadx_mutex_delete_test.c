/* This test is designed to test the mutex suspension and mutex delete with
   suspended threads.  */

#include <stdio.h>
#include "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;


static TX_MUTEX        mutex_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_delete_application_define(void *first_unused_memory)
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
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Delete Test........................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Delete Test........................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Delete Test........................................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create a mutex.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Delete Test........................................... ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Mutex Delete Test........................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Grab the mutex so it is owned by this thread.  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Relinquish to let other threads run.  */
    tx_thread_relinquish();

    /* Other threads should now be suspended on the mutex.  */

    /* Delete the mutex to test it out!  */
    status =  tx_mutex_delete(&mutex_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Relinquish to allow other threads to run again before we return.  */
    tx_thread_relinquish();

    /* Now check the run counter of each thread.  */
    if ((thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Mutex error.  */
        printf("ERROR #7\n");
        test_control_return(1);
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

UINT    status;


    /* Suspend on the mutex. */
    status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

    /* Did we get the right status?  */
    if (status == TX_DELETED)
        thread_1_counter++;
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;


    /* Suspend on the mutex. */
    status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

    /* Did we get the right status?  */
    if (status == TX_DELETED)
        thread_2_counter++;
}
