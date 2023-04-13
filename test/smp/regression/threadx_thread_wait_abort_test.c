/* This test is designed to test thread wait abort.  */

#include   <stdio.h>
#include   "tx_api.h"

//static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_wait_abort_application_define(void *first_unused_memory)
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
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort Test...................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort Test...................................... ERROR #2\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Thread Wait Abort Test...................................... ");

    /* Let other thread run.  */
    tx_thread_resume(&thread_1);

    /* Make sure thread 1 is sleeping.  */
    if (thread_1.tx_thread_state != TX_SLEEP)
    {

        /* Thread error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* At this point, call tx_thread_wait_abort to abort the sleep in thread1.  */
    status =  tx_thread_wait_abort(&thread_1);

    /* Check for status.  */
    if ((status != TX_SUCCESS) || (thread_1.tx_thread_state !=  TX_SUSPENDED))
    {

        /* Thread error.  */
        printf("ERROR #4\n");
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

    /* Sleep for a long long time.  */
    status =  tx_thread_sleep(1000000000);

    /* Check for the proper status.  */
    if (status != TX_WAIT_ABORTED)
    {

        /* Thread error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Increment the thread counter.  */
    thread_1_counter++;

    /* Suspend self.  */
    tx_thread_suspend(&thread_1);
}

