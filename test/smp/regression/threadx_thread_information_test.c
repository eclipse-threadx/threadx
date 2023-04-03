/* This test is for the thread information services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

/* Define task prototypes.  */

static void    thread_0_entry(ULONG task_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_information_application_define(void *first_unused_memory)
#endif
{

INT     status;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            first_unused_memory, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Information Test..................................... ERROR #1\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT        status;
CHAR        *name;
UINT        state;
ULONG       run_count;
UINT        priority;
UINT        preemption_threshold;
ULONG       time_slice;
TX_THREAD   *next_thread;
TX_THREAD   *suspended_thread;
ULONG       resumptions;
ULONG       suspensions;
ULONG       solicited_preemptions;
ULONG       interrupt_preemptions;
ULONG       priority_inversions;
ULONG       time_slices;
ULONG       relinquishes;
ULONG       timeouts;
ULONG       wait_aborts;
TX_THREAD   *last_preempted_by;
ULONG       non_idle_returns;
ULONG       idle_returns;


    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Inform user of success getting to this test.  */
    printf("Running Thread Information Test..................................... ");

    /* Get information about this thread.  */
    status =  tx_thread_info_get(&thread_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_thread_info_get(&thread_0, &name, &state, &run_count, &priority, &preemption_threshold, &time_slice, &next_thread, &suspended_thread);
    
    /* Check for error status.  */
    if ((status != TX_SUCCESS) || (state != TX_READY) || (run_count != thread_0.tx_thread_run_count) || (priority != 16) || (preemption_threshold != 16) || 
        (time_slice != 0) || (next_thread != thread_0.tx_thread_created_next) || (suspended_thread != thread_0.tx_thread_suspended_next))
    {

        /* Thread error.  */
        printf("ERROR #2\n");
        test_control_return(1);
    }


#ifdef TX_THREAD_ENABLE_PERFORMANCE_INFO

    /* Get the performance information about a NULL thread pointer.  */
    status =  _tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {
    
        /* Thread error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(&thread_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_thread_performance_info_get(&thread_0, &resumptions, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check for error.  */
    if ((status != TX_SUCCESS) || (resumptions != thread_0.tx_thread_performance_resume_count) || (suspensions != thread_0.tx_thread_performance_suspend_count) ||
        (solicited_preemptions != thread_0.tx_thread_performance_solicited_preemption_count) || (interrupt_preemptions != thread_0.tx_thread_performance_interrupt_preemption_count) ||
        (priority_inversions != thread_0.tx_thread_performance_priority_inversion_count) || (time_slices != thread_0.tx_thread_performance_time_slice_count) ||
        (relinquishes != thread_0.tx_thread_performance_relinquish_count) || (timeouts != thread_0.tx_thread_performance_timeout_count) || (wait_aborts != thread_0.tx_thread_performance_wait_abort_count) ||
        (last_preempted_by != thread_0.tx_thread_performance_last_preempting_thread))
    {

        /* Thread error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_thread_performance_system_info_get(&resumptions, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check for error.  */
    if ((status != TX_SUCCESS) || (resumptions != _tx_thread_performance_resume_count) || (suspensions != _tx_thread_performance_suspend_count) ||
        (solicited_preemptions != _tx_thread_performance_solicited_preemption_count) || (interrupt_preemptions != _tx_thread_performance_interrupt_preemption_count) ||
        (priority_inversions != _tx_thread_performance_priority_inversion_count) || (time_slices != _tx_thread_performance_time_slice_count) ||
        (relinquishes != _tx_thread_performance_relinquish_count) || (timeouts != _tx_thread_performance_timeout_count) || (wait_aborts != _tx_thread_performance_wait_abort_count) ||
        (non_idle_returns != _tx_thread_performance_non_idle_return_count) || (idle_returns != _tx_thread_performance_idle_return_count))
    {

        /* Thread error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
    else
    {
    
        /* Success!  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
#else

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(&thread_0, &resumptions, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, &resumptions, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                &relinquishes, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, &timeouts, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, &wait_aborts, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, TX_NULL, &last_preempted_by);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Get the performance information about this thread.  */
    status =  tx_thread_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(&resumptions, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, &suspensions, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, &solicited_preemptions, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &interrupt_preemptions, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &priority_inversions, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &time_slices, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                &relinquishes, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, &timeouts, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, &wait_aborts, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, TX_NULL, &non_idle_returns, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, TX_NULL, TX_NULL, &idle_returns);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Get the system performance information.  */
    status =  tx_thread_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, 
                                                TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Thread error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Success!  */
    printf("SUCCESS!\n");
    test_control_return(0);

#endif

}
