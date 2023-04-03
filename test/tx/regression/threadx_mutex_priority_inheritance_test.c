/* This test is designed to test the mutex suspension and priority inheritance with another 
   thread resuming the higher priority thread by doing a mutex put.  Higher-priority thread should preempt.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;

static unsigned long   thread_4_counter =  0;
static TX_THREAD       thread_4;

static unsigned long   thread_5_counter =  0;
static TX_THREAD       thread_5;

static unsigned long   thread_6_counter =  0;
static TX_THREAD       thread_6;

static unsigned long   thread_7_counter =  0;
static TX_THREAD       thread_7;

static TX_MUTEX        mutex_0;
static TX_MUTEX        mutex_1;
static TX_MUTEX        mutex_2;
static TX_MUTEX        mutex_3;


extern UINT            test_mutex_from_init;
extern TEST_FLAG       test_forced_mutex_timeout;


VOID        _tx_mutex_priority_change(TX_THREAD *thread_ptr, UINT priority, UINT threshold);



/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);
static void    thread_7_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_priority_inheritance_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Test for an error creating/using a mutex from initialization.  */
    if (test_mutex_from_init != TX_SUCCESS)
    {
    
        printf("Running Mutex Priority Inheritance Test............................. ERROR #0\n");
        test_control_return(1);
    }    

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            14, 14, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #3\n");
        test_control_return(1);
    }

    /* Create a high-priority thread that will get all the priority inheritance mutexes and return from
       it's entry function in order to test the auto delete feature.  */
    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            10, 10, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #4\n");
        test_control_return(1);
    }

    /* Create a higher-priority thread that is used to get thread 4 into a priority inheritance state.  */
    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            8, 8, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #5\n");
        test_control_return(1);
    }


    /* Create a higher-priority thread that is used to suspend on priority inheritance mutex 3.  */
    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #6\n");
        test_control_return(1);
    }


    /* Create a higher-priority thread that is used to suspend on priority inheritance mutex 3.  */
    status =  tx_thread_create(&thread_7, "thread 7", thread_7_entry, 7,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            7, 7, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #7\n");
        test_control_return(1);
    }

    /* Create a mutex.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #8\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_1, "mutex 1", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #9\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_2, "mutex 2", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #10\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_3, "mutex 3", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Priority Inheritance Test............................. ERROR #11\n");
        test_control_return(1);
    }

    /* Drive one path of the internal mutex priority change routine directly for code coverage.  */
    thread_3.tx_thread_user_priority =   1;
    thread_3.tx_thread_user_preempt_threshold =  1;
    thread_3.tx_thread_state =  4;
    _tx_mutex_priority_change(&thread_3, 3, 3);
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Mutex Priority Inheritance Test............................. ");

    /* Resume thread 4 to test the automatic release of the mutexes.  */
    tx_thread_resume(&thread_4);
    
    /* Determine if thread 4 was able to get the mutexes before completion... and
       have its original priority restored after the priority inheritance.  */
    if ((thread_4_counter != 1) || (thread_5_counter != 1) || (thread_4.tx_thread_priority != 10) ||
        (thread_4.tx_thread_inherit_priority != TX_MAX_PRIORITIES))
    {

        /* Mutex error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Get all the mutexex.  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);
    status += tx_mutex_get(&mutex_1, TX_NO_WAIT);
    status += tx_mutex_get(&mutex_2, TX_NO_WAIT);

    /* Should be successful.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    
    /* Release mutex 2 to be compatible with original test.  */
    tx_mutex_put(&mutex_2);
    
    /* Now resume the higher priority thread to cause suspension.  */
    tx_thread_resume(&thread_1);

    /* The other thread should now be suspended on the mutex.  */
    if ((thread_1_counter != 1) || (thread_0.tx_thread_priority != 15))
    {

        /* Mutex error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Release the mutex, this should cause the other thread
       to preempt.  */
    status =  tx_mutex_put(&mutex_0);

    /* Check status and run counter of other thread.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 2) || (thread_0.tx_thread_priority != 16))
    {

        /* Mutex error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* At this point, get the mutex again.  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Should be successful.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
    
    /* Now sleep for 20 ticks in order to test the priority inheritance change of a 
       non-ready thread.  */
    tx_thread_sleep(20);

    /* The other thread should now be suspended on the mutex.  */
    if ((thread_1_counter != 3) || (thread_0.tx_thread_priority != 15))
    {

        /* Mutex error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Resume thread 2 in order to get two threads suspended on the mutex.  */
    tx_thread_resume(&thread_2);
    
    /* Now do a mutex put to release both threads suspended on this mutex.  */
    status =  tx_mutex_put(&mutex_0);
    
    /* The other thread should now be suspended on the mutex.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 4) || (thread_2_counter != 2) || (thread_0.tx_thread_priority != 16))
    {

        /* Mutex error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }
    
    /* At this point, get the mutex again.  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Should be successful.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }
    
    /* Abort the sleep.  */
    tx_thread_wait_abort(&thread_1);
    tx_thread_wait_abort(&thread_2);

    /* Now both threads are suspended again on mutex... and then terminate them.  */
    tx_thread_terminate(&thread_1);
    tx_thread_terminate(&thread_2);
    
    /* Now do a mutex put to release both threads suspended on this mutex.  */
    status =  tx_mutex_put(&mutex_0);
    
    /* The other thread should now be suspended on the mutex.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 5) || (thread_2_counter != 3) || (thread_0.tx_thread_priority != 16))
    {

        /* Mutex error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Now test the timeout on the suspension list of a priority inheritance mutex.  */
    
    /* First, obtain priority inheritance mutex 3.  */
    status =  tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);
    
    /* Next resume threads 6 and 7 so they will block on trying to get this mutex forever.  */
    status += tx_thread_resume(&thread_7);
    status += tx_thread_resume(&thread_6);
    
    /* Now set the flag which will cause the last thread in the suspension list to timeout (abort)
       resulting in a NULL suspension list and covering that branch condition in tx_mutex_put  */
    test_forced_mutex_timeout =  1;
    
    /* Perform a mutex put to release the mutex.  */
    status += tx_mutex_put(&mutex_3);
    
    /* Now check for errors.  */
#ifndef TX_MISRA_ENABLE
#ifndef TX_MANUAL_TEST
#ifndef TX_NOT_INTERRUPTABLE
    if ((status != TX_SUCCESS) || (thread_6_counter != 1) || (thread_7_counter != 0))
#else
    if ((status != TX_SUCCESS) || (thread_6_counter != 1))
#endif
#else
    if ((status != TX_SUCCESS) || (thread_6_counter != 1))
#endif
#else
    if ((status != TX_SUCCESS) || (thread_6_counter != 1))
#endif     
    {
    
        /* Mutex error.  */
        printf("ERROR #21\n");
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

UINT    status;


    while(1)
    {
    
        /* Increment thread run counter.  */
        thread_1_counter++;

        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
        status += tx_mutex_put(&mutex_0);

        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_1_counter++;
        
        /* Sleep for 10 ticks... to delay.  */
        tx_thread_sleep(10);  
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;


    /* Get mutex to cause additional ownership linked-list processing.  */
    tx_mutex_get(&mutex_2, TX_WAIT_FOREVER);

    while(1)
    {
    
        /* Increment thread run counter.  */
        thread_2_counter++;

        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
        status += tx_mutex_put(&mutex_0);

        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_2_counter++;
        
        /* Sleep for 10 ticks... to delay.  */
        tx_thread_sleep(10);  
    }
}


static void    thread_4_entry(ULONG thread_input)
{

UINT    status;

     
    /* Get mutex to cause additional ownership linked-list processing.  */
    status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
    status += tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
    status += tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
    status += tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
    status += tx_mutex_get(&mutex_2, TX_WAIT_FOREVER);

    /* Resume thread 5 to get into priority inheritance.  */
    tx_thread_resume(&thread_5);

    /* Determine if all the mutex gets were successful... and we have 
       inherited priority 8.  */
    if ((status == TX_SUCCESS) && (thread_4.tx_thread_priority == 8))
    {

        /* Yes, increment the thread counter.  */
        thread_4_counter++;
    }

    /* Now fall through and make sure the mutex cleanup function 
       releases all the mutexes.  */
}


static void    thread_5_entry(ULONG thread_input)
{

UINT    status;

     
    /* Get mutex to cause priority inheritance in thread 4.  */
    status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

    /* Determine if all the mutex gets were successful.  */
    if (status == TX_SUCCESS)
    {

        /* Yes, increment the thread counter.  */
        thread_5_counter++;
    }

    /* Now fall through and make sure the mutex cleanup function 
       releases all the mutexes.  */
}


static void    thread_6_entry(ULONG thread_input)
{

UINT    status;

     
    /* Get mutex to cause priority inheritance in thread 0.  */
    status =  tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);

    /* Determine if all the mutex gets were successful.  */
    if (status == TX_SUCCESS)
    {

        /* Yes, increment the thread counter.  */
        thread_6_counter++;
    }

    /* Now fall through and make sure the mutex cleanup function 
       releases all the mutexes.  */
}


static void    thread_7_entry(ULONG thread_input)
{

UINT    status;

     
    /* Get mutex to cause priority inheritance in thread 0.  */
    status =  tx_mutex_get(&mutex_3, TX_WAIT_FOREVER);

    /* Determine if all the mutex gets were successful.  */
    if (status == TX_SUCCESS)
    {

        /* Yes, increment the thread counter.  */
        thread_7_counter++;
    }

    /* Now fall through and make sure the mutex cleanup function 
       releases all the mutexes.  */
}

