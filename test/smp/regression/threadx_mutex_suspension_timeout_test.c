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
extern UINT            mutex_priority_change_extension_selection; 


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

TX_INTERRUPT_SAVE_AREA

TX_THREAD   *thread_ptr;


    /* Determine which extension to perform...   to get to the different error checks in _tx_mutex_priority_change.  */
    if (mutex_priority_change_extension_selection == 0)
    {
    
        /* Setup the thread pointer.  */
        thread_ptr =  &thread_0;

        /* Disable interrupts.  */
        TX_DISABLE

        /* Set the state to suspended.  */
        thread_ptr -> tx_thread_state =    TX_SUSPENDED;

#ifdef TX_NOT_INTERRUPTABLE

        /* Call actual non-interruptable thread suspension routine.  */
        _tx_thread_system_ni_suspend(thread_ptr, ((ULONG) 0));

        /* Restore interrupts.  */
        TX_RESTORE
#else

        /* Set the suspending flag. */
        thread_ptr -> tx_thread_suspending =  TX_TRUE;

        /* Setup for no timeout period.  */
        thread_ptr -> tx_thread_timer.tx_timer_internal_remaining_ticks =  ((ULONG) 0);

        /* Temporarily disable preemption.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Call actual thread suspension routine.  */
        _tx_thread_system_suspend(thread_ptr);
#endif
    }
    else if (mutex_priority_change_extension_selection == 1)
    {
    
        /* Make the mapped core field to be invalid to exercise the error checking branch for core_index.  */
        thread_0.tx_thread_smp_core_mapped =  TX_THREAD_SMP_MAX_CORES;
    }
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

TX_INTERRUPT_SAVE_AREA

TX_THREAD   *temp_thread;
UINT        status;


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
     
	 /* Disable interrupts.  */
	 TX_DISABLE

     /* Simulate a call from inside of mutex put, but doing it here makes life easier.  */
     _tx_thread_preempt_disable++;
     
	 /* Move the current thread to a lower priority.  */
	 _tx_mutex_priority_change(&thread_0, 15);
	 _tx_mutex_priority_change(&thread_0, 16);

	 /* Move the lower priority thread to a higher priority.  */
	 _tx_mutex_priority_change(&low_priority, 28);
	 _tx_mutex_priority_change(&low_priority, 29);
	 _tx_mutex_priority_change(&low_priority, 30);
     
     /* Change the priority of the current thread, such that we should have reverse the preemption-threshold issue.  */
	 thread_0.tx_thread_user_priority =  30;
	 thread_0.tx_thread_user_preempt_threshold =  30;
     _tx_mutex_priority_change(&thread_0, 30);    

     /* Change to an even lower priority.  */
	 thread_0.tx_thread_user_priority =  31;
	 thread_0.tx_thread_user_preempt_threshold =  31;
     _tx_mutex_priority_change(&thread_0, 31);        

	 /* Move back to a higher-priority.  */
	 thread_0.tx_thread_user_priority =  30;
	 thread_0.tx_thread_user_preempt_threshold =  30;
	 _tx_mutex_priority_change(&thread_0, 16);

#ifdef TX_MANUAL_TEST

     /* Set BP here and step into code and step through the code until the internal thread resume function returns, then issue an IRQ 1 to cause an ISR to suspend the thread and test the first condition.  */
     _tx_mutex_priority_change(&thread_0, 30);
#else
     
     /* Set the flag to suspend the thread and test the first condition after internal resume is called.  */
     mutex_priority_change_extension_selection =  0;
     threadx_mutex_suspension_priority_test =  1;
     _tx_mutex_priority_change(&thread_0, 30);
#endif
    
     /* Resume this thread.  */
     tx_thread_resume(&thread_0);
	 _tx_mutex_priority_change(&thread_0, 16);
    
     /* Change to an even lower priority.  */
	 thread_0.tx_thread_user_priority =  27;
	 thread_0.tx_thread_user_preempt_threshold =  27;
     _tx_mutex_priority_change(&thread_0, 28);        

     /* Setup the low priority thread infromation.  */
	 low_priority.tx_thread_user_priority =  30;
	 low_priority.tx_thread_user_preempt_threshold =  29;
     _tx_mutex_priority_change(&low_priority, 31);
     
     /* Change to an even lower priority.  */
	 thread_0.tx_thread_user_priority =  30;
	 thread_0.tx_thread_user_preempt_threshold =  29;
     _tx_mutex_priority_change(&thread_0, 26);        

