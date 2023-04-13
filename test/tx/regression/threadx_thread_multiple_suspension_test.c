/* This test is designed to see if multiple threads can be created and suspend.  
   The order the suspension and resumption occurs makes sure everything is working 
   right. All the counters should increment at the same rate.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"


static unsigned long   thread_0_counter = 0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter = 0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter = 0;
static TX_THREAD       thread_2;

static unsigned long   thread_3_counter = 0;
static TX_THREAD       thread_3;

static unsigned long   thread_4_counter = 0;
static TX_THREAD       thread_4;

static unsigned long   thread_5_counter = 0;
static TX_THREAD       thread_5;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_multiple_suspension_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Create thread 0.  */
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            13, 13, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Suspend/Resume Test.................. ERROR #1\n");
        test_control_return(1);
    }

    /* Create thread 1.  */
    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            (TX_MAX_PRIORITIES/2), (TX_MAX_PRIORITIES/2), TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Suspend/Resume Test.................. ERROR #2\n");
        test_control_return(1);
    }

    /* Create thread 2.  */
    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            (TX_MAX_PRIORITIES/2), (TX_MAX_PRIORITIES/2), TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Suspend/Resume Test.................. ERROR #3\n");
        test_control_return(1);
    }

    /* Create thread 3.  */
    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            (TX_MAX_PRIORITIES/2), (TX_MAX_PRIORITIES/2), TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Suspend/Resume Test.................. ERROR #4\n");
        test_control_return(1);
    }

    /* Create thread 4.  */
    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Suspend/Resume Test.................. ERROR #5\n");
        test_control_return(1);
    }

    /* Create thread 5.  Make this thread non-preemptable for the range of priorities here...  */
    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            (TX_MAX_PRIORITIES-1), 13, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Suspend/Resume Test.................. ERROR #6\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 0 counter.  */
        thread_0_counter++;

        /* Suspend this thread... */
        tx_thread_suspend(&thread_0);
    
        /* Resume thread 5...  */
        tx_thread_resume(&thread_5);
    }
}


static void    thread_1_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 1 counter.  */
        thread_1_counter++;

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_2_counter++;

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_2);
    }
}

static void    thread_3_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 3 counter.  */
        thread_3_counter++;

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_3);
    }
}

static void    thread_4_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 4 counter.  */
        thread_4_counter++;

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_4);
    }
}

static void    thread_5_entry(ULONG thread_input)
{

UINT    status;

    /* Inform user.  */
    printf("Running Thread Multiple Thread Suspend/Resume Test.................. ");

    /* Determine if all the other threads are in a suspended state.  */
    if ((thread_0.tx_thread_state != TX_SUSPENDED) || (thread_1.tx_thread_state != TX_SUSPENDED) ||
        (thread_2.tx_thread_state != TX_SUSPENDED) || (thread_3.tx_thread_state != TX_SUSPENDED) ||
        (thread_4.tx_thread_state != TX_SUSPENDED))
    {

        /* Thread Suspend error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run once.  */
    if ((thread_0_counter != 1) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_3_counter != 1) ||
        (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Resume all of the threads.  */
    status =  tx_thread_resume(&thread_0);

    /* Determine if all the other threads are in the proper state.  */
    if ((thread_0.tx_thread_state != TX_READY)     || (thread_1.tx_thread_state != TX_SUSPENDED) ||
        (thread_2.tx_thread_state != TX_SUSPENDED) || (thread_3.tx_thread_state != TX_SUSPENDED) ||
        (thread_4.tx_thread_state != TX_SUSPENDED) || (status != TX_SUCCESS))
    {

        /* Thread Suspend error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run once.  */
    if ((thread_0_counter != 1) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_3_counter != 1) ||
        (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    status =  tx_thread_resume(&thread_1);

    /* Determine if all the other threads are in the proper state.  */
    if ((thread_0.tx_thread_state != TX_READY)     || (thread_1.tx_thread_state != TX_READY) ||
        (thread_2.tx_thread_state != TX_SUSPENDED) || (thread_3.tx_thread_state != TX_SUSPENDED) ||
        (thread_4.tx_thread_state != TX_SUSPENDED) || (status != TX_SUCCESS))
    {

        /* Thread Suspend error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run once.  */
    if ((thread_0_counter != 1) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_3_counter != 1) ||
        (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    status =  tx_thread_resume(&thread_2);

    /* Determine if all the other threads are in the proper state.  */
    if ((thread_0.tx_thread_state != TX_READY)     || (thread_1.tx_thread_state != TX_READY) ||
        (thread_2.tx_thread_state != TX_READY)     || (thread_3.tx_thread_state != TX_SUSPENDED) ||
        (thread_4.tx_thread_state != TX_SUSPENDED) || (status != TX_SUCCESS))
    {

        /* Thread Suspend error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run once.  */
    if ((thread_0_counter != 1) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_3_counter != 1) ||
        (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    status =  tx_thread_resume(&thread_3);

    /* Determine if all the other threads are in the proper state.  */
    if ((thread_0.tx_thread_state != TX_READY)     || (thread_1.tx_thread_state != TX_READY) ||
        (thread_2.tx_thread_state != TX_READY)     || (thread_3.tx_thread_state != TX_READY) ||
        (thread_4.tx_thread_state != TX_SUSPENDED) || (status != TX_SUCCESS))
    {

        /* Thread Suspend error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run once.  */
    if ((thread_0_counter != 1) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_3_counter != 1) ||
        (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    status =  tx_thread_resume(&thread_4);

    /* Determine if all the other threads are in the proper state.  */
    if ((thread_0.tx_thread_state != TX_READY)     || (thread_1.tx_thread_state != TX_READY) ||
        (thread_2.tx_thread_state != TX_READY)     || (thread_3.tx_thread_state != TX_READY) ||
        (thread_4.tx_thread_state != TX_READY)     || (status != TX_SUCCESS))
    {

        /* Thread Suspend error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run once.  */
    if ((thread_0_counter != 1) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_3_counter != 1) ||
        (thread_4_counter != 1))
    {

        /* Thread Suspend error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Relinquish to allow other threads to run.  */
    tx_thread_relinquish();

    /* Determine if all the other threads are in a suspended state.  */
    if ((thread_0.tx_thread_state != TX_SUSPENDED) || (thread_1.tx_thread_state != TX_SUSPENDED) ||
        (thread_2.tx_thread_state != TX_SUSPENDED) || (thread_3.tx_thread_state != TX_SUSPENDED) ||
        (thread_4.tx_thread_state != TX_SUSPENDED))
    {

        /* Thread Suspend error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Make sure that each thread has run twice.  */
    if ((thread_0_counter != 2) || (thread_1_counter != 2) || 
        (thread_2_counter != 2) || (thread_3_counter != 2) ||
        (thread_4_counter != 2))
    {

        /* Thread Suspend error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Suspend a thread that is already suspended.  */
    status = tx_thread_suspend(&thread_4);
    
    /* Check for error condition.  */
    if (status != TX_SUCCESS)
    {
        /* Thread Suspend error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }
    else
    {
    
        /* Increment thread 5 counter.  */
        thread_5_counter++;

        /* Successful Thread Suspend test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

