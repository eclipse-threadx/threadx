/* This test is designed to test multi-level preemption threshold. The protection placed 
   by a thread must be preserved after higher-priority thread preemption that is above the threshold.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_2a_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static unsigned long   thread_1_counter =  0;

static unsigned long   thread_2_counter =  0;

static unsigned long   thread_2a_counter =  0;
static TX_THREAD       thread_2a;

static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_3;

static unsigned long   thread_4_counter =  0;
static TX_THREAD       thread_4;

#endif

static TX_THREAD       thread_0;


#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

/* Define new preemption-threshold threads for complete bit map checking.  */

static TX_THREAD       thread_1_0;
static TX_THREAD       thread_2_1;
static TX_THREAD       thread_3_2;
static TX_THREAD       thread_4_3;
static TX_THREAD       thread_6_5;
static TX_THREAD       thread_8_7;
static TX_THREAD       thread_10_9;
static TX_THREAD       thread_12_11;
static TX_THREAD       thread_14_13;
static TX_THREAD       thread_16_15;
static TX_THREAD       thread_18_17;
static TX_THREAD       thread_20_19;
static TX_THREAD       thread_22_21;
static TX_THREAD       thread_24_23;
static TX_THREAD       thread_26_25;
static TX_THREAD       thread_28_27;
static TX_THREAD       thread_30_29;


/* Define timer for preemption of priority 1 test.  */

static TX_TIMER         timer_0;


/* Define counters.  */

static unsigned long   timer_0_counter =     0;
static unsigned long   thread_1_0_counter =  0;
static unsigned long   thread_2_1_counter =  0;
static unsigned long   thread_3_2_counter =  0;
static unsigned long   thread_4_3_counter =  0;
static unsigned long   thread_6_5_counter =  0;
static unsigned long   thread_8_7_counter =  0;
static unsigned long   thread_10_9_counter =  0;
static unsigned long   thread_12_11_counter =  0;
static unsigned long   thread_14_13_counter =  0;
static unsigned long   thread_16_15_counter =  0;
static unsigned long   thread_18_17_counter =  0;
static unsigned long   thread_20_19_counter =  0;
static unsigned long   thread_22_21_counter =  0;
static unsigned long   thread_24_23_counter =  0;
static unsigned long   thread_26_25_counter =  0;
static unsigned long   thread_28_27_counter =  0;
static unsigned long   thread_30_29_counter =  0;


/* Define new preemption-threshold test thread entry points.  */

static void    thread_1_0_entry(ULONG thread_input);
static void    thread_2_1_entry(ULONG thread_input);
static void    thread_3_2_entry(ULONG thread_input);
static void    thread_4_3_entry(ULONG thread_input);
static void    thread_6_5_entry(ULONG thread_input);
static void    thread_8_7_entry(ULONG thread_input);
static void    thread_10_9_entry(ULONG thread_input);
static void    thread_12_11_entry(ULONG thread_input);
static void    thread_14_13_entry(ULONG thread_input);
static void    thread_16_15_entry(ULONG thread_input);
static void    thread_18_17_entry(ULONG thread_input);
static void    thread_20_19_entry(ULONG thread_input);
static void    thread_22_21_entry(ULONG thread_input);
static void    thread_24_23_entry(ULONG thread_input);
static void    thread_26_25_entry(ULONG thread_input);
static void    thread_28_27_entry(ULONG thread_input);
static void    thread_30_29_entry(ULONG thread_input);

/* Define timer 1 entry.  */

static void    timer_0_entry(ULONG id);


