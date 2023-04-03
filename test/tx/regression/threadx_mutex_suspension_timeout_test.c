/* This test is designed to test the mutex suspension and timeout functionality.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"
#include   "tx_mutex.h"


//static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;
static TX_THREAD       thread_4;
static TX_THREAD       low_priority;


static TX_MUTEX        mutex_0;
static TX_MUTEX        mutex_1;



/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    low_priority_entry(ULONG thread_input);


/* Prototype for test control return.  */
extern void  test_control_return(UINT status);


#ifndef TX_MANUAL_TEST

/* Define test flags for automated test.  */

extern TEST_FLAG    threadx_mutex_suspension_put_test;
extern TEST_FLAG    threadx_mutex_suspension_priority_test;

#endif


/* This test routine is used to get NULL suspension lists in various parts of tx_mutex_put.  This is hooked up to IRQ 0 on this simulation and is entered manually at the 
   correct time.  */
void  abort_all_threads_suspended_on_mutex(void)
{

TX_THREAD       *thread_ptr;

    thread_ptr =  _tx_thread_created_ptr;
    while (thread_ptr)
    {
       if (thread_ptr -> tx_thread_state == TX_MUTEX_SUSP)
           tx_thread_wait_abort(thread_ptr);
       
       thread_ptr =  thread_ptr -> tx_thread_created_next;
       if (thread_ptr == _tx_thread_created_ptr)
            break;
    }
}


/* This test routine is used to get a thread of a non ready state into _tx_mutex_change, called froim _tx_mutex_put.  This is hooked up to IRQ 1 on this simulation and is entered manually at the 
   correct time.  */
void  suspend_lowest_priority(void)
{

    tx_thread_suspend(&low_priority);
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_suspension_timeout_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 4, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&low_priority, "low priority", low_priority_entry, 30,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            30, 30, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Suspension Timeout Test............................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create mutexes.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);
    status += tx_mutex_create(&mutex_1, "mutex 1", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Suspension Timeout Test............................... ERROR #2\n");
        test_control_return(1);
    }

    /* Get the mutex to prevent thread from getting it.  */
    tx_mutex_get(&mutex_0, TX_NO_WAIT);
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
UINT    old_preempt;
UINT    old_priority;


    /* Inform user.  */
    printf("Running Mutex Suspension Timeout Test............................... ");

    /* Sleep for 2 ticks for fresh timer.  */
    tx_thread_sleep(2);

    /* Set clock to 0.  */
    tx_time_set(0);

    /* Suspend on the mutex. */
    status =  tx_mutex_get(&mutex_0, 33);

    /* Did we get the right status at the right time?  */
    if ((status != TX_NOT_AVAILABLE) || (tx_time_get() != 33))
    {

        /* Mutex error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Get the mutex.  */
     status =  tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        
     /* Make sure the three higher priority threads suspend on the mutex.  */
     tx_thread_resume(&thread_4);
     tx_thread_resume(&thread_3);
     tx_thread_resume(&thread_2);
     tx_thread_resume(&thread_1);


#ifdef TX_MANUAL_TEST

     /* Set BP hear and step into code and generate appropriate interrupt IRQ 0 by hand right after the prioritize function with interrupts enabled. This is to achieve the empty list branch coverage after the prioritize function. */
     tx_mutex_put(&mutex_1);
#else

     /* Set the flag such that generate the condition wehre the empty list condition is satisfied in the branch coverage.  */
     threadx_mutex_suspension_put_test =  1;
     tx_mutex_put(&mutex_1);
#endif

     /* Now some hand testing for tx_mutex_priority_change.  */
     
     /* Resume the low priority thread.  */    
     tx_thread_resume(&low_priority);
     
     /* Simulate a call from inside of mutex put, but doing it here makes life easier.  */
     _tx_thread_preempt_disable++;
     
#ifdef TX_MANUAL_TEST

     /* Set BP here and step into code and step through the code until the internal thread resume function returns, then issue an IRQ 1 to cause an ISR to suspend the thread and test the first condition.  */
     _tx_mutex_priority_change(&low_priority, 30);
#else
     
     /* Set the flag to suspend the thread and test the first condition after internal resume is called.  */
     threadx_mutex_suspension_priority_test =  1;
     _tx_mutex_priority_change(&low_priority, 30);
#endif
     
     /* Resume the low priority thread.  */    
     tx_thread_resume(&low_priority);

     /* Now call internal _tx_mutex_priority_change, this should test the preemption-threshold throw-away path.  */
     _tx_mutex_priority_change(&low_priority, 30);
  
     /* Now make it so we can have a higher-priority thread ready but not the execute pointer because of preemption-threshold.  */
     tx_thread_preemption_change(&thread_0, 10, &old_preempt);
     tx_thread_priority_change(&low_priority, 10, &old_priority);
          
     /* Now call the internal mutex priority change on this thread to get the throw-away path on original priority being the same when execute ptr is different.  */
     _tx_thread_execute_ptr =  &low_priority;
     _tx_mutex_priority_change(&low_priority, 10);
     _tx_thread_execute_ptr =  &thread_0;
     
     /* Now make the low priority thread lower again, but with a preemption-threshold equal to thread_0.  This will test yet another throw away condition.  */
     low_priority.tx_thread_inherit_priority =  ((UINT) TX_MAX_PRIORITIES);
     tx_thread_priority_change(&low_priority, 30, &old_priority);
     tx_thread_preemption_change(&low_priority, 10, &old_preempt);
     
     /* Now call the internal mutex priority change on this thread with the same priority.  */
     _tx_mutex_priority_change(&low_priority, 30);
     _tx_thread_preempt_disable--;
     
     /* Successful test.  */
     printf("SUCCESS!\n");
     test_control_return(0);
}


static void    thread_1_entry(ULONG thread_input)
{
UINT    status;

    /* Loop to get the mutex.  */
    while(1)
    {

        /* Get and release the mutex. */
        status =  tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        if (status == TX_SUCCESS)
          tx_mutex_put(&mutex_1);
        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;

    /* Loop to get the mutex.  */
    while(1)
    {

        /* Get and release the mutex. */
        status =  tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        if (status == TX_SUCCESS)
            tx_mutex_put(&mutex_1);
        tx_thread_suspend(&thread_2);
    }
}


static void    thread_3_entry(ULONG thread_input)
{

UINT    status;

    /* Loop to get the mutex.  */
    while(1)
    {

        /* Get and release the mutex. */
        status =  tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        if (status == TX_SUCCESS)
            tx_mutex_put(&mutex_1);
        tx_thread_suspend(&thread_3);
    }
}

static void    thread_4_entry(ULONG thread_input)
{

UINT    status;

    /* Loop to get the mutex.  */
    while(1)
    {

        /* Get and release the mutex. */
        status =  tx_mutex_get(&mutex_1, TX_WAIT_FOREVER);
        if (status == TX_SUCCESS)
            tx_mutex_put(&mutex_1);
        tx_thread_suspend(&thread_4);
    }
}


static void    low_priority_entry(ULONG thread_input)
{
}
