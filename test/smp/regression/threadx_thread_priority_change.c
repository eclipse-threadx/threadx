/* This test is designed to test the change priority service call.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static TX_THREAD       thread_3;
static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_4;
static unsigned long   thread_4_counter =  0;
static TX_THREAD       thread_5;
static unsigned long   thread_5_counter =  0;
extern UINT            priority_change_extension_selection;


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);



extern unsigned long _tx_thread_priority_map;


/* Prototype for test control return.  */
void  test_control_return(UINT status);

/* Prototype for internal function.  */
VOID  _tx_thread_smp_simple_priority_change(TX_THREAD *thread_ptr,UINT new_priority);


/* This test routine is used to get a thread of a non ready state.  */ 
void  suspend_thread(void)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD   *thread_ptr;

   
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


/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

UINT    saved_preempt_disable;

#ifndef TX_NOT_INTERRUPTABLE

    /* Determine if we have the interrupt condition we are looking for.  */
    if ((thread_3.tx_thread_priority == 6) && 
        (thread_3.tx_thread_state == TX_READY) &&
        (_tx_thread_priority_list[6] != &thread_3) &&
        (thread_3_counter > 100))
    {

        /* Save the preempt disable flag.  */
        saved_preempt_disable =  _tx_thread_preempt_disable;
        
        /* Clear the preempt disable flag to ensure the API works correctly.  */
        _tx_thread_preempt_disable =  0;

        /* Suspend the thread to generate the condition.  */
        tx_thread_suspend(&thread_3);
        
        /* Restore the preempt disable flag.  */
        _tx_thread_preempt_disable =  saved_preempt_disable;
        
        /* Done trying to generate this test condition.  */
        test_isr_dispatch =  TX_NULL;
    }
#else

    /* Can't get the interrupt inside the code wit TX_NOT_INTERRUPTABLE defined, so simply stop after thread_3_counter > 100.  */
    if (thread_3_counter > 100)
    {
    
        /* Done trying to generate this test condition.  */
        test_isr_dispatch =  TX_NULL;
    }
#endif
}

/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_priority_change_application_define(void *first_unused_memory)
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
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Priority Change Test................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            22, 22, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Priority Change Test................................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            30, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Priority Change Test................................. ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

TX_INTERRUPT_SAVE_AREA

