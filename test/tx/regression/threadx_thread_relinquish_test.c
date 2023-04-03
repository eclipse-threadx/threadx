/* This test is designed to see if multiple threads can be created and relinquish control between them.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long     thread_0_counter = 0;
static TX_THREAD         thread_0;

static unsigned long     thread_1_counter = 0;
static TX_THREAD         thread_1;

static unsigned long     thread_2_counter = 0;
static TX_THREAD         thread_2;

static unsigned long     thread_3_counter = 0;
static TX_THREAD         thread_3;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_relinquish_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Create thread 0.  */
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create thread 1.  */
    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create thread 2.  */
    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create thread 3.  */
    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

    /* Check for correct input value and execution of other threads.  */
    if ((thread_input != 0) || (thread_1_counter) || (thread_2_counter) ||
        (thread_3_counter))
        return;

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Relinquish to other thread(s).  */
    tx_thread_relinquish();
}


static void    thread_1_entry(ULONG thread_input)
{

    /* Check for correct input value and execution of other threads.  */
    if ((thread_input != 1) || (thread_0_counter != 1) || (thread_2_counter) ||
        (thread_3_counter))
        return;

    /* Increment thread 1 counter.  */
    thread_1_counter++;

    /* Relinquish to other thread(s).  */
    tx_thread_relinquish();
}


static void    thread_2_entry(ULONG thread_input)
{

    /* Check for correct input value and execution of other threads.  */
    if ((thread_input != 2) || (thread_0_counter != 1) || (thread_1_counter != 1) ||
        (thread_3_counter))
        return;

    /* Increment thread 2 counter.  */
    thread_2_counter++;

    /* Relinquish to other thread(s).  */
    tx_thread_relinquish();
}


static void    thread_3_entry(ULONG thread_input)
{

    /* Inform user.  */
    printf("Running Thread Relinquish Test...................................... ");

    /* Check for correct input value and execution of other threads.  */
    if ((thread_input != 3) || (thread_0_counter != 1) || (thread_1_counter != 1) ||
        (thread_2_counter != 1) || (thread_0.tx_thread_state != TX_READY) ||
        (thread_1.tx_thread_state != TX_READY) || (thread_2.tx_thread_state != TX_READY))
        return;

    /* Increment thread 3 counter.  */
    thread_3_counter++;

    /* Immediate response relinquish.  */
    tx_thread_relinquish();
    
    /* All other threads should be completed now.  */
    if ((thread_0.tx_thread_state != TX_COMPLETED) || (thread_1.tx_thread_state != TX_COMPLETED) ||
        (thread_2.tx_thread_state != TX_COMPLETED))
    {

        /* Thread Relinquish error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Execute immediate response relinquish.  */
    tx_thread_relinquish();

    /* Successful thread relinquish test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