#endif


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_multi_level_preemption_threshold_application_define(void *first_unused_memory)
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
            (TX_MAX_PRIORITIES-1), (TX_MAX_PRIORITIES/2), TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #1\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD /* skip this test and pretend it passed */

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            (TX_MAX_PRIORITIES/2), (TX_MAX_PRIORITIES/2), TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 10, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2a, "thread 2a", thread_2a_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            11, 11, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            9, 9, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #6\n");
        test_control_return(1);
    }

    /* Create new cascading preemption-threshold test threads.  */

    status =  tx_thread_create(&thread_30_29, "thread 30-29", thread_30_29_entry, 30,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            30, 29, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #7\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_28_27, "thread 28-27", thread_28_27_entry, 28,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            28, 27, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #8\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_26_25, "thread 26-25", thread_26_25_entry, 26,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            26, 25, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #9\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_24_23, "thread 24-23", thread_24_23_entry, 24,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            24, 23, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #10\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_22_21, "thread 22-21", thread_22_21_entry, 22,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            22, 21, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #11\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_20_19, "thread 20-19", thread_20_19_entry, 20,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            20,19, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #12\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_18_17, "thread 18-17", thread_18_17_entry, 18,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 17, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #13\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_16_15, "thread 16-15", thread_16_15_entry, 16,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #14\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_14_13, "thread 14-13", thread_14_13_entry, 14,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            14, 13, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #15\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_12_11, "thread 12-11", thread_12_11_entry, 12,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            12, 11, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #16\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_10_9, "thread 10-9", thread_10_9_entry, 10,
            pointer, TEST_STACK_SIZE_PRINTF, 
            10, 9, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #17\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_8_7, "thread 8-7", thread_8_7_entry, 8,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            8, 7, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #18\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6_5, "thread 6-5", thread_6_5_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 5, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #19\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4_3, "thread 4-3", thread_4_3_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 3, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #20\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3_2, "thread 3-2", thread_3_2_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #21\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2_1, "thread 2-1", thread_2_1_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #22\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1_0, "thread 1-0", thread_1_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #23\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_entry, 0, 1, 0, TX_NO_ACTIVATE);
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multi-Level Preemption Threshold Test................ ERROR #24\n");
        test_control_return(1);
    }
    
#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
UINT status;
UINT old_preempt;
#endif


    /* Inform user.  */
    printf("Running Thread Multi-Level Preemption Threshold Test................ ");

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD /* skip this test and pretend it passed */

    /* Wakeup Thread 1.  It has a higher-priority but should be blocked by
       preemption threshold.  */
    status =  tx_thread_resume(&thread_1);

    /* Check status and make sure the other run counters are proper.  */
    if ((status != TX_SUCCESS) || (thread_1_counter) || (thread_2_counter) ||
        (thread_3_counter) || (thread_4_counter))
    {

        /* Thread Preempt Threshold error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Resume thread 2 which will preempt this thread.  */
    status = tx_thread_resume(&thread_2);

    /* Check for good status and proper run counters.  All other threads except for
       thread 1 should have executed.  */
    if ((status != TX_SUCCESS) || (thread_1_counter) || (thread_2_counter != 1) ||
        (thread_3_counter != 1) || (thread_4_counter != 1))
    {

        /* Thread Preempt Threshold error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Set preemption threshold low enough to allow thread 1 to execute.  */
    status =  tx_thread_preemption_change(&thread_0, ((TX_MAX_PRIORITIES/2)+1), &old_preempt);

    /* Check for good status and proper run counters.  All other threads except for
       thread 1 should have executed.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 1) ||
        (thread_3_counter != 1) || (thread_4_counter != 1) ||
        (_tx_thread_preempted_maps[0]) || (_tx_thread_preempt_disable))
    {

        /* Thread Preempt Threshold error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Set preemption threshold to keep new test threads from running.  */
    status =  tx_thread_preemption_change(&thread_0, 17, &old_preempt);
    
    /* Now wakup the lowest priority preemption-threshold thread.  */
    status += tx_thread_resume(&thread_30_29);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread Preempt Threshold error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Now, self suspend.  */
    status =  tx_thread_suspend(&thread_0);
    
    /* Check to make sure all the preemption-threshold threads ran.  */
    if ((thread_1_0_counter !=  1) ||
        (thread_2_1_counter !=  1) ||
        (thread_3_2_counter !=  1) ||
        (thread_4_3_counter !=  1) ||
        (thread_6_5_counter !=  1) ||
        (thread_8_7_counter !=  1) ||
        (thread_10_9_counter !=  1) ||
        (thread_12_11_counter !=  1) ||
        (thread_14_13_counter !=  1) ||
        (thread_16_15_counter !=  1) ||
        (thread_18_17_counter !=  1) ||
        (thread_20_19_counter !=  1) ||
        (thread_22_21_counter !=  1) ||
        (thread_24_23_counter !=  1) ||
        (thread_26_25_counter !=  1) ||
        (thread_28_27_counter !=  1) ||
        (thread_30_29_counter !=  1))
    {

        /* Thread Preempt Threshold error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

#endif

    /* Successful Thread Suspend non-current thread test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

static void    thread_1_entry(ULONG thread_input)
{

    thread_1_counter++;
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;

    /* Resume thread 3 which will not preempt because of thread 2's preemption
       threshold.  */
    status =  tx_thread_resume(&thread_3);

    /* Resume thread 2a which will not preempt this thread because it is at
       the same priority.  */
    status += tx_thread_resume(&thread_2a);

    /* Check status and run counters.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0) || (thread_3_counter) ||
        (thread_4_counter))
        return;

    /* Resume thread 4 which will preempt.  We should get back here before thread
       three runs because of preemption threshold.  */
    status =  tx_thread_resume(&thread_4);

    /* Check status and run counters.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0) || (thread_3_counter) ||
        (thread_4_counter != 1))
        return;

    /* Relinquish to the other thread at this priority level.  This should 
       clear the preemption threshold condition and allow thread 3 to run. */
    tx_thread_relinquish();

    /* Check status and run counters.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0) || (thread_2a_counter != 1) ||
        (thread_3_counter != 1) || (thread_4_counter != 1))
        return;

    /* If all is okay, increment thread 2's counter.  */
    thread_2_counter++;
}


static void    thread_2a_entry(ULONG thread_input)
{

    if (thread_3_counter == 1)
        thread_2a_counter++;
}


static void    thread_3_entry(ULONG thread_input)
{

    thread_3_counter++;
}


static void    thread_4_entry(ULONG thread_input)
{

    thread_4_counter++;
}



static void    timer_0_entry(ULONG id)
{

    /* Pretend like a preemption occurred on a thread priority of 1 with preemption-threshold set to 0.  */
    _tx_thread_preempted_maps[0] =  _tx_thread_preempted_maps[0] | 2;
    
    /* Set the thread's preemption threshold as well.  */
    thread_1_0.tx_thread_preempt_threshold =  0;

    /* Increment timer 0 counter.  */
    timer_0_counter++;
}


static void    thread_1_0_entry(ULONG thread_input)
{

UINT    status;


    /* Activate the timer to force a priority 0 thread to interrupt.  */
    status =  tx_timer_activate(&timer_0);
    
    /* Loop to wait until timer 0 runs.  */
    while (timer_0_counter == 0)
    {
    }
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
      
        /* Increment this thread's counter.  */
        thread_1_0_counter++;
    }
}


static void    thread_2_1_entry(ULONG thread_input)
{

    /* Increment this thread's counter.  */
    thread_2_1_counter++;
}


static void    thread_3_2_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_1_0);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_3_2_counter++;
    }
}


