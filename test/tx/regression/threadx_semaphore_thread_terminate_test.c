/* This test is designed to test thread terminate calls when threads are suspended on
   a semaphore.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;


static TX_SEMAPHORE    semaphore_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void   put_notify(TX_SEMAPHORE *semaphore_ptr)
{

    /* Don't need to do anything in here...  */
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_semaphore_thread_terminate_application_define(void *first_unused_memory)
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

        printf("Running Semaphore Thread Terminate Test............................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Thread Terminate Test............................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Thread Terminate Test............................. ERROR #3\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 0.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Thread Terminate Test............................. ERROR #4\n");
        test_control_return(1);
    }
    
    /* Setup the semaphore notify callback.  */
    status =  tx_semaphore_put_notify(&semaphore_0, put_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Thread Terminate Test............................. ERROR #5\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Semaphore Thread Terminate Test............................. ERROR #6\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Semaphore Thread Terminate Test............................. ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Relinquish to let other threads run.  */
    tx_thread_relinquish();

    /* Other threads should now be suspended on the semaphore.  */
    if ((thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Semaphore error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Terminate the other threads to make sure the semaphore gets
       cleaned up.  */
    status =  tx_thread_terminate(&thread_1);

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Semaphore error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Terminate the other thread.  */
    status =  tx_thread_terminate(&thread_2);

    /* Relinquish just to make sure. */
    tx_thread_relinquish();

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Semaphore error.  */
        printf("ERROR #9\n");
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

    /* Increment thread run counter.  */
    thread_1_counter++;

    /* Suspend on the semaphore. */
    tx_semaphore_get(&semaphore_0, 33);

    /* Should never get here!  */
    thread_1_counter++;
}


static void    thread_2_entry(ULONG thread_input)
{

    /* Increment thread run counter.  */
    thread_2_counter++;

    /* Suspend on the semaphore. */
    tx_semaphore_get(&semaphore_0, 44);

    /* Should never get here! */
    thread_2_counter++;
}
