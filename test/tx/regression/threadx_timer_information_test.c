/* This test is designed to test timer information services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_timer.h"


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;

static unsigned long   timer_1_counter =  0;
static TX_TIMER        timer_1;


static TX_TIMER        timer_2;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    timer_0_expiration(ULONG timer_input);
static void    timer_1_expiration(ULONG timer_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_timer_information_application_define(void *first_unused_memory)
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
            16, 16, 3, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Information Test...................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Information Test...................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_1, "timer 1", timer_1_expiration, 0x1234,
                        1000, 1000, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Information Test...................................... ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT        status;
UINT        interrupt_status;
CHAR        *name;
UINT        active;
ULONG       remaining_ticks;
ULONG       reschedule_ticks;
TX_TIMER    *next_timer;
ULONG       activates;
ULONG       reactivates;
ULONG       deactivates;
ULONG       expirations;
ULONG       expiration_adjusts;
TX_TIMER_INTERNAL **list_head;


    /* Inform user.  */
    printf("Running Timer Information Test...................................... ");

    /* Sleep for a couple ticks.  */
    tx_thread_sleep(19);

    /* Check for an error.  */
    if (timer_0_counter != 2)
    {

        /* Application timer error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }    

    /* Deactivate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Sleep again.  */
    tx_thread_sleep(19);

    /* Check for an error.  */
    if (timer_0_counter != 2)
    {

        /* Application timer error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }    

    /* Modify the timer.  */
    status =  tx_timer_change(&timer_0, 100, 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Clear the system time.  */
    tx_time_set(0);

    /* Activate the timer.  */
    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Sleep for 120.  */
    tx_thread_sleep(120);

    /* Check the counters to make sure everything is where it should be.  */
    if ((timer_0_counter != 23) || (tx_time_get() != 120))
    {

        /* Application timer error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Get the timer information.  */
    status =  tx_timer_info_get(&timer_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &next_timer);
    status += tx_timer_info_get(&timer_0, &name, &active, &remaining_ticks, &reschedule_ticks, &next_timer);

    /* Check for successful completion.  */
    if ((status != TX_SUCCESS) || (active != TX_TRUE) || (remaining_ticks != 1) || (reschedule_ticks != 1) || (next_timer != &timer_1))
    {
    
        /* Application timer error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    } 
    
    /* Now, deactivate timer 0 to get another path through the info get service.  */
    status =  tx_timer_deactivate(&timer_0);
    status += tx_timer_info_get(&timer_0, &name, &active, &remaining_ticks, &reschedule_ticks, &next_timer);

    /* Check for successful completion.  */
    if ((status != TX_SUCCESS) || (active != TX_FALSE) || (remaining_ticks != 1) || (reschedule_ticks != 1) || (next_timer != &timer_1))
    {
    
        /* Application timer error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    } 

    /* Change timer 0 to a large value and get the information again.  */  
    status =  tx_timer_change(&timer_0, 100, 200);
    status += tx_timer_activate(&timer_0);
    
    status += tx_timer_info_get(&timer_0, &name, &active, &remaining_ticks, &reschedule_ticks, &next_timer);

    /* Check for successful completion.  */
    if ((status != TX_SUCCESS) || (active != TX_TRUE) || (remaining_ticks != 100) || (reschedule_ticks != 200) || (next_timer != &timer_1))
    {
    
        /* Application timer error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    } 

#ifdef TX_TIMER_ENABLE_PERFORMANCE_INFO

    /* Check for NULL pointer.  */
    status =  _tx_timer_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_PTR_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(&timer_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_timer_performance_info_get(&timer_0, &activates, &reactivates, &deactivates, &expirations, &expiration_adjusts);
    
    /* Check for successful completion.  */
    if ((status != TX_SUCCESS) || (activates != timer_0.tx_timer_performance_activate_count) || (reactivates != timer_0.tx_timer_performance_reactivate_count) || 
        (deactivates != timer_0.tx_timer_performance_deactivate_count) || (expirations != timer_0.tx_timer_performance_expiration_count) || (expiration_adjusts != timer_0.tx_timer_performance__expiration_adjust_count))
    {
    
        /* Application timer error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    } 

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_timer_performance_system_info_get(&activates, &reactivates, &deactivates, &expirations, &expiration_adjusts);
    
    /* Check for successful completion.  */
    if ((status != TX_SUCCESS) || (activates != _tx_timer_performance_activate_count) || (reactivates != _tx_timer_performance_reactivate_count) || 
        (deactivates != _tx_timer_performance_deactivate_count) || (expirations != _tx_timer_performance_expiration_count) || (expiration_adjusts != _tx_timer_performance__expiration_adjust_count))
    {
    
        /* Application timer error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    } 

#else

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(&timer_0, &activates, &reactivates, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(TX_NULL, &activates, &reactivates, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(TX_NULL, TX_NULL, &reactivates, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Now get the performance information.  */
    status =  tx_timer_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(&activates, &reactivates, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(TX_NULL, &reactivates, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(TX_NULL, TX_NULL, &deactivates, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &expirations, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &expiration_adjusts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Now get the system performance information.  */
    status =  tx_timer_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Application timer error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

#endif

    /* Test timer that is in the process of expiration - on temporary "expired" list.  */
    TX_MEMSET(&timer_2, 0, (sizeof(TX_TIMER)));
    
    /* Setup fake timer and test for no-reactivate condition.  */
    timer_2.tx_timer_id =  TX_TIMER_ID;
    timer_2.tx_timer_internal.tx_timer_internal_list_head =  (TX_TIMER_INTERNAL **) &list_head;
    list_head =  (struct TX_TIMER_INTERNAL_STRUCT **) &(timer_2.tx_timer_internal);
    timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks =  10; 
    status =  tx_timer_info_get(&timer_2, TX_NULL, TX_NULL, &remaining_ticks, &reschedule_ticks, TX_NULL);

    /* Check for error.  */
    if ((status != TX_SUCCESS) || (remaining_ticks != 0) || (reschedule_ticks != 0))
    {
    
        /* Application timer error.  */
        printf("ERROR #28a\n");
        test_control_return(1);
    }

    /* Setup fake timer and test for reactivate condition - on temporary "expired" list.  */
    timer_2.tx_timer_id =  TX_TIMER_ID;
    timer_2.tx_timer_internal.tx_timer_internal_list_head =  (TX_TIMER_INTERNAL **) &list_head;
    list_head =  (struct TX_TIMER_INTERNAL_STRUCT **) &(timer_2.tx_timer_internal);
    timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks =  TX_TIMER_ENTRIES * 2; 
    status =  tx_timer_info_get(&timer_2, TX_NULL, TX_NULL, &remaining_ticks, &reschedule_ticks, TX_NULL);

    /* Check for error.  */
    if ((status != TX_SUCCESS) || (remaining_ticks != TX_TIMER_ENTRIES) || (reschedule_ticks != 0))
    {
    
        /* Application timer error.  */
        printf("ERROR #28a\n");
        test_control_return(1);
    }


    /* Lockout interrupts.  */
    interrupt_status =  tx_interrupt_control(TX_INT_DISABLE);

    /* Setup fake timer and test for reactivate condition - current timer.  */
    timer_2.tx_timer_id =  TX_TIMER_ID;
    timer_2.tx_timer_internal.tx_timer_internal_list_head =  (TX_TIMER_INTERNAL **) &list_head;
    list_head =  (struct TX_TIMER_INTERNAL_STRUCT **) &(timer_2.tx_timer_internal);
    timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks =  13; 
    _tx_timer_expired_timer_ptr =  &timer_2.tx_timer_internal;
    status =  tx_timer_info_get(&timer_2, TX_NULL, TX_NULL, &remaining_ticks, &reschedule_ticks, TX_NULL);
    _tx_timer_expired_timer_ptr =  TX_NULL;

    /* Restore interrupts.  */
    tx_interrupt_control(interrupt_status);

    /* Check for error.  */
    if ((status != TX_SUCCESS) || (remaining_ticks != 0) || (reschedule_ticks != 0))
    {
    
        /* Application timer error.  */
        printf("ERROR #28b\n");
        test_control_return(1);
    }


    /* Delete the timer...  that are currently active and on the same expiration
       list!  */
    status =  tx_timer_deactivate(&timer_0);
    status += tx_timer_deactivate(&timer_1);
    status += tx_timer_change(&timer_0, 100, 200);
    status += tx_timer_change(&timer_1, 100, 200);
    status += tx_timer_activate(&timer_0);
    status += tx_timer_activate(&timer_1);
    status += tx_timer_delete(&timer_0);
    status += tx_timer_delete(&timer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }
    else
    {

        /* Successful Multiple Sleep test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    timer_0_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_0_counter++;
}


static void    timer_1_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_1_counter++;
}
