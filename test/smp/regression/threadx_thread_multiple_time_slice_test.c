/* This test is designed to see if two threads can be created and execute with a time-slice.  
   Thread 7 should run twice as long because it has more of a time-slice. */

#include   <stdio.h>
#include   "tx_api.h"

static volatile unsigned long   thread_0_counter =  0;
static          TX_THREAD       thread_0;
static volatile unsigned long   thread_1_counter =  0;
static          TX_THREAD       thread_1;
static volatile unsigned long   thread_2_counter =  0;
static          TX_THREAD       thread_2;
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
static volatile unsigned long   thread_3_counter =  0;
static          TX_THREAD       thread_3;
static volatile unsigned long   thread_4_counter =  0;
static          TX_THREAD       thread_4;
#endif


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
#endif


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_multiple_time_slice_application_define(void *first_unused_memory)
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
            16, 16, 2, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Time-Slice Test...................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 4, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Time-Slice Test...................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create control thread.  */
    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Time-Slice Test...................... ERROR #3\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

    /* Create threads with preemption-threshold and time-slice, such and make sure time-slice is defeated by 
       preemption-threshold.  */
    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 17, 2, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Time-Slice Test...................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 4, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Time-Slice Test...................... ERROR #5\n");
        test_control_return(1);
    }
#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 0 counter.  */
        thread_0_counter++;

        /* Call thread identify for Win32 test case.  */
        tx_thread_identify();
    }
}


static void    thread_1_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 1 counter.  */
        thread_1_counter++;

        /* Call thread identify for Win32 test case.  */
        tx_thread_identify();
    }
}


static void    thread_2_entry(ULONG thread_input)
{
  
unsigned long   counter_sum;
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
UINT            status;
#endif


    /* Inform user.  */
    printf("Running Thread Multiple Thread Time-Slice Test...................... ");

    /* Sleep for some multiple of 6.  */
    tx_thread_sleep(48);

    /* Increment thread 2 counter.  */
    thread_2_counter++;

    /* Compute the delta. Should be twice as much, but some test environments (Windows/Linux) are 
       not as good in terms of real time processing.  */
    counter_sum =  thread_0_counter;
    counter_sum =  counter_sum + (thread_0_counter/4);
    if (thread_1_counter <= counter_sum)
    {

        /* Thread Time-slice error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }  
#ifdef TX_DISABLE_PREEMPTION_THRESHOLD
    else
    {
        /* Successful Thread Time-slice test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }  
#else
    /* Now suspend threads 0 and 1 so we can let 3 and 4 run.  */
    status =  tx_thread_suspend(&thread_0);
    status += tx_thread_suspend(&thread_1);
    
    /* Check status.  */
    if (status)
    {
        /* Thread Time-slice error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
    
    /* Now sleep and see if thread 4 ever runs.  */
    tx_thread_sleep(4);

    /* Determine if thread 3 ran and thread 4 didn't.  */
    if ((thread_3_counter) && (thread_4_counter == 0))
    {
        /* Successful Thread Time-slice test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
    else
    {

        /* Thread Time-slice error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
#endif
}

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

/* Define the test threads.  */

static void    thread_3_entry(ULONG thread_input)
{

    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 3 counter.  */
        thread_3_counter++;

        /* Call thread identify for Win32 test case.  */
        tx_thread_identify();
    }
}


static void    thread_4_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* We should never get here!  */
        
        /* Increment thread 4 counter.  */
        thread_4_counter++;

        /* Call thread identify for Win32 test case.  */
        tx_thread_identify();
    }
}
#endif