static void    thread_4_3_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_2_1);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_4_3_counter++;
    }
}


static void    thread_6_5_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_4_3);

    /* In this particular case, we have two different preemptions to make 
       sure we exercise all the code.  */

    /* Now resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_3_2);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_6_5_counter++;
    }
}


static void    thread_8_7_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_6_5);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_8_7_counter++;
    }
}


static void    thread_10_9_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_8_7);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_10_9_counter++;
    }
}

static void    thread_12_11_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_10_9);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_12_11_counter++;
    }
}


static void    thread_14_13_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_12_11);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_14_13_counter++;
    }
}


static void    thread_16_15_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_14_13);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_16_15_counter++;
    }
}


static void    thread_18_17_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_16_15);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_18_17_counter++;
    }
}


static void    thread_20_19_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_18_17);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_20_19_counter++;
    }
}


static void    thread_22_21_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_20_19);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_22_21_counter++;
    }
}


static void    thread_24_23_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_22_21);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_24_23_counter++;
    }
}

static void    thread_26_25_entry(ULONG thread_input)\
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_24_23);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_26_25_counter++;
    }
}

static void    thread_28_27_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_26_25);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_28_27_counter++;
    }
}

static void    thread_30_29_entry(ULONG thread_input)
{

UINT    status;


    /* Resume next highest priority thread.  */
    status =  tx_thread_resume(&thread_28_27);
    
    /* Check for good status.  */
    if (status == TX_SUCCESS)
    {
    
        /* Increment this thread's counter.  */
        thread_30_29_counter++;
    }
    
    /* Resume thread_0.  */
    tx_thread_resume(&thread_0);
}


#endif
