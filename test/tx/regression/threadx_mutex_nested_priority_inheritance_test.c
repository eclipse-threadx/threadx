/* This test is designed to test multiple mutex priority inheritance situations.  */

#include   <stdio.h>
#include   "tx_api.h"

#define     DEMO_STACK_SIZE         TEST_STACK_SIZE_PRINTF


/* Define the ThreadX object control blocks...  */

static TX_THREAD               thread_0;
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 
static TX_THREAD               thread_1;
static TX_THREAD               thread_2;
static TX_THREAD               thread_3;
static TX_THREAD               thread_4;
static TX_THREAD               thread_5;
static TX_THREAD               thread_6;

static TX_MUTEX                mutex_0;
static TX_MUTEX                mutex_1;
static TX_MUTEX                mutex_2;
static TX_MUTEX                mutex_3;


/* Define the counters used in the demo application...  */

static ULONG           thread_0_counter;
static ULONG           thread_1_counter;
static ULONG           thread_2_counter;
static ULONG           thread_3_counter;
static ULONG           thread_4_counter;
static ULONG           thread_5_counter;
static ULONG           thread_6_counter;
#endif

/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);
#endif

/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_nested_priority_inheritance_application_define(void *first_unused_memory)
#endif
{

CHAR    *pointer;
UINT    status;


    pointer =  (CHAR *) first_unused_memory;

    /* Create thread.  */
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            16, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #1\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 
    
    /* Create thread.  */
    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            8, 8, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create thread.  */
    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create thread.  */
    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, DEMO_STACK_SIZE, 
            30, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #4\n");
        test_control_return(1);
    }

    /* Create thread.  */
    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #4a\n");
        test_control_return(1);
    }

    /* Create thread.  */
    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, DEMO_STACK_SIZE, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #4b\n");
        test_control_return(1);
    }

    /* Create thread.  */
    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, DEMO_STACK_SIZE, 
            30, 30, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #4c\n");
        test_control_return(1);
    }

    /* Create the mutexes.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);
    status += tx_mutex_create(&mutex_1, "mutex 1", TX_INHERIT);
    status += tx_mutex_create(&mutex_2, "mutex 2", TX_INHERIT);
    status += tx_mutex_create(&mutex_3, "mutex 3", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Nested Priority Inheritance Test...................... ERROR #5\n");
        test_control_return(1);
    }
#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

#ifdef TX_DISABLE_PREEMPTION_THRESHOLD 

    /* Preemption threshold is not enabled, skip this test.  */

    /* Inform user.  */
    printf("Running Mutex Nested Priority Inheritance Test...................... SUCCESS!\n");
    test_control_return(0);

#else  
  