#ifdef TX_MANUAL_TEST

     /* Set BP here and step into code and step through the code until the internal thread resume function returns, and then issue the IRQ that modifies the core mapped.  */
     mutex_priority_change_extension_selection =  1;
     _tx_mutex_priority_change(&thread_0, 30);
     thread_0.tx_thread_smp_core_mapped =  0;
#else
     
     /* Set the flag to suspend the thread and test the first condition after internal resume is called.  */
     mutex_priority_change_extension_selection =  1;
     threadx_mutex_suspension_priority_test =  1;
     _tx_mutex_priority_change(&thread_0, 30);
     thread_0.tx_thread_smp_core_mapped =  0;
#endif    

    /* Move the thread back to priority 28.  */
    _tx_mutex_priority_change(&thread_0, 28);
    
#ifdef TX_MANUAL_TEST

     /* Set BP here and step into code and step through the code until the internal thread resume function returns, and then issue the IRQ that modifies the original priority.  */
     mutex_priority_change_extension_selection =  2;
     _tx_mutex_priority_change(&thread_0, 29);
#else
     
     /* Set the flag to suspend the thread and test the first condition after internal resume is called.  */
     mutex_priority_change_extension_selection =  2;
     threadx_mutex_suspension_priority_test =  1;
     _tx_mutex_priority_change(&thread_0, 29);
     _tx_thread_execute_ptr[0] =  &thread_0;
#endif    

#ifdef TX_MANUAL_TEST

     /* Set BP here and step into code and step through the code until the internal thread resume function returns, and then issue the IRQ that sets the original_pt_thread to NULL.  */
     mutex_priority_change_extension_selection =  3;
     _tx_mutex_priority_change(&thread_0, 30);
#else
     
     /* Set the flag to suspend the thread and test the first condition after internal resume is called.  */
     mutex_priority_change_extension_selection =  3;
     threadx_mutex_suspension_priority_test =  1;
     _tx_mutex_priority_change(&thread_0, 30);
#endif    

#ifdef TX_MANUAL_TEST

     /* Set BP here and step into code and step through the code until the internal thread resume function returns, and then issue the IRQ that sets the _tx_thread_preemption__threshold_scheduled to NULL.  */
     temp_thread =  _tx_thread_preemption__threshold_scheduled;
     mutex_priority_change_extension_selection =  4;
     _tx_mutex_priority_change(&thread_0, 31);
     _tx_thread_preemption__threshold_scheduled =  temp_thread;
#else
     
     /* Set the flag to suspend the thread and test the first condition after internal resume is called.  */
     temp_thread =  _tx_thread_preemption__threshold_scheduled;
     mutex_priority_change_extension_selection =  4;
     threadx_mutex_suspension_priority_test =  1;
     _tx_mutex_priority_change(&thread_0, 31);
     _tx_thread_preemption__threshold_scheduled =  temp_thread;
#endif    

     /* Just change to same priority to ensure that path is executed.  */
     mutex_priority_change_extension_selection =  0;
     suspend_lowest_priority();
	 thread_0.tx_thread_user_priority =  31;
	 thread_0.tx_thread_user_preempt_threshold =  30;
	 thread_0.tx_thread_priority =  31;
	 thread_0.tx_thread_preempt_threshold =  30;    
     _tx_mutex_priority_change(&thread_0, 31);
	 thread_0.tx_thread_user_priority =  31;
	 thread_0.tx_thread_user_preempt_threshold =  31;
	 thread_0.tx_thread_priority =  31;
	 thread_0.tx_thread_preempt_threshold =  31;    
	 _tx_mutex_priority_change(&thread_0, 31);
     tx_thread_resume(&thread_0);

     /* Restore the preempt disable flag.  */ 
     _tx_thread_preempt_disable--;
     
	 /* Restore interrupts.  */
	 TX_RESTORE

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
