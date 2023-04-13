/* This test is designed to see if a thread can successfully suspend itself in a single 
   thread system.  This also tests a thread created that is not automatically enabled.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long     thread_0_counter =  0;
static TX_THREAD         thread_0;

static TX_THREAD         thread_1;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_simple_supsend_application_define(void *first_unused_memory)
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
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Simple Suspend Test.................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Simple Suspend Test.................................. ERROR #2\n");
        test_control_return(1);
    }
}



/* Define the test thread.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* Resume the test control thread.  */
    status =  tx_thread_resume(&thread_1);

    /* Check status.  */
    if (status != TX_SUCCESS)
        return;

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Suspend the running thread.  */
    tx_thread_suspend(&thread_0);
}


static void    thread_1_entry(ULONG thread_input)
{

    /* Inform user.  */
    printf("Running Thread Simple Suspend Test.................................. ");

    /* The other thread should be in a suspended state now.  */
    if ((thread_0.tx_thread_state != TX_SUSPENDED) || (thread_0_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }
    else
    {

        /* Successful Thread Suspend test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