UINT    test_case =  0;
UINT    loop_count =  0;
UINT    priority;
UINT    status;


    /* Inform user.  */
    printf("Running Mutex Nested Priority Inheritance Test...................... ");

    /* Get the mutex that will be owned always....  */
    tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);

    while(loop_count++ < 50)
    {

        /* Increment the thread counter.  */
        thread_0_counter++;

        tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
        tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        tx_mutex_get(&mutex_2, TX_WAIT_FOREVER);

        tx_thread_resume(&thread_1);
        tx_thread_resume(&thread_2);

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_0);

        /* Resume thread thread 3.  */
        tx_thread_resume(&thread_3);

        /* Thread priority should be 4.  */
        if (thread_0.tx_thread_priority != 4)
        {

            /* Test error!  */
            printf("ERROR #6\n");
            test_control_return(1);
        }

        /* Release the mutexes... Depending on the order they are released should dictate 
           the thread's returned to priority.  */
        if (test_case == 0)
        {


            tx_mutex_put(&mutex_2);

            /* Priority should now be returned to priority 8.  */
            if (thread_0.tx_thread_priority != 8)
            {

                /* Test error!  */
                printf("ERROR #7\n");
                test_control_return(2);
            }

            tx_mutex_put(&mutex_1);

            /* Priority should now be returned to priority 15.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #8\n");
                test_control_return(3);
            }

            tx_mutex_put(&mutex_0);
 
            /* No change.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #9\n");
                test_control_return(4);
            }
        }
        else if (test_case == 1)
        {

            priority =  thread_0.tx_thread_priority;
            tx_mutex_put(&mutex_0);     

            /* Should have no change in priority since nothing was inherited for this mutex.  */
            if (thread_0.tx_thread_priority != priority)
            {

                /* Test error!  */
                printf("ERROR #10\n");
                test_control_return(4);
            }

            tx_mutex_put(&mutex_1);     

            /* Should not do anything since mutex 2 elevated to a higher priority.  */
            if (thread_0.tx_thread_priority != priority)
            {

                /* Test error!  */
                printf("ERROR #11\n");
                test_control_return(5);
            }

            tx_mutex_put(&mutex_2);     

            /* Should go back to priority 15.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #12\n");
                test_control_return(6);
            }
        }
        else if (test_case == 2)
        {

            tx_mutex_put(&mutex_2);     

            /* Should go back to priority 8.  */
            if (thread_0.tx_thread_priority != 8)
            {

                /* Test error!  */
                printf("ERROR #13\n");
                test_control_return(7);
            }

            tx_mutex_put(&mutex_0);     

            /* Should not do anything.  */
            if (thread_0.tx_thread_priority != 8)
            {

                /* Test error!  */
                printf("ERROR #14\n");
                test_control_return(8);
            }

            tx_mutex_put(&mutex_1);     

            /* Should go back to priority 15.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #15\n");
                test_control_return(9);
            }
        }
        else if (test_case == 3)
        {

            priority =  thread_0.tx_thread_priority;
            tx_mutex_put(&mutex_1);     

            /* Should not do anything since mutex 2 is still owned.  */
            if (thread_0.tx_thread_priority != priority)
            {

                /* Test error!  */
                printf("ERROR #16\n");
                test_control_return(10);
            }

            tx_mutex_put(&mutex_0);
                 
            /* Should not do anything.  */
            if (thread_0.tx_thread_priority != priority)
            {

                /* Test error!  */
                printf("ERROR #17\n");
                test_control_return(11);
            }

            tx_mutex_put(&mutex_2);     

            /* Should finally go back to priority 15.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #18\n");
                test_control_return(12);
            }
        }
        else if (test_case == 4)
        {

            priority =  thread_0.tx_thread_priority;
            tx_mutex_put(&mutex_1);     

            /* Should not do anything since mutex 2 is still owned.  */
            if (thread_0.tx_thread_priority != priority)
            {

                /* Test error!  */
                printf("ERROR #19\n");
                test_control_return(13);
            }

            tx_mutex_put(&mutex_2);     

            /* Should reurn us back to priority 15.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #20\n");
                test_control_return(14);
            }

            tx_mutex_put(&mutex_0);     

            /* Should not do anything.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #21\n");
                test_control_return(15);
            }
        }
        else if (test_case == 5)
        {

            priority =  thread_0.tx_thread_priority;
            tx_mutex_put(&mutex_0);     

            /* Should not do anything since mutex 2 is still owned.  */
            if (thread_0.tx_thread_priority != priority)
            {

                /* Test error!  */
                printf("ERROR #22\n");
                test_control_return(16);
            }

            tx_mutex_put(&mutex_2);     

            /* Should reurn us back to priority 8.  */
            if (thread_0.tx_thread_priority != 8)
            {

                /* Test error!  */
                printf("ERROR #23\n");
                test_control_return(17);
            }

            tx_mutex_put(&mutex_1);
                 
            /* Should return us back to priority 15.  */
            if (thread_0.tx_thread_priority != 15)
            {

                /* Test error!  */
                printf("ERROR #24\n");
                test_control_return(18);
            }
        }

        test_case++;
        if (test_case > 5)
            test_case =  0;
    }

    /* Check for thread 3 running... this should not happen!  */
    if (thread_3_counter != 50)
    {
    
        printf("ERROR #25\n");
        test_control_return(19);
    }

    /* At this point, mutex 3 owned by this thread.  */
    
    /* Resume thread 6, lowest priority thread.  */
    status =  tx_thread_resume(&thread_6);
    
    /* Check for an error.  */
    if ((status != TX_SUCCESS) || (thread_0.tx_thread_priority != 15) || (thread_6_counter != 0))
    {
    
        printf("ERROR #27\n");
        test_control_return(19);
    }
      
    /* Now resume thread 4.  */
    status =  tx_thread_resume(&thread_4);
    
    /* Check for an error.  */
    if ((status != TX_SUCCESS) || (thread_0.tx_thread_priority != 4) || (thread_6_counter != 0))
    {
    
        printf("ERROR #28\n");
        test_control_return(19);
    }
    
    /* Now resume thread 5.  */
    status =  tx_thread_resume(&thread_5);
    
    /* Check for an error.  */
    if ((status != TX_SUCCESS) || (thread_0.tx_thread_priority != 2) || (thread_6_counter != 0))
    {
    
        printf("ERROR #29\n");
        test_control_return(19);
    }
    
    /* Sleep to let thread 6 run, which is lower priority.  */
    tx_thread_sleep(1);
    
    /* Now release the mutex.  */
    status =  tx_mutex_put(&mutex_3);

    /* Check for an error.  */
    if ((status != TX_SUCCESS) || (thread_0.tx_thread_priority != 15) || (thread_6_counter != 0) || (thread_4_counter != 1) || (thread_5_counter != 1))
    {
    
        printf("ERROR #30\n");
        test_control_return(19);
    }
    
    /* Sleep to let thread 6 run and release the mutex.  */
    tx_thread_sleep(2);


    /* If we get here the test was successful!  */
    printf("SUCCESS!\n");
    test_control_return(0);
