/* This test is designed to test the change time-slice service call.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

extern unsigned long _tx_timer_time_slice;

/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_time_slice_change_application_define(void *first_unused_memory)
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

        printf("Running Thread Time-Slice Change Test............................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            22, 22, 200, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Time-Slice Change Test............................... ERROR #2\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   old_time_slice =  0;


    /* Inform user.  */
    printf("Running Thread Time-Slice Change Test............................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Change the time-slice of other thread.  */
    status =  tx_thread_time_slice_change(&thread_1, 33, &old_time_slice);

    /* Check status and the time sice of specified thread.  */
    if ((status != TX_SUCCESS) || (old_time_slice != 200) || (thread_1.tx_thread_new_time_slice != 33))
    {

        /* Thread error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Sleep to get a fresh timer.  */
    tx_thread_sleep(1);

    /* Change the time-slice of this thread.  */
    status =  tx_thread_time_slice_change(&thread_0, 66, &old_time_slice);

    /* Check status and the time sice of specified thread.  */
    if ((status != TX_SUCCESS) || (old_time_slice != 100) || (thread_0.tx_thread_new_time_slice != 66) ||
        (_tx_timer_time_slice != 66))
    {

        /* Thread error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Change the time-slice of this thread.  */
    status =  tx_thread_time_slice_change(&thread_1, 2, &old_time_slice);

    /* Sleep for 8 ticks just to allow thread 1 to run and time-slice with no other thread ready.  */
    tx_thread_sleep(8);

    if (status != TX_SUCCESS)
    {

        /* Thread error.  */
        printf("ERROR #5\n");
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

    while(1)
    {
    
        /* Identify.  */
        tx_thread_identify();

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}

