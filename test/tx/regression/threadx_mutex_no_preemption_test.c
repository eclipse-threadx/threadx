/* This test is designed to test the mutex suspension and another thread resuming the
   same priority thread by doing a mutex put.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;


static TX_MUTEX        mutex_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutext_no_preemption_application_define(void *first_unused_memory)
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

        printf("Running Mutex Put with No Preemption Test........................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Put with No Preemption Test........................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create a mutex.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Put with No Preemption Test........................... ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Mutex Put with No Preemption Test........................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Get the mutex.  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Should be successful.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Relinquish to make the other thread suspend on the mutex.  */
    tx_thread_relinquish();

    /* Make sure the other thread has run.  */
    if (thread_1_counter != 1)
    {

        /* Mutex error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Put the mutex, this should resume the other thread
       but not preempt this thread.  */
    status =  tx_mutex_put(&mutex_0);

    /* Check the status and the run counter of the other thread.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1))
    {

        /* Mutex error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Relinquish to allow the other thread to run.  */
    tx_thread_relinquish();

    /* Make sure the other thread has run.  */
    if (thread_1_counter != 2)
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

    /* Increment thread run counter.  */
    thread_1_counter++;

    /* Suspend on the mutex. */
    status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

    /* Did we get the right status?  */
    if (status == TX_SUCCESS)
        thread_1_counter++;
}