#endif
}

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 

static void    thread_1_entry(ULONG thread_input)
{

UINT    old_priority;
UINT    old_threshold;


    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Update the thread priority and thread preemption-threshold of thread 0.  */
        tx_thread_priority_change(&thread_0, 15, &old_priority);
        tx_thread_preemption_change(&thread_0, 14, &old_threshold);        
    
        /* Get mutex.  */
        tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        tx_mutex_put(&mutex_1);

        tx_thread_suspend(&thread_1);

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Update the thread priority and thread preemption-threshold of thread 0.  */
        tx_thread_priority_change(&thread_0, 15, &old_priority);
        tx_thread_preemption_change(&thread_0, 8, &old_threshold);        
    
        /* Get mutex.  */
        tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        tx_mutex_put(&mutex_1);

        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    old_priority;
UINT    old_threshold;

    while(1)
    {

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Update the thread priority and thread preemption-threshold of thread 0.  */
        tx_thread_priority_change(&thread_0, 15, &old_priority);
        tx_thread_preemption_change(&thread_0, 14, &old_threshold);

        /* Get mutex.  */
        tx_mutex_get(&mutex_2, TX_WAIT_FOREVER);
        tx_mutex_put(&mutex_2);

        tx_thread_suspend(&thread_2);

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Update the thread priority and thread preemption-threshold of thread 0.  */
        tx_thread_priority_change(&thread_0, 15, &old_priority);
        tx_thread_preemption_change(&thread_0, 8, &old_threshold);

        /* Get mutex.  */
        tx_mutex_get(&mutex_2, TX_WAIT_FOREVER);
        tx_mutex_put(&mutex_2);

        tx_thread_suspend(&thread_2);
    }
}


static void    thread_3_entry(ULONG thread_input)
{


    while(1)
    {

        /* Resume thread 0.  */
        tx_thread_resume(&thread_0);

        /* Increment the thread counter.  */
        thread_3_counter++;

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_3);
    }
}


static void    thread_4_entry(ULONG thread_input)
{


    while(1)
    {
    
        /* Get priority inherit mutex.  */
        tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);

        /* Increment the thread counter.  */
        thread_4_counter++;

        /* Release the priority inherit mutex.  */
        tx_mutex_put(&mutex_3);

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_4);
    }
}


static void    thread_5_entry(ULONG thread_input)
{


    while(1)
    {

        /* Get priority inherit mutex.  */
        tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);

        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Release the priority inherit mutex.  */
        tx_mutex_put(&mutex_3);

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_5);
    }
}


static void    thread_6_entry(ULONG thread_input)
{


    while(1)
    {

        /* Get priority inherit mutex.  */
        tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);

        /* Increment the thread counter.  */
        thread_6_counter++;

        /* Release the priority inherit mutex.  */
        tx_mutex_put(&mutex_3);

        /* Suspend this thread.  */
        tx_thread_suspend(&thread_6);
    }
}
#endif


