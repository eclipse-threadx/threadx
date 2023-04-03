/* This test is designed to test the semaphore suspension and another thread resuming the
   higher priority thread by doing a semaphore put.  Higher-priority thread should preempt.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;


static TX_SEMAPHORE    semaphore_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


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
void    threadx_semaphore_preemption_application_define(void *first_unused_memory)
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

        printf("Running Semaphore Preemption Test................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Preemption Test................................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 0.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Preemption Test................................... ERROR #3\n");
        test_control_return(1);
    }

    /* Setup the semaphore notify callback.  */
    status =  tx_semaphore_put_notify(&semaphore_0, put_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Preemption Test................................... ERROR #4\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Semaphore Preemption Test................................... ERROR #5\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Semaphore Preemption Test................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* The other thread should now be suspended on the semaphore.  */
    if (thread_1_counter != 1)
    {

        /* Semaphore error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Place an instance on the semaphore, this should cause the other thread
       to preempt.  */
    status =  tx_semaphore_put(&semaphore_0);

    /* Check status and run counter of other thread.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 2))
    {

        /* Semaphore error.  */
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

    /* Suspend on the semaphore. */
    status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

    /* Did we get the right status?  */
    if (status == TX_SUCCESS)
        thread_1_counter++;
}

