/* This test is designed to test the semaphore suspension and timeout functionality.  */

#include   <stdio.h>
#include   "tx_api.h"

//static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static TX_SEMAPHORE    semaphore_0;
static TX_SEMAPHORE    semaphore_1;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


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
void    threadx_semaphore_timeout_application_define(void *first_unused_memory)
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

        printf("Running Semaphore Suspension Timeout Test........................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 0.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Suspension Timeout Test........................... ERROR #2\n");
        test_control_return(1);
    }
    
    /* Setup the semaphore notify callback.  */
    status =  tx_semaphore_put_notify(&semaphore_0, put_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Suspension Timeout Test........................... ERROR #3\n");
        test_control_return(1);
    }

#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Semaphore Suspension Timeout Test........................... ERROR #4\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* Inform user.  */
    printf("Running Semaphore Suspension Timeout Test........................... ");

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to setup semaphore notify callback on non-semaphore.  */
    status =  tx_semaphore_put_notify(TX_NULL, put_notify);

    /* Check status  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to setup semaphore notify callback on non-created semaphore.  */
    semaphore_1.tx_semaphore_id =  0;
    status =  tx_semaphore_put_notify(&semaphore_1, put_notify);

    /* Check status  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }
#endif

    /* Sleep for 2 ticks for fresh timer.  */
    tx_thread_sleep(2);

    /* Set clock to 0.  */
    tx_time_set(0);

    /* Suspend on the semaphore. */
    status =  tx_semaphore_get(&semaphore_0, 33);

    /* Did we get the right status at the right time?  */
    if ((status != TX_NO_INSTANCE) || (tx_time_get() != 33))
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


