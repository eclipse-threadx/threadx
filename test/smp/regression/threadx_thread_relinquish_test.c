/* This test is designed to see if multiple threads can be created and relinquish control between them.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"


static unsigned long     thread_0_counter = 0;
static TX_THREAD         thread_0;

static unsigned long     thread_1_counter = 0;
static TX_THREAD         thread_1;

static unsigned long     thread_2_counter = 0;
static TX_THREAD         thread_2;

static unsigned long     thread_3_counter = 0;
static TX_THREAD         thread_3;

static unsigned long     thread_4_counter = 0;
static TX_THREAD         thread_4;

static unsigned long     thread_5_counter = 0;
static TX_THREAD         thread_5;

static unsigned long     thread_6_counter = 0;
static TX_THREAD         thread_6;

static unsigned long     thread_7_counter = 0;
static TX_THREAD         thread_7;

static unsigned long     thread_8_counter = 0;
static TX_THREAD         thread_8;

static unsigned long     thread_9_counter = 0;
static TX_THREAD         thread_9;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);
static void    thread_7_entry(ULONG thread_input);
static void    thread_8_entry(ULONG thread_input);
static void    thread_9_entry(ULONG thread_input);


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

    /* Create thread 4.  */
    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #5\n");
        test_control_return(1);
    }

    /* Create thread 5.  */
    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 15, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #6\n");
        test_control_return(1);
    }

    /* Create thread 6.  */
    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #7\n");
        test_control_return(1);
    }

    /* Create thread 7.  */
    status =  tx_thread_create(&thread_7, "thread 7", thread_7_entry, 7,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #8\n");
        test_control_return(1);
    }

    /* Create thread 8.  */
    status =  tx_thread_create(&thread_8, "thread 8", thread_8_entry, 8,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #9\n");
        test_control_return(1);
    }

    /* Create thread 9.  */
    status =  tx_thread_create(&thread_9, "thread 9", thread_9_entry, 9,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Relinquish Test...................................... ERROR #10\n");
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

UINT    status;
UINT    old_threshold;
UINT    old_priority;


    /* Inform user.  */
    printf("Running Thread Relinquish Test...................................... ");

    /* Check for correct input value and execution of other threads.  */
    if ((thread_input != 3) || (thread_0_counter != 1) || (thread_1_counter != 1) ||
        (thread_2_counter != 1) || (thread_0.tx_thread_state != TX_READY) ||
        (thread_1.tx_thread_state != TX_READY) || (thread_2.tx_thread_state != TX_READY))
    {
    
        /* Thread Relinquish error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Increment thread 3 counter.  */
    thread_3_counter++;

    /* Immediate response relinquish.  */
    tx_thread_relinquish();
    
    /* All other threads should be completed now.  */
    if ((thread_0.tx_thread_state != TX_COMPLETED) || (thread_1.tx_thread_state != TX_COMPLETED) ||
        (thread_2.tx_thread_state != TX_COMPLETED))
    {

        /* Thread Relinquish error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Execute immediate response relinquish.  */
    tx_thread_relinquish();
    
    /* Move to the lowest default priority.  */
    status =  tx_thread_priority_change(&thread_3, 31, &old_priority);   
    
    /* Temporarily make enable preemption-threshold with this thread.  */
    status += tx_thread_preemption_change(&thread_3, 3, &old_threshold);
    
    /* Execute immediate response, bypassing the error checking.  */
	_tx_thread_preemption__threshold_scheduled =  TX_NULL;
    _tx_thread_relinquish();

    /* Restore the preemtion-threshold and priority.  */
    status += tx_thread_preemption_change(&thread_3, old_threshold, &old_threshold);
    
    /* Enable all cores for all threads.  */
    status += tx_thread_smp_core_exclude(&thread_4, 0);
    status += tx_thread_smp_core_exclude(&thread_5, 0);
    status += tx_thread_smp_core_exclude(&thread_6, 0);
    status += tx_thread_smp_core_exclude(&thread_7, 0);
    status += tx_thread_smp_core_exclude(&thread_8, 0);
    status += tx_thread_smp_core_exclude(&thread_9, 0);
    
    /* Now lets resume threads 4 through 9. Thread 5 has preemption-threshold set which will excercise 
       the preemption-threshold logic.  */
    status += tx_thread_resume(&thread_4);
    status += tx_thread_resume(&thread_5);
    status += tx_thread_resume(&thread_6);
    status += tx_thread_resume(&thread_7);
    status += tx_thread_resume(&thread_8);
    status += tx_thread_resume(&thread_9);
    
    /* Now relinquish to let these other threads run.  */
    tx_thread_relinquish();
    
    /* Sleep for 20 ticks to test the paths.  */
    tx_thread_sleep(20);
    
    /* Now see if the results are as expected.  */
    if (status != TX_SUCCESS)
    {
    
        /* Thread Relinquish error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Successful thread relinquish test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void    thread_4_entry(ULONG thread_input)
{

    /* Loop to test relinquish.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_4_counter++;

        /* Relinquish to other thread(s).  */
        tx_thread_relinquish();
    }
}


static void    thread_5_entry(ULONG thread_input)
{


    /* Loop to test relinquish.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_5_counter++;

        /* Relinquish to other thread(s).  */
        tx_thread_relinquish();
    }
}


static void    thread_6_entry(ULONG thread_input)
{


    /* Loop to test relinquish.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_6_counter++;

        /* Relinquish to other thread(s).  */
        tx_thread_relinquish();
    }
}


static void    thread_7_entry(ULONG thread_input)
{


    /* Loop to test relinquish.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_7_counter++;

        /* Relinquish to other thread(s).  */
        tx_thread_relinquish();
    }
}


static void    thread_8_entry(ULONG thread_input)
{


    /* Loop to test relinquish.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_8_counter++;

        /* Relinquish to other thread(s).  */
        tx_thread_relinquish();
    }
}


static void    thread_9_entry(ULONG thread_input)
{


    /* Loop to test relinquish.  */
    while(1)
    {

        /* Increment thread 2 counter.  */
        thread_9_counter++;

        /* Relinquish to other thread(s).  */
        tx_thread_relinquish();
    }
}


