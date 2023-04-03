/* This test is designed to see if multiple non-current threads can be suspended.  
   The order the suspension and resumption occurs makes sure everything is working 
   right. Thread execution should remain predictable even after suspension and 
   resumption of threads within a priority group.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_3;

static unsigned long   thread_4_counter =  0;
static TX_THREAD       thread_4;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_multiple_non_current_suspension_application_define(void *first_unused_memory)
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

        printf("Running Thread Non-Current Suspend Test............................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Non-Current Suspend Test............................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Non-Current Suspend Test............................. ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Non-Current Suspend Test............................. ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Non-Current Suspend Test............................. ERROR #5\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT status;

    /* Inform user.  */
    printf("Running Thread Non-Current Suspend Test............................. ");

    /* Wakeup all other threads at same priority.  */
    status =  tx_thread_resume(&thread_1);
    status += tx_thread_resume(&thread_2);

    /* Check for good status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread Suspend error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Wakeup thread with preempt.  */
    status =  tx_thread_resume(&thread_3);

    /* Check for good status and proper counters.  */
    if ((status != TX_SUCCESS) || (thread_3_counter != 1) || (thread_1_counter) ||
        (thread_2_counter) || (thread_4_counter))
    {

        /* Thread Suspend error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Suspend thread 1.  */
    status =  tx_thread_suspend(&thread_1);

    /* Resume thread 4.  */
    status += tx_thread_resume(&thread_4);

    /* Check for good status and proper counters.  */
    if ((status != TX_SUCCESS) || (thread_3_counter != 1) || (thread_1_counter) ||
        (thread_2_counter) || (thread_4_counter))
    {

        /* Thread Suspend error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Relinquish to thread 2 and 4 before we get back.  */
    tx_thread_relinquish();

    /* Check for good status and proper counters.  */
    if ((status != TX_SUCCESS) || (thread_3_counter != 1) || (thread_1_counter) ||
        (thread_2_counter != 1) || (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Successful Thread Suspend non-current thread test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

static void    thread_1_entry(ULONG thread_input)
{

    thread_1_counter++;
}

static void    thread_2_entry(ULONG thread_input)
{
    if (thread_4_counter == 0)
        thread_2_counter++;
}

static void    thread_3_entry(ULONG thread_input)
{

    thread_3_counter++;
}

static void    thread_4_entry(ULONG thread_input)
{

    if (thread_2_counter == 1)
        thread_4_counter++;
}