//ULONG   current_time;
UINT        old_threshold;
UINT        old_priority;
UINT        status;
TX_THREAD   *temp_thread;


    /* Inform user.  */
    printf("Running Thread Priority Change Test................................. ");

    /* Resume thread 3.  */
    tx_thread_resume(&thread_3);

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Change priority to 22, to match that of the next highest priority ready thread. 
       This is to test the update of the priority list when a thread is moved to a 
       priority with already ready threads.  */
    status =  tx_thread_priority_change(&thread_0, 22, &old_priority);
    
    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 16) || (thread_1_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Restore original priority.  */
    tx_thread_priority_change(&thread_0, old_priority, &old_priority);    

    /* See if we can change priority of this thread.  */
    status =  tx_thread_priority_change(&thread_0, 7, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 16) || (thread_1_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* See if we can change priority of another ready thread.  */
    status =  tx_thread_priority_change(&thread_1, 21, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 22) || (thread_1_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* See if we can cause preemption by lowering this thread's priority.  */
    status =  tx_thread_priority_change(&thread_0, 22, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 7) || (thread_1_counter != 1))
    {

        /* Thread error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Thread 1 should have run already...  Raise this threads priority 
       back up.  */
    status =  tx_thread_priority_change(&thread_0, 8, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 22) || (thread_1_counter != 1))
    {

        /* Thread error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* See if we can change the priority of a suspended thread.  */
    status =  tx_thread_priority_change(&thread_1, 19, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 21) || (thread_1_counter != 1))
    {

        /* Thread error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Resume the suspended thread so it is in a ready condition.  */
    status =  tx_thread_resume(&thread_1);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1))
    {

        /* Thread error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Should not have run yet.  Raise its priority to 8 still should not run yet!  */
    status =  tx_thread_priority_change(&thread_1, 8, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 19) || (thread_1_counter != 1))
    {

        /* Thread error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Raise its priority to 7, now it should run.  */
    status =  tx_thread_priority_change(&thread_1, 7, &old_priority);

    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 8) || (thread_1_counter != 2) || (thread_2_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }
    
    /* Now thread 1 should be suspended.  Let's change thread 0's priority and make sure thread 2 doesn't run yet!  */
    status =  tx_thread_priority_change(&thread_0, 7, &old_priority);
    
    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 8) || (thread_1_counter != 2) || (thread_2_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }
    
    /* Now resume thread 1, it won't run because it's priority is less than this thread.  */
    status =  tx_thread_resume(&thread_1);
    
    /* Change the priority of thread 1 - the non executing thread to test that path.  */
    status += tx_thread_priority_change(&thread_1, 23, &old_priority);
    
    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 7) || (thread_1_counter != 2) || (thread_2_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
    /* Now setup the preemption-threshold for thread 1.  */
    status =  tx_thread_preemption_change(&thread_1, 14, &old_threshold);
    
    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_threshold != 23) || (thread_1_counter != 2) || (thread_2_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }
       
    /* Resume a higher priority preemption-threshold thread.  */
    status =  tx_thread_resume(&thread_5);

    /* Now lower this thread's priority.  */
    status +=  tx_thread_priority_change(&thread_0, 15, &old_priority);
    
    /* Check status, return priority, and run count of other thread.  */
    if ((status != TX_SUCCESS) || (old_priority != 7) || (thread_1_counter != 2) || (thread_2_counter != 0) || (thread_5_counter != 0))
    {

        /* Thread error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }
#endif

    /* Change to a higher priority.  */
    tx_thread_priority_change(&thread_0, 7, &old_priority);

	/* Disable interrupts.  */
	TX_DISABLE

    /* Simulate a call from inside of mutex put, but doing it here makes life easier.  */
    _tx_thread_preempt_disable++;

    /* Suspend thread 0.  */
    suspend_thread();
 
    /* Resume thread 0. */
    tx_thread_resume(&thread_0);
     
    /* Change priority to get avoid the reset of the priority list.  */
    tx_thread_priority_change(&thread_0, 16, &old_priority);
    
    /* Move the priority back so we have some room to test.  */
    tx_thread_priority_change(&thread_0, 7, &old_priority);
    
    /* Now create the invalid core index situation.  */
    priority_change_extension_selection =  1;
    tx_thread_priority_change(&thread_0, 8, &old_priority);
    thread_0.tx_thread_smp_core_mapped =  0;
    
    /* Now fiddle with the original priority to satisfy that branch.  */
    priority_change_extension_selection =  2;
    tx_thread_priority_change(&thread_0, 9, &old_priority);
    _tx_thread_execute_ptr[0] =  &thread_0;
    
    /* Create the condition where original pt thread is same as the current thread.  */
    temp_thread =  _tx_thread_preemption__threshold_scheduled;
    priority_change_extension_selection =  3;
    tx_thread_priority_change(&thread_0, 15, &old_priority);
    _tx_thread_preemption__threshold_scheduled =  temp_thread;

    /* Move the thread back to a higher priority.  */
    tx_thread_priority_change(&thread_0, 7, &old_priority);

    /* Create the condition where the new thread is NULL.  */
    temp_thread =  _tx_thread_preemption__threshold_scheduled;
    priority_change_extension_selection =  4;
    tx_thread_priority_change(&thread_0, 15, &old_priority);
    _tx_thread_preemption__threshold_scheduled =  temp_thread;    

    /* Decrement the preempt disable count.  */
    _tx_thread_preempt_disable--;
    
    /* Restore interrupts.  */
    TX_RESTORE

    /* Hit branch in _tx_thread_smp_simple_priority_change such that the
       new priority is less than the inheritance priority.  */
    thread_0.tx_thread_inherit_priority = 0;
    _tx_thread_smp_simple_priority_change(&thread_0, 16);
     
    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

static void    thread_1_entry(ULONG thread_input)
{

    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Suspend self and wait for upper thread to resume.  */
        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* This thread should never run!  */

        /* Increment the thread counter.  */
        thread_2_counter++;
        
        /* Self suspend.  */
        tx_thread_suspend(&thread_2);
    }
}


static void    thread_3_entry(ULONG thread_input)
{

UINT    old_priority;
UINT    loop;


    /* Resume threads 4.  */
    tx_thread_resume(&thread_4);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;
    do
    {

        loop =  rand() % 100;
        while (loop--)
        {
            thread_3_counter++;
        }

        /* Raise priority of thread 3 for code coverage.  */
        tx_thread_priority_change(&thread_3, 6, &old_priority);
        tx_thread_priority_change(&thread_3, 5, &old_priority);
        
        /* Check to see if thread 4 has run... it should not have executed
           yet. If it does, set the thread_1_counter to indicate an error!  */
        if (thread_4_counter)
            thread_1_counter++;
    
    } while (test_isr_dispatch);
}


static void    thread_4_entry(ULONG thread_input)
{
    thread_4_counter++;
}


static void    thread_5_entry(ULONG thread_input)
{
    thread_5_counter++;
}


