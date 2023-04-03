/* This test is designed to test for preemption-threshold use during thread creation during initialization.  */

#include   <stdio.h>
#include   "tx_api.h"

//static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;


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
void    threadx_thread_create_preemption_threshold_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 0, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Create Preemption-Threshold from Init Test........... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 0, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Create Preemption-Threshold from Init Test........... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 0, 100, TX_DONT_START);
    status +=  tx_thread_resume(&thread_0);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Create Preemption-Threshold from Init Test........... ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

    /* Inform user.  */
    printf("Running Thread Create Preemption-Threshold from Init Test........... ");

    /* If either of the other threads have run, an error is present.  */
    if ((thread_1_counter) || (thread_2_counter))
    {

        /* Test error!  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Sleep for two ticks (one is insufficient to guarantee the other
       tasks will run, if this executes too close to the tick interrupt.  */
    tx_thread_sleep(2);


    /* Now, both threads should have run.  */
    if ((thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Test error!  */
        printf("ERROR #5\n");
        test_control_return(2);
    }

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void    thread_1_entry(ULONG thread_input)
{

    /* Increment this thread's counter.  */
    thread_1_counter++;
}


static void    thread_2_entry(ULONG thread_input)
{

    /* Increment the thread counter.  */
    thread_2_counter++;
}

