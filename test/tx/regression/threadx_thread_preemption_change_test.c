/* This test is designed to test the change preemption service call.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"
#include   "tx_initialize.h"

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
static unsigned long   thread_0_counter =  0;
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static TX_MUTEX        mutex_0;
#endif

static TX_THREAD       thread_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_preemption_change_application_define(void *first_unused_memory)
#endif
{

UINT     status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Preemption Change Test............................... ERROR #1\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD /* skip this test and pretend it passed */

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Preemption Change Test............................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            14, 14, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Preemption Change Test............................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Preemption Change Test............................... ERROR #4\n");
        test_control_return(1);
    }
#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
UINT    old_threshold;
UINT    status;
UINT    i;
#endif


    /* Inform user.  */
    printf("Running Thread Preemption Change Test............................... ");

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD /* skip this test and pretend it passed */

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Resume thread 1, which has a higher priority.  Preemption is disabled 
       though so thread 1 should not run yet.  */
    status =  tx_thread_resume(&thread_1);

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0) || (thread_2_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Change the preemption-threshold such that thread 1 doesn't run yet... just for code coverage.  */
    status =  tx_thread_preemption_change(&thread_0, 15, &old_threshold);

    /* Change the preemption threshold.  This should cause thread 1 to execute!  */
    status +=  tx_thread_preemption_change(&thread_0, 16, &old_threshold);

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 0) ||
        (old_threshold != 15))
    {

        /* Thread error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Change the preemption threshold of another thread.  */
    status =  tx_thread_preemption_change(&thread_2, 11, &old_threshold);

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 0) ||
        (old_threshold != 14))
    {

        /* Thread error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Change the preemption threshold back to 15.  */
    status =  tx_thread_preemption_change(&thread_0, 15, &old_threshold);    

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 0) ||
        (old_threshold != 16))
    {

        /* Thread error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Resume thread 2.  This should preempt because it is priority 14 and the 
       current preemption threshold is 15.  */
    status =  tx_thread_resume(&thread_2);

    /* Check status and run counters of other threads.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Thread error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* At this point, we are going to loop through preemption changes that result in 
       preemption.  */
    for (i = 0; i < (TX_THREAD_EXECUTE_LOG_SIZE*3); i++)
    {

        /* Change the preemption threshold back to 14.  */
        status =  tx_thread_preemption_change(&thread_0, 14, &old_threshold);    

        /* Check status.  */
        if (status != TX_SUCCESS) 
        {

            /* Thread error.  */
            printf("ERROR #10\n");
            test_control_return(1);
        }

        /* Resume thread 2 again.  */
        status =  tx_thread_resume(&thread_2);
        
        /* Check status an thread 2 run counter.  */
        if ((status != TX_SUCCESS) && (thread_2_counter != (i+1)))
        {

            /* Thread error.  */
            printf("ERROR #11\n");
            test_control_return(1);
        }
        
        /* Change the preemption threshold back to 15 to allow thread 2 to run.  */
        status =  tx_thread_preemption_change(&thread_0, 15, &old_threshold);    

        /* Check status an thread 2 run counter.  */
        if ((status != TX_SUCCESS) && (thread_2_counter != (i+2)))
        {

            /* Thread error.  */
            printf("ERROR #12\n");
            test_control_return(1);
        }
    }

    /* Change the priority of threads 0 and 1.  */
    status =  tx_thread_priority_change(&thread_0, 7, &old_threshold);    
    status += tx_thread_priority_change(&thread_1, 5, &old_threshold);    

    /* Check status.  */
    if (status != TX_SUCCESS) 
    {

        /* Thread error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    } 

    /* Change the preemption-threshold of this thread.  */
    status =  tx_thread_preemption_change(&thread_0, 0, &old_threshold);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (old_threshold != 7))
    {

        /* Thread error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    } 
    
    /* Get the mutex that has priority inheritance.  */
    status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

    /* Check status.  */
    if (status != TX_SUCCESS) 
    {

        /* Thread error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    } 
    
    /* Resume thread 1 so that it can suspend on the mutex and automatically raise its priority.  */
    tx_thread_resume(&thread_1);
    
    /* Self suspend so that thread 1 and run.  */
    tx_thread_suspend(&thread_0);
    
    /* Restore the preemption-threshold of this thread.  */
    status =  tx_thread_preemption_change(&thread_0, old_threshold, &old_threshold);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (old_threshold != 0) || (thread_0.tx_thread_priority != 5) || (thread_0.tx_thread_preempt_threshold != 5))
    {

        /* Thread error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    } 

    /* Let thread 1 run again so it can release the mutex and undo the priority inheritance.  */
    status =  tx_mutex_put(&mutex_0);    

    /* Check status.  */
    if ((status != TX_SUCCESS) || (thread_0.tx_thread_priority != 7) || (thread_0.tx_thread_preempt_threshold != 7))
    {

        /* Thread error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    } 

    /* Test direct call to the thread preemption change routine with a threshold greater than the current priority.  */
    status =  _tx_thread_preemption_change(&thread_0, 8, &old_threshold);

    /* Check status.  */
    if (status != TX_THRESH_ERROR)
    {

        /* Thread error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    } 

#endif

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);

}

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
static void    thread_1_entry(ULONG thread_input)
{

    /* Self suspend after initial run.  */
    tx_thread_suspend(&thread_1);
    
    /* Increment the thread counter.  */
    thread_1_counter++;

    /* Self suspend.  */
    tx_thread_suspend(&thread_1);

    /* Resume thread 0.  */
    tx_thread_resume(&thread_0);

    /* Get the mutex, which will cause suspension. */
    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

    /* Release the mutex.  */
    tx_mutex_put(&mutex_0);    
}


static void    thread_2_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Increment thread counter.  */
        thread_2_counter++;
        
        /* Self suspend.  */
        tx_thread_suspend(&thread_2);
    }
}
#endif
